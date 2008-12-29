
namespace XRefresh
{
	public partial class PageFilters : GUI.FirefoxDialog.PropertyPage
	{
		Model model;
		private FilterTable tableIncludes;
		private FilterTable tableExcludes;

		public PageFilters(Model model)
		{
			this.model = model;
			InitializeComponent();

			this.tableIncludes = new XRefresh.FilterTable(false, true);
			this.tableExcludes = new XRefresh.FilterTable(false, true);
			((System.ComponentModel.ISupportInitialize)(this.tableIncludes)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.tableExcludes)).BeginInit();
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
			this.tableIncludes.Location = new System.Drawing.Point(3, 243);
			this.tableIncludes.Name = "tableIncludes";
			this.tableIncludes.NoItemsText = "There are no file filters. Drag and drop a file here ...";
			this.tableIncludes.SelectionBackColor = System.Drawing.Color.Yellow;
			this.tableIncludes.SelectionForeColor = System.Drawing.Color.Black;
			this.tableIncludes.Size = new System.Drawing.Size(418, 194);
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
			this.tableExcludes.Location = new System.Drawing.Point(3, 23);
			this.tableExcludes.Name = "tableExcludes";
			this.tableExcludes.NoItemsText = "There are no file filters. Drag and drop a file here ...";
			this.tableExcludes.SelectionBackColor = System.Drawing.Color.Yellow;
			this.tableExcludes.SelectionForeColor = System.Drawing.Color.Black;
			this.tableExcludes.Size = new System.Drawing.Size(418, 194);
			this.tableExcludes.TabIndex = 5;
			this.tableExcludes.Text = "table2";
			((System.ComponentModel.ISupportInitialize)(this.tableIncludes)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.tableExcludes)).EndInit();
			this.tableLayoutPanel.Controls.Add(this.tableExcludes, 0, 1);
			this.tableLayoutPanel.Controls.Add(this.tableIncludes, 0, 3);
		}

		private void Init()
		{
			tableIncludes.InitFrom(model.GlobalIncludeFilters);
			tableExcludes.InitFrom(model.GlobalExcludeFilters);
		}

		private void Save()
		{
			tableIncludes.SaveToGlobalIncludes(model);
			tableExcludes.SaveToGlobalExcludes(model);
		}

		public override void OnInit()
		{
			Init();
		}

		public override void OnSetActive()
		{
		}

		public override void OnApply()
		{
			Save();
		}
	 }
}
