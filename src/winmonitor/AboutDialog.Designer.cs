namespace XRefresh
{
    partial class AboutDialog
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
            if(disposing && (components != null))
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AboutDialog));
			this.panelTop = new System.Windows.Forms.Panel();
			this.pictureBox1 = new System.Windows.Forms.PictureBox();
			this.labelVersion = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.labelTitle = new System.Windows.Forms.Label();
			this.panelDevider = new System.Windows.Forms.Panel();
			this.labelStat1 = new System.Windows.Forms.Label();
			this.buttonOK = new System.Windows.Forms.Button();
			this.linkLabelSite = new System.Windows.Forms.LinkLabel();
			this.label4 = new System.Windows.Forms.Label();
			this.labelThanks = new System.Windows.Forms.Label();
			this.linkLabelContact = new System.Windows.Forms.LinkLabel();
			this.linkLabelPeople = new System.Windows.Forms.LinkLabel();
			this.label3 = new System.Windows.Forms.Label();
			this.labelStat2 = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.linkDonate = new System.Windows.Forms.LinkLabel();
			this.label6 = new System.Windows.Forms.Label();
			this.label7 = new System.Windows.Forms.Label();
			this.panelTop.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
			this.SuspendLayout();
			// 
			// panelTop
			// 
			this.panelTop.BackColor = System.Drawing.SystemColors.Window;
			this.panelTop.Controls.Add(this.pictureBox1);
			this.panelTop.Controls.Add(this.labelVersion);
			this.panelTop.Controls.Add(this.label2);
			this.panelTop.Controls.Add(this.labelTitle);
			this.panelTop.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelTop.Location = new System.Drawing.Point(0, 0);
			this.panelTop.Name = "panelTop";
			this.panelTop.Size = new System.Drawing.Size(425, 63);
			this.panelTop.TabIndex = 0;
			// 
			// pictureBox1
			// 
			this.pictureBox1.Image = global::XRefresh.Properties.Resources.icon;
			this.pictureBox1.Location = new System.Drawing.Point(15, 18);
			this.pictureBox1.Name = "pictureBox1";
			this.pictureBox1.Size = new System.Drawing.Size(32, 32);
			this.pictureBox1.TabIndex = 4;
			this.pictureBox1.TabStop = false;
			// 
			// labelVersion
			// 
			this.labelVersion.AutoSize = true;
			this.labelVersion.Location = new System.Drawing.Point(172, 9);
			this.labelVersion.Name = "labelVersion";
			this.labelVersion.Size = new System.Drawing.Size(0, 13);
			this.labelVersion.TabIndex = 3;
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 20F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label2.Location = new System.Drawing.Point(49, 9);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(136, 31);
			this.label2.TabIndex = 2;
			this.label2.Text = "XRefresh";
			// 
			// labelTitle
			// 
			this.labelTitle.AutoSize = true;
			this.labelTitle.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelTitle.Location = new System.Drawing.Point(53, 40);
			this.labelTitle.Name = "labelTitle";
			this.labelTitle.Size = new System.Drawing.Size(165, 13);
			this.labelTitle.TabIndex = 0;
			this.labelTitle.Text = "automagical browser refresh tool";
			// 
			// panelDevider
			// 
			this.panelDevider.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.panelDevider.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelDevider.Location = new System.Drawing.Point(0, 63);
			this.panelDevider.Name = "panelDevider";
			this.panelDevider.Size = new System.Drawing.Size(425, 2);
			this.panelDevider.TabIndex = 1;
			// 
			// labelStat1
			// 
			this.labelStat1.AutoSize = true;
			this.labelStat1.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelStat1.ForeColor = System.Drawing.Color.IndianRed;
			this.labelStat1.Location = new System.Drawing.Point(12, 82);
			this.labelStat1.Name = "labelStat1";
			this.labelStat1.Size = new System.Drawing.Size(342, 14);
			this.labelStat1.TabIndex = 3;
			this.labelStat1.Text = "XRefresh has done 1000000 refresh operations so far.";
			// 
			// buttonOK
			// 
			this.buttonOK.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.buttonOK.Location = new System.Drawing.Point(338, 201);
			this.buttonOK.Name = "buttonOK";
			this.buttonOK.Size = new System.Drawing.Size(75, 25);
			this.buttonOK.TabIndex = 6;
			this.buttonOK.Text = "OK";
			this.buttonOK.UseVisualStyleBackColor = true;
			this.buttonOK.Click += new System.EventHandler(this.buttonOK_Click);
			// 
			// linkLabelSite
			// 
			this.linkLabelSite.AutoSize = true;
			this.linkLabelSite.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F);
			this.linkLabelSite.LinkColor = System.Drawing.Color.RoyalBlue;
			this.linkLabelSite.Location = new System.Drawing.Point(84, 144);
			this.linkLabelSite.Name = "linkLabelSite";
			this.linkLabelSite.Size = new System.Drawing.Size(98, 13);
			this.linkLabelSite.TabIndex = 8;
			this.linkLabelSite.TabStop = true;
			this.linkLabelSite.Text = "http://xrefresh.com";
			this.linkLabelSite.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabelSite_LinkClicked);
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(12, 144);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(62, 13);
			this.label4.TabIndex = 9;
			this.label4.Text = "Homepage:";
			// 
			// labelThanks
			// 
			this.labelThanks.AutoSize = true;
			this.labelThanks.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelThanks.Location = new System.Drawing.Point(254, 123);
			this.labelThanks.Name = "labelThanks";
			this.labelThanks.Size = new System.Drawing.Size(159, 13);
			this.labelThanks.TabIndex = 10;
			this.labelThanks.Text = "thank you for using this software";
			// 
			// linkLabelContact
			// 
			this.linkLabelContact.AutoSize = true;
			this.linkLabelContact.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F);
			this.linkLabelContact.LinkColor = System.Drawing.Color.RoyalBlue;
			this.linkLabelContact.Location = new System.Drawing.Point(100, 213);
			this.linkLabelContact.Name = "linkLabelContact";
			this.linkLabelContact.Size = new System.Drawing.Size(97, 13);
			this.linkLabelContact.TabIndex = 11;
			this.linkLabelContact.TabStop = true;
			this.linkLabelContact.Text = "Antonin Hildebrand";
			this.linkLabelContact.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabelContact_LinkClicked);
			// 
			// linkLabelPeople
			// 
			this.linkLabelPeople.AutoSize = true;
			this.linkLabelPeople.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F);
			this.linkLabelPeople.LinkColor = System.Drawing.Color.RoyalBlue;
			this.linkLabelPeople.Location = new System.Drawing.Point(84, 160);
			this.linkLabelPeople.Name = "linkLabelPeople";
			this.linkLabelPeople.Size = new System.Drawing.Size(135, 13);
			this.linkLabelPeople.TabIndex = 12;
			this.linkLabelPeople.TabStop = true;
			this.linkLabelPeople.Text = "http://xrefresh.com/people";
			this.linkLabelPeople.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabelPeople_LinkClicked);
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(12, 160);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(66, 13);
			this.label3.TabIndex = 13;
			this.label3.Text = "Contributors:";
			// 
			// labelStat2
			// 
			this.labelStat2.AutoSize = true;
			this.labelStat2.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.labelStat2.ForeColor = System.Drawing.Color.IndianRed;
			this.labelStat2.Location = new System.Drawing.Point(12, 99);
			this.labelStat2.Name = "labelStat2";
			this.labelStat2.Size = new System.Drawing.Size(344, 14);
			this.labelStat2.TabIndex = 14;
			this.labelStat2.Text = "It had saved you approximatelly 1000000 mouse clicks";
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(12, 176);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(58, 13);
			this.label1.TabIndex = 16;
			this.label1.Text = "Donations:";
			// 
			// linkDonate
			// 
			this.linkDonate.AutoSize = true;
			this.linkDonate.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F);
			this.linkDonate.LinkColor = System.Drawing.Color.RoyalBlue;
			this.linkDonate.Location = new System.Drawing.Point(84, 176);
			this.linkDonate.Name = "linkDonate";
			this.linkDonate.Size = new System.Drawing.Size(136, 13);
			this.linkDonate.TabIndex = 17;
			this.linkDonate.TabStop = true;
			this.linkDonate.Text = "http://xrefresh.com/donate";
			this.linkDonate.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkDonate_LinkClicked);
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
			this.label6.ForeColor = System.Drawing.Color.DarkGray;
			this.label6.Location = new System.Drawing.Point(12, 213);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(91, 14);
			this.label6.TabIndex = 18;
			this.label6.Text = "Copyright © 2007";
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
			this.label7.ForeColor = System.Drawing.Color.DarkGray;
			this.label7.Location = new System.Drawing.Point(12, 201);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(193, 14);
			this.label7.TabIndex = 19;
			this.label7.Text = "This program is open source software";
			// 
			// AboutDialog
			// 
			this.AcceptButton = this.buttonOK;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.CancelButton = this.buttonOK;
			this.ClientSize = new System.Drawing.Size(425, 234);
			this.ControlBox = false;
			this.Controls.Add(this.label7);
			this.Controls.Add(this.label6);
			this.Controls.Add(this.linkDonate);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.labelStat2);
			this.Controls.Add(this.linkLabelPeople);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.linkLabelContact);
			this.Controls.Add(this.labelThanks);
			this.Controls.Add(this.linkLabelSite);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.buttonOK);
			this.Controls.Add(this.labelStat1);
			this.Controls.Add(this.panelDevider);
			this.Controls.Add(this.panelTop);
			this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "AboutDialog";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "XRefresh About Box";
			this.TopMost = true;
			this.panelTop.ResumeLayout(false);
			this.panelTop.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel panelTop;
        private System.Windows.Forms.Panel panelDevider;
		private System.Windows.Forms.Label labelStat1;
        private System.Windows.Forms.Button buttonOK;
        internal System.Windows.Forms.Label labelTitle;
        internal System.Windows.Forms.LinkLabel linkLabelSite;
		private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label labelVersion;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label labelThanks;
        internal System.Windows.Forms.LinkLabel linkLabelContact;
		internal System.Windows.Forms.LinkLabel linkLabelPeople;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label labelStat2;
		private System.Windows.Forms.PictureBox pictureBox1;
		private System.Windows.Forms.Label label1;
		internal System.Windows.Forms.LinkLabel linkDonate;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label7;
    }
}