using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

namespace XRefresh
{
	public partial class Configuration : Form
	{
		public static Configuration Current;
		private Model model; // temporary model copy for configuration form
		bool cancelled;

		public Configuration()
		{
			InitializeComponent();
			Current = this;
			SetupTable();
		}

		public void SetupTable()
		{
			SuspendLayout();

			// table
			table.ColumnModel = columnModel;
			table.EnableHeaderContextMenu = false;
			table.SelectionBackColor = Color.Yellow;
			table.SelectionForeColor = Color.Black;
			table.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			table.FullRowSelect = true;
			tableModel.RowHeight = 18;
			table.GridLineStyle = XPTable.Models.GridLineStyle.Dot;
			table.HeaderFont = new System.Drawing.Font("Arial", 9.0F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			table.Name = "table";
			table.NoItemsText = "There are no project folders to watch. Drag and drop your project folder here ...";
			table.TabIndex = 1;
			table.TableModel = tableModel;
			table.Text = "table";

			// columnModel
			columnModel.Columns.AddRange(new XPTable.Models.Column[] {
			nameColumn,
			folderColumn,
			typeColumn});

			// nameColumn
			nameColumn.Text = "Project";
			nameColumn.Width = 120;
			nameColumn.Renderer = new ProjectCellRenderer(imageListTypes);
			nameColumn.Editor = new ProjectCellEditor();

			// folderColumn
			folderColumn.Text = "Path";
			folderColumn.Width = 470;
			folderColumn.Renderer = new FolderCellRenderer();
			folderColumn.Editor = new FolderCellEditor();

			// typeColumn
			typeColumn.Text = "Type";
			typeColumn.Width = 80;
			ProjectTypeCellEditor typeEditor = new ProjectTypeCellEditor(imageListTypes);
			typeEditor.DropDownStyle = XPTable.Editors.DropDownStyle.DropDownList;
			typeEditor.FillItems += new ProjectTypeCellEditor.FillItemsHandler(ProjectTypePopulationEvent);
			typeEditor.HideDropDownEvent += new ProjectTypeCellEditor.HideDropDownEventHandler(ProjectTypeHideDropDownEvent);
			typeColumn.Editor = typeEditor;
			ProjectTypeCellRenderer typeRenderer = new ProjectTypeCellRenderer();
			typeColumn.Renderer = typeRenderer;

			table.KeyDown += new KeyEventHandler(OnKeyDown);
			table.CellPropertyChanged += new XPTable.Events.CellEventHandler(OnCellPropertyChanged);

			ResumeLayout(true);
		}

		string MakeUniqueName(string name)
		{
			if (name.Length>0)
			{
				bool isOk = true;
				foreach (Model.FoldersRow folder in model.Folders)
				{
					if (folder.Name == name)
					{
						isOk = false;
						break;
					}
				}
                if (isOk) return name;
			}

			char[] numbers = new char[] { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
			string naked = name.TrimEnd(numbers);
			if (naked.Length == 0) naked = "Project";

			int maxTries = 99;
			int current = 1;
			string result = "";
			while (current<maxTries)
			{
				string candidate = naked;
				if (current > 1) candidate = candidate + current.ToString();

				bool isOk = true;
				foreach (Model.FoldersRow folder in model.Folders)
				{
					if (folder.Name==candidate) 
					{
						isOk = false;
						break;
					}
				}

				if (isOk)
				{
					result = candidate;
					break;
				}

				current++;
			}

			if (result.Length==0)
			{
				// all 100 names has been taken?
				// return some randomized stuff
				for (int i=0; i<8; i++)
				{
					naked = naked + i.ToString();
				}
				result = naked;
			}

			return result;
		}

		void StrikeRow(XPTable.Models.Row row, bool strike)
		{
			XPTable.Models.Cell c = row.Cells[1];
			c.CellStyle.Font = new Font(c.CellStyle.Font, strike ? FontStyle.Strikeout : FontStyle.Regular);
			table.InvalidateCell(c);
		}

		void OnCellPropertyChanged(object sender, XPTable.Events.CellEventArgs e)
		{
			if (e.EventType != XPTable.Events.CellEventType.ValueChanged && e.EventType != XPTable.Events.CellEventType.CheckStateChanged) return;
			Model.FoldersRow folder = table.TableModel.Rows[e.Row].Tag as Model.FoldersRow;

			lock (folder)
			{
				// update model with the new value from table
				switch (e.CellPos.Column)
				{
					case 0: // project name
						if (folder.Name != e.Cell.Text)
						{
							folder.Name = MakeUniqueName(e.Cell.Text);
							e.Cell.Text = folder.Name;
						}
						folder.Enabled = e.Cell.Checked;
						StrikeRow(table.TableModel.Rows[e.Row], !folder.Enabled);
						break;
					case 1: // project path
						folder.Path = e.Cell.Text;
						e.Cell.Checked = Directory.Exists(folder.Path);
						break;
					case 2: // project type
						folder.Type = e.Cell.Text;
						table.TableModel.Rows[e.Row].Cells[0].Data = (int)e.Cell.Data; // set new project icon
						table.InvalidateCell(e.Row, 0);
						break;
				}
			}
		}

		void OnKeyDown(object sender, KeyEventArgs e)
		{
			// handle delete action
			if (e.KeyCode==Keys.Delete)
			{
				if (table.TableModel.Selections.SelectedItems.Length>0)
					DeleteRow(table.TableModel.Selections.SelectedItems[0]);
			}
		}

		private void DeleteRow(XPTable.Models.Row row)
		{
			DialogResult res = MessageBox.Show(String.Format("Do you really want to remove folder {0}?", row.Cells[0].Text), "Delete item", MessageBoxButtons.OKCancel, MessageBoxIcon.Question);
			if (res != DialogResult.OK) return;
			((Model.FoldersRow)row.Tag).Delete();
			int index = row.Index;
			table.TableModel.Rows.RemoveAt(index);
			if (table.TableModel.Rows.Count <= index && index>0) index--;
			table.TableModel.Selections.SelectCells(index, 0, index, 2);
			
		}

		private void Configuration_FormClosing(object sender, FormClosingEventArgs e)
		{
			if (!cancelled) ApplyConfiguration();
			Dispose();
			Utils.ReduceMemoryUsage();
		}

		public int ProjectTypeComparer(ProjecTypeItem a, ProjecTypeItem b)
		{
			return a.Text.ToString().CompareTo(b.Text.ToString());
		}

		// thanks to: http://www.codeproject.com/cs/threads/BackgroundWorker_Threads.asp

		// The BackgroundWorker will be used to perform a long running action
		// on a background thread.  This allows the UI to be free for painting
		// as well as other actions the user may want to perform.  The background
		// thread will use the ReportProgress event to update the ProgressBar
		// on the UI thread.
		private BackgroundWorker detectorWorker;
		private Dictionary<object, DetectorData> detectorDatas = new Dictionary<object, DetectorData>();

		class DetectorData {
			public string folder;
			public List<ProjecTypeItem> items; // items being prepared for editor
			public ProjectTypeCellEditor editor;
			public int counter;

			public DetectorData(string folder, ProjectTypeCellEditor editor)
			{
				this.folder = folder;
				this.editor = editor;
				this.items = new List<ProjecTypeItem>();
				this.counter = 0;
			}
		};

		private void DetectorDoWork(object sender, DoWorkEventArgs e)
		{
			try 
			{
				// The sender is the BackgroundWorker object we need it to
				// report progress and check for cancellation.
				BackgroundWorker worker = sender as BackgroundWorker;

				DetectorData detectorData = detectorDatas[sender];

				// build list of project types using scanners
				List<ProjecTypeItem> recommendedTypes = new List<ProjecTypeItem>();
				List<ProjecTypeItem> otherTypes = new List<ProjecTypeItem>();

				foreach (Scanner scanner in Detector.Current.scanners)
				{
					bool isRecommended = scanner.Scan(detectorData.folder);

					ProjecTypeItem item = new ProjecTypeItem();
					item.Text = scanner.GetName();
					item.Description = scanner.GetDescription();
					item.ImageIndex = scanner.GetImageIndex();
					item.Mark = true;
					item.ForeColor = isRecommended ? Color.Black : Color.Gray;

					if (isRecommended)
						recommendedTypes.Add(item);
					else
						otherTypes.Add(item);
				}

				// enable separator lines
				if (recommendedTypes.Count > 0)
					recommendedTypes[recommendedTypes.Count - 1].Separator = true;
				if (otherTypes.Count > 0)
					otherTypes[otherTypes.Count - 1].Separator = true;

				// add lists into listbox
				detectorData.items.AddRange(recommendedTypes.ToArray());
				detectorData.items.AddRange(otherTypes.ToArray());

				ProjecTypeItem custom = new ProjecTypeItem();
				custom.Text = "Custom";
				custom.Description = "Custom settings";
				custom.ImageIndex = 0;
				custom.Mark = true;
				custom.ForeColor = Color.Blue;
				detectorData.items.Add(custom);
			}
			catch (CancelException)
			{
				e.Cancel = true;
			}
		}

		private void DetectorWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
		{
			// The background process is complete. We need to inspect
			// our response to see if an error occurred, a cancel was
			// requested or if we completed successfully.

			DetectorData detectorData = detectorDatas[sender];

			// Check to see if an error occurred in the
			// background process.
			if (e.Cancelled)
			{
			} 
			else if (e.Error != null)
			{
				MessageBox.Show(e.Error.Message, "Error");
			}
			else
			{
				detectorData.editor.Items.Clear();
				detectorData.editor.Items.AddRange(detectorData.items.ToArray());
				detectorData.editor.UpdateDropDownSize();
			}

			detectorWorker = null;
			detectorDatas.Remove(sender);
		}

		private void DetectorProgressChanged(object sender, ProgressChangedEventArgs e)
		{
			// This function fires on the UI thread so it's safe to edit
			// the UI control directly, no funny business with Control.Invoke.
			// Update the progressBar with the integer supplied to us from the
			// ReportProgress() function.  Note, e.UserState is a "tag" property
			// that can be used to send other information from the
			// BackgroundThread to the UI thread.

			BackgroundWorker worker = sender as BackgroundWorker;
			if (worker.CancellationPending) return;

			DetectorData detectorData = detectorDatas[sender];
			detectorData.counter++;
			if (detectorData.counter % 10 != 0) return;

			ProjecTypeItem item = detectorData.editor.Items[0] as ProjecTypeItem;
			item.Description = e.UserState as string;
			detectorData.editor.UpdateDropDownSize();
		}

		private void ProjectTypePopulation(ProjectTypeCellEditor editor)
		{
			// retrieve current row
			int row = editor.EditingCellPos.Row;

			// retrieve current folder path
			string folder = tableModel.Rows[row].Cells[1].Text;

			bool hasCache = Detector.Current.HasExtCache(folder);
			if (!hasCache)
			{
				// create waiting item, to signal progress
				ProjecTypeItem wait = new ProjecTypeItem();
				wait.Text = "Scanning";
				wait.Description = "Wait please ...";
				wait.ImageIndex = -1;
				wait.Mark = true;
				wait.ForeColor = Color.Red;
				editor.Items.Clear();
				editor.Items.Add(wait);
			}

			// cancel previous worker if running
			if (detectorWorker!=null)
			{
				DetectorCancel();
			}

			// create a background worker thread that ReportsProgress & SupportsCancellation
			// hook up the appropriate events.
			DetectorData detectorData = new DetectorData(folder, editor);

			detectorWorker = new BackgroundWorker();
			detectorWorker.WorkerReportsProgress = true;
			detectorWorker.WorkerSupportsCancellation = true;
			detectorWorker.ProgressChanged += new ProgressChangedEventHandler(DetectorProgressChanged);
			detectorWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(DetectorWorkerCompleted);
			detectorWorker.DoWork += new DoWorkEventHandler(DetectorDoWork);
			Detector.Current.currentWorker = detectorWorker;
			detectorDatas.Add(detectorWorker, detectorData);
			if (!hasCache)
			{
				detectorWorker.RunWorkerAsync();
			}
			else
			{
				// we have cache, so do operation synchronously, to prevent UI blinking
				DetectorDoWork(detectorWorker, new DoWorkEventArgs(null));
				DetectorWorkerCompleted(detectorWorker, new RunWorkerCompletedEventArgs(null, null, false));
			}
		}

		private void ProjectTypePopulationEvent(object sender, EventArgs e)
		{
			if (sender is ProjectTypeCellEditor)
			{
				ProjectTypePopulation(sender as ProjectTypeCellEditor);
			}
		}

		private void ProjectTypeHideDropDownEvent(object sender, EventArgs e)
		{
			if (detectorWorker == null) return;
			DetectorCancel();
		}

		private void DetectorCancel()
		{
			detectorWorker.CancelAsync();
			detectorWorker.ProgressChanged -= new ProgressChangedEventHandler(ScannerProgressChanged);
			detectorWorker.RunWorkerCompleted -= new RunWorkerCompletedEventHandler(ScannerWorkerCompleted);
		}

		private void VisualizeFolder(Model.FoldersRow folder)
		{
			XPTable.Models.Row row = new XPTable.Models.Row();

			XPTable.Models.Cell cell1 = new XPTable.Models.Cell();
			XPTable.Models.CellStyle cellStyle1 = new XPTable.Models.CellStyle();
			XPTable.Models.Cell cell2 = new XPTable.Models.Cell();
			XPTable.Models.CellStyle cellStyle2 = new XPTable.Models.CellStyle();
			XPTable.Models.Cell cell3 = new XPTable.Models.Cell();
			XPTable.Models.CellStyle cellStyle3 = new XPTable.Models.CellStyle();

			cell1.Text = folder.Name;
			cell1.Checked = folder.Enabled;
			cell1.Data = Detector.Current.GetIcon(folder.Type);
			cell2.Text = folder.Path;
			cell2.Checked = Directory.Exists(folder.Path);
			cell3.Text = folder.Type;

			cellStyle1.Font = new System.Drawing.Font("Arial", 8.0F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			cell1.CellStyle = cellStyle1;

			cellStyle2.Font = new System.Drawing.Font("Courier", 8.0F, 0, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			cell2.CellStyle = cellStyle2;

			cellStyle3.Font = new System.Drawing.Font("Arial", 8.0F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			cell3.CellStyle = cellStyle3;

			row.Cells.AddRange(new XPTable.Models.Cell[] { cell1, cell2, cell3 });
			row.ChildIndex = 0;
			row.Editable = true;
			row.Tag = folder;

			StrikeRow(row, !folder.Enabled);

			tableModel.Rows.Add(row);
		}

		public void LoadConfiguration()
		{
			tableModel.Rows.Clear();
			model = Context.Model.Copy() as Model; // deep copy
			model.IterateFolders(VisualizeFolder);
		}

		private void ApplyConfiguration()
		{
			// completely stop old model
			Context.Model.Stop();

			// add error into log
			ActivityLog.Current.AddEventLog(Properties.Resources.Information, "Applying new configuration ...");

			// set reference to new model and run it
			Context.Model = model;
			Context.Model.Init();
			Worker.Current.RefreshSettings();
			Context.Model.Start();
			
			// nice to have new settings saved to disk
			Context.Current.SaveSettings(); // possibly onto new place ...
		}

		// thanks to: http://www.codeproject.com/cs/threads/BackgroundWorker_Threads.asp

		// The BackgroundWorker will be used to perform a long running action
		// on a background thread.  This allows the UI to be free for painting
		// as well as other actions the user may want to perform.  The background
		// thread will use the ReportProgress event to update the ProgressBar
		// on the UI thread.
		private BackgroundWorker scannerWorker;
		private ScannerData scannerData;

		class ScannerData
		{
			public string folder;
			public ScannerProgress dialog;
			public int counter;
			public Scanner scanner;

			public ScannerData(string folder, BackgroundWorker worker)
			{
				this.folder = folder;
				this.dialog = new ScannerProgress(folder, worker);
				this.counter = 0;
				this.scanner = null;
			}
		}

		private void ScannerDoWork(object sender, DoWorkEventArgs e)
		{
			// The sender is the BackgroundWorker object we need it to
			// report progress and check for cancellation.
			try {
				scannerData.scanner = Detector.Current.GetBestScanner(scannerData.folder);
			}
			catch (CancelException)
			{
				e.Cancel = true;
			}
		}

		private void ScannerWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
		{
			// the background process is complete. We need to inspect
			// our response to see if an error occurred, a cancel was
			// requested or if we completed successfully.

			scannerData.dialog.Close();

			// check to see if an error occurred in the background process.
			if (e.Cancelled)
			{
				scannerData = null;
				scannerWorker = null;
				return;
			}

			if (e.Error != null)
			{
				MessageBox.Show(e.Error.Message, "Error");
				scannerData = null;
				scannerWorker = null;
				return;
			}

			string name = scannerData.scanner.SuggestName(scannerData.folder);
			string type = scannerData.scanner.GetName();
            string uname = MakeUniqueName(name);
            try
            {
                Model.FoldersRow folder = model.AddFolder(uname, scannerData.folder, type);
                VisualizeFolder(folder);
                int row = table.TableModel.Rows.Count - 1;
                table.TableModel.Selections.SelectCells(row, 0, row, 2);
                table.EnsureVisible(row, 0);
            }
            catch (Exception ex)
            {
                MessageBox.Show(String.Format("Unable to add new folder: Folder:'{0}'\nType:'{1}'\nUniqueName:'{2}'\nError: {3}", name, type, uname, ex.Message), "Save failed", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
			scannerData = null;
			scannerWorker = null;
		}

		private void ScannerProgressChanged(object sender, ProgressChangedEventArgs e)
		{
			// This function fires on the UI thread so it's safe to edit
			// the UI control directly, no funny business with Control.Invoke.
			// Update the progressBar with the integer supplied to us from the
			// ReportProgress() function.  Note, e.UserState is a "tag" property
			// that can be used to send other information from the
			// BackgroundThread to the UI thread.

			BackgroundWorker worker = sender as BackgroundWorker;
			if (worker.CancellationPending) return;

			scannerData.counter++;
			if (scannerData.counter % 10 != 0) return;

			scannerData.dialog.UpdateAction(e.UserState as string);
		}

		private void AddProject(string folder)
		{
			// cancel previous worker if running
			if (scannerWorker != null)
			{
				ScannerCancel();
			}

			// create a background worker thread that ReportsProgress & SupportsCancellation
			// hook up the appropriate events.
			scannerWorker = new BackgroundWorker();
			scannerWorker.WorkerReportsProgress = true;
			scannerWorker.WorkerSupportsCancellation = true;
			scannerWorker.ProgressChanged += new ProgressChangedEventHandler(ScannerProgressChanged);
			scannerWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(ScannerWorkerCompleted);
			scannerWorker.DoWork += new DoWorkEventHandler(ScannerDoWork);
			Detector.Current.currentWorker = scannerWorker;
			scannerData = new ScannerData(folder, scannerWorker);
			scannerWorker.RunWorkerAsync();

			scannerData.dialog.ShowDialog();
		}

		private void ScannerCancel()
		{
			scannerWorker.CancelAsync();
			scannerWorker.ProgressChanged -= new ProgressChangedEventHandler(ScannerProgressChanged);
			scannerWorker.RunWorkerCompleted -= new RunWorkerCompletedEventHandler(ScannerWorkerCompleted);
		}

		private void buttonAdd_Click(object sender, EventArgs e)
		{
			FolderBrowserDialog browser = new FolderBrowserDialog();
			browser.Description = "Select web project folder";
			if (browser.ShowDialog() != DialogResult.OK) return;
			AddProject(browser.SelectedPath);
		}

		private void applyButton_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void cancelButton_Click(object sender, EventArgs e)
		{
			cancelled = true;
			Close();
		}

		private void table_DragDrop(object sender, DragEventArgs e)
		{
			if (!e.Data.GetDataPresent(DataFormats.FileDrop)) return;

			Array a = (Array)e.Data.GetData(DataFormats.FileDrop);
			if ( a == null ) return;

			foreach (object file in a)
			{
				string filename = file.ToString();
				if (Directory.Exists(filename)) AddProject(filename);
			}
		}

		private void table_DragEnter(object sender, DragEventArgs e)
		{
			e.Effect = DragDropEffects.None;
			if (!e.Data.GetDataPresent(DataFormats.FileDrop)) return;
			Array a = (Array)e.Data.GetData(DataFormats.FileDrop);
			if (a == null) return;
			foreach (object file in a)
			{
				string filename = file.ToString();
				if (Directory.Exists(filename)) 
				{
					e.Effect = DragDropEffects.Link;
					return;
				}
			}
		}

		private void advancedButton_Click(object sender, EventArgs e)
		{
			AdvancedSettings f = new AdvancedSettings(model);
			f.ShowDialog();
		}

		private void Configuration_Shown(object sender, EventArgs e)
		{
			cancelled = false;
		}

		private void Configuration_Activated(object sender, EventArgs e)
		{
			cancelled = false;
		}
	}
}