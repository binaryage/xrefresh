namespace XRefresh
{
    partial class Configuration
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Configuration));
			this.applyButton = new System.Windows.Forms.Button();
			this.cancelButton = new System.Windows.Forms.Button();
			this.advancedButton = new System.Windows.Forms.Button();
			this.labelTip = new System.Windows.Forms.Label();
			this.buttonAdd = new System.Windows.Forms.Button();
			this.imageListTypes = new System.Windows.Forms.ImageList(this.components);
			this.table = new XPTable.Models.Table();
			this.columnModel = new XPTable.Models.ColumnModel();
			this.nameColumn = new XPTable.Models.TextColumn();
			this.folderColumn = new XPTable.Models.TextColumn();
			this.typeColumn = new XPTable.Models.ComboBoxColumn();
			this.tableModel = new XPTable.Models.TableModel();
			((System.ComponentModel.ISupportInitialize)(this.table)).BeginInit();
			this.SuspendLayout();
			// 
			// applyButton
			// 
			this.applyButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.applyButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.applyButton.Image = global::XRefresh.Properties.Resources.Accept;
			this.applyButton.Location = new System.Drawing.Point(633, 165);
			this.applyButton.Name = "applyButton";
			this.applyButton.Size = new System.Drawing.Size(75, 25);
			this.applyButton.TabIndex = 4;
			this.applyButton.Text = "Apply";
			this.applyButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.applyButton.UseVisualStyleBackColor = true;
			this.applyButton.Click += new System.EventHandler(this.applyButton_Click);
			// 
			// cancelButton
			// 
			this.cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cancelButton.Image = global::XRefresh.Properties.Resources.Cancel;
			this.cancelButton.Location = new System.Drawing.Point(552, 165);
			this.cancelButton.Name = "cancelButton";
			this.cancelButton.Size = new System.Drawing.Size(75, 25);
			this.cancelButton.TabIndex = 3;
			this.cancelButton.Text = "Cancel";
			this.cancelButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.cancelButton.UseVisualStyleBackColor = true;
			this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
			// 
			// advancedButton
			// 
			this.advancedButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.advancedButton.Image = global::XRefresh.Properties.Resources.Settings;
			this.advancedButton.Location = new System.Drawing.Point(12, 165);
			this.advancedButton.Name = "advancedButton";
			this.advancedButton.Size = new System.Drawing.Size(138, 25);
			this.advancedButton.TabIndex = 2;
			this.advancedButton.Text = "Advanced Settings ...";
			this.advancedButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.advancedButton.UseVisualStyleBackColor = true;
			this.advancedButton.Click += new System.EventHandler(this.advancedButton_Click);
			// 
			// labelTip
			// 
			this.labelTip.AutoSize = true;
			this.labelTip.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
			this.labelTip.Location = new System.Drawing.Point(118, 9);
			this.labelTip.Name = "labelTip";
			this.labelTip.Size = new System.Drawing.Size(291, 14);
			this.labelTip.TabIndex = 5;
			this.labelTip.Text = "Tip: you can drag and drop project folder onto this dialog ...";
			// 
			// buttonAdd
			// 
			this.buttonAdd.Image = global::XRefresh.Properties.Resources.Add;
			this.buttonAdd.Location = new System.Drawing.Point(13, 4);
			this.buttonAdd.Name = "buttonAdd";
			this.buttonAdd.Size = new System.Drawing.Size(97, 25);
			this.buttonAdd.TabIndex = 6;
			this.buttonAdd.Text = "Add Folder ...";
			this.buttonAdd.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.buttonAdd.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
			this.buttonAdd.UseVisualStyleBackColor = true;
			this.buttonAdd.Click += new System.EventHandler(this.buttonAdd_Click);
			// 
			// imageListTypes
			// 
			this.imageListTypes.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageListTypes.ImageStream")));
			this.imageListTypes.TransparentColor = System.Drawing.Color.Magenta;
			this.imageListTypes.Images.SetKeyName(0, "Custom.png");
			this.imageListTypes.Images.SetKeyName(1, "Generic.png");
			this.imageListTypes.Images.SetKeyName(2, "HTML.png");
			this.imageListTypes.Images.SetKeyName(3, "JS.png");
			this.imageListTypes.Images.SetKeyName(4, "Ruby.png");
			this.imageListTypes.Images.SetKeyName(5, "RoR.png");
			this.imageListTypes.Images.SetKeyName(6, "Py.png");
			this.imageListTypes.Images.SetKeyName(7, "PHP.png");
			this.imageListTypes.Images.SetKeyName(8, "ASP.png");
			this.imageListTypes.Images.SetKeyName(9, "Java.png");
			this.imageListTypes.Images.SetKeyName(10, "Perl.png");
			// 
			// table
			// 
			this.table.AllowDrop = true;
			this.table.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.table.CustomEditKey = System.Windows.Forms.Keys.Return;
			this.table.Location = new System.Drawing.Point(12, 33);
			this.table.Name = "table";
			this.table.Size = new System.Drawing.Size(696, 126);
			this.table.TabIndex = 1;
			this.table.DragDrop += new System.Windows.Forms.DragEventHandler(this.table_DragDrop);
			this.table.DragEnter += new System.Windows.Forms.DragEventHandler(this.table_DragEnter);
			// 
			// Configuration
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(720, 200);
			this.Controls.Add(this.buttonAdd);
			this.Controls.Add(this.labelTip);
			this.Controls.Add(this.table);
			this.Controls.Add(this.applyButton);
			this.Controls.Add(this.advancedButton);
			this.Controls.Add(this.cancelButton);
			this.DoubleBuffered = true;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.MinimumSize = new System.Drawing.Size(440, 200);
			this.Name = "Configuration";
			this.Text = "XRefresh Configuration";
			this.Shown += new System.EventHandler(this.Configuration_Shown);
			this.Activated += new System.EventHandler(this.Configuration_Activated);
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Configuration_FormClosing);
			((System.ComponentModel.ISupportInitialize)(this.table)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button applyButton;
        private System.Windows.Forms.Button cancelButton;
        private XPTable.Models.Table table;
        private XPTable.Models.TableModel tableModel;
        private XPTable.Models.ColumnModel columnModel;
        private XPTable.Models.TextColumn folderColumn;
        private XPTable.Models.ComboBoxColumn typeColumn;
        private XPTable.Models.TextColumn nameColumn;
        private System.Windows.Forms.Button advancedButton;
        private System.Windows.Forms.Label labelTip;
        private System.Windows.Forms.Button buttonAdd;
        private System.Windows.Forms.ImageList imageListTypes;
    }
}

