using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Threading;
using System.Windows.Forms;
using XPTable.Models;

namespace XRefresh
{
	public partial class ActivityLog : Form
	{
		const int MAX_EVENT_COUNT = 100;

		class LogEvent 
		{
			public Bitmap icon;
			public DateTime time;
			public string text;
			public Pair<Bitmap, string>[] lines;
			public bool presented;

			public LogEvent(Bitmap icon, DateTime time, string text, Pair<Bitmap, string>[] lines)
			{
				this.icon = icon;
				this.time = time;
				this.text = text;
				this.lines = lines;
				presented = false;
			}

			public LogEvent(Bitmap icon, DateTime time, string text)
			{
				this.icon = icon;
				this.time = time;
				this.text = text;
				this.lines = new Pair<Bitmap, string>[0];
				presented = false;
			}
		}

		class LogEvents : List<LogEvent> 
		{
			public delegate void EventRemovedHandler(LogEvent e);
			public event EventRemovedHandler EventRemoved;

			public void AddEvent(LogEvent e)
			{
				Add(e);

				// keep limit count
				while (Count>MAX_EVENT_COUNT) 
				{
					if (EventRemoved!=null)
					{
						EventRemoved(GetRange(0,1)[0]);
					}
					RemoveAt(0);
				}
			}
		}

		/////////////////////////////////////////////////////////////////////////////

		public static ActivityLog Current;
		LogEvents events = new LogEvents();
		private delegate void UpdateEventsDelegate();
		private delegate void RemoveRowDelegate(Row row);
		Dictionary<LogEvent, Row> publishedEvents = new Dictionary<LogEvent, Row>();

		public ActivityLog()
		{
			Current = this;
			InitializeComponent();
			SetupTable();
			events.EventRemoved += new LogEvents.EventRemovedHandler(events_EventRemoved);
			table.MouseClick +=new MouseEventHandler(table_MouseClick);
		}

		void events_EventRemoved(ActivityLog.LogEvent e)
		{
			Row row;
			if (publishedEvents.TryGetValue(e, out row))
			{
				if (IsHandleCreated)
				{
					BeginInvoke(new RemoveRowDelegate(RemoveRow), new object[] { row } );
				}
				publishedEvents.Remove(e);
			}
		}

		void RemoveRow(Row row)
		{
			foreach (Row subrow in row.SubRows)
			{
				table.TableModel.Rows.Remove(subrow);
			}
			table.TableModel.Rows.Remove(row);
		}

		public void SetupTable()
		{
			SuspendLayout();

			// table
			table.ColumnModel = columnModel;
			table.Dock = System.Windows.Forms.DockStyle.Fill;
			table.GridColor = System.Drawing.Color.WhiteSmoke;
			table.GridLines = XPTable.Models.GridLines.Rows;
			table.Location = new System.Drawing.Point(0, 0);
			table.Name = "table";
			table.Size = new System.Drawing.Size(700, 400);
			table.TabIndex = 0;
			table.TableModel = tableModel;
			table.AllowSelection = true;
			table.FullRowSelect = true;
			table.SelectionStyle = XPTable.Models.SelectionStyle.ListView;
			table.MultiSelect = true;
			table.EnableToolTips = true;

			// columnModel
			columnModel.Columns.AddRange(new XPTable.Models.Column[] {
			timeColumn,
			textColumn});

			// timeColumn
			timeColumn.DateTimeFormat = DateTimePickerFormat.Custom;
			timeColumn.Format = "HH:mm:ss";
			timeColumn.Editable = false;
			timeColumn.ShowDropDownButton = false;
			timeColumn.Text = "Time";
			timeColumn.ToolTipText = "Event Time";
			timeColumn.Width = 60;

			// textColumn
			textColumn.Text = "Event";
			textColumn.ToolTipText = "Event Description";
			textColumn.Width = 630;
			textColumn.Editable = false;

			ResumeLayout(true);
		}

