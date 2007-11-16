using System;
using System.ComponentModel;
using System.Drawing;
using System.IO;
using System.Threading;
using System.Windows.Forms;
using XPTable.Models;

namespace XRefresh
{
    public partial class FilterTester : Table
    {
        public FilterTester()
        {
            InitializeComponent();
            SetupTable();
        }

        public void SetupTable()
        {
            SuspendLayout();

            Table table = this;

            // table
            table.AlternatingRowColor = System.Drawing.Color.WhiteSmoke;
            table.ForeColor = System.Drawing.Color.Black;
            table.BackColor = System.Drawing.Color.White;
            table.ColumnModel = columnModel;
            table.EnableHeaderContextMenu = false;
            table.SelectionBackColor = Color.Yellow;
            table.SelectionForeColor = Color.Black;
            table.FullRowSelect = true;
            tableModel.RowHeight = 18;
            table.Name = "FilterTester";
            table.NoItemsText = "There are no files. Click on 'Run Test' button ...";
            table.TabIndex = 1;
            table.TableModel = tableModel;
            table.Text = "FilterTester";

            // columnModel
            columnModel.Columns.AddRange(new XPTable.Models.Column[] {
            fileColumn,
            reasonColumn});

            // maskColumn
            fileColumn.Text = "File Name";
            fileColumn.Width = 520;
            fileColumn.Renderer = new GDIImageCellRenderer();
            fileColumn.Editor = new GDIImageCellEditor();
            fileColumn.Editable = false;

            // infoColumn
            reasonColumn.Text = "Reason";
            reasonColumn.Width = 150;
            reasonColumn.Renderer = new GDITextCellRenderer();
            reasonColumn.Editor = new InfoGDITextCellEditor();
            reasonColumn.Editable = false;

            ResumeLayout(true);
        }

        private void StyleRow(Row row, MatchReason reason)
        {
            Cell cell1 = row.Cells[0];
            cell1.Tag = reason.status;
            Cell cell2 = row.Cells[1];
            cell2.Text = reason.text;
			cell1.ForeColor = Color.Black;
			if (reason.status == MatchReason.Status.Excluded) cell1.ForeColor = Color.FromArgb(255, 0, 0);
			if (reason.status == MatchReason.Status.Included) cell1.ForeColor = Color.FromArgb(180, 130, 0);
			FontStyle style = FontStyle.Regular;
			if (reason.status == MatchReason.Status.Included) style = FontStyle.Bold;
			if (reason.status == MatchReason.Status.Excluded) style = FontStyle.Strikeout;
            cell1.CellStyle.Font = new System.Drawing.Font("Courier", 8.00F, style, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            cell2.CellStyle.Font = new System.Drawing.Font("Arial", 8.25F, FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
        }

        private void VisualizeFile(string filename, string fullpath, MatchReason reason)
        {
            Row row = new Row();

            Cell cell1 = new Cell();
            CellStyle cellStyle1 = new CellStyle();
            Cell cell2 = new Cell();
            CellStyle cellStyle2 = new CellStyle();

            cell1.Text = filename;
            cell1.Icon = GetIconForFile(fullpath);
            cell1.Image = Properties.Resources.Information; // HACK: fake bitmap with same size as icon

            cell1.CellStyle = cellStyle1;
            cell2.CellStyle = cellStyle2;

            row.Cells.AddRange(new XPTable.Models.Cell[] { cell1, cell2 });
            row.ChildIndex = 0;
            row.Editable = false;

            StyleRow(row, reason);

			lock (tableModel)
			{
				tableModel.Rows.Add(row);
			}
        }

        protected Icon GetIconForFile(string fullpath)
        {
            return ShellIcon.GetSmallIcon(fullpath);
        }

        // The BackgroundWorker will be used to perform a long running action
        // on a background thread.  This allows the UI to be free for painting
        // as well as other actions the user may want to perform.  The background
        // thread will use the ReportProgress event to update the ProgressBar
        // on the UI thread.
        private BackgroundWorker worker;
		private Object monitor = new Object();

        class WorkerData
        {
            public string path;
            public Model.FoldersRow folder;
            public int len;
            public Model model;
			public BackgroundWorker worker;

			public WorkerData(string path, Model.FoldersRow folder, BackgroundWorker worker)
            {
                this.path = path.TrimEnd(Path.DirectorySeparatorChar);
                this.folder = folder;
                this.len = this.path.Length;
                this.model = folder.Table.DataSet as Model;
				this.worker = worker;
            }
        }

        class ReportInfo 
        {
            public string filename;
            public string fullpath;
            public MatchReason reason;

            public ReportInfo(string filename, string fullpath, MatchReason reason)
            {
                this.filename = filename;
                this.reason = reason.Clone();
                this.fullpath = fullpath;
            }
        }

        private string ChopFilename(String path, int index)
        {
            if (path.Length <= index) return "?";
            return Utils.NormalizePath(path.Substring(index));
        }

        private void TesterFileEnumerator(String path, MatchReason reason, WorkerData data)
        {
			if (data.worker.CancellationPending) throw new CancelException();
            
            // first look into directories
            String[] dirs = Directory.GetDirectories(path);
            foreach (String dir in dirs)
            {
				if (data.worker.CancellationPending) throw new CancelException();
                string filename = ChopFilename(dir, data.len);

                // optimization, directory must pass global exclude filters
                // this is here mainly to not traverse .svn subdirectories
                reason.Reset();
				if (data.model.PassesGlobalFilters(filename, reason))
                {
                    TesterFileEnumerator(dir, reason, data);
                }
                else
                {
					data.worker.ReportProgress(0, new ReportInfo(filename, dir, reason));
                }
                Thread.Sleep(10); // don't hung the UI thread
            }
            
            String[] files = Directory.GetFiles(path);
            // next look for files
            foreach (String file in files)
            {
				if (data.worker.CancellationPending) throw new CancelException();
				string filename = ChopFilename(file, data.len);
                reason.Reset();
				lock (data.folder)
				{
					try
					{
						if (data.folder.PassesFilters(filename, reason))
						{
							data.worker.ReportProgress(0, new ReportInfo(filename, file, reason));
						}
						else
						{
							data.worker.ReportProgress(0, new ReportInfo(filename, file, reason));
						}
					}
					catch (Exception)
					{
						// hack
					}
				}
				Thread.Sleep(10); // don't hung the UI thread
			}
        }

        private void TesterDoWork(object sender, DoWorkEventArgs e)
        {
			lock (monitor)
			{
				WorkerData data = (WorkerData)e.Argument;
				data.worker.ReportProgress(-1, null);

				// the sender is the BackgroundWorker object we need it to
				// report progress and check for cancellation.
				try
				{
					MatchReason reason = new MatchReason();
					TesterFileEnumerator(data.path, reason, data);
				}
				catch (CancelException)
				{
					e.Cancel = true;
				}
			}
        }

        private void TesterWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            // the background process is complete. We need to inspect
            // our response to see if an error occurred, a cancel was
            // requested or if we completed successfully.

            // check to see if an error occurred in the background process.
            if (e.Error != null)
            {
                MessageBox.Show(e.Error.Message, "Error");
            }
        }

        private void TesterProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            // This function fires on the UI thread so it's safe to edit
            // the UI control directly, no funny business with Control.Invoke.
            // Update the progressBar with the integer supplied to us from the
            // ReportProgress() function.  Note, e.UserState is a "tag" property
            // that can be used to send other information from the
            // BackgroundThread to the UI thread.

			if (e.ProgressPercentage == -1)
			{
				tableModel.Rows.Clear();
				return;
			}

            ReportInfo info = e.UserState as ReportInfo;
            VisualizeFile(info.filename, info.fullpath, info.reason);
        }

