namespace GUI.FirefoxDialog
{
	partial class FirefoxDialog
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

		#region Component Designer generated code

		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.pagePanel = new System.Windows.Forms.Panel();
			this.leftPanel = new System.Windows.Forms.Panel();
			this.mozPane1 = new Pabo.MozBar.MozPane();
			this.bottomPanel = new System.Windows.Forms.Panel();
			this.cancelButton = new System.Windows.Forms.Button();
			this.applyButton = new System.Windows.Forms.Button();
			this.buttonReset = new System.Windows.Forms.Button();
			this.leftPanel.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.mozPane1)).BeginInit();
			this.bottomPanel.SuspendLayout();
			this.SuspendLayout();
			// 
			// pagePanel
			// 
			this.pagePanel.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pagePanel.Location = new System.Drawing.Point(104, 0);
			this.pagePanel.Name = "pagePanel";
			this.pagePanel.Padding = new System.Windows.Forms.Padding(0, 0, 5, 0);
			this.pagePanel.Size = new System.Drawing.Size(435, 355);
			this.pagePanel.TabIndex = 7;
			// 
			// leftPanel
			// 
			this.leftPanel.Controls.Add(this.mozPane1);
			this.leftPanel.Dock = System.Windows.Forms.DockStyle.Left;
			this.leftPanel.Location = new System.Drawing.Point(0, 0);
			this.leftPanel.Name = "leftPanel";
			this.leftPanel.Padding = new System.Windows.Forms.Padding(8);
			this.leftPanel.Size = new System.Drawing.Size(104, 355);
			this.leftPanel.TabIndex = 8;
			// 
			// mozPane1
			// 
			this.mozPane1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.mozPane1.ImageList = null;
			this.mozPane1.Location = new System.Drawing.Point(8, 8);
			this.mozPane1.Name = "mozPane1";
			this.mozPane1.Size = new System.Drawing.Size(88, 339);
			this.mozPane1.TabIndex = 0;
			this.mozPane1.ItemClick += new Pabo.MozBar.MozItemClickEventHandler(this.mozPane1_ItemClick);
			// 
			// bottomPanel
			// 
			this.bottomPanel.Controls.Add(this.cancelButton);
			this.bottomPanel.Controls.Add(this.applyButton);
			this.bottomPanel.Controls.Add(this.buttonReset);
			this.bottomPanel.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.bottomPanel.Location = new System.Drawing.Point(0, 355);
			this.bottomPanel.Name = "bottomPanel";
			this.bottomPanel.Size = new System.Drawing.Size(539, 40);
			this.bottomPanel.TabIndex = 6;
			// 
			// cancelButton
			// 
			this.cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cancelButton.Image = global::XRefresh.Properties.Resources.Cancel;
			this.cancelButton.Location = new System.Drawing.Point(368, 8);
			this.cancelButton.Name = "cancelButton";
			this.cancelButton.Size = new System.Drawing.Size(75, 25);
			this.cancelButton.TabIndex = 7;
			this.cancelButton.Text = "Cancel";
			this.cancelButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cancelButton.UseVisualStyleBackColor = true;
			this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
			// 
			// applyButton
			// 
			this.applyButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.applyButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.applyButton.Image = global::XRefresh.Properties.Resources.Accept;
			this.applyButton.Location = new System.Drawing.Point(449, 8);
			this.applyButton.Name = "applyButton";
			this.applyButton.Size = new System.Drawing.Size(75, 25);
			this.applyButton.TabIndex = 6;
			this.applyButton.Text = "OK";
			this.applyButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.applyButton.UseVisualStyleBackColor = true;
			this.applyButton.Click += new System.EventHandler(this.applyButton_Click);
			// 
			// buttonReset
			// 
			this.buttonReset.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.buttonReset.Image = global::XRefresh.Properties.Resources.Tick;
			this.buttonReset.Location = new System.Drawing.Point(8, 8);
			this.buttonReset.Name = "buttonReset";
			this.buttonReset.Size = new System.Drawing.Size(120, 25);
			this.buttonReset.TabIndex = 5;
			this.buttonReset.Text = "Reset to Defaults";
			this.buttonReset.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.buttonReset.UseVisualStyleBackColor = true;
			this.buttonReset.Click += new System.EventHandler(this.buttonReset_Click);
			// 
			// FirefoxDialog
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.pagePanel);
			this.Controls.Add(this.leftPanel);
			this.Controls.Add(this.bottomPanel);
			this.Name = "FirefoxDialog";
			this.Size = new System.Drawing.Size(539, 395);
			this.leftPanel.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.mozPane1)).EndInit();
			this.bottomPanel.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Panel pagePanel;
		private System.Windows.Forms.Panel leftPanel;
		private Pabo.MozBar.MozPane mozPane1;
        private System.Windows.Forms.Panel bottomPanel;
        private System.Windows.Forms.Button buttonReset;
        private System.Windows.Forms.Button applyButton;
        private System.Windows.Forms.Button cancelButton;

	}
}
