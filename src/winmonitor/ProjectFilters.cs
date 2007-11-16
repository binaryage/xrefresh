using System;
using System.Drawing;
using System.Windows.Forms;
using XPTable.Models;

namespace XRefresh
{
	public partial class ProjectFilters : Form
	{
		Model.FoldersRow folder;
		Model model;
		public FilterTable tableIncludes;
		public FilterTable tableExcludes;
		bool inImportMode = false;
		bool inDebugMode = true;
		bool editGlobalIncludes = false;
		bool editGlobalExcludes = false;

		public ProjectFilters(Model model, Model.FoldersRow folder)
		{
			this.model = model;
			this.folder = folder;
			// this must go after InitFrom !
			Model.SettingsRow settings = model.GetSettings();
			this.tableIncludes = new XRefresh.FilterTable(settings.ShowGlobalIncludes, false);
			this.tableExcludes = new XRefresh.FilterTable(settings.ShowGlobalExcludes, false);
			((System.ComponentModel.ISupportInitialize)(this.tableIncludes)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.tableExcludes)).BeginInit();
			InitializeComponent();
			// 
			// tableIncludes
			// 
			this.tableIncludes.AllowDrop = true;
			this.tableIncludes.AlternatingRowColor = System.Drawing.Color.WhiteSmoke;
			this.tableIncludes.CustomEditKey = System.Windows.Forms.Keys.Return;
			this.tableIncludes.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableIncludes.EnableHeaderContextMenu = false;
			this.tableIncludes.ForeColor = System.Drawing.Color.Black;
			this.tableIncludes.FullRowSelect = true;
			this.tableIncludes.GridLines = XPTable.Models.GridLines.Both;
			this.tableIncludes.GridLineStyle = XPTable.Models.GridLineStyle.Dot;
			this.tableIncludes.Location = new System.Drawing.Point(349, 25);
			this.tableIncludes.Name = "tableIncludes";
			this.tableIncludes.NoItemsText = "There are no file filters. Drag and drop a file here ...";
			this.tableIncludes.SelectionBackColor = System.Drawing.Color.Yellow;
			this.tableIncludes.SelectionForeColor = System.Drawing.Color.Black;
			this.tableIncludes.Size = new System.Drawing.Size(340, 140);
			this.tableIncludes.TabIndex = 0;
			this.tableIncludes.Text = "tableIncludes";
			// 
			// tableExcludes
			// 
			this.tableExcludes.AllowDrop = true;
			this.tableExcludes.AlternatingRowColor = System.Drawing.Color.WhiteSmoke;
			this.tableExcludes.CustomEditKey = System.Windows.Forms.Keys.Return;
			this.tableExcludes.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableExcludes.EnableHeaderContextMenu = false;
			this.tableExcludes.ForeColor = System.Drawing.Color.Black;
			this.tableExcludes.FullRowSelect = true;
			this.tableExcludes.GridLines = XPTable.Models.GridLines.Both;
			this.tableExcludes.GridLineStyle = XPTable.Models.GridLineStyle.Dot;
			this.tableExcludes.Location = new System.Drawing.Point(3, 25);
			this.tableExcludes.Name = "tableExcludes";
			this.tableExcludes.NoItemsText = "There are no file filters. Drag and drop a file here ...";
			this.tableExcludes.SelectionBackColor = System.Drawing.Color.Yellow;
			this.tableExcludes.SelectionForeColor = System.Drawing.Color.Black;
			this.tableExcludes.Size = new System.Drawing.Size(340, 140);
			this.tableExcludes.TabIndex = 3;
			this.tableExcludes.Text = "table2";

			((System.ComponentModel.ISupportInitialize)(this.tableIncludes)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.tableExcludes)).EndInit();

			this.tableLayoutPanel.Controls.Add(this.tableIncludes, 1, 1);
			this.tableLayoutPanel.Controls.Add(this.tableExcludes, 0, 1);
			HideDebugger();
			this.Text = folder.Name + " Filters Designer";

