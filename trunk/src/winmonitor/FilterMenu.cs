using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Windows.Forms;

namespace XRefresh
{
	class FilterMenu : ContextMenu
	{
		const int ADD_IGNORE = 0;
		const int DEL_IGNORE = 1;
		const int ADD_INCLUDE = 2;
		const int DEL_INCLUDE = 3;

		private MenuExtender menuExtender;
		private ImageList imageList;
		private IContainer components;
		private string path;
		private string reason;
		private MatchReason.Status status;
		private ProjectFilters parent;

		public FilterMenu(ProjectFilters parent, string path, string reason, MatchReason.Status status)
		{
			InitializeComponent();

			menuExtender = new MenuExtender();
			menuExtender.ImageList = imageList;

			this.path = path.Replace('/', '\\');
			this.status = status;
			this.reason = reason;
			this.parent = parent;

			RebuildFilterMenu();
		}

		protected override void OnPopup(EventArgs e)
		{
			RebuildFilterMenu();
		}

		private void AddItem(string name, int icon)
		{
			string mask = name.Replace('\\', '/');
			if (icon == ADD_IGNORE)
			{
				if (parent.tableExcludes.ContainsMask(mask)) return; // already added
			}
			if (icon == ADD_INCLUDE)
			{
				if (parent.tableIncludes.ContainsMask(mask)) return; // already added
			}

			MenuItem item;
			item = new MenuItem(mask,
				delegate(object sender, EventArgs e)
				{
					if (icon == ADD_IGNORE)
					{
						parent.tableExcludes.AddRow(mask, "");
					}
					if (icon == ADD_INCLUDE)
					{
						parent.tableIncludes.AddRow(mask, "");
					}
					if (icon == DEL_IGNORE)
					{
						parent.tableExcludes.RemoveRow(mask);
					}
					if (icon == DEL_INCLUDE)
					{
						parent.tableIncludes.RemoveRow(mask);
					}
				}
				);
			EnableExtension(item, icon);
			MenuItems.Add(item);
		}

		void RebuildFilterMenu()
		{
			MenuItems.Clear();

			if (status == MatchReason.Status.Normal)
			{
				// add excludes
				BuildFileItems(path, ADD_IGNORE);

				AddSeparator();

				// add includes
				BuildFileItems(path, ADD_INCLUDE);
			}
			else if (status == MatchReason.Status.Excluded)
			{
				// remove exclude
				AddItem(reason, DEL_IGNORE);

				AddSeparator();

				// includes
				BuildFileItems(path, ADD_INCLUDE);
			}
			else if (status == MatchReason.Status.Included)
			{
				// remove include
				AddItem(reason, DEL_INCLUDE);
			}
		}

		private void AddSeparator()
		{
			MenuItem item = new MenuItem();
			item.Text = "-";
			MenuItems.Add(item);
		}

		private void BuildFileItems(string path, int icon)
		{
			string ext = Path.GetExtension(path);
			if (ext.Length > 0) AddItem("*" + ext, icon);
			AddItem(Path.GetFileName(path), icon);
			AddItem(path, icon);

			string dir = Path.GetDirectoryName(path);
			string[] parts = dir.Split('\\');
			string cur = "";
			List<string> candidates = new List<string>();
			foreach (string part in parts)
			{
				if (part.Length > 0)
				{
					cur += "/" + part;
					candidates.Add(cur + "/*");
				}
			}
			candidates.Reverse();
			if (candidates.Count > 5)
			{
				candidates.RemoveRange(5, candidates.Count - 5);
			}

			foreach (string candidate in candidates)
			{
				AddItem(candidate, icon);
			}
		}

		private void EnableExtension(MenuItem item)
		{
			menuExtender.SetExtEnable(item, true);
			menuExtender.SetImageIndex(item, -1);
			item.OwnerDraw = true;
		}

		private void EnableExtension(MenuItem item, int index)
		{
			menuExtender.SetExtEnable(item, true);
			menuExtender.SetImageIndex(item, index);
			item.OwnerDraw = true;
		}

		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FilterMenu));
			this.imageList = new System.Windows.Forms.ImageList(this.components);
			// 
			// imageList
			// 
			this.imageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList.ImageStream")));
			this.imageList.TransparentColor = System.Drawing.Color.Transparent;
			this.imageList.Images.SetKeyName(0, "AddIgnore.png");
			this.imageList.Images.SetKeyName(1, "DelIgnore.png");
			this.imageList.Images.SetKeyName(2, "AddInclude.png");
			this.imageList.Images.SetKeyName(3, "DelInclude.png");

		}
	}
}