        public void RunTest(string path, Model.FoldersRow folder)
        {
            // cancel previous worker if running
            if (worker != null)
            {
                worker.CancelAsync();
				Thread.Sleep(100);
            }

            // create a background worker thread that ReportsProgress & SupportsCancellation
            // hook up the appropriate events.
			worker = new BackgroundWorker();
            worker.WorkerReportsProgress = true;
            worker.WorkerSupportsCancellation = true;
            worker.ProgressChanged += new ProgressChangedEventHandler(TesterProgressChanged);
            worker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(TesterWorkerCompleted);
            worker.DoWork += new DoWorkEventHandler(TesterDoWork);
			worker.RunWorkerAsync(new WorkerData(path, folder, worker));
        }

		// The BackgroundWorker will be used to perform a long running action
		// on a background thread.  This allows the UI to be free for painting
		// as well as other actions the user may want to perform.  The background
		// thread will use the ReportProgress event to update the ProgressBar
		// on the UI thread.
		private BackgroundWorker reEvalWorker;
		private Object monitor2 = new Object();

		public void ReEval(Model.FoldersRow folder)
        {
			if (reEvalWorker != null)
			{
				reEvalWorker.CancelAsync();
				Thread.Sleep(100);
			}

			reEvalWorker = new BackgroundWorker();
			reEvalWorker.WorkerReportsProgress = false;
			reEvalWorker.WorkerSupportsCancellation = true;
			reEvalWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(ReEvalWorkerCompleted);
			reEvalWorker.DoWork += new DoWorkEventHandler(ReEvalDoWork);
			reEvalWorker.RunWorkerAsync(new WorkerData("", folder, reEvalWorker));
        }

		private void ReEvalDoWork(object sender, DoWorkEventArgs e)
		{
			lock (monitor2)
			{
				WorkerData data = (WorkerData)e.Argument;
				int count = tableModel.Rows.Count;
				for (int i = 0; i < count; i++)
				{
					if (data.worker.CancellationPending) throw new CancelException();
					lock (tableModel)
					{
						Row row = tableModel.Rows[i];
						if (row != null)
						{
							string path = row.Cells[0].Text;
							MatchReason reason = new MatchReason();
							lock (data.folder)
							{
								try
								{
									data.folder.PassesFilters(path, reason);
								}
								catch (Exception)
								{
									// hack
								}
							}
							StyleRow(row, reason);
						}
					}
				}
			}
		}

		private void ReEvalWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
		{
		}

		internal void CancelAll()
		{
			// cancel previous worker if running
			if (worker != null)
			{
				worker.CancelAsync();
				Thread.Sleep(100);
			}

			if (reEvalWorker != null)
			{
				reEvalWorker.CancelAsync();
				Thread.Sleep(100);
			}
		}
	}
}