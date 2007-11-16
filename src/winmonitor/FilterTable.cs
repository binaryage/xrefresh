using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;
using XPTable.Events;
using XPTable.Models;

namespace XRefresh
{
	public partial class FilterTable : Table
	{
		public delegate void ChangedHandler(object sender, CellEventArgs e);
		public event ChangedHandler Changed;
		private bool dualMode;
		private bool globalMode;

		public FilterTable(bool dualMode, bool globalMode)
		{
			this.dualMode = dualMode;
			this.globalMode = globalMode;
			InitializeComponent();
			SetupTable();
			KeyDown += new KeyEventHandler(table_KeyDown);
			DragDrop += new DragEventHandler(OnDragDrop);
			DragEnter += new DragEventHandler(OnDragEnter);
			MouseDoubleClick += new MouseEventHandler(table_MouseDoubleClick);
			CellPropertyChanged += new XPTable.Events.CellEventHandler(table_CellPropertyChanged);
		}

		public void SetupTable()
		{
			SuspendLayout();
			AllowDrop = true;

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
			table.Name = "FilterTable";
			table.NoItemsText = "There are no file filters. Drag and drop a file here ...";
			table.TabIndex = 1;
			table.TableModel = tableModel;
			table.Text = "FilterTable";
			table.TableModel.Rows.Clear();

			// columnModel
			columnModel.Columns.Clear();
			columnModel.Columns.AddRange(new XPTable.Models.Column[] {
			maskColumn,
			infoColumn});

			// maskColumn
			maskColumn.Text = "Mask";
			maskColumn.Width = 120;
			if (dualMode)
			{
				maskColumn.Renderer = new MaskCellRenderer(null);
				maskColumn.Editor = new MaskCellEditor();
			}
			else
			{
				maskColumn.Renderer = new GDIImageCellRenderer();
				maskColumn.Editor = new GDIImageCellEditor();
			}
			maskColumn.Editable = true;

			// infoColumn
			infoColumn.Text = "Info";
			infoColumn.Width = 170;
			infoColumn.Renderer = new GDITextCellRenderer();
			infoColumn.Editor = new InfoGDITextCellEditor();

			ResumeLayout(true);
		}

		void table_KeyDown(object sender, KeyEventArgs e)
		{
			if (!AllowSelection) return;

			// handle delete action
			if (e.KeyCode == Keys.Delete)
			{
				if (TableModel.Selections.SelectedItems.Length > 0)
				{
					if (TableModel.Selections.SelectedItems[0].Editable)
						DeleteRow(TableModel.Selections.SelectedItems[0]);
				}
			}
		}

		void table_CellPropertyChanged(object sender, XPTable.Events.CellEventArgs e)
		{
			if (e.EventType != CellEventType.ValueChanged) return;
			if (Changed != null) Changed(this, e);
			UpdateIconAndInfo(e.Row);
		}

		void UpdateIconAndInfo(int row)
		{
			string mask = TableModel.Rows[row].Cells[0].Text;
			TableModel.Rows[row].Cells[0].Icon = GetIconForMask(mask);
			if (TableModel.Rows[row].Cells[1].CellStyle.ForeColor==Color.Gray)
			{
				TableModel.Rows[row].Cells[1].Text = GetInfoForMask(mask);
			}
		}

		private void DeleteRow(Row row)
		{
			//DialogResult res = MessageBox.Show(String.Format("Do you really want to remove mask {0}?", row.Cells[0].Text), "Delete item", MessageBoxButtons.OKCancel, MessageBoxIcon.Question);
			//if (res != DialogResult.OK) return;

			int index = row.Index;
			TableModel.Rows.RemoveAt(index);
			if (TableModel.Rows.Count <= index && index > 0) index--;
			TableModel.Selections.SelectCells(index, 0, index, 2);
			if (Changed != null) Changed(this, null);
		}

		void table_MouseDoubleClick(object sender, MouseEventArgs e)
		{
			if (!AllowSelection) return;

			int cindex = ColumnIndexAt(e.X, e.Y);
			if (cindex!=0) return;

			int rindex = RowIndexAt(e.X, e.Y, true); // take virtual row
			if (rindex < 0) return;
			if (TableModel.Rows.Count > rindex) return;

			// we need to create new row and start edit mode
			int rowsToAdd = rindex - TableModel.Rows.Count + 1;
			while (rowsToAdd > 0)
			{
				VisualizeFilter("");
				rowsToAdd--;
			}

			lastMouseCell = new CellPos(rindex, 0);
			FocusedCell = lastMouseCell;
			TableModel.Selections.SelectCell(lastMouseCell);
			base.OnDoubleClick(e);
		}
        
