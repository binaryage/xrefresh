namespace XRefresh
{
    partial class ProjectFilters
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProjectFilters));
			this.tableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
			this.tableLayoutPanel4 = new System.Windows.Forms.TableLayoutPanel();
			this.panel2 = new System.Windows.Forms.Panel();
			this.buttonBrowseTestPath = new System.Windows.Forms.Button();
			this.buttonDebugger = new System.Windows.Forms.Button();
			this.buttonRunTest = new System.Windows.Forms.Button();
			this.editTestPath = new System.Windows.Forms.TextBox();
			this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
			this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
			this.buttonImport = new System.Windows.Forms.Button();
			this.comboProjects = new System.Windows.Forms.ComboBox();
			this.buttonDoImport = new System.Windows.Forms.Button();
			this.buttonCancelImport = new System.Windows.Forms.Button();
			this.panel1 = new System.Windows.Forms.Panel();
			this.buttonOK = new System.Windows.Forms.Button();
			this.tableLayoutPanelTopLeft = new System.Windows.Forms.TableLayoutPanel();
			this.label1 = new System.Windows.Forms.Label();
			this.checkBoxGlobalIncludes = new System.Windows.Forms.CheckBox();
			this.tableLayoutPanel2 = new System.Windows.Forms.TableLayoutPanel();
			this.checkBoxGlobalExcludes = new System.Windows.Forms.CheckBox();
			this.label2 = new System.Windows.Forms.Label();
			this.tableTester = new XRefresh.FilterTester();
			this.comboBox1 = new System.Windows.Forms.ComboBox();
			this.tableLayoutPanel3 = new System.Windows.Forms.TableLayoutPanel();
			this.button1 = new System.Windows.Forms.Button();
			this.flowLayoutPanel2 = new System.Windows.Forms.FlowLayoutPanel();
			this.button2 = new System.Windows.Forms.Button();
			this.comboBox2 = new System.Windows.Forms.ComboBox();
			this.button3 = new System.Windows.Forms.Button();
			this.button4 = new System.Windows.Forms.Button();
			this.toolTip = new System.Windows.Forms.ToolTip(this.components);
			this.tableLayoutPanel.SuspendLayout();
			this.tableLayoutPanel4.SuspendLayout();
			this.panel2.SuspendLayout();
			this.tableLayoutPanel1.SuspendLayout();
			this.flowLayoutPanel1.SuspendLayout();
			this.panel1.SuspendLayout();
			this.tableLayoutPanelTopLeft.SuspendLayout();
			this.tableLayoutPanel2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.tableTester)).BeginInit();
			this.tableLayoutPanel3.SuspendLayout();
			this.flowLayoutPanel2.SuspendLayout();
			this.SuspendLayout();
			// 
			// tableLayoutPanel
			// 
			this.tableLayoutPanel.ColumnCount = 2;
			this.tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel.Controls.Add(this.tableLayoutPanel4, 0, 2);
			this.tableLayoutPanel.Controls.Add(this.tableLayoutPanel1, 0, 4);
			this.tableLayoutPanel.Controls.Add(this.tableLayoutPanelTopLeft, 1, 0);
			this.tableLayoutPanel.Controls.Add(this.tableLayoutPanel2, 0, 0);
			this.tableLayoutPanel.Controls.Add(this.tableTester, 0, 3);
			this.tableLayoutPanel.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel.Location = new System.Drawing.Point(0, 0);
			this.tableLayoutPanel.Name = "tableLayoutPanel";
			this.tableLayoutPanel.RowCount = 5;
			this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 22F));
			this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 32F));
			this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 32F));
			this.tableLayoutPanel.Size = new System.Drawing.Size(692, 374);
			this.tableLayoutPanel.TabIndex = 5;
			// 
			// tableLayoutPanel4
			// 
			this.tableLayoutPanel4.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.tableLayoutPanel4.ColumnCount = 2;
			this.tableLayoutPanel.SetColumnSpan(this.tableLayoutPanel4, 2);
			this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 300F));
			this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20F));
			this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20F));
			this.tableLayoutPanel4.Controls.Add(this.panel2, 1, 0);
			this.tableLayoutPanel4.Controls.Add(this.editTestPath, 0, 0);
			this.tableLayoutPanel4.Location = new System.Drawing.Point(0, 166);
			this.tableLayoutPanel4.Margin = new System.Windows.Forms.Padding(0);
			this.tableLayoutPanel4.Name = "tableLayoutPanel4";
			this.tableLayoutPanel4.RowCount = 1;
			this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel4.Size = new System.Drawing.Size(692, 32);
			this.tableLayoutPanel4.TabIndex = 7;
			// 
			// panel2
			// 
			this.panel2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.panel2.Controls.Add(this.buttonBrowseTestPath);
			this.panel2.Controls.Add(this.buttonDebugger);
			this.panel2.Controls.Add(this.buttonRunTest);
			this.panel2.Location = new System.Drawing.Point(392, 3);
			this.panel2.Margin = new System.Windows.Forms.Padding(0, 3, 3, 3);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(297, 26);
			this.panel2.TabIndex = 10;
			// 
			// buttonBrowseTestPath
			// 
			this.buttonBrowseTestPath.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.buttonBrowseTestPath.Location = new System.Drawing.Point(-1, 6);
			this.buttonBrowseTestPath.Margin = new System.Windows.Forms.Padding(0, 3, 3, 3);
			this.buttonBrowseTestPath.Name = "buttonBrowseTestPath";
			this.buttonBrowseTestPath.Size = new System.Drawing.Size(24, 21);
			this.buttonBrowseTestPath.TabIndex = 17;
			this.buttonBrowseTestPath.Text = "...";
			this.buttonBrowseTestPath.UseVisualStyleBackColor = true;
			this.buttonBrowseTestPath.Click += new System.EventHandler(this.buttonBrowseTestPath_Click);
			// 
			// buttonDebugger
			// 
			this.buttonDebugger.Anchor = System.Windows.Forms.AnchorStyles.None;
			this.buttonDebugger.Image = global::XRefresh.Properties.Resources.Bug;
			this.buttonDebugger.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.buttonDebugger.Location = new System.Drawing.Point(188, 1);
			this.buttonDebugger.Name = "buttonDebugger";
			this.buttonDebugger.Size = new System.Drawing.Size(109, 25);
			this.buttonDebugger.TabIndex = 7;
			this.buttonDebugger.Text = "Show Debugger";
			this.buttonDebugger.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.buttonDebugger.UseVisualStyleBackColor = true;
			this.buttonDebugger.Click += new System.EventHandler(this.buttonTesterToggle_Click);
			// 
			// buttonRunTest
			// 
			this.buttonRunTest.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.buttonRunTest.Image = global::XRefresh.Properties.Resources.Cog;
			this.buttonRunTest.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.buttonRunTest.Location = new System.Drawing.Point(25, 2);
			this.buttonRunTest.Name = "buttonRunTest";
			this.buttonRunTest.Size = new System.Drawing.Size(78, 25);
			this.buttonRunTest.TabIndex = 15;
			this.buttonRunTest.Text = "Run Test";
			this.buttonRunTest.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.buttonRunTest.UseVisualStyleBackColor = true;
			this.buttonRunTest.Click += new System.EventHandler(this.buttonRunTest_Click);
			// 
			// editTestPath
			// 
			this.editTestPath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.editTestPath.Location = new System.Drawing.Point(3, 9);
			this.editTestPath.Margin = new System.Windows.Forms.Padding(3, 3, 0, 3);
			this.editTestPath.Name = "editTestPath";
			this.editTestPath.Size = new System.Drawing.Size(389, 20);
			this.editTestPath.TabIndex = 11;
			// 
			// tableLayoutPanel1
			// 
			this.tableLayoutPanel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.tableLayoutPanel1.ColumnCount = 2;
			this.tableLayoutPanel.SetColumnSpan(this.tableLayoutPanel1, 2);
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 160F));
			this.tableLayoutPanel1.Controls.Add(this.flowLayoutPanel1, 0, 0);
			this.tableLayoutPanel1.Controls.Add(this.panel1, 1, 0);
			this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 342);
			this.tableLayoutPanel1.Margin = new System.Windows.Forms.Padding(0);
			this.tableLayoutPanel1.Name = "tableLayoutPanel1";
			this.tableLayoutPanel1.RowCount = 1;
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 32F));
			this.tableLayoutPanel1.Size = new System.Drawing.Size(692, 32);
			this.tableLayoutPanel1.TabIndex = 4;
			// 
			// flowLayoutPanel1
			// 
			this.flowLayoutPanel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.flowLayoutPanel1.Controls.Add(this.buttonImport);
			this.flowLayoutPanel1.Controls.Add(this.comboProjects);
			this.flowLayoutPanel1.Controls.Add(this.buttonDoImport);
			this.flowLayoutPanel1.Controls.Add(this.buttonCancelImport);
			this.flowLayoutPanel1.Location = new System.Drawing.Point(0, 0);
			this.flowLayoutPanel1.Margin = new System.Windows.Forms.Padding(0);
			this.flowLayoutPanel1.Name = "flowLayoutPanel1";
			this.flowLayoutPanel1.Size = new System.Drawing.Size(532, 32);
			this.flowLayoutPanel1.TabIndex = 8;
			// 
			// buttonImport
			// 
			this.buttonImport.Image = global::XRefresh.Properties.Resources.Renamed;
			this.buttonImport.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.buttonImport.Location = new System.Drawing.Point(3, 3);
			this.buttonImport.Name = "buttonImport";
			this.buttonImport.Size = new System.Drawing.Size(130, 23);
			this.buttonImport.TabIndex = 11;
			this.buttonImport.Text = "Import from Project ...";
			this.buttonImport.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.buttonImport.UseVisualStyleBackColor = true;
			this.buttonImport.Click += new System.EventHandler(this.buttonImport_Click);
			// 
			// comboProjects
			// 
			this.comboProjects.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboProjects.FormattingEnabled = true;
			this.comboProjects.ItemHeight = 13;
			this.comboProjects.Location = new System.Drawing.Point(139, 3);
			this.comboProjects.Name = "comboProjects";
			this.comboProjects.Size = new System.Drawing.Size(140, 21);
			this.comboProjects.Sorted = true;
			this.comboProjects.TabIndex = 10;
			this.comboProjects.Visible = false;
			this.comboProjects.SelectedIndexChanged += new System.EventHandler(this.comboProjects_SelectedIndexChanged);
			// 
			// buttonDoImport
			// 
			this.buttonDoImport.Image = global::XRefresh.Properties.Resources.Renamed;
			this.buttonDoImport.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.buttonDoImport.Location = new System.Drawing.Point(285, 3);
			this.buttonDoImport.Name = "buttonDoImport";
			this.buttonDoImport.Size = new System.Drawing.Size(57, 23);
			this.buttonDoImport.TabIndex = 12;
			this.buttonDoImport.Text = "Import";
			this.buttonDoImport.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.buttonDoImport.UseVisualStyleBackColor = true;
			this.buttonDoImport.Visible = false;
			this.buttonDoImport.Click += new System.EventHandler(this.buttonDoImport_Click);
			// 
			// buttonCancelImport
			// 
			this.buttonCancelImport.Image = global::XRefresh.Properties.Resources.Deleted;
			this.buttonCancelImport.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.buttonCancelImport.Location = new System.Drawing.Point(348, 3);
			this.buttonCancelImport.Name = "buttonCancelImport";
			this.buttonCancelImport.Size = new System.Drawing.Size(62, 23);
			this.buttonCancelImport.TabIndex = 13;
			this.buttonCancelImport.Text = "Cancel";
			this.buttonCancelImport.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.buttonCancelImport.UseVisualStyleBackColor = true;
			this.buttonCancelImport.Visible = false;
			this.buttonCancelImport.Click += new System.EventHandler(this.buttonCancelImport_Click);
			// 
			// panel1
			// 
			this.panel1.Controls.Add(this.buttonOK);
			this.panel1.Location = new System.Drawing.Point(535, 3);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(154, 26);
			this.panel1.TabIndex = 9;
			// 
			// buttonOK
			// 
			this.buttonOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.buttonOK.Image = global::XRefresh.Properties.Resources.Accept;
			this.buttonOK.Location = new System.Drawing.Point(81, 1);
			this.buttonOK.Name = "buttonOK";
			this.buttonOK.Size = new System.Drawing.Size(74, 25);
			this.buttonOK.TabIndex = 8;
			this.buttonOK.Text = "Done";
			this.buttonOK.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.buttonOK.UseVisualStyleBackColor = true;
			this.buttonOK.Click += new System.EventHandler(this.buttonOK_Click);
			// 
			// tableLayoutPanelTopLeft
			// 
			this.tableLayoutPanelTopLeft.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.tableLayoutPanelTopLeft.ColumnCount = 2;
			this.tableLayoutPanelTopLeft.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanelTopLeft.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanelTopLeft.Controls.Add(this.label1, 0, 0);
			this.tableLayoutPanelTopLeft.Controls.Add(this.checkBoxGlobalIncludes, 1, 0);
			this.tableLayoutPanelTopLeft.Location = new System.Drawing.Point(346, 0);
			this.tableLayoutPanelTopLeft.Margin = new System.Windows.Forms.Padding(0);
			this.tableLayoutPanelTopLeft.Name = "tableLayoutPanelTopLeft";
			this.tableLayoutPanelTopLeft.RowCount = 1;
			this.tableLayoutPanelTopLeft.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanelTopLeft.Size = new System.Drawing.Size(346, 22);
			this.tableLayoutPanelTopLeft.TabIndex = 5;
			// 
			// label1
			// 
			this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
			this.label1.Image = global::XRefresh.Properties.Resources.Include;
			this.label1.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.label1.Location = new System.Drawing.Point(3, 7);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(101, 15);
			this.label1.TabIndex = 2;
			this.label1.Text = "Include List:";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.toolTip.SetToolTip(this.label1, "List here all files you explicitly want to be reported. This is stronger than ign" +
					"ore list.");
			// 
			// checkBoxGlobalIncludes
			// 
			this.checkBoxGlobalIncludes.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.checkBoxGlobalIncludes.AutoSize = true;
			this.checkBoxGlobalIncludes.Font = new System.Drawing.Font("Microsoft Sans Serif", 7F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
			this.checkBoxGlobalIncludes.Location = new System.Drawing.Point(252, 7);
			this.checkBoxGlobalIncludes.Margin = new System.Windows.Forms.Padding(3, 7, 3, 0);
			this.checkBoxGlobalIncludes.Name = "checkBoxGlobalIncludes";
			this.checkBoxGlobalIncludes.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
			this.checkBoxGlobalIncludes.Size = new System.Drawing.Size(91, 15);
			this.checkBoxGlobalIncludes.TabIndex = 3;
			this.checkBoxGlobalIncludes.Text = "Show Globals";
			this.checkBoxGlobalIncludes.UseVisualStyleBackColor = true;
			// 
			// tableLayoutPanel2
			// 
			this.tableLayoutPanel2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.tableLayoutPanel2.ColumnCount = 2;
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel2.Controls.Add(this.checkBoxGlobalExcludes, 0, 0);
			this.tableLayoutPanel2.Controls.Add(this.label2, 0, 0);
			this.tableLayoutPanel2.Location = new System.Drawing.Point(0, 0);
			this.tableLayoutPanel2.Margin = new System.Windows.Forms.Padding(0);
			this.tableLayoutPanel2.Name = "tableLayoutPanel2";
			this.tableLayoutPanel2.RowCount = 1;
			this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel2.Size = new System.Drawing.Size(346, 22);
			this.tableLayoutPanel2.TabIndex = 6;
			// 
			// checkBoxGlobalExcludes
			// 
			this.checkBoxGlobalExcludes.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.checkBoxGlobalExcludes.AutoSize = true;
			this.checkBoxGlobalExcludes.Font = new System.Drawing.Font("Microsoft Sans Serif", 7F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
			this.checkBoxGlobalExcludes.Location = new System.Drawing.Point(252, 7);
			this.checkBoxGlobalExcludes.Margin = new System.Windows.Forms.Padding(3, 7, 3, 0);
			this.checkBoxGlobalExcludes.Name = "checkBoxGlobalExcludes";
			this.checkBoxGlobalExcludes.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
			this.checkBoxGlobalExcludes.Size = new System.Drawing.Size(91, 15);
			this.checkBoxGlobalExcludes.TabIndex = 4;
			this.checkBoxGlobalExcludes.Text = "Show Globals";
			this.checkBoxGlobalExcludes.UseVisualStyleBackColor = true;
			// 
			// label2
			// 
			this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
			this.label2.Image = global::XRefresh.Properties.Resources.Ignore;
			this.label2.ImageAlign = System.Drawing.ContentAlignment.TopLeft;
			this.label2.Location = new System.Drawing.Point(3, 7);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(94, 15);
			this.label2.TabIndex = 3;
			this.label2.Text = "Ignore List:";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.toolTip.SetToolTip(this.label2, "List here all files that you don\'t want to be reported\r\n.");
			// 
			// tableTester
			// 
			this.tableTester.AlternatingRowColor = System.Drawing.Color.WhiteSmoke;
			this.tableTester.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.tableLayoutPanel.SetColumnSpan(this.tableTester, 2);
			this.tableTester.CustomEditKey = System.Windows.Forms.Keys.Return;
			this.tableTester.EnableHeaderContextMenu = false;
			this.tableTester.ForeColor = System.Drawing.Color.Black;
			this.tableTester.FullRowSelect = true;
			this.tableTester.Location = new System.Drawing.Point(3, 201);
			this.tableTester.Name = "tableTester";
			this.tableTester.NoItemsText = "There are no files. Click on \'Run Test\' button ...";
			this.tableTester.SelectionBackColor = System.Drawing.Color.Yellow;
			this.tableTester.SelectionForeColor = System.Drawing.Color.Black;
			this.tableTester.Size = new System.Drawing.Size(686, 138);
			this.tableTester.TabIndex = 8;
			this.tableTester.Text = "table1";
			this.tableTester.MouseClick += new System.Windows.Forms.MouseEventHandler(this.tableTester_MouseClick);
			// 
			// comboBox1
			// 
			this.comboBox1.FormattingEnabled = true;
			this.comboBox1.Location = new System.Drawing.Point(3, 3);
			this.comboBox1.Name = "comboBox1";
			this.comboBox1.Size = new System.Drawing.Size(121, 21);
			this.comboBox1.TabIndex = 0;
			// 
			// tableLayoutPanel3
			// 
			this.tableLayoutPanel3.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.tableLayoutPanel3.ColumnCount = 2;
			this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 80F));
			this.tableLayoutPanel3.Controls.Add(this.button1, 1, 0);
			this.tableLayoutPanel3.Location = new System.Drawing.Point(0, 0);
			this.tableLayoutPanel3.Margin = new System.Windows.Forms.Padding(0);
			this.tableLayoutPanel3.Name = "tableLayoutPanel3";
			this.tableLayoutPanel3.RowCount = 1;
			this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
			this.tableLayoutPanel3.Size = new System.Drawing.Size(200, 100);
			this.tableLayoutPanel3.TabIndex = 0;
			// 
			// button1
			// 
			this.button1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.button1.Location = new System.Drawing.Point(123, 3);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(74, 23);
			this.button1.TabIndex = 7;
			this.button1.Text = "OK";
			this.button1.UseVisualStyleBackColor = true;
			// 
			// flowLayoutPanel2
			// 
			this.flowLayoutPanel2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.flowLayoutPanel2.Controls.Add(this.button2);
			this.flowLayoutPanel2.Controls.Add(this.comboBox2);
			this.flowLayoutPanel2.Controls.Add(this.button3);
			this.flowLayoutPanel2.Controls.Add(this.button4);
			this.flowLayoutPanel2.Location = new System.Drawing.Point(0, 0);
			this.flowLayoutPanel2.Margin = new System.Windows.Forms.Padding(0);
			this.flowLayoutPanel2.Name = "flowLayoutPanel2";
			this.flowLayoutPanel2.Size = new System.Drawing.Size(216, 30);
			this.flowLayoutPanel2.TabIndex = 8;
			// 
			// button2
			// 
			this.button2.Image = global::XRefresh.Properties.Resources.Renamed;
			this.button2.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.button2.Location = new System.Drawing.Point(3, 3);
			this.button2.Name = "button2";
			this.button2.Size = new System.Drawing.Size(130, 23);
			this.button2.TabIndex = 11;
			this.button2.Text = "Import from Project ...";
			this.button2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.button2.UseVisualStyleBackColor = true;
			// 
			// comboBox2
			// 
			this.comboBox2.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBox2.FormattingEnabled = true;
			this.comboBox2.ItemHeight = 13;
			this.comboBox2.Location = new System.Drawing.Point(3, 32);
			this.comboBox2.Name = "comboBox2";
			this.comboBox2.Size = new System.Drawing.Size(140, 21);
			this.comboBox2.Sorted = true;
			this.comboBox2.TabIndex = 10;
			this.comboBox2.Visible = false;
			// 
			// button3
			// 
			this.button3.Image = global::XRefresh.Properties.Resources.Renamed;
			this.button3.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.button3.Location = new System.Drawing.Point(149, 32);
			this.button3.Name = "button3";
			this.button3.Size = new System.Drawing.Size(57, 23);
			this.button3.TabIndex = 12;
			this.button3.Text = "Import";
			this.button3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.button3.UseVisualStyleBackColor = true;
			this.button3.Visible = false;
			// 
			// button4
			// 
			this.button4.Image = global::XRefresh.Properties.Resources.Deleted;
			this.button4.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.button4.Location = new System.Drawing.Point(3, 61);
			this.button4.Name = "button4";
			this.button4.Size = new System.Drawing.Size(62, 23);
			this.button4.TabIndex = 13;
			this.button4.Text = "Cancel";
			this.button4.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.button4.UseVisualStyleBackColor = true;
			this.button4.Visible = false;
			// 
			// ProjectFilters
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(692, 374);
			this.Controls.Add(this.tableLayoutPanel);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MinimizeBox = false;
			this.MinimumSize = new System.Drawing.Size(500, 400);
			this.Name = "ProjectFilters";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.CustomType_FormClosing);
			this.tableLayoutPanel.ResumeLayout(false);
			this.tableLayoutPanel4.ResumeLayout(false);
			this.tableLayoutPanel4.PerformLayout();
			this.panel2.ResumeLayout(false);
			this.tableLayoutPanel1.ResumeLayout(false);
			this.flowLayoutPanel1.ResumeLayout(false);
			this.panel1.ResumeLayout(false);
			this.tableLayoutPanelTopLeft.ResumeLayout(false);
			this.tableLayoutPanelTopLeft.PerformLayout();
			this.tableLayoutPanel2.ResumeLayout(false);
			this.tableLayoutPanel2.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.tableTester)).EndInit();
			this.tableLayoutPanel3.ResumeLayout(false);
			this.flowLayoutPanel2.ResumeLayout(false);
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel;
        private System.Windows.Forms.ComboBox comboBox1;
		private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
        private System.Windows.Forms.ComboBox comboProjects;
        private System.Windows.Forms.Button buttonImport;
        private System.Windows.Forms.Button buttonDoImport;
        private System.Windows.Forms.Button buttonCancelImport;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanelTopLeft;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox checkBoxGlobalIncludes;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
        private System.Windows.Forms.CheckBox checkBoxGlobalExcludes;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel3;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel2;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.ComboBox comboBox2;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button4;
        private FilterTester tableTester;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel4;
        private System.Windows.Forms.Button buttonDebugger;
        private System.Windows.Forms.Button buttonBrowseTestPath;
        private System.Windows.Forms.Button buttonRunTest;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.TextBox editTestPath;
		private System.Windows.Forms.ToolTip toolTip;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Button buttonOK;
    }
}