			editGlobalIncludes = checkBoxGlobalIncludes.Checked = settings.ShowGlobalIncludes;
			editGlobalExcludes = checkBoxGlobalExcludes.Checked = settings.ShowGlobalExcludes;
			this.checkBoxGlobalIncludes.CheckedChanged += new System.EventHandler(this.checkBoxGlobalIncludes_CheckedChanged);
			this.checkBoxGlobalExcludes.CheckedChanged += new System.EventHandler(this.checkBoxGlobalExcludes_CheckedChanged);

			InitFrom(folder);
			editTestPath.Text = folder.Path;
			tableIncludes.Changed += new FilterTable.ChangedHandler(tableIncludes_Changed);
			tableExcludes.Changed += new FilterTable.ChangedHandler(tableExcludes_Changed);
		}

		void tableIncludes_Changed(object sender, XPTable.Events.CellEventArgs e)
		{
			SaveTo(folder);
			tableTester.ReEval(folder);
		}

		void tableExcludes_Changed(object sender, XPTable.Events.CellEventArgs e)
		{
			SaveTo(folder);
			tableTester.ReEval(folder);
		}

		private void InitFrom(Model.FoldersRow folder)
		{
			tableIncludes.InitFrom(folder.GetIncludeFiltersRows(), editGlobalIncludes ? model.GlobalIncludeFilters : null);
			tableExcludes.InitFrom(folder.GetExcludeFiltersRows(), editGlobalExcludes ? model.GlobalExcludeFilters : null);
		}

		private void SaveTo(Model.FoldersRow folder)
		{
			tableIncludes.SaveToIncludes(folder);
			tableExcludes.SaveToExcludes(folder);
			if (editGlobalIncludes)
				tableIncludes.SaveToGlobalIncludes(model);
			if (editGlobalExcludes)
				tableExcludes.SaveToGlobalExcludes(model);
		}

		private void buttonOK_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void CustomType_FormClosing(object sender, FormClosingEventArgs e)
		{
			if (inImportMode)
			{
				// we are in import mode, cancel import mode before saving
				buttonCancelImport_Click(null, null);
			}
			SaveTo(folder);
			Model.SettingsRow settings = model.GetSettings();
			settings.ShowGlobalIncludes = checkBoxGlobalIncludes.Checked;
			settings.ShowGlobalExcludes = checkBoxGlobalExcludes.Checked;
			tableTester.CancelAll();
		}

		private void buttonImport_Click(object sender, EventArgs e)
		{
			EnterImportMode();
		}

		private void EnterImportMode()
		{
			SaveTo(folder);
			LockTables();
			FillInProjectsCombo();
			buttonImport.Visible = false;
			comboProjects.Visible = true;
			buttonDoImport.Visible = true;
			buttonCancelImport.Visible = true;
			buttonOK.Enabled = false;
			buttonRunTest.Enabled = false;
			editTestPath.Enabled = false;
			buttonBrowseTestPath.Enabled = false;
			comboProjects.Focus();
			inImportMode = true;
		}

		private void LeaveImportMode()
		{
			inImportMode = false;
			UnlockTables();
			buttonImport.Visible = true;
			comboProjects.Visible = false;
			buttonDoImport.Visible = false;
			buttonCancelImport.Visible = false;
			buttonOK.Enabled = true;
			buttonRunTest.Enabled = true;
			editTestPath.Enabled = true;
			buttonBrowseTestPath.Enabled = true;
			tableTester.ReEval(folder);
		}

		private void LockTable(XPTable.Models.Table table)
		{
			foreach (XPTable.Models.Row row in table.TableModel.Rows)
			{
				row.Editable = false;
			}
			table.BackColor = Color.LightGray;
			table.AlternatingRowColor = Color.LightGray;
			table.AllowSelection = false;
		}

		private void LockTables()
		{
			LockTable(tableIncludes);
			LockTable(tableExcludes);
			LockTable(tableTester);
		}

		private void UnlockTable(XPTable.Models.Table table)
		{
			foreach (XPTable.Models.Row row in table.TableModel.Rows)
			{
				row.Editable = true;
			}
			table.BackColor = Color.White;
			table.AlternatingRowColor = Color.WhiteSmoke;
			table.AllowSelection = true;
		}

		private void UnlockTables()
		{
			UnlockTable(tableIncludes);
			UnlockTable(tableExcludes);
			UnlockTable(tableTester);
		}

		private void FillInProjectsCombo()
		{
			comboProjects.Items.Clear();
			foreach (Model.FoldersRow folder in model.Folders)
			{
				comboProjects.Items.Add(folder);
			}
			comboProjects.SelectedItem = this.folder;
		}

		private void buttonCancelImport_Click(object sender, EventArgs e)
		{
			InitFrom(folder);
			LockTables();
			LeaveImportMode();
		}

		private void buttonDoImport_Click(object sender, EventArgs e)
		{
			LeaveImportMode();
		}

		private void comboProjects_SelectedIndexChanged(object sender, EventArgs e)
		{
			Model.FoldersRow selectedFolder = comboProjects.SelectedItem as Model.FoldersRow;
			InitFrom(selectedFolder);
			LockTables();
		}

		private void checkBoxGlobalIncludes_CheckedChanged(object sender, EventArgs e)
		{
			tableIncludes.ToggleDualMode();
			RefreshView();
		}

		private void checkBoxGlobalExcludes_CheckedChanged(object sender, EventArgs e)
		{
			tableExcludes.ToggleDualMode();
			RefreshView();
		}

		private void RefreshView()
		{
			this.SuspendLayout();

			bool wasInImportMode = inImportMode;
			int importIndex = -1;
			if (inImportMode)
			{
				importIndex = comboProjects.SelectedIndex;
				buttonCancelImport_Click(null, null);
			}

			SaveTo(folder);
			editGlobalIncludes = checkBoxGlobalIncludes.Checked;
			editGlobalExcludes = checkBoxGlobalExcludes.Checked;
			tableIncludes.SetupTable();
			tableExcludes.SetupTable();
			InitFrom(folder);

			if (wasInImportMode)
			{
				EnterImportMode();
				comboProjects.SelectedIndex = importIndex;
				//comboProjects_SelectedIndexChanged(null, null);
			}

			this.ResumeLayout();
		}

		private void buttonTesterToggle_Click(object sender, EventArgs e)
		{
			if (inDebugMode) HideDebugger(); else ShowDebugger();
		}

		private void buttonRunTest_Click(object sender, EventArgs e)
		{
			SaveTo(folder);
			tableTester.RunTest(editTestPath.Text, folder);
		}

		private void buttonBrowseTestPath_Click(object sender, EventArgs e)
		{
			FolderBrowserDialog browser = new FolderBrowserDialog();
			browser.Description = "Select test project folder";
			if (browser.ShowDialog() != DialogResult.OK) return;
			editTestPath.Text = browser.SelectedPath;
		}

		private void ShowDebugger()
		{
			if (inDebugMode) return;
			buttonDebugger.Text = "Hide Debugger";
			tableLayoutPanel.RowStyles[3].Height = 50;
			tableLayoutPanel.RowStyles[1].Height = 50;
			buttonRunTest.Visible = true;
			editTestPath.Visible = true;
			buttonBrowseTestPath.Visible = true;
			inDebugMode = true;
		}

		private void HideDebugger()
		{
			if (!inDebugMode) return;
			buttonDebugger.Text = "Show Debugger";
			tableLayoutPanel.RowStyles[3].Height = 0;
			tableLayoutPanel.RowStyles[1].Height = 100;
			buttonRunTest.Visible = false;
			editTestPath.Visible = false;
			buttonBrowseTestPath.Visible = false;
			inDebugMode = false;
		}

		private void tableTester_MouseClick(object sender, MouseEventArgs e)
		{
			if (e.Button != MouseButtons.Right) return;

			int rowIndex = tableTester.RowIndexAt(e.X, e.Y);
			if (rowIndex == -1) return;

			Row row = tableTester.TableModel.Rows[rowIndex];
			string path = row.Cells[0].Text;
			string reason = row.Cells[1].Text;
			MatchReason.Status status = (MatchReason.Status)row.Cells[0].Tag;

			FilterMenu menu = new FilterMenu(this, path, reason, status);
			menu.Show(tableTester, new Point(e.X, e.Y));
		}

		private void export_Click(object sender, EventArgs e)
		{

		}
	}
}