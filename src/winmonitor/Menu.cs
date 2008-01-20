using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Forms;

namespace XRefresh
{
	class Menu : ContextMenu
	{
		private MenuExtender menuExtender;
		private ImageList imageList;
		private IContainer components;

		public Menu()
		{
			InitializeComponent();

			menuExtender = new MenuExtender();
			menuExtender.ImageList = imageList;
		}

		protected override void OnPopup(EventArgs e)
		{
			RebuildMenu();
		}

		public void RebuildMenu()
		{
			MenuItems.Clear();
			MenuItem item; 

			item = new MenuItem("&Configuration", new EventHandler(Context.Current.ShowConfiguration));
			item.DefaultItem = true;
			EnableExtension(item, 0);
			MenuItems.Add(item);

			Model.SettingsRow settings = Context.Model.GetSettings();
			if (settings.EnableLogging)
			{
				item = new MenuItem("Activity &Log", new EventHandler(Context.Current.ShowActivityLog));
				EnableExtension(item, 1);
				MenuItems.Add(item);
			}

			item = new MenuItem();
			item.Text = "-";
			MenuItems.Add(item);

			// build list of clients dynamically
			Server server = Server.Current;
			lock (server)
			{
				if (server.clients.Count>0)
				{
					foreach (KeyValuePair<int, Server.ClientInfo> pair in server.clients)
					{
						Server.ClientInfo client = pair.Value;

						int icon = Context.GetClientTypeIndex(client.type);
						string text = Context.GetClientTypeString(client.type);

						if (!client.muted)
						{
							if (client.page.Length > 0) text += " - " + client.page;
							item = new MenuItem(text, new EventHandler(client.OnToggle));
							EnableExtension(item, icon);
						}
						else
						{
							text = text + " [muted]";
							item = new MenuItem(text, new EventHandler(client.OnToggle));
							EnableExtension(item, icon+4);
						}

						MenuItems.Add(item);
					}
				}
				else
				{
					item = new MenuItem("no browser connected");
					EnableExtension(item);
					MenuItems.Add(item);
				}
			}

			item = new MenuItem();
			item.Text = "-";
			MenuItems.Add(item);

			// TODO: add HTML page when site is up and running
			//item = new MenuItem("&Help", new EventHandler(Context.Current.Exit));
			//item.Shortcut = Shortcut.CtrlH;
			//EnableExtension(item, 2);
			//MenuItems.Add(item);

			item = new MenuItem("&About", new EventHandler(Context.Current.About));
			EnableExtension(item, 3);
			MenuItems.Add(item);

			item = new MenuItem("E&xit", new EventHandler(Context.Current.Exit));
			EnableExtension(item);
			MenuItems.Add(item);
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Menu));
			this.imageList = new System.Windows.Forms.ImageList(this.components);
			// 
			// imageList
			// 
			this.imageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList.ImageStream")));
			this.imageList.TransparentColor = System.Drawing.Color.Transparent;
			this.imageList.Images.SetKeyName(0, "FolderWrench.png");
			this.imageList.Images.SetKeyName(1, "FolderTable.png");
			this.imageList.Images.SetKeyName(2, "Help.png");
			this.imageList.Images.SetKeyName(3, "Information.png");
			this.imageList.Images.SetKeyName(4, "New.png");
			this.imageList.Images.SetKeyName(5, "InternetExplorer.png");
			this.imageList.Images.SetKeyName(6, "Firefox.png");
			this.imageList.Images.SetKeyName(7, "Opera.png");
			this.imageList.Images.SetKeyName(8, "Safari.png");
			this.imageList.Images.SetKeyName(9, "InternetExplorerGray.png");
			this.imageList.Images.SetKeyName(10, "FirefoxGray.png");
			this.imageList.Images.SetKeyName(11, "OperaGray.png");
			this.imageList.Images.SetKeyName(12, "SafariGray.png");

		}
	}
}