		public void VisualizeFilter(string mask)
		{
			VisualizeFilter(mask, "", !globalMode);
		}

		public void VisualizeFilter(string mask, string info, bool local)
		{
			Row row = new Row();

			Cell cell1 = new Cell();
			CellStyle cellStyle1 = new CellStyle();
			Cell cell2 = new Cell();
			CellStyle cellStyle2 = new CellStyle();

			cell1.Text = mask;
			cell1.Checked = !local;
			cell1.Icon = GetIconForMask(mask);
			cell1.Image = Properties.Resources.Information; // HACK: fake bitmap with same size as icon
			if (info.Length > 0)
				cell2.Text = info;
			else
				cell2.Text = GetInfoForMask(mask);

			cellStyle1.Font = new System.Drawing.Font("Courier", 8.00F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			if (globalMode)
			{
				cellStyle1.ForeColor = Color.Blue;
			}

			cell1.CellStyle = cellStyle1;
			cellStyle2.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			cell2.CellStyle = cellStyle2;
			if (info.Length > 0)
			{
				cell2.ForeColor = Color.Black;
			}
			else
			{
				cell2.ForeColor = Color.Gray;
			}

			row.Cells.AddRange(new XPTable.Models.Cell[] { cell1, cell2 });
			row.ChildIndex = 0;
			row.Editable = true;

			tableModel.Rows.Add(row);
		}

		public void AddRow(string mask, string info)
		{
			VisualizeFilter(mask, info, !globalMode);
			int row = tableModel.Rows.Count - 1;
			tableModel.Selections.SelectCells(row, 0, row, 2);
			EnsureVisible(row, 0);
			if (Changed != null) Changed(this, null);
		}

		public void RemoveRow(string mask)
		{
			string test = mask.ToLower();
			foreach (Row row in tableModel.Rows)
			{
				if (row.Cells[0].Text.ToLower()==test)
				{
					DeleteRow(row);
					return;
				}
			}
		}

		private string GetInfoForMask(string mask)
		{
			if (mask.Length == 0) return "";
			FileMask fileMask = new FileMask(mask);
			if (fileMask.type == FileMask.Type.Mask)
			{
				string ext = mask.Substring(2);
				return ShellIcon.GetTypeInfo("x." + ext);
			}
			return fileMask.GetTypeInfo();
		}

		protected Icon GetIconForMask(string mask)
		{
			if (mask.Length == 0) return null;
			if (mask.StartsWith("*."))
			{
				string ext = mask.Substring(2);
				return ShellIcon.GetSmallTypeIcon("x."+ext);
			}
			return null;
		}

		public void InitFrom(Model.GlobalIncludeFiltersDataTable includes)
		{
			TableModel.Rows.Clear();

			foreach (Model.GlobalIncludeFiltersRow row in includes)
			{
				VisualizeFilter(row.Mask, row.Info, false);
			}
		}

		public void InitFrom(Model.GlobalExcludeFiltersDataTable excludes)
		{
			TableModel.Rows.Clear();

			foreach (Model.GlobalExcludeFiltersRow row in excludes)
			{
				VisualizeFilter(row.Mask, row.Info, false);
			}
		}

		public void InitFrom(Model.IncludeFiltersRow[] includes, Model.GlobalIncludeFiltersDataTable gincludes)
		{
			TableModel.Rows.Clear();

			if (gincludes!=null)
			{
				foreach (Model.GlobalIncludeFiltersRow row in gincludes)
				{
					VisualizeFilter(row.Mask, row.Info, false);
				}
			}

			foreach (Model.IncludeFiltersRow row in includes)
			{
				VisualizeFilter(row.Mask, row.Info, true);
			}
		}

		public void InitFrom(Model.ExcludeFiltersRow[] excludes, Model.GlobalExcludeFiltersDataTable gexcludes)
		{
			TableModel.Rows.Clear();

			if (gexcludes != null)
			{
				foreach (Model.GlobalExcludeFiltersRow row in gexcludes)
				{
					VisualizeFilter(row.Mask, row.Info, false);
				}
			}

			foreach (Model.ExcludeFiltersRow row in excludes)
			{
				VisualizeFilter(row.Mask, row.Info, true);
			}
		}

		internal void SaveToIncludes(Model.FoldersRow folder)
		{
			folder.ClearIncludes();
			Model model = folder.Table.DataSet as Model;
			foreach (Row row in TableModel.Rows)
			{
				if (!row.Cells[0].Checked && row.Cells[0].Text.Length>0)
				{
					if (row.Cells[1].ForeColor == Color.Gray)
						model.IncludeFilters.AddIncludeFiltersRow(row.Cells[0].Text, null, folder);
					else
						model.IncludeFilters.AddIncludeFiltersRow(row.Cells[0].Text, row.Cells[1].Text, folder);
				}
			}
		}

		internal void SaveToExcludes(Model.FoldersRow folder)
		{
			folder.ClearExcludes();
			Model model = folder.Table.DataSet as Model;
			foreach (Row row in TableModel.Rows)
			{
				if (!row.Cells[0].Checked && row.Cells[0].Text.Length > 0)
				{
					if (row.Cells[1].ForeColor == Color.Gray)
						model.ExcludeFilters.AddExcludeFiltersRow(row.Cells[0].Text, null, folder);
					else
						model.ExcludeFilters.AddExcludeFiltersRow(row.Cells[0].Text, row.Cells[1].Text, folder);
				}
			}
		}

		internal void SaveToGlobalIncludes(Model model)
		{
			model.GlobalIncludeFilters.Clear();

			foreach (Row row in TableModel.Rows)
			{
				if (row.Cells[0].Checked && row.Cells[0].Text.Length > 0)
				{
					if (row.Cells[1].ForeColor == Color.Gray)
						model.GlobalIncludeFilters.AddGlobalIncludeFiltersRow(row.Cells[0].Text, null);
					else
						model.GlobalIncludeFilters.AddGlobalIncludeFiltersRow(row.Cells[0].Text, row.Cells[1].Text);
				}
			}
		}

		internal void SaveToGlobalExcludes(Model model)
		{
			model.GlobalExcludeFilters.Clear();

			foreach (Row row in TableModel.Rows)
			{
				if (row.Cells[0].Checked && row.Cells[0].Text.Length > 0)
				{
					if (row.Cells[1].ForeColor == Color.Gray)
						model.GlobalExcludeFilters.AddGlobalExcludeFiltersRow(row.Cells[0].Text, null);
					else
						model.GlobalExcludeFilters.AddGlobalExcludeFiltersRow(row.Cells[0].Text, row.Cells[1].Text);
				}
			}
		}

		private void OnDragDrop(object sender, DragEventArgs e)
		{
			if (!e.Data.GetDataPresent(DataFormats.FileDrop)) return;

			Array a = (Array)e.Data.GetData(DataFormats.FileDrop);
			if (a == null) return;

			foreach (object file in a)
			{
				string filename = file.ToString();
				string ext = Path.GetExtension(filename);
				if (ext.Length>0)
				{
					string filter = "*" + ext;
					if (!ContainsMask(filter)) VisualizeFilter("*"+ext);
				}
			}
		}

		private void OnDragEnter(object sender, DragEventArgs e)
		{
			e.Effect = DragDropEffects.None;
			if (!e.Data.GetDataPresent(DataFormats.FileDrop)) return;
			Array a = (Array)e.Data.GetData(DataFormats.FileDrop);
			if (a == null) return;
			if (a.Length == 0) return;

			foreach (object file in a)
			{
				string filename = file.ToString();
				string ext = Path.GetExtension(filename);
				if (ext.Length > 0)
				{
					string filter = "*" + ext;
					if (!ContainsMask(filter)) 
					{
						e.Effect = DragDropEffects.Link;
					}
				}
			}
		}

		public bool ContainsMask(string filter)
		{
			foreach (XPTable.Models.Row row in tableModel.Rows)
			{
				if (row.Cells[0].Text.ToLower() == filter.ToLower()) return true;
			}
			return false;
		}

		internal void ToggleDualMode()
		{
			dualMode = !dualMode;
		}
	}
}