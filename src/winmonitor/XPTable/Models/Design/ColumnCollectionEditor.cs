/*
 * Copyright © 2005, Mathew Hall
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 *    - Redistributions of source code must retain the above copyright notice, 
 *      this list of conditions and the following disclaimer.
 * 
 *    - Redistributions in binary form must reproduce the above copyright notice, 
 *      this list of conditions and the following disclaimer in the documentation 
 *      and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 */


using System;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Drawing;
using System.Windows.Forms;

using XPTable.Events;
using XPTable.Models;


namespace XPTable.Models.Design
{
	/// <summary>
	/// Provides a user interface that can edit collections of Columns 
	/// at design time
	/// </summary>
	public class ColumnCollectionEditor : HelpfulCollectionEditor
	{
		#region Class Data
		
		/// <summary>
		/// The ColumnCollection being edited
		/// </summary>
		private ColumnCollection columns;

		/// <summary>
		/// Preview table
		/// </summary>
		private Table previewTable;

		/// <summary>
		/// ColumnModel for the preview table
		/// </summary>
		private ColumnModel previewColumnModel;

		/// <summary>
		/// TableModel for the preview table
		/// </summary>
		private TableModel previewTableModel;

		/// <summary>
		/// 
		/// </summary>
		private Label previewLabel;

		#endregion

		
		#region Constructor
		
		/// <summary>
		/// Initializes a new instance of the ColumnCollectionEditor class 
		/// using the specified collection type
		/// </summary>
		/// <param name="type">The type of the collection for this editor to edit</param>
		public ColumnCollectionEditor(Type type) : base(type)
		{
			this.columns = null;

			this.previewColumnModel = new ColumnModel();
			this.previewColumnModel.Columns.Add(new TextColumn("Column", 116));
			
			this.previewTableModel = new TableModel();
			this.previewTableModel.Rows.Add(new Row());
			
			Cell cell = new Cell();
			cell.Editable = false;
			cell.ToolTipText = "This is a Cell ToolTip";
			
			this.previewTableModel.Rows[0].Cells.Add(cell);
			this.previewTableModel.RowHeight = 20;

			this.previewTable = new Table();
			this.previewTable.Preview = true;
			this.previewTable.Size = new Size(120, 274);
			this.previewTable.Location = new Point(246, 24);
			this.previewTable.GridLines = GridLines.Both;
			this.previewTable.TabStop = false;
			this.previewTable.EnableToolTips = true;
			this.previewTable.ColumnModel = this.previewColumnModel;
			this.previewTable.TableModel = this.previewTableModel;

			this.previewLabel = new Label();
			this.previewLabel.Text = "Preview:";
			this.previewLabel.Size = new Size(140, 16);
			this.previewLabel.Location = new Point(247, 8);
		}

		#endregion
		
		
		#region Methods

		/// <summary>
		/// Edits the value of the specified object using the specified 
		/// service provider and context
		/// </summary>
		/// <param name="context">An ITypeDescriptorContext that can be 
		/// used to gain additional context information</param>
		/// <param name="isp">A service provider object through which 
		/// editing services can be obtained</param>
		/// <param name="value">The object to edit the value of</param>
		/// <returns>The new value of the object. If the value of the 
		/// object has not changed, this should return the same object 
		/// it was passed</returns>
		public override object EditValue(ITypeDescriptorContext context, IServiceProvider isp, object value)
		{
			this.columns = (ColumnCollection) value;

			// for some reason (might be beacause Column is an 
			// abstract class) the table doesn't get redrawn 
			// when a columns property changes, but we can get 
			// around that by subscribing to the columns 
			// PropertyChange event and passing the message on 
			// to the table ourselves.  we need to do this for 
			// all the existing columns in the collection
			for (int i=0; i<this.columns.Count; i++)
			{
				this.columns[i].PropertyChanged += new ColumnEventHandler(column_PropertyChanged);
			}

			object returnObject = base.EditValue(context, isp, value);

			ColumnModel model = (ColumnModel) context.Instance;
			
			if (model.Table != null)
			{
				model.Table.PerformLayout();
				model.Table.Refresh();
			}
			
			return returnObject;
		}


		/// <summary>
		/// Gets the data types that this collection editor can contain
		/// </summary>
		/// <returns>An array of data types that this collection can contain</returns>
		protected override Type[] CreateNewItemTypes()
		{
			return new Type[] {typeof(TextColumn),
								  typeof(ButtonColumn),
								  typeof(CheckBoxColumn),
								  typeof(ColorColumn),
								  typeof(ComboBoxColumn),
								  typeof(DateTimeColumn),
								  typeof(ImageColumn),
								  typeof(NumberColumn),
								  typeof(ProgressBarColumn)};
		}


