namespace XRefresh
{
    partial class AdvancedSettings
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AdvancedSettings));
            this.imageList = new System.Windows.Forms.ImageList(this.components);
            this.firefoxDialog = new GUI.FirefoxDialog.FirefoxDialog();
            this.SuspendLayout();
            // 
            // imageList
            // 
            this.imageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList.ImageStream")));
            this.imageList.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList.Images.SetKeyName(0, "PageMain.png");
            this.imageList.Images.SetKeyName(1, "PageFilters.png");
            this.imageList.Images.SetKeyName(2, "PageNetwork.png");
            // 
            // firefoxDialog
            // 
            this.firefoxDialog.Dock = System.Windows.Forms.DockStyle.Fill;
            this.firefoxDialog.ImageList = this.imageList;
            this.firefoxDialog.Location = new System.Drawing.Point(0, 0);
            this.firefoxDialog.Name = "firefoxDialog";
            this.firefoxDialog.Size = new System.Drawing.Size(492, 366);
            this.firefoxDialog.TabIndex = 0;
            this.firefoxDialog.Load += new System.EventHandler(this.OnLoad);
            // 
            // AdvancedSettings
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(492, 366);
            this.Controls.Add(this.firefoxDialog);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimumSize = new System.Drawing.Size(500, 400);
            this.Name = "AdvancedSettings";
            this.Text = "XRefresh Advanced Settings";
            this.ResumeLayout(false);

        }

        #endregion

        private GUI.FirefoxDialog.FirefoxDialog firefoxDialog;
        private System.Windows.Forms.ImageList imageList;
    }
}