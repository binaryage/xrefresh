using System;
using System.ComponentModel;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using XPTable.Editors;
using XPTable.Events;
using XPTable.Models;
using XPTable.Renderers;
using XPTable.Themes;

namespace XRefresh
{
	public class ProjecTypeItem : object
	{
		// forecolor: transparent = inherit
		public Color ForeColor = Color.FromKnownColor(KnownColor.Transparent);
		public bool Mark = false;
		public bool Separator = false;
		public int ImageIndex = -1;
		public object Tag = null;
		public string Text = null;
		public string Description = null;

		// ToString() should return item text
		public override string ToString()
		{
			return Text;
		}
	}


	public class ProjectTypeListBox : ListBox
	{
		private ImageList imgs = new ImageList();

		// constructor
		public ProjectTypeListBox()
		{
			// set draw mode to owner draw
			this.DrawMode = DrawMode.OwnerDrawFixed;
		}

		// ImageList property
		public ImageList ImageList
		{
			get
			{
				return imgs;
			}
			set
			{
				imgs = value;
			}
		}

		// customized drawing process
		protected override void OnDrawItem(DrawItemEventArgs e)
		{
			if (e.State != DrawItemState.None && e.State != DrawItemState.Focus)
			{
				DrawItemEventArgs f = new DrawItemEventArgs(e.Graphics, e.Font, e.Bounds, e.Index, DrawItemState.None, e.ForeColor, Color.Yellow);
				f.DrawBackground();
			}
			else
				e.DrawBackground();

			// check if it is an item from the Items collection
			if (e.Index < 0)

				// not an item, draw the text (indented)
				e.Graphics.DrawString(this.Text, e.Font, new SolidBrush(e.ForeColor), e.Bounds.Left + imgs.ImageSize.Width, e.Bounds.Top + 1);

			else
			{

				// check if item is an ImageComboItem
				if (this.Items[e.Index].GetType() == typeof(ProjecTypeItem))
				{
					Rectangle r = e.Bounds;
					//r.X += 3;
					//r.Y += 1;

					// get item to draw
					ProjecTypeItem item = (ProjecTypeItem)this.Items[e.Index];

					// get forecolor & font
					Color forecolor = (item.ForeColor != Color.FromKnownColor(KnownColor.Transparent)) ? item.ForeColor : e.ForeColor;
					Font font = item.Mark ? new Font(e.Font, FontStyle.Bold) : e.Font;

					// -1: no image
					if (item.ImageIndex != -1)
					{
						// draw image, then draw text next to it
						this.ImageList.Draw(e.Graphics, r.Left + 1, r.Top + 1, item.ImageIndex);
						e.Graphics.DrawString(item.Text, font, new SolidBrush(forecolor), r.Left + imgs.ImageSize.Width + 3, r.Top + 2);
					}
					else
						// draw text (indented)
						e.Graphics.DrawString(item.Text, font, new SolidBrush(forecolor), r.Left + imgs.ImageSize.Width + 3, r.Top + 2);

					// draw text (indented)
					int rpart = 74;
					Font font2 = new Font(e.Font, FontStyle.Regular);
					e.Graphics.DrawString(item.Description, font2, new SolidBrush(forecolor), r.Left + rpart + 4, r.Top + 2);

					if (item.Separator == true)
					{
						e.Graphics.DrawLine(new Pen(new SolidBrush(Color.Gray)), new Point(r.Left, r.Bottom - 1), new Point(r.Right, r.Bottom - 1));
					}

					e.Graphics.DrawLine(new Pen(new SolidBrush(Color.LightGray)), new Point(rpart, r.Top), new Point(rpart, r.Bottom - 1));
				}
				else

					// it is not an ImageComboItem, draw it
					e.Graphics.DrawString(this.Items[e.Index].ToString(),
					  e.Font, new SolidBrush(e.ForeColor), e.Bounds.Left +
					  imgs.ImageSize.Width, e.Bounds.Top + 1);

			}


			base.OnDrawItem(e);
		}
	}

	/// <summary>
	/// A class for editing Cells that look like a ComboBox
	/// </summary>
	public class ProjectTypeCellEditor : DropDownCellEditor
	{
		#region EventHandlers

		/// <summary>
		/// Occurs when the SelectedIndex property has changed
		/// </summary>
		public event EventHandler SelectedIndexChanged;

		/// <summary>
		/// Occurs when a visual aspect of an owner-drawn ComboBoxCellEditor changes
		/// </summary>
		public event DrawItemEventHandler DrawItem;

		/// <summary>
		/// Occurs each time an owner-drawn ComboBoxCellEditor item needs to be 
		/// drawn and when the sizes of the list items are determined
		/// </summary>
		public event MeasureItemEventHandler MeasureItem;

		/// <summary>
		/// Occurs each time an owner-drawn ComboBoxCellEditor item needs to be 
		/// drawn and when the sizes of the list items are determined
		/// </summary>
		public delegate void FillItemsHandler(object sender, EventArgs e);
		public event FillItemsHandler FillItems;
		public delegate void HideDropDownEventHandler(object sender, EventArgs e);
		public event HideDropDownEventHandler HideDropDownEvent;

		#endregion


		#region Class Data

		/// <summary>
		/// The ListBox that contains the items to be shown in the 
		/// drop-down portion of the ComboBoxCellEditor
		/// </summary>
		private ProjectTypeListBox listbox;

		/// <summary>
		/// The maximum number of items to be shown in the drop-down 
		/// portion of the ComboBoxCellEditor
		/// </summary>
		private int maxDropDownItems;

		/// <summary>
		/// The width of the Cell being edited
		/// </summary>
		private int cellWidth;

		#endregion

		#region Constructor

		[DllImport("user32")]
		private static extern bool HideCaret(IntPtr hWnd);

		/// <summary>
		/// Initializes a new instance of the ComboBoxCellEditor class with default settings
		/// </summary>
		public ProjectTypeCellEditor(ImageList images)
			: base()
		{
			this.listbox = new ProjectTypeListBox();
			this.listbox.BorderStyle = BorderStyle.None;
			this.listbox.Location = new Point(0, 0);
			this.listbox.Size = new Size(100, 100);
			this.listbox.Dock = DockStyle.Fill;
			this.listbox.DrawItem += new DrawItemEventHandler(this.listbox_DrawItem);
			this.listbox.MeasureItem += new MeasureItemEventHandler(this.listbox_MeasureItem);
			this.listbox.MouseEnter += new EventHandler(this.listbox_MouseEnter);
			this.listbox.KeyDown += new KeyEventHandler(this.OnKeyDown);
			this.listbox.KeyPress += new KeyPressEventHandler(base.OnKeyPress);
			this.listbox.Click += new EventHandler(listbox_Click);
			this.listbox.ItemHeight = 19;
			this.listbox.ImageList = images;
			this.listbox.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));


			this.TextBox.MouseClick += new MouseEventHandler(TextBox_HideCarretM);
			this.TextBox.MouseDown += new MouseEventHandler(TextBox_HideCarretM);
			this.TextBox.GotFocus += new EventHandler(TextBox_HideCarret);
			this.TextBox.KeyDown += new KeyEventHandler(OnKeyDown);
			this.TextBox.MouseWheel += new MouseEventHandler(OnMouseWheel);
			HideCaret(TextBox.Handle);

			this.maxDropDownItems = 16;

			this.cellWidth = 0;

			this.DropDown.Control = this.listbox;
		}

		void TextBox_HideCarretM(object sender, MouseEventArgs e)
		{
			HideCaret((sender as System.Windows.Forms.Control).Handle);
		}

		void TextBox_HideCarret(object sender, EventArgs e)
		{
			HideCaret((sender as System.Windows.Forms.Control).Handle);
		}

		#endregion


		#region Methods

		/// <summary>
		/// Sets the location and size of the CellEditor
		/// </summary>
		/// <param name="cellRect">A Rectangle that represents the size and location 
		/// of the Cell being edited</param>
		protected override void SetEditLocation(Rectangle cellRect)
		{
			// calc the size of the textbox
			ICellRenderer renderer = this.EditingTable.ColumnModel.GetCellRenderer(this.EditingCellPos.Column);
			int buttonWidth = ((ComboBoxCellRenderer)renderer).ButtonWidth;

			this.TextBox.Location = new Point(cellRect.Location.X + 4 + 2, cellRect.Location.Y + 2);
			this.TextBox.Size = new Size(cellRect.Width - 1 - 6 - buttonWidth - 2, cellRect.Height - 2);

			this.cellWidth = cellRect.Width - 4;
		}

		/// <summary>
		/// Sets the initial value of the editor based on the contents of 
		/// the Cell being edited
		/// </summary>
		protected override void SetEditValue()
		{
			this.TextBox.Text = this.EditingCell.Text;
			this.listbox.SelectedItem = this.EditingCell.Text;
		}


		/// <summary>
		/// Sets the contents of the Cell being edited based on the value 
		/// in the editor
		/// </summary>
		protected override void SetCellValue()
		{
			if (EditingCell.Text == "Custom" && TextBox.Text!="Custom")
			{
				// switch from custom to predefined project type
				if (DialogResult.OK!=MessageBox.Show("Do you really want switch project type to " + TextBox.Text + " ?\nYour previous custom settings will be lost.", "Custom settings warning", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning))
				{
					return;
				}
			}

			Model.FoldersRow folder = EditingCell.Row.Tag as Model.FoldersRow;
			EditingCell.Data = Detector.Current.GetIcon(TextBox.Text);
			EditingCell.Text = TextBox.Text;
			if (EditingCell.Text == "Custom")
			{
				OpenCustomTypeDialog(folder);
			}
			else
			{
				// fill in project settings
				Scanner scanner = Detector.Current.GetScanner(TextBox.Text);
				scanner.SetFolder(folder);
				scanner.SetupFilters();
			}
		}

		private void OpenCustomTypeDialog(Model.FoldersRow folder)
		{
			ProjectFilters customType = new ProjectFilters(folder.Table.DataSet as Model, folder);
			customType.ShowDialog();
		}

		/// <summary>
		/// Starts editing the Cell
		/// </summary>
		public override void StartEditing()
		{
			this.listbox.SelectedIndexChanged += new EventHandler(listbox_SelectedIndexChanged);
			this.TextBox.BackColor = Color.Yellow;

			base.StartEditing();
			HideCaret(TextBox.Handle);
		}


		/// <summary>
		/// Stops editing the Cell and commits any changes
		/// </summary>
		public override void StopEditing()
		{
			this.listbox.SelectedIndexChanged -= new EventHandler(listbox_SelectedIndexChanged);

			base.StopEditing();
		}


		/// <summary>
		/// Stops editing the Cell and ignores any changes
		/// </summary>
		public override void CancelEditing()
		{
			this.listbox.SelectedIndexChanged -= new EventHandler(listbox_SelectedIndexChanged);

			base.CancelEditing();
		}

		public void UpdateDropDownSize()
		{
			if (this.InternalDropDownWidth == -1)
			{
				this.DropDown.Width = 300;//this.cellWidth;
				this.listbox.Width = 300; // this.cellWidth;
			}

			if (this.IntegralHeight)
			{
				int visItems = this.listbox.Height / this.ItemHeight;

				if (visItems > this.MaxDropDownItems)
				{
					visItems = this.MaxDropDownItems;
				}

				if (this.listbox.Items.Count < this.MaxDropDownItems)
				{
					visItems = this.listbox.Items.Count;
				}

				if (visItems == 0)
				{
					visItems = 1;
				}

				this.DropDown.Height = (visItems * this.ItemHeight) + 2;
				this.listbox.Height = visItems * this.ItemHeight;
			}
		}

		/// <summary>
		/// Displays the drop down portion to the user
		/// </summary>
		protected override void ShowDropDown()
		{
			if (FillItems != null)
			{
				FillItems(this, new EventArgs());
			}

			if (this.InternalDropDownWidth == -1)
			{
				this.DropDown.Width = 300;//this.cellWidth;
				this.listbox.Width = 300; // this.cellWidth;
			}

			if (this.IntegralHeight)
			{
				int visItems = this.listbox.Height / this.ItemHeight;

				if (visItems > this.MaxDropDownItems)
				{
					visItems = this.MaxDropDownItems;
				}

				if (this.listbox.Items.Count < this.MaxDropDownItems)
				{
					visItems = this.listbox.Items.Count;
				}

				if (visItems == 0)
				{
					visItems = 1;
				}

				this.DropDown.Height = (visItems * this.ItemHeight) + 2;
				this.listbox.Height = visItems * this.ItemHeight;
			}

			base.ShowDropDown();
		}

		protected override void HideDropDown()
		{
			if (HideDropDownEvent != null)
			{
				HideDropDownEvent(this, new EventArgs());
			}
			base.HideDropDown();
		}

		#endregion


		#region Properties

		/// <summary>
		/// Gets or sets the maximum number of items to be shown in the drop-down 
		/// portion of the ComboBoxCellEditor
		/// </summary>
		public int MaxDropDownItems
		{
			get
			{
				return this.maxDropDownItems;
			}

			set
			{
				if ((value < 1) || (value > 100))
				{
					throw new ArgumentOutOfRangeException("MaxDropDownItems must be between 1 and 100");
				}

				this.maxDropDownItems = value;
			}
		}


		/// <summary>
		/// Gets or sets a value indicating whether your code or the operating 
		/// system will handle drawing of elements in the list
		/// </summary>
		public DrawMode DrawMode
		{
			get
			{
				return this.listbox.DrawMode;
			}

			set
			{
				if (!Enum.IsDefined(typeof(DrawMode), value))
				{
					throw new InvalidEnumArgumentException("value", (int)value, typeof(DrawMode));
				}

				this.listbox.DrawMode = value;
			}
		}


		/// <summary>
		/// Gets or sets a value indicating whether the drop-down portion of the 
		/// editor should resize to avoid showing partial items
		/// </summary>
		public bool IntegralHeight
		{
			get
			{
				return this.listbox.IntegralHeight;
			}

			set
			{
				this.listbox.IntegralHeight = value;
			}
		}


		/// <summary>
		/// Gets or sets the height of an item in the editor
		/// </summary>
		public int ItemHeight
		{
			get
			{
				return this.listbox.ItemHeight;
			}

			set
			{
				this.listbox.ItemHeight = value;
			}
		}


		/// <summary>
		/// Gets an object representing the collection of the items contained 
		/// in this ComboBoxCellEditor
		/// </summary>
		public ListBox.ObjectCollection Items
		{
			get
			{
				return this.listbox.Items;
			}
		}


		/// <summary>
		/// Gets or sets the maximum number of characters allowed in the editable 
		/// portion of a ComboBoxCellEditor
		/// </summary>
		public int MaxLength
		{
			get
			{
				return this.TextBox.MaxLength;
			}

			set
			{
				this.TextBox.MaxLength = value;
			}
		}


		/// <summary>
		/// Gets or sets the index specifying the currently selected item
		/// </summary>
		public int SelectedIndex
		{
			get
			{
				return this.listbox.SelectedIndex;
			}

			set
			{
				if (this.listbox.Items.Count >= value) this.listbox.SelectedIndex = value;
			}
		}


		/// <summary>
		/// Gets or sets currently selected item in the ComboBoxCellEditor
		/// </summary>
		public object SelectedItem
		{
			get
			{
				return this.listbox.SelectedItem;
			}

			set
			{
				this.listbox.SelectedItem = value;
			}
		}

		#endregion


		#region Events

		/// <summary>
		/// Handler for the editors TextBox.KeyDown and ListBox.KeyDown events
		/// </summary>
		/// <param name="sender">The object that raised the event</param>
		/// <param name="e">A KeyEventArgs that contains the event data</param>
		protected virtual void OnKeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyData == Keys.Up)
			{
				int index = this.SelectedIndex;

				if (index == -1)
				{
					this.SelectedIndex = 0;
				}
				else if (index > 0)
				{
					this.SelectedIndex--;
				}

				e.Handled = true;
			}
			else if (e.KeyData == Keys.Down)
			{
				int index = this.SelectedIndex;

				if (index == -1)
				{
					this.SelectedIndex = 0;
				}
				else if (index < this.Items.Count - 1)
				{
					this.SelectedIndex++;
				}

				e.Handled = true;
			}
		}


		/// <summary>
		/// Handler for the editors TextBox.MouseWheel event
		/// </summary>
		/// <param name="sender">The object that raised the event</param>
		/// <param name="e">A MouseEventArgs that contains the event data</param>
		protected virtual void OnMouseWheel(object sender, MouseEventArgs e)
		{
			int index = this.SelectedIndex;

			if (index == -1)
			{
				this.SelectedIndex = 0;
			}
			else
			{
				if (e.Delta > 0)
				{
					if (index > 0)
					{
						this.SelectedIndex--;
					}
				}
				else
				{
					if (index < this.Items.Count - 1)
					{
						this.SelectedIndex++;
					}
				}
			}
		}


		/// <summary>
		/// Raises the DrawItem event
		/// </summary>
		/// <param name="e">A DrawItemEventArgs that contains the event data</param>
		protected virtual void OnDrawItem(DrawItemEventArgs e)
		{
			if (DrawItem != null)
			{
				DrawItem(this, e);
			}
		}


		/// <summary>
		/// Raises the MeasureItem event
		/// </summary>
		/// <param name="e">A MeasureItemEventArgs that contains the event data</param>
		protected virtual void OnMeasureItem(MeasureItemEventArgs e)
		{
			if (MeasureItem != null)
			{
				MeasureItem(this, e);
			}
		}


		/// <summary>
		/// Raises the SelectedIndexChanged event
		/// </summary>
		/// <param name="e">An EventArgs that contains the event data</param>
		protected virtual void OnSelectedIndexChanged(EventArgs e)
		{
			if (SelectedIndexChanged != null)
			{
				SelectedIndexChanged(this, e);
			}

			this.TextBox.Text = this.SelectedItem.ToString();
		}


		/// <summary>
		/// Handler for the editors ListBox.Click event
		/// </summary>
		/// <param name="sender">The object that raised the event</param>
		/// <param name="e">An EventArgs that contains the event data</param>
		private void listbox_Click(object sender, EventArgs e)
		{
			this.DroppedDown = false;

			this.EditingTable.StopEditing();
		}


		/// <summary>
		/// Handler for the editors ListBox.SelectedIndexChanged event
		/// </summary>
		/// <param name="sender">The object that raised the event</param>
		/// <param name="e">An EventArgs that contains the event data</param>
		private void listbox_SelectedIndexChanged(object sender, EventArgs e)
		{
			this.OnSelectedIndexChanged(e);
		}


		/// <summary>
		/// Handler for the editors ListBox.MouseEnter event
		/// </summary>
		/// <param name="sender">The object that raised the event</param>
		/// <param name="e">An EventArgs that contains the event data</param>
		private void listbox_MouseEnter(object sender, EventArgs e)
		{
			this.EditingTable.RaiseCellMouseLeave(this.EditingCellPos);
		}


		/// <summary>
		/// Handler for the editors ListBox.DrawItem event
		/// </summary>
		/// <param name="sender">The object that raised the event</param>
		/// <param name="e">A DrawItemEventArgs that contains the event data</param>
		private void listbox_DrawItem(object sender, DrawItemEventArgs e)
		{
			this.OnDrawItem(e);
		}


		/// <summary>
		/// Handler for the editors ListBox.MeasureItem event
		/// </summary>
		/// <param name="sender">The object that raised the event</param>
		/// <param name="e">A MeasureItemEventArgs that contains the event data</param>
		private void listbox_MeasureItem(object sender, MeasureItemEventArgs e)
		{
			this.OnMeasureItem(e);
		}


		#endregion
	}

	/// <summary>
	/// A CellRenderer that draws Cell contents as a ComboBox
	/// </summary>
	public class ProjectTypeCellRenderer : ComboBoxCellRenderer
	{
		#region Constructor

		/// <summary>
		/// Initializes a new instance of the ComboBoxCellRenderer class with 
		/// default settings
		/// </summary>
		public ProjectTypeCellRenderer()
			: base()
		{

		}

		#endregion

		public override Rectangle ClientRectangle
		{
			get
			{
				Rectangle client = new Rectangle(this.Bounds.Location, this.Bounds.Size);

				// take borders into account
				client.Width -= Renderer.BorderWidth;
				client.Height -= Renderer.BorderWidth;

				// take cell padding into account
				client.X += this.Padding.Left + 1;
				client.Y += this.Padding.Top + 1;
				client.Width -= this.Padding.Left + this.Padding.Right + 2;
				client.Height -= this.Padding.Top + this.Padding.Bottom + 2;

				return client;
			}
		}

		#region Events

		#region Paint

		public override void OnMouseDown(CellMouseEventArgs e)
		{
			//base.OnMouseDown(e);

			if (this.ShowDropDownButton || (e.Table.IsEditing && e.CellPos == e.Table.EditingCell))
			{
				if (e.Table.IsCellEditable(e.CellPos))
				{
					// get the button renderer data
					DropDownRendererData rendererData = this.GetDropDownRendererData(e.Cell);
					if (!(e.Table.ColumnModel.GetCellEditor(e.CellPos.Column) is DropDownCellEditor))
					{
						throw new InvalidOperationException("Cannot edit Cell as DropDownCellRenderer requires a DropDownColumn that uses a DropDownCellEditor");
					}

					rendererData.ButtonState = ComboBoxStates.Pressed;

					if (!e.Table.IsEditing)
					{
						e.Table.EditCell(e.CellPos);
					}

					((IEditorUsesRendererButtons)e.Table.EditingCellEditor).OnEditorButtonMouseDown(this, e);

					e.Table.Invalidate(e.CellRect);
				}
			}
		}

		protected new Rectangle CalcDropDownButtonBounds()
		{
			Rectangle buttonRect = this.ClientRectangle;

			buttonRect.Width = this.ButtonWidth;
			buttonRect.X = this.ClientRectangle.Right - buttonRect.Width + 2;

			if (buttonRect.Width > this.ClientRectangle.Width)
			{
				buttonRect = this.ClientRectangle;
			}

			return buttonRect;
		}

		protected override void OnPaint(PaintCellEventArgs e)
		{
			//base.OnPaint(e);

			// don't bother going any further if the Cell is null 
			if (e.Cell == null)
			{
				return;
			}

			Rectangle buttonRect = this.CalcDropDownButtonBounds();

			Rectangle textRect = this.ClientRectangle;

			if (this.ShowDropDownButton)
			{
				textRect.Width -= buttonRect.Width - 1;
			}

			// draw the text
			if (e.Cell.Text != null && e.Cell.Text.Length != 0)
			{
				textRect.Y += 1;
				textRect.X += 2;
				if (e.Enabled)
				{
					TextRenderer.DrawText(e.Graphics, e.Cell.Text, this.Font, textRect, this.ForeBrush.Color, TextFormatFlags.Left);
				}
				else
				{
					TextRenderer.DrawText(e.Graphics, e.Cell.Text, this.Font, textRect, SystemColors.GrayText, TextFormatFlags.Left);
				}
			}

			if (e.Focused && e.Enabled)
			{
				Rectangle focusRect = this.ClientRectangle;

				if (this.ShowDropDownButton)
				{
					focusRect.Width -= buttonRect.Width + 2;
				}

				ControlPaint.DrawFocusRectangle(e.Graphics, focusRect);
			}
		}

		#endregion

		#endregion
	}

	public class GDITextCellEditor : TextCellEditor
	{
		protected override void SetEditLocation(Rectangle cellRect)
		{
			this.TextBox.Location = new Point(cellRect.Location.X + 4, cellRect.Location.Y + 2);
			this.TextBox.Size = new Size(cellRect.Width - 4 - 1, cellRect.Height - 2);
		}

		public override void StartEditing()
		{
			this.TextBox.Font = cell.CellStyle.Font;
			this.TextBox.BackColor = Color.Yellow;
			base.StartEditing();
		}
	}

	public class GDITextCellRenderer : CellRenderer
	{
		#region Constructor

		/// <summary>
		/// Initializes a new instance of the TextCellRenderer class with 
		/// default settings
		/// </summary>
		public GDITextCellRenderer()
			: base()
		{
		}

		#endregion

		public override Rectangle ClientRectangle
		{
			get
			{
				Rectangle client = new Rectangle(this.Bounds.Location, this.Bounds.Size);

				// take borders into account
				client.Width -= Renderer.BorderWidth;
				client.Height -= Renderer.BorderWidth;

				// take cell padding into account
				client.X += this.Padding.Left + 1;
				client.Y += this.Padding.Top + 1;
				client.Width -= this.Padding.Left + this.Padding.Right + 2;
				client.Height -= this.Padding.Top + this.Padding.Bottom + 2;

				return client;
			}
		}

		/// <summary>
		/// Returns the height that is required to render this cell. If zero is returned then the default row height is used.
		/// </summary>
		/// <param name="graphics"></param>
		/// <param name="cell"></param>
		/// <returns></returns>
		public override int GetCellHeight(Graphics graphics, Cell cell)
		{
			if (cell != null)
			{
				this.Font = cell.Font;
				// Need to set this.Bounds before we access Client rectangle
				SizeF size = TextRenderer.MeasureText(graphics, cell.Text, this.Font, new Size(this.ClientRectangle.Width, this.ClientRectangle.Height), TextFormatFlags.Left);
				return (int)Math.Ceiling(size.Height);
			}
			else
				return 0;
		}

		#region Events

		#region Paint

		/// <summary>
		/// Raises the Paint event
		/// </summary>
		/// <param name="e">A PaintCellEventArgs that contains the event data</param>
		protected override void OnPaint(PaintCellEventArgs e)
		{
			base.OnPaint(e);

			// don't bother going any further if the Cell is null 
			if (e.Cell == null)
			{
				return;
			}

			string text = e.Cell.Text;

			if (text != null && text.Length != 0)
			{
				// v1.1.1 fix - removed hardcoded alignment
				Rectangle rect = this.ClientRectangle;
				rect.Y += 1;

				if (e.Enabled)
				{
					TextRenderer.DrawText(e.Graphics, text, this.Font, rect, this.ForeBrush.Color, TextFormatFlags.Left);
				}
				else
				{
					TextRenderer.DrawText(e.Graphics, text, this.Font, rect, SystemColors.GrayText, TextFormatFlags.Left);
				}
			}

			if (e.Focused && e.Enabled)
			{
				ControlPaint.DrawFocusRectangle(e.Graphics, this.ClientRectangle);
			}
		}

		#endregion

		#endregion
	}

	public class FolderCellRenderer : GDITextCellRenderer
	{
		#region Class Data

		/// <summary>
		/// The width of the DropDownCellRenderer's dropdown button
		/// </summary>
		private int buttonWidth;

		/// <summary>
		/// Specifies whether the DropDownCellRenderer dropdown button should be drawn
		/// </summary>
		private bool showButton;

		#endregion


		#region Constructor

		/// <summary>
		/// Initializes a new instance of the DropDownCellRenderer class with 
		/// default settings
		/// </summary>
		public FolderCellRenderer()
			: base()
		{
			this.buttonWidth = 15;
			this.showButton = true;
		}

		#endregion

		public override Rectangle ClientRectangle
		{
			get
			{
				Rectangle client = new Rectangle(this.Bounds.Location, this.Bounds.Size);

				// take borders into account
				client.Width -= Renderer.BorderWidth;
				client.Height -= Renderer.BorderWidth;

				// take cell padding into account
				client.X += this.Padding.Left + 1 + 16;
				client.Y += this.Padding.Top + 1;
				client.Width -= this.Padding.Left + this.Padding.Right + 2 + 18 + 16;
				client.Height -= this.Padding.Top + this.Padding.Bottom + 2;

				return client;
			}
		}

		#region Methods

		/// <summary>
		/// Gets the Rectangle that specifies the Size and Location of 
		/// the current Cell's dropdown button
		/// </summary>
		/// <returns>A Rectangle that specifies the Size and Location of 
		/// the current Cell's dropdown button</returns>
		protected internal Rectangle CalcDropDownButtonBounds()
		{
			Rectangle buttonRect = this.ClientRectangle;

			buttonRect.Width = this.ButtonWidth;
			buttonRect.X = this.ClientRectangle.Right + 2;// -buttonRect.Width;

			if (buttonRect.Width > this.ClientRectangle.Width)
			{
				buttonRect = this.ClientRectangle;
			}

			return buttonRect;
		}


		/// <summary>
		/// Gets the DropDownRendererData specific data used by the Renderer from 
		/// the specified Cell
		/// </summary>
		/// <param name="cell">The Cell to get the DropDownRendererData data for</param>
		/// <returns>The DropDownRendererData data for the specified Cell</returns>
		protected DropDownRendererData GetDropDownRendererData(Cell cell)
		{
			object rendererData = this.GetRendererData(cell);

			if (rendererData == null || !(rendererData is DropDownRendererData))
			{
				rendererData = new DropDownRendererData();

				this.SetRendererData(cell, rendererData);
			}

			return (DropDownRendererData)rendererData;
		}

		#endregion


		#region Properties

		/// <summary>
		/// Gets or sets the width of the dropdown button
		/// </summary>
		public int ButtonWidth
		{
			get
			{
				return this.buttonWidth;
			}

			set
			{
				this.buttonWidth = value;
			}
		}


		/// <summary>
		/// Gets or sets whether the DropDownCellRenderer dropdown button should be drawn
		/// </summary>
		protected bool ShowDropDownButton
		{
			get
			{
				return this.showButton;
			}

			set
			{
				this.showButton = value;
			}
		}

		#endregion


		#region Events

		#region Mouse

		#region MouseLeave

		/// <summary>
		/// Raises the MouseLeave event
		/// </summary>
		/// <param name="e">A CellMouseEventArgs that contains the event data</param>
		public override void OnMouseLeave(CellMouseEventArgs e)
		{
			base.OnMouseLeave(e);

			if (this.ShowDropDownButton || (e.Table.IsEditing && e.CellPos == e.Table.EditingCell))
			{
				if (e.Table.IsCellEditable(e.CellPos))
				{
					// get the button renderer data
					DropDownRendererData rendererData = this.GetDropDownRendererData(e.Cell);

					if (rendererData.ButtonState != ComboBoxStates.Normal)
					{
						rendererData.ButtonState = ComboBoxStates.Normal;

						e.Table.Invalidate(e.CellRect);
					}
				}
			}
		}

		#endregion

		#region MouseUp

		/// <summary>
		/// Raises the MouseUp event
		/// </summary>
		/// <param name="e">A CellMouseEventArgs that contains the event data</param>
		public override void OnMouseUp(CellMouseEventArgs e)
		{
			base.OnMouseUp(e);

			if (this.ShowDropDownButton || (e.Table.IsEditing && e.CellPos == e.Table.EditingCell))
			{
				if (e.Table.IsCellEditable(e.CellPos))
				{
					// get the renderer data
					DropDownRendererData rendererData = this.GetDropDownRendererData(e.Cell);

					if (this.CalcDropDownButtonBounds().Contains(e.X, e.Y))
					{
						rendererData.ButtonState = ComboBoxStates.Hot;

						e.Table.Invalidate(e.CellRect);
					}
				}
			}
		}

		#endregion

		#region MouseDown

		/// <summary>
		/// Raises the MouseDown event
		/// </summary>
		/// <param name="e">A CellMouseEventArgs that contains the event data</param>
		public override void OnMouseDown(CellMouseEventArgs e)
		{
			//base.OnMouseDown(e);

			if (this.ShowDropDownButton || (e.Table.IsEditing && e.CellPos == e.Table.EditingCell))
			{
				if (e.Table.IsCellEditable(e.CellPos))
				{
					// get the button renderer data
					DropDownRendererData rendererData = this.GetDropDownRendererData(e.Cell);

					if (this.CalcDropDownButtonBounds().Contains(e.X, e.Y))
					{
						//if (!(e.Table.ColumnModel.GetCellEditor(e.CellPos.Column) is DropDownCellEditor))
						//{
						//    throw new InvalidOperationException("Cannot edit Cell as DropDownCellRenderer requires a DropDownColumn that uses a DropDownCellEditor");
						//}

						rendererData.ButtonState = ComboBoxStates.Pressed;

						if (!e.Table.IsEditing)
						{
							e.Table.EditCell(e.CellPos);
						}

						((IEditorUsesRendererButtons)e.Table.EditingCellEditor).OnEditorButtonMouseDown(this, e);

						e.Table.Invalidate(e.CellRect);
					}
				}
			}
		}

		#endregion

		#region MouseMove

		/// <summary>
		/// Raises the MouseMove event
		/// </summary>
		/// <param name="e">A CellMouseEventArgs that contains the event data</param>
		public override void OnMouseMove(XPTable.Events.CellMouseEventArgs e)
		{
			base.OnMouseMove(e);

			if (this.ShowDropDownButton || (e.Table.IsEditing && e.CellPos == e.Table.EditingCell))
			{
				if (e.Table.IsCellEditable(e.CellPos))
				{
					// get the button renderer data
					DropDownRendererData rendererData = this.GetDropDownRendererData(e.Cell);

					if (this.CalcDropDownButtonBounds().Contains(e.X, e.Y))
					{
						if (rendererData.ButtonState == ComboBoxStates.Normal)
						{
							if (e.Button == MouseButtons.Left && e.Row == e.Table.LastMouseDownCell.Row && e.Column == e.Table.LastMouseDownCell.Column)
							{
								rendererData.ButtonState = ComboBoxStates.Pressed;
							}
							else
							{
								rendererData.ButtonState = ComboBoxStates.Hot;
							}

							e.Table.Invalidate(e.CellRect);
						}
					}
					else
					{
						if (rendererData.ButtonState != ComboBoxStates.Normal)
						{
							rendererData.ButtonState = ComboBoxStates.Normal;

							e.Table.Invalidate(e.CellRect);
						}
					}
				}
			}
		}

		#endregion

		#endregion

		#region Paint

		/// <summary>
		/// Raises the PaintCell event
		/// </summary>
		/// <param name="e">A PaintCellEventArgs that contains the event data</param>
		public override void OnPaintCell(PaintCellEventArgs e)
		{
			if (e.Table.ColumnModel.Columns[e.Column] is DropDownColumn)
			{
				this.showButton = ((DropDownColumn)e.Table.ColumnModel.Columns[e.Column]).ShowDropDownButton;
			}
			else
			{
				this.showButton = true;
			}

			base.OnPaintCell(e);
		}


		/// <summary>
		/// Paints the Cells background
		/// </summary>
		/// <param name="e">A PaintCellEventArgs that contains the event data</param>
		protected override void OnPaintBackground(PaintCellEventArgs e)
		{
			base.OnPaintBackground(e);

			// don't bother going any further if the Cell is null 
			if (e.Cell == null)
			{
				return;
			}

			// image 
			Rectangle imageRect = this.ClientRectangle;
			imageRect.X -= 16;
			if (e.Cell.Checked)
				e.Graphics.DrawImageUnscaled(Properties.Resources.Accept, imageRect);
			else
				e.Graphics.DrawImageUnscaled(Properties.Resources.Error, imageRect);

			if (this.ShowDropDownButton || (e.Table.IsEditing && e.CellPos == e.Table.EditingCell))
			{
				ComboBoxStates state = this.GetDropDownRendererData(e.Cell).ButtonState;

				if (!e.Enabled)
				{
					state = ComboBoxStates.Disabled;
				}

				Rectangle r = this.CalcDropDownButtonBounds();
				ThemeManager.DrawButton(e.Graphics, r, state);
				Font font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
				e.Graphics.DrawString("...", font, new SolidBrush(Color.Black), r.X + 1, r.Y - 1);
			}
		}

		protected override void OnPaint(PaintCellEventArgs e)
		{
			//base.OnPaint(e);

			// don't bother going any further if the Cell is null 
			if (e.Cell == null)
			{
				return;
			}

			string text = e.Cell.Text;

			if (text != null && text.Length != 0)
			{
				// v1.1.1 fix - removed hardcoded alignment
				Rectangle rect = this.ClientRectangle;
				rect.Y += 1;

				if (e.Enabled)
				{
					TextRenderer.DrawText(e.Graphics, text, this.Font, rect, this.ForeBrush.Color, TextFormatFlags.Left);
				}
				else
				{
					TextRenderer.DrawText(e.Graphics, text, this.Font, rect, SystemColors.GrayText, TextFormatFlags.Left);
				}
			}

			if (e.Focused && e.Enabled)
			{
				ControlPaint.DrawFocusRectangle(e.Graphics, this.ClientRectangle);
			}
		}

		#endregion

		#endregion
	}

	public class FolderCellEditor : GDITextCellEditor, IEditorUsesRendererButtons
	{
		private bool inBrowseMode = false;

		protected override void OnLostFocus(object sender, EventArgs e)
		{
			if (!inBrowseMode && this.EditingTable != null)
			{
				this.EditingTable.StopEditing();
			}
		}

		/// <summary>
		/// Handler for the editors drop down button MouseDown event
		/// </summary>
		/// <param name="sender">The object that raised the event</param>
		/// <param name="e">A CellMouseEventArgs that contains the event data</param>
		public virtual void OnEditorButtonMouseDown(object sender, CellMouseEventArgs e)
		{
			FolderBrowserDialog browser = new FolderBrowserDialog();
			browser.Description = "Select web project folder";
			browser.SelectedPath = TextBox.Text;
			inBrowseMode = true;
			DialogResult res = browser.ShowDialog();
			inBrowseMode = false;
			if (res == DialogResult.OK) TextBox.Text = browser.SelectedPath;
			EditingTable.StopEditing();
		}

		protected override void SetCellValue()
		{
			this.EditingCell.Text = this.TextBox.Text;
		}

		/// <summary>
		/// Handler for the editors drop down button MouseUp event
		/// </summary>
		/// <param name="sender">The object that raised the event</param>
		/// <param name="e">A CellMouseEventArgs that contains the event data</param>
		public virtual void OnEditorButtonMouseUp(object sender, CellMouseEventArgs e)
		{
		}

		protected override void SetEditLocation(Rectangle cellRect)
		{
			// calc the size of the textbox
			ICellRenderer renderer = this.EditingTable.ColumnModel.GetCellRenderer(this.EditingCellPos.Column);
			int buttonWidth = ((FolderCellRenderer)renderer).ButtonWidth;

			this.TextBox.Location = new Point(cellRect.Location.X + 4 + 16, cellRect.Location.Y + 2);
			this.TextBox.Size = new Size(cellRect.Width - 1 - 6 - buttonWidth - 16, cellRect.Height - 2);
		}
	}

	public class GDIImageCellEditor : GDITextCellEditor
	{
		protected override void SetEditLocation(Rectangle cellRect)
		{
			this.TextBox.Location = new Point(cellRect.Location.X + 16 + 6, cellRect.Location.Y + 2);
			this.TextBox.Size = new Size(cellRect.Width - 6 - 16 - 1, cellRect.Height - 2);
		}

		public override void StartEditing()
		{
			this.TextBox.Font = cell.CellStyle.Font;
			this.TextBox.BackColor = Color.Yellow;
			base.StartEditing();
		}
	}

	public class GDIImageCellRenderer : ImageCellRenderer
	{
		#region Constructor

		/// <summary>
		/// Initializes a new instance of the TextCellRenderer class with 
		/// default settings
		/// </summary>
		public GDIImageCellRenderer()
			: base()
		{
			padding.Top += 1;
		}

		#endregion

		public override Rectangle ClientRectangle
		{
			get
			{
				Rectangle client = new Rectangle(this.Bounds.Location, this.Bounds.Size);

				// take borders into account
				client.Width -= Renderer.BorderWidth;
				client.Height -= Renderer.BorderWidth;

				// take cell padding into account
				client.X += this.Padding.Left + 1;
				client.Y += this.Padding.Top + 1;
				client.Width -= this.Padding.Left + this.Padding.Right + 2;
				client.Height -= this.Padding.Top + this.Padding.Bottom + 2;

				return client;
			}
		}

		#region Events

		#region Paint

		/// <summary>
		/// Raises the Paint event
		/// </summary>
		/// <param name="e">A PaintCellEventArgs that contains the event data</param>
		protected override void OnPaint(PaintCellEventArgs e)
		{
			//base.OnPaint(e);

			// don't bother if the Cell is null or doesn't have an image
			if (e.Cell == null)
			{
				return;
			}

			if (e.Cell.Icon != null)
			{
				// work out the size and location of the image
				Rectangle imageRect = this.CalcImageRect(e.Cell.Image, e.Cell.ImageSizeMode, this.LineAlignment, this.Alignment);
				//imageRect.Y -= 1;
				imageRect.X += 1;
				imageRect.Width = 16;
				imageRect.Height = 16;

				e.Graphics.DrawIconUnstretched(e.Cell.Icon, imageRect);
			}
			//else if (e.Cell.Image!=null)
			//{
			//    // work out the size and location of the image
			//    Rectangle imageRect = this.CalcImageRect(e.Cell.Image, e.Cell.ImageSizeMode, this.LineAlignment, this.Alignment);
			//    //imageRect.Y -= 1;
			//    imageRect.X += 1;
			//    imageRect.Width = 16;
			//    imageRect.Height = 16;

			//    // draw the image
			//    bool scaled = false; // (this.DrawText || e.Cell.ImageSizeMode != ImageSizeMode.Normal);
			//    this.DrawImage(e.Graphics, e.Cell.Image, imageRect, scaled, e.Table.Enabled);
			//}

			// check if we need to draw any text
			if (this.DrawText)
			{
				if (e.Cell.Text != null && e.Cell.Text.Length != 0)
				{
					// rectangle the text will be drawn in
					Rectangle textRect = this.ClientRectangle;

					// take the imageRect into account so we don't 
					// draw over it
					textRect.X += 18;//imageRect.Width;
					textRect.Width -= 18; // imageRect.Width;
					textRect.Y += 1;

					// check that we will be able to see the text
					if (textRect.Width > 0)
					{
						// draw the text
						if (e.Enabled)
						{
							TextRenderer.DrawText(e.Graphics, e.Cell.Text, this.Font, textRect, this.ForeBrush.Color, TextFormatFlags.Left);
						}
						else
						{
							TextRenderer.DrawText(e.Graphics, e.Cell.Text, this.Font, textRect, SystemColors.GrayText, TextFormatFlags.Left);
						}
					}
				}
			}

			if (e.Focused && e.Enabled)
			{
				Rectangle rect = this.ClientRectangle;
				rect.X += 19;
				rect.Width -= 19;
				ControlPaint.DrawFocusRectangle(e.Graphics, rect);
			}
		}

		#endregion

		#endregion
	}

	public class ProjectCellRenderer : GDITextCellRenderer
	{
		ImageList imageList;
		Cell mustBeChecked;

		#region Constructor

		/// <summary>
		/// Initializes a new instance of the TextCellRenderer class with 
		/// default settings
		/// </summary>
		public ProjectCellRenderer(ImageList images)
			: base()
		{
			imageList = images;
		}

		#endregion

		public override Rectangle ClientRectangle
		{
			get
			{
				Rectangle client = new Rectangle(this.Bounds.Location, this.Bounds.Size);

				// take borders into account
				client.Width -= Renderer.BorderWidth;
				client.Height -= Renderer.BorderWidth;

				// take cell padding into account
				client.X += this.Padding.Left + 1 + 14 + 16 + 6;
				client.Y += this.Padding.Top + 1;
				client.Width -= this.Padding.Left + this.Padding.Right + 2 + 14 + 16 + 6;
				client.Height -= this.Padding.Top + this.Padding.Bottom + 2;

				return client;
			}
		}

		#region Events

		protected CheckBoxRendererData GetCheckBoxRendererData(Cell cell)
		{
			object rendererData = this.GetRendererData(cell);

			if (rendererData == null || !(rendererData is CheckBoxRendererData))
			{
				if (cell.CheckState == CheckState.Unchecked)
				{
					rendererData = new CheckBoxRendererData(CheckBoxStates.UncheckedNormal);
				}
				else if (cell.CheckState == CheckState.Indeterminate && cell.ThreeState)
				{
					rendererData = new CheckBoxRendererData(CheckBoxStates.MixedNormal);
				}
				else
				{
					rendererData = new CheckBoxRendererData(CheckBoxStates.CheckedNormal);
				}

				this.SetRendererData(cell, rendererData);
			}

			//this.ValidateCheckState(cell, (CheckBoxRendererData)rendererData);

			return (CheckBoxRendererData)rendererData;
		}

		#region Mouse

		#region MouseLeave

		/// <summary>
		/// Raises the MouseLeave event
		/// </summary>
		/// <param name="e">A CellMouseEventArgs that contains the event data</param>
		public override void OnMouseLeave(CellMouseEventArgs e)
		{
			base.OnMouseLeave(e);

			if (e.Table.IsCellEditable(e.CellPos))
			{
				// get the renderer data
				CheckBoxRendererData rendererData = this.GetCheckBoxRendererData(e.Cell);

				if (e.Cell.CheckState == CheckState.Checked)
				{
					if (rendererData.CheckState != CheckBoxStates.CheckedNormal)
					{
						rendererData.CheckState = CheckBoxStates.CheckedNormal;

						e.Table.Invalidate(e.CellRect);
					}
				}
				else if (e.Cell.CheckState == CheckState.Indeterminate)
				{
					if (rendererData.CheckState != CheckBoxStates.MixedNormal)
					{
						rendererData.CheckState = CheckBoxStates.MixedNormal;

						e.Table.Invalidate(e.CellRect);
					}
				}
				else //if (e.Cell.CheckState == CheckState.Unchecked)
				{
					if (rendererData.CheckState != CheckBoxStates.UncheckedNormal)
					{
						rendererData.CheckState = CheckBoxStates.UncheckedNormal;

						e.Table.Invalidate(e.CellRect);
					}
				}
			}
		}

		#endregion

		#region MouseUp

		/// <summary>
		/// Raises the MouseUp event
		/// </summary>
		/// <param name="e">A CellMouseEventArgs that contains the event data</param>
		public override void OnMouseUp(CellMouseEventArgs e)
		{
			base.OnMouseUp(e);

			if (mustBeChecked != null)
			{
				// get the renderer data
				CheckBoxRendererData rendererData = this.GetCheckBoxRendererData(mustBeChecked);

				if (mustBeChecked.CheckState == CheckState.Checked)
				{
					if (!mustBeChecked.ThreeState || !(e.Table.ColumnModel.Columns[e.Column] is CheckBoxColumn) ||
						((CheckBoxColumn)e.Table.ColumnModel.Columns[e.Column]).CheckStyle == CheckBoxColumnStyle.RadioButton)
					{
						rendererData.CheckState = CheckBoxStates.UncheckedHot;
						mustBeChecked.CheckState = CheckState.Unchecked;
					}
					else
					{
						rendererData.CheckState = CheckBoxStates.MixedHot;
						mustBeChecked.CheckState = CheckState.Indeterminate;
					}
				}
				else if (mustBeChecked.CheckState == CheckState.Indeterminate)
				{
					rendererData.CheckState = CheckBoxStates.UncheckedHot;
					mustBeChecked.CheckState = CheckState.Unchecked;
				}
				else //if (mustBeChecked.CheckState == CheckState.Unchecked)
				{
					rendererData.CheckState = CheckBoxStates.CheckedHot;
					mustBeChecked.CheckState = CheckState.Checked;
				}

				e.Table.Invalidate(CalcCheckRectangle());
				mustBeChecked = null;
			}
		}

		#endregion

		#region MouseDown

		/// <summary>
		/// Raises the MouseDown event
		/// </summary>
		/// <param name="e">A CellMouseEventArgs that contains the event data</param>
		public override void OnMouseDown(CellMouseEventArgs e)
		{
			if (e.Table.IsCellEditable(e.CellPos))
			{
				// get the renderer data
				CheckBoxRendererData rendererData = this.GetCheckBoxRendererData(e.Cell);

				if (this.CalcCheckRect(e.Table.TableModel.Rows[e.Row].Alignment, e.Table.ColumnModel.Columns[e.Column].Alignment).Contains(e.X, e.Y))
				{
					//
					if (e.Cell.CheckState == CheckState.Checked)
					{
						rendererData.CheckState = CheckBoxStates.CheckedPressed;
					}
					else if (e.Cell.CheckState == CheckState.Indeterminate)
					{
						rendererData.CheckState = CheckBoxStates.MixedPressed;
					}
					else //if (e.Cell.CheckState == CheckState.Unchecked)
					{
						rendererData.CheckState = CheckBoxStates.UncheckedPressed;
					}

					mustBeChecked = e.Cell;

					e.Table.Invalidate(e.CellRect);
				}
			}
			base.OnMouseDown(e);
		}

		#endregion

		#region MouseMove

		/// <summary>
		/// Raises the MouseMove event
		/// </summary>
		/// <param name="e">A CellMouseEventArgs that contains the event data</param>
		public override void OnMouseMove(XPTable.Events.CellMouseEventArgs e)
		{
			base.OnMouseMove(e);

			if (e.Table.IsCellEditable(e.CellPos))
			{
				// get the renderer data
				CheckBoxRendererData rendererData = this.GetCheckBoxRendererData(e.Cell);

				if (this.CalcCheckRect(e.Table.TableModel.Rows[e.Row].Alignment, e.Table.ColumnModel.Columns[e.Column].Alignment).Contains(e.X, e.Y))
				{
					if (e.Cell.CheckState == CheckState.Checked)
					{
						if (rendererData.CheckState == CheckBoxStates.CheckedNormal)
						{
							if (e.Button == MouseButtons.Left && e.Row == e.Table.LastMouseDownCell.Row && e.Column == e.Table.LastMouseDownCell.Column)
							{
								rendererData.CheckState = CheckBoxStates.CheckedPressed;
							}
							else
							{
								rendererData.CheckState = CheckBoxStates.CheckedHot;
							}

							e.Table.Invalidate(e.CellRect);
						}
					}
					else if (e.Cell.CheckState == CheckState.Indeterminate)
					{
						if (rendererData.CheckState == CheckBoxStates.MixedNormal)
						{
							if (e.Button == MouseButtons.Left && e.Row == e.Table.LastMouseDownCell.Row && e.Column == e.Table.LastMouseDownCell.Column)
							{
								rendererData.CheckState = CheckBoxStates.MixedPressed;
							}
							else
							{
								rendererData.CheckState = CheckBoxStates.MixedHot;
							}

							e.Table.Invalidate(e.CellRect);
						}
					}
					else //if (e.Cell.CheckState == CheckState.Unchecked)
					{
						if (rendererData.CheckState == CheckBoxStates.UncheckedNormal)
						{
							if (e.Button == MouseButtons.Left && e.Row == e.Table.LastMouseDownCell.Row && e.Column == e.Table.LastMouseDownCell.Column)
							{
								rendererData.CheckState = CheckBoxStates.UncheckedPressed;
							}
							else
							{
								rendererData.CheckState = CheckBoxStates.UncheckedHot;
							}

							e.Table.Invalidate(e.CellRect);
						}
					}
				}
				else
				{
					if (e.Cell.CheckState == CheckState.Checked)
					{
						rendererData.CheckState = CheckBoxStates.CheckedNormal;
					}
					else if (e.Cell.CheckState == CheckState.Indeterminate)
					{
						rendererData.CheckState = CheckBoxStates.MixedNormal;
					}
					else //if (e.Cell.CheckState == CheckState.Unchecked)
					{
						rendererData.CheckState = CheckBoxStates.UncheckedNormal;
					}

					e.Table.Invalidate(e.CellRect);
				}
			}
		}

		#endregion

		#endregion

		Rectangle CalcCheckRect(RowAlignment rowAlignment, ColumnAlignment columnAlignment)
		{
			return CalcCheckRectangle();
		}

		Rectangle CalcCheckRectangle()
		{
			Rectangle r = this.ClientRectangle;
			r.Offset(-16, 1);
			r.Width = 16;
			r.Height = 16;
			return r;
		}

		public override void OnKeyDown(CellKeyEventArgs e)
		{
			base.OnKeyDown(e);

			if (e.KeyData == Keys.Space && e.Table.IsCellEditable(e.CellPos))
			{
				// get the renderer data
				CheckBoxRendererData rendererData = this.GetCheckBoxRendererData(e.Cell);

				//
				if (e.Cell.CheckState == CheckState.Checked)
				{
					rendererData.CheckState = CheckBoxStates.CheckedPressed;
				}
				else if (e.Cell.CheckState == CheckState.Indeterminate)
				{
					rendererData.CheckState = CheckBoxStates.MixedPressed;
				}
				else //if (e.Cell.CheckState == CheckState.Unchecked)
				{
					rendererData.CheckState = CheckBoxStates.UncheckedPressed;
				}

				e.Table.Invalidate(e.CellRect);
			}
		}

		public override void OnKeyUp(CellKeyEventArgs e)
		{
			base.OnKeyUp(e);

			if (e.KeyData == Keys.Space && e.Table.IsCellEditable(e.CellPos))
			{
				// get the renderer data
				CheckBoxRendererData rendererData = this.GetCheckBoxRendererData(e.Cell);

				//
				if (e.Cell.CheckState == CheckState.Checked)
				{
					if (!e.Cell.ThreeState || !(e.Table.ColumnModel.Columns[e.Column] is CheckBoxColumn) ||
						((CheckBoxColumn)e.Table.ColumnModel.Columns[e.Column]).CheckStyle == CheckBoxColumnStyle.RadioButton)
					{
						rendererData.CheckState = CheckBoxStates.UncheckedNormal;
						e.Cell.CheckState = CheckState.Unchecked;
					}
					else
					{
						rendererData.CheckState = CheckBoxStates.MixedNormal;
						e.Cell.CheckState = CheckState.Indeterminate;
					}
				}
				else if (e.Cell.CheckState == CheckState.Indeterminate)
				{
					rendererData.CheckState = CheckBoxStates.UncheckedNormal;
					e.Cell.CheckState = CheckState.Unchecked;
				}
				else //if (e.Cell.CheckState == CheckState.Unchecked)
				{
					rendererData.CheckState = CheckBoxStates.CheckedNormal;
					e.Cell.CheckState = CheckState.Checked;
				}

				e.Table.Invalidate(e.CellRect);
			}
		}

		#region Paint

		/// <summary>
		/// Raises the Paint event
		/// </summary>
		/// <param name="e">A PaintCellEventArgs that contains the event data</param>
		protected override void OnPaint(PaintCellEventArgs e)
		{
			base.OnPaint(e);

			// don't bother going any further if the Cell is null 
			if (e.Cell == null)
			{
				return;
			}

			Rectangle r = this.ClientRectangle;
			CheckBoxStates state = this.GetCheckBoxRendererData(e.Cell).CheckState;
			Rectangle r2 = CalcCheckRectangle();
			ThemeManager.DrawCheck(e.Graphics, r2, state);

			int imageIndex = (int)e.Cell.Data;
			if (imageIndex >= 0)
			{
				imageList.Draw(e.Graphics, r.Left - 35, r.Top, imageIndex);
			}

			string text = e.Cell.Text;

			if (text != null && text.Length != 0)
			{
				// v1.1.1 fix - removed hardcoded alignment
				r.Y += 1;
				if (e.Enabled)
				{
					TextRenderer.DrawText(e.Graphics, text, this.Font, r, this.ForeBrush.Color, TextFormatFlags.Left);
				}
				else
				{
					TextRenderer.DrawText(e.Graphics, text, this.Font, r, SystemColors.GrayText, TextFormatFlags.Left);
				}
			}

			if (e.Focused && e.Enabled)
			{
				ControlPaint.DrawFocusRectangle(e.Graphics, this.ClientRectangle);
			}
		}

		#endregion

		#endregion
	}

	public class ProjectCellEditor : GDITextCellEditor
	{
		protected override void SetEditLocation(Rectangle cellRect)
		{
			this.TextBox.Location = new Point(cellRect.Location.X + 4 + 14 + 16 + 6, cellRect.Location.Y + 2);
			this.TextBox.Size = new Size(cellRect.Width - 4 - 1 - 14 - 16 - 6, cellRect.Height - 2);
		}
	}

	public class InfoGDITextCellEditor : GDITextCellEditor
	{
		public override void StartEditing()
		{
			this.TextBox.Font = cell.CellStyle.Font;
			this.TextBox.BackColor = Color.Yellow;
			this.TextBox.ForeColor = Color.Black;
			base.StartEditing();
		}

		public override void StopEditing()
		{
			base.StopEditing();
		}

		protected override void SetEditValue()
		{
			if (EditingCell.CellStyle.ForeColor == Color.Black)
				this.TextBox.Text = this.EditingCell.Text;
			else
				this.TextBox.Text = "";
		}

		protected override void SetCellValue()
		{
			if (TextBox.Text.Length == 0)
				EditingCell.CellStyle.ForeColor = Color.Gray;
			else
				EditingCell.CellStyle.ForeColor = Color.Black;

			this.EditingCell.Text = this.TextBox.Text;
		}
	}

	public class MaskCellRenderer : GDIImageCellRenderer
	{
		Cell mustBeChecked;

		#region Constructor

		/// <summary>
		/// Initializes a new instance of the TextCellRenderer class with 
		/// default settings
		/// </summary>
		public MaskCellRenderer(ImageList images)
			: base()
		{
		}

		#endregion

		public override Rectangle ClientRectangle
		{
			get
			{
				Rectangle client = new Rectangle(this.Bounds.Location, this.Bounds.Size);

				// take borders into account
				client.Width -= Renderer.BorderWidth;
				client.Height -= Renderer.BorderWidth;

				// take cell padding into account
				client.X += this.Padding.Left + 1 + 14 + 16 + 6;
				client.Y += this.Padding.Top + 1;
				client.Width -= this.Padding.Left + this.Padding.Right + 2 + 14 + 16 + 6;
				client.Height -= this.Padding.Top + this.Padding.Bottom + 2;

				return client;
			}
		}

		#region Events

		protected CheckBoxRendererData GetCheckBoxRendererData(Cell cell)
		{
			object rendererData = this.GetRendererData(cell);

			if (rendererData == null || !(rendererData is CheckBoxRendererData))
			{
				if (cell.CheckState == CheckState.Unchecked)
				{
					rendererData = new CheckBoxRendererData(CheckBoxStates.UncheckedNormal);
				}
				else if (cell.CheckState == CheckState.Indeterminate && cell.ThreeState)
				{
					rendererData = new CheckBoxRendererData(CheckBoxStates.MixedNormal);
				}
				else
				{
					rendererData = new CheckBoxRendererData(CheckBoxStates.CheckedNormal);
				}

				this.SetRendererData(cell, rendererData);
			}

			//this.ValidateCheckState(cell, (CheckBoxRendererData)rendererData);

			return (CheckBoxRendererData)rendererData;
		}

		#region Mouse

		#region MouseLeave

		/// <summary>
		/// Raises the MouseLeave event
		/// </summary>
		/// <param name="e">A CellMouseEventArgs that contains the event data</param>
		public override void OnMouseLeave(CellMouseEventArgs e)
		{
			base.OnMouseLeave(e);

			if (e.Table.IsCellEditable(e.CellPos))
			{
				// get the renderer data
				CheckBoxRendererData rendererData = this.GetCheckBoxRendererData(e.Cell);

				if (e.Cell.CheckState == CheckState.Checked)
				{
					if (rendererData.CheckState != CheckBoxStates.CheckedNormal)
					{
						rendererData.CheckState = CheckBoxStates.CheckedNormal;

						e.Table.Invalidate(e.CellRect);
					}
				}
				else if (e.Cell.CheckState == CheckState.Indeterminate)
				{
					if (rendererData.CheckState != CheckBoxStates.MixedNormal)
					{
						rendererData.CheckState = CheckBoxStates.MixedNormal;

						e.Table.Invalidate(e.CellRect);
					}
				}
				else //if (e.Cell.CheckState == CheckState.Unchecked)
				{
					if (rendererData.CheckState != CheckBoxStates.UncheckedNormal)
					{
						rendererData.CheckState = CheckBoxStates.UncheckedNormal;

						e.Table.Invalidate(e.CellRect);
					}
				}
			}
		}

		#endregion

		#region MouseUp

		/// <summary>
		/// Raises the MouseUp event
		/// </summary>
		/// <param name="e">A CellMouseEventArgs that contains the event data</param>
		public override void OnMouseUp(CellMouseEventArgs e)
		{
			base.OnMouseUp(e);

			if (mustBeChecked != null)
			{
				// get the renderer data
				CheckBoxRendererData rendererData = this.GetCheckBoxRendererData(mustBeChecked);

				if (mustBeChecked.CheckState == CheckState.Checked)
				{
					if (!mustBeChecked.ThreeState || !(e.Table.ColumnModel.Columns[e.Column] is CheckBoxColumn) ||
						((CheckBoxColumn)e.Table.ColumnModel.Columns[e.Column]).CheckStyle == CheckBoxColumnStyle.RadioButton)
					{
						rendererData.CheckState = CheckBoxStates.UncheckedHot;
						mustBeChecked.CheckState = CheckState.Unchecked;
					}
					else
					{
						rendererData.CheckState = CheckBoxStates.MixedHot;
						mustBeChecked.CheckState = CheckState.Indeterminate;
					}
				}
				else if (mustBeChecked.CheckState == CheckState.Indeterminate)
				{
					rendererData.CheckState = CheckBoxStates.UncheckedHot;
					mustBeChecked.CheckState = CheckState.Unchecked;
				}
				else //if (mustBeChecked.CheckState == CheckState.Unchecked)
				{
					rendererData.CheckState = CheckBoxStates.CheckedHot;
					mustBeChecked.CheckState = CheckState.Checked;
				}

				e.Table.Invalidate(CalcCheckRectangle());
				mustBeChecked = null;
			}
		}

		#endregion

		#region MouseDown

		/// <summary>
		/// Raises the MouseDown event
		/// </summary>
		/// <param name="e">A CellMouseEventArgs that contains the event data</param>
		public override void OnMouseDown(CellMouseEventArgs e)
		{
			if (e.Table.IsCellEditable(e.CellPos))
			{
				// get the renderer data
				CheckBoxRendererData rendererData = this.GetCheckBoxRendererData(e.Cell);

				if (this.CalcCheckRect(e.Table.TableModel.Rows[e.Row].Alignment, e.Table.ColumnModel.Columns[e.Column].Alignment).Contains(e.X, e.Y))
				{
					//
					if (e.Cell.CheckState == CheckState.Checked)
					{
						rendererData.CheckState = CheckBoxStates.CheckedPressed;
					}
					else if (e.Cell.CheckState == CheckState.Indeterminate)
					{
						rendererData.CheckState = CheckBoxStates.MixedPressed;
					}
					else //if (e.Cell.CheckState == CheckState.Unchecked)
					{
						rendererData.CheckState = CheckBoxStates.UncheckedPressed;
					}

					mustBeChecked = e.Cell;

					e.Table.Invalidate(e.CellRect);
				}
			}
			base.OnMouseDown(e);
		}

		#endregion

		#region MouseMove

		/// <summary>
		/// Raises the MouseMove event
		/// </summary>
		/// <param name="e">A CellMouseEventArgs that contains the event data</param>
		public override void OnMouseMove(XPTable.Events.CellMouseEventArgs e)
		{
			base.OnMouseMove(e);

			if (e.Table.IsCellEditable(e.CellPos))
			{
				// get the renderer data
				CheckBoxRendererData rendererData = this.GetCheckBoxRendererData(e.Cell);

				if (this.CalcCheckRect(e.Table.TableModel.Rows[e.Row].Alignment, e.Table.ColumnModel.Columns[e.Column].Alignment).Contains(e.X, e.Y))
				{
					if (e.Cell.CheckState == CheckState.Checked)
					{
						if (rendererData.CheckState == CheckBoxStates.CheckedNormal)
						{
							if (e.Button == MouseButtons.Left && e.Row == e.Table.LastMouseDownCell.Row && e.Column == e.Table.LastMouseDownCell.Column)
							{
								rendererData.CheckState = CheckBoxStates.CheckedPressed;
							}
							else
							{
								rendererData.CheckState = CheckBoxStates.CheckedHot;
							}

							e.Table.Invalidate(e.CellRect);
						}
					}
					else if (e.Cell.CheckState == CheckState.Indeterminate)
					{
						if (rendererData.CheckState == CheckBoxStates.MixedNormal)
						{
							if (e.Button == MouseButtons.Left && e.Row == e.Table.LastMouseDownCell.Row && e.Column == e.Table.LastMouseDownCell.Column)
							{
								rendererData.CheckState = CheckBoxStates.MixedPressed;
							}
							else
							{
								rendererData.CheckState = CheckBoxStates.MixedHot;
							}

							e.Table.Invalidate(e.CellRect);
						}
					}
					else //if (e.Cell.CheckState == CheckState.Unchecked)
					{
						if (rendererData.CheckState == CheckBoxStates.UncheckedNormal)
						{
							if (e.Button == MouseButtons.Left && e.Row == e.Table.LastMouseDownCell.Row && e.Column == e.Table.LastMouseDownCell.Column)
							{
								rendererData.CheckState = CheckBoxStates.UncheckedPressed;
							}
							else
							{
								rendererData.CheckState = CheckBoxStates.UncheckedHot;
							}

							e.Table.Invalidate(e.CellRect);
						}
					}
				}
				else
				{
					if (e.Cell.CheckState == CheckState.Checked)
					{
						rendererData.CheckState = CheckBoxStates.CheckedNormal;
					}
					else if (e.Cell.CheckState == CheckState.Indeterminate)
					{
						rendererData.CheckState = CheckBoxStates.MixedNormal;
					}
					else //if (e.Cell.CheckState == CheckState.Unchecked)
					{
						rendererData.CheckState = CheckBoxStates.UncheckedNormal;
					}

					e.Table.Invalidate(e.CellRect);
				}
			}
		}

		#endregion

		#endregion

		Rectangle CalcCheckRect(RowAlignment rowAlignment, ColumnAlignment columnAlignment)
		{
			return CalcCheckRectangle();
		}

		Rectangle CalcCheckRectangle()
		{
			Rectangle r = this.ClientRectangle;
			r.Offset(-35, 1);
			r.Width = 16;
			r.Height = 16;
			return r;
		}

		public override void OnKeyDown(CellKeyEventArgs e)
		{
			base.OnKeyDown(e);

			if (e.KeyData == Keys.Space && e.Table.IsCellEditable(e.CellPos))
			{
				// get the renderer data
				CheckBoxRendererData rendererData = this.GetCheckBoxRendererData(e.Cell);

				//
				if (e.Cell.CheckState == CheckState.Checked)
				{
					rendererData.CheckState = CheckBoxStates.CheckedPressed;
				}
				else if (e.Cell.CheckState == CheckState.Indeterminate)
				{
					rendererData.CheckState = CheckBoxStates.MixedPressed;
				}
				else //if (e.Cell.CheckState == CheckState.Unchecked)
				{
					rendererData.CheckState = CheckBoxStates.UncheckedPressed;
				}

				e.Table.Invalidate(e.CellRect);
			}
		}

		public override void OnKeyUp(CellKeyEventArgs e)
		{
			base.OnKeyUp(e);

			if (e.KeyData == Keys.Space && e.Table.IsCellEditable(e.CellPos))
			{
				// get the renderer data
				CheckBoxRendererData rendererData = this.GetCheckBoxRendererData(e.Cell);

				//
				if (e.Cell.CheckState == CheckState.Checked)
				{
					if (!e.Cell.ThreeState || !(e.Table.ColumnModel.Columns[e.Column] is CheckBoxColumn) ||
						((CheckBoxColumn)e.Table.ColumnModel.Columns[e.Column]).CheckStyle == CheckBoxColumnStyle.RadioButton)
					{
						rendererData.CheckState = CheckBoxStates.UncheckedNormal;
						e.Cell.CheckState = CheckState.Unchecked;
					}
					else
					{
						rendererData.CheckState = CheckBoxStates.MixedNormal;
						e.Cell.CheckState = CheckState.Indeterminate;
					}
				}
				else if (e.Cell.CheckState == CheckState.Indeterminate)
				{
					rendererData.CheckState = CheckBoxStates.UncheckedNormal;
					e.Cell.CheckState = CheckState.Unchecked;
				}
				else //if (e.Cell.CheckState == CheckState.Unchecked)
				{
					rendererData.CheckState = CheckBoxStates.CheckedNormal;
					e.Cell.CheckState = CheckState.Checked;
				}

				e.Table.Invalidate(e.CellRect);
			}
		}

		#region Paint

		/// <summary>
		/// Raises the Paint event
		/// </summary>
		/// <param name="e">A PaintCellEventArgs that contains the event data</param>
		protected override void OnPaint(PaintCellEventArgs e)
		{
			//base.OnPaint(e);

			// don't bother going any further if the Cell is null 
			if (e.Cell == null)
			{
				return;
			}

			Rectangle r = this.ClientRectangle;
			CheckBoxStates state = this.GetCheckBoxRendererData(e.Cell).CheckState;
			Rectangle r2 = CalcCheckRectangle();
			ThemeManager.DrawCheck(e.Graphics, r2, state);

			if (e.Cell.Icon != null)
			{
				// work out the size and location of the image
				Rectangle imageRect = this.CalcImageRect(e.Cell.Image, e.Cell.ImageSizeMode, this.LineAlignment, this.Alignment);
				//imageRect.Y -= 1;
				imageRect.X -=16;
				imageRect.Width = 16;
				imageRect.Height = 16;

				e.Graphics.DrawIconUnstretched(e.Cell.Icon, imageRect);
			}

			//int imageIndex = (int)e.Cell.Data;
			//if (imageIndex >= 0)
			//{
			//    imageList.Draw(e.Graphics, r.Left - 35, r.Top, imageIndex);
			//}

			string text = e.Cell.Text;

			if (text != null && text.Length != 0)
			{
				// v1.1.1 fix - removed hardcoded alignment
				r.Y += 1;
				if (e.Enabled)
				{
					if (e.Cell.Checked)
					{
						TextRenderer.DrawText(e.Graphics, text, this.Font, r, Color.Blue, TextFormatFlags.Left);
					}
					else
					{
						TextRenderer.DrawText(e.Graphics, text, this.Font, r, this.ForeBrush.Color, TextFormatFlags.Left);
					}
				}
				else
				{
					TextRenderer.DrawText(e.Graphics, text, this.Font, r, SystemColors.GrayText, TextFormatFlags.Left);
				}
			}

			if (e.Focused && e.Enabled)
			{
				ControlPaint.DrawFocusRectangle(e.Graphics, this.ClientRectangle);
			}
		}

		#endregion

		#endregion
	}

	public class MaskCellEditor : GDIImageCellEditor
	{
		protected override void SetEditLocation(Rectangle cellRect)
		{
			this.TextBox.Location = new Point(cellRect.Location.X + 4 + 14 + 16 + 6, cellRect.Location.Y + 2);
			this.TextBox.Size = new Size(cellRect.Width - 4 - 1 - 14 - 16 - 6, cellRect.Height - 2);
		}
	}
}