		void table_MouseClick(object sender, MouseEventArgs ev)
		{
			int rindex = table.RowIndexAt(ev.X, ev.Y);
			if (rindex < 0) return;
			if (rindex >= table.TableModel.Rows.Count) return;
			XPTable.Models.Row row = table.TableModel.Rows[rindex];
			if (row.Parent != null) return;

			LogEvent e = row.Tag as LogEvent;
			if (row.SubRows.Count>0)
			{
				int index = row.Index + 1;
				while (true)
				{
					if (table.TableModel.Rows.Count<=index) break;
					XPTable.Models.Row xrow = table.TableModel.Rows[index];
					if (xrow.ChildIndex==0) break;
					table.TableModel.Rows.Remove(xrow);
				}
				row.SubRows.Clear();
			}
			else
			{
				foreach (Pair<Bitmap, string> line in e.lines)
				{
					XPTable.Models.Row subrow = new XPTable.Models.Row();

					XPTable.Models.Cell subcell1 = new XPTable.Models.Cell();
					XPTable.Models.CellStyle subcellStyle1 = new XPTable.Models.CellStyle();
					XPTable.Models.Cell subcell2 = new XPTable.Models.Cell();
					XPTable.Models.CellStyle subcellStyle2 = new XPTable.Models.CellStyle();

					subcell2.Image = line.First;
					subcell2.Text = line.Second;
					subcell2.ColSpan = 2;
					subcell2.ForeColor = Color.Gray;

					subrow.Cells.Add(subcell1);
					subrow.Cells.Add(subcell2);
					subrow.Editable = false;

					row.SubRows.Add(subrow);
				}
			}
		}

		void ScrollDown()
		{
			if (table.RowCount>0) table.EnsureVisible(table.RowCount-1, 0);
		}

		void VisualizeEvent(LogEvent e)
		{
			XPTable.Models.Row row = new XPTable.Models.Row();

			XPTable.Models.Cell cell1 = new XPTable.Models.Cell();
			XPTable.Models.CellStyle cellStyle1 = new XPTable.Models.CellStyle();
			XPTable.Models.Cell cell2 = new XPTable.Models.Cell();
			XPTable.Models.CellStyle cellStyle2 = new XPTable.Models.CellStyle();

			cell1.Data = e.time;
			cell1.ToolTipText = e.time.ToString("dd.MM.yyyy");

			cell2.Image = e.icon;
			cell2.Text = e.text;

			if (e.lines.Length == 1)
			{
				cell2.ToolTipText = e.lines[0].Second;
			}

			row.Cells.AddRange(new XPTable.Models.Cell[] { cell1, cell2 });
			row.ChildIndex = 0;
			row.Editable = false;
			row.Tag = e;

			tableModel.Rows.Add(row);
			publishedEvents.Add(e, row);
		}

		void UpdateEvents()
		{
			// must be run by UI thread
			Debug.Assert(Context.Current.MainThreadID == Thread.CurrentThread.GetHashCode());

			// save vertical scrollbar settings
			bool wasScrolledDown = table.IsScrolledDown();
			int scrollValue = 0;
			if (!wasScrolledDown) scrollValue = table.GetVerticalScroll();

			// quick brute-force solution, TODO: optimize
			bool start = false;
			foreach (LogEvent e in events)
			{
				if (!start && !publishedEvents.ContainsKey(e))
				{
					start = true;
				}
				if (start)
					VisualizeEvent(e);
			}

			// preserve vertical scrollbar settings
			if (wasScrolledDown) 
				ScrollDown(); 
			else 
				table.SetVerticalScroll(scrollValue);
		}

		public void AddEventLog(Bitmap icon, string text)
		{
			Model.SettingsRow settings = Context.Model.GetSettings();
			if (!settings.EnableLogging) return;

			lock (events)
			{
				events.AddEvent(new LogEvent(icon, DateTime.Now, text));
			}
			if (IsHandleCreated)
			{
				BeginInvoke(new UpdateEventsDelegate(UpdateEvents));
			}
		}

		public void AddEventLog(Bitmap icon, string text, Pair<Bitmap, string>[] lines)
		{
			Model.SettingsRow settings = Context.Model.GetSettings();
			if (!settings.EnableLogging) return;
			
			lock (events)
			{
				events.AddEvent(new LogEvent(icon, DateTime.Now, text, lines));
			}
			if (IsHandleCreated)
			{
				BeginInvoke(new UpdateEventsDelegate(UpdateEvents));
			}
		}

		private void OnShown(object sender, EventArgs e)
		{
			UpdateEvents();
		}

		private void OnFormClosing(object sender, FormClosingEventArgs e)
		{
			Dispose();
			Utils.ReduceMemoryUsage();
		}
	}
}