		/// <summary>
		/// Creates a new instance of the specified collection item type
		/// </summary>
		/// <param name="itemType">The type of item to create</param>
		/// <returns>A new instance of the specified object</returns>
		protected override object CreateInstance(Type itemType)
		{
			Column column = (Column) base.CreateInstance(itemType);

			// newly created items aren't added to the collection 
			// until editing has finished.  we'd like the newly 
			// created column to show up in the table immediately
			// so we'll add it to the ColumnCollection now
			this.columns.Add(column);

			// for some reason (might be beacause Column is an 
			// abstract class) the table doesn't get redrawn 
			// when a columns property changes, but we can get 
			// around that by subscribing to the columns 
			// PropertyChange event and passing the message on 
			// to the table ourselves
			column.PropertyChanged += new XPTable.Events.ColumnEventHandler(column_PropertyChanged);

			return column;
		}


		/// <summary>
		/// Destroys the specified instance of the object
		/// </summary>
		/// <param name="instance">The object to destroy</param>
		protected override void DestroyInstance(object instance)
		{
			if (instance != null && instance is Column)
			{
				Column column = (Column) instance;

				// the specified column is about to be destroyed 
				// so we need to remove it from the ColumnCollection first
				this.columns.Remove(column);
				column.PropertyChanged -= new XPTable.Events.ColumnEventHandler(column_PropertyChanged);
			}
			
			base.DestroyInstance(instance);
		}


		/// <summary>
		/// Creates a new form to display and edit the current collection
		/// </summary>
		/// <returns>An instance of CollectionEditor.CollectionForm to provide 
		/// as the user interface for editing the collection</returns>
		protected override CollectionEditor.CollectionForm CreateCollectionForm()
		{
			CollectionEditor.CollectionForm editor = base.CreateCollectionForm();

			editor.Width += 140;

			foreach (Control control in editor.Controls)
			{
				if (control.Name.Equals("propertiesLabel"))
				{
					control.Location = new Point(control.Left + 140, control.Top);
				}
				
				//
				if (control is PropertyGrid)
				{
					PropertyGrid grid = (PropertyGrid) control;
					
					grid.SelectedObjectsChanged += new EventHandler(this.PropertyGrid_SelectedObjectsChanged);
					grid.Location = new Point(grid.Left + 140, grid.Top);
					grid.Width -= 140;
				}
			}

			editor.Controls.Add(this.previewLabel);
			editor.Controls.Add(this.previewTable);

			return editor;
		}

		#endregion


		#region Events

		/// <summary>
		/// Handler for the PropertyGrid's SelectedObjectsChanged event
		/// </summary>
		/// <param name="sender">The object that raised the event</param>
		/// <param name="e">An EventArgs that contains the event data</param>
		protected void PropertyGrid_SelectedObjectsChanged(object sender, EventArgs e)
		{
			object[] objects = ((PropertyGrid) sender).SelectedObjects;

			this.previewColumnModel.Columns.Clear();

			if (objects.Length == 1)
			{
				Column column = (Column) objects[0];
				Cell cell = this.previewTableModel[0, 0];

				if (column is ButtonColumn)
				{
					cell.Text = "Button";
					cell.Data = null;
				}
				else if (column is CheckBoxColumn)
				{
					cell.Text = "Checkbox";
					cell.Data = null;
					cell.Checked = true;
				}
				else if (column is ColorColumn)
				{
					cell.Text = null;
					cell.Data = Color.Red;
				}
				else if (column is ComboBoxColumn)
				{
					cell.Text = "ComboBox";
					cell.Data = null;
				}
				else if (column is DateTimeColumn)
				{
					cell.Text = null;
					cell.Data = DateTime.Now;
				}
				else if (column is ImageColumn)
				{
					cell.Text = "Image";
					cell.Data = null;
				}
				else if (column is NumberColumn || column is ProgressBarColumn)
				{
					cell.Text = null;
					cell.Data = 50;
				}
				else //if (column is TextColumn)
				{
					cell.Text = "Text";
					cell.Data = null;
				}
				
				this.previewColumnModel.Columns.Add(column);
			}

			this.previewTable.Invalidate();
		}


		/// <summary>
		/// Handler for a Column's PropertyChanged event
		/// </summary>
		/// <param name="sender">The object that raised the event</param>
		/// <param name="e">A ColumnEventArgs that contains the event data</param>
		private void column_PropertyChanged(object sender, ColumnEventArgs e)
		{
			this.columns.ColumnModel.OnColumnPropertyChanged(e);
		}

		#endregion
	}
}
