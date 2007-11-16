//
// MenuExtender.cs
//
// This class provides MenuItems with Image and Font properties.
//
// Acknowledgments: The base for this code is the Chris Beckett's 
// article "Menu Images using C# and IExtenderProvider - a better mousetrap!"
// on the CodeProject.
//
// Author: Eugene Pankov, 2004

using System;
using System.Collections;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Drawing;
using System.Drawing.Design;
using System.Globalization;
using System.Windows.Forms;
using System.Windows.Forms.Design;

namespace XRefresh
{
	[ProvideProperty( "ExtEnable", typeof(MenuItem)) ]
	[ProvideProperty( "ImageIndex", typeof(MenuItem)) ]

	public class MenuExtender : Component, IExtenderProvider
	{
		#region Private Attributes
		
		Hashtable	hashTable = null;
		ImageList	imageList = null;
		Size		iconSize = SystemInformation.SmallIconSize;
		Font		menuFont = null;
		bool		useSystemFont = true;
		
		#endregion
		
		#region Constructors

		public MenuExtender(System.ComponentModel.IContainer container) : this()
		{
			container.Add(this);
		}

		public MenuExtender()
		{
			hashTable = new Hashtable();
		}
		#endregion
		
		#region ImageIndex Property

		[Description("The image index associated with the menu item.")]
		[DefaultValue("(none)")]
		[Category("Menu Extender")]
		[TypeConverter(typeof(IndexConverter)), Editor(typeof(ImageIndexEditor), typeof(System.Drawing.Design.UITypeEditor))]
		public int GetImageIndex( Component component )
		{
			if( hashTable.Contains( component ) )
			{
				Properties prop = ( Properties )hashTable[ component ];
				int imageIndex = prop.ExtImageIndex;
				return ( imageIndex > -1 ) ? imageIndex : -1;
			}
			return -1;
		}

		public void SetImageIndex( Component component, int indexValue )
		{
			if( indexValue < -1 )
				indexValue = -1;
			Properties prop = null;

			if( ! hashTable.Contains( component ) )
			{
				prop = new Properties();
				prop.ExtImageIndex = indexValue;
				hashTable.Add( component, prop );
			}
			else 
			{
				prop = ( Properties )hashTable[ component ];
				prop.ExtImageIndex = indexValue;
				hashTable[ component ] = prop;
			}
		}
		#endregion

		#region ExtEnable Property

		[Description("Enable to use the ExtenderProvider for the menu item.")]
		[DefaultValue(false)]
		[Category("Menu Extender")]

		public bool GetExtEnable( Component component )
		{
			if( hashTable.Contains( component ) )
			{
				Properties prop = ( Properties )hashTable[ component ];
				return prop.ExtEnable;
			}
			return false;
		}

		public void SetExtEnable( Component component, bool enableExt )
		{
			Properties prop = null;

			if( ! hashTable.Contains( component ) )
			{
				prop = new Properties();
				prop.ExtEnable = enableExt;
				hashTable.Add( component, prop ) ;
			}
			else 
			{
				prop = ( Properties )hashTable[ component ];
				prop.ExtEnable = enableExt;
				hashTable[ component ] = prop;
			}

			MenuItem menuItem = ( MenuItem )component ;
			menuItem.MeasureItem	-= new MeasureItemEventHandler( OnMeasureItem ) ;
			menuItem.DrawItem		-= new DrawItemEventHandler( OnDrawItem ) ;

			if( prop.ExtEnable )
			{
				menuItem.MeasureItem	+= new MeasureItemEventHandler( OnMeasureItem ) ;
				menuItem.DrawItem		+= new DrawItemEventHandler( OnDrawItem ) ;
				menuItem.OwnerDraw		= true;
			}
		}
		#endregion

		#region CanExtend

		public bool CanExtend( object component )
		{
			// only support not top-level MenuItem objects
			if( component is MenuItem )
			{
				MenuItem menuItem = ( MenuItem )component;
				return !( menuItem.Parent is MainMenu );
			}
			return false ;
		}
		#endregion

		#region Extender Properties

		[Description("The ImageList from which the MenuImage will get all of the MenuItem images.")]
		[Category("Misc")]
		[DefaultValue(null)]

		public ImageList ImageList
		{
			get{ return imageList; }
			set{ imageList = value; }
		}

		[Description("The font used to display text in the menu.")]
		[Category("Misc")]

		public Font Font
		{
			get{ return menuFont; }
			set{ menuFont = value; }
		}

		public void ResetFont()
		{
			this.SystemFont = true;
		}

		[Description("Enable to use a System MenuFont to display text in the menu.")]
		[Category("Misc")]
		[RefreshProperties(RefreshProperties.Repaint)]

		public bool SystemFont
		{
			get{ return useSystemFont; }
			set{ 
				useSystemFont = value; 
				if( useSystemFont )
					Font = null;
				}
		}
		#endregion

		#region Properties class
		private class Properties
		{
			public bool	ExtEnable;			// Extender is enabled
			public int	ExtImageIndex;		// Image index in the ImageList

			public Properties()
			{
				ExtEnable = false;
				ExtImageIndex = -1;
			}
		}
		#endregion

		#region Private Members

		private int GetMenuImageIndex( Component component )
		{
			if( imageList == null )
				return -1;				// Nothing to draw

			if( hashTable.Contains( component ) )
			{
				Properties prop = ( Properties )hashTable[ component ];
				if( prop.ExtEnable && prop.ExtImageIndex < imageList.Images.Count )
					return prop.ExtImageIndex;
			}
			return -1;
		}

		private void OnMeasureItem( object sender, MeasureItemEventArgs e )
		{
			MenuItem menuItem = ( MenuItem )sender;
			MenuHelper menuHelper = new MenuHelper( menuItem, e.Graphics, this );

			e.ItemHeight = menuHelper.CalcHeight();
			e.ItemWidth  = menuHelper.CalcWidth();
		}
		
		private void OnDrawItem( object sender, DrawItemEventArgs e )
		{
			MenuItem menuItem = ( MenuItem )sender;
			MenuHelper menuHelper = new MenuHelper( menuItem, e.Graphics, this );
			bool menuSelected = ( e.State & DrawItemState.Selected ) == DrawItemState.Selected;

			menuHelper.DrawBackground( e.Bounds, menuSelected );

			if( menuHelper.IsSeperator() )
				menuHelper.DrawSeperator( e.Bounds );
			else
			{
				int imageIndex = GetMenuImageIndex( sender as Component );
				menuHelper.DrawMenu( e.Bounds, menuSelected, imageIndex );
			}
		}
		#endregion
		
		#region MenuHelper Class

		private class MenuHelper
		{
			#region Private Attributes
			
			const int	SEPERATOR_HEIGHT	= 8;
			const int	BORDER_VERTICAL		= 2;
			const int	LEFT_MARGIN			= 4;
			const int	RIGHT_MARGIN		= 6;
			const int	SHORTCUT_MARGIN		= 20;
			const int	ARROW_MARGIN		= 12;
			const int	BULLET_DIAMETER		= 7;

			MenuExtender	extender;
			MenuItem		menuItem;
			Size			iconSize;
			Font			menuFont;
			Graphics		gfx;
			#endregion

			#region Constructors
			
			public MenuHelper( MenuItem item, Graphics graphics, MenuExtender ext )
			{
				menuItem	= item;
				extender	= ext;
				iconSize	= ( ext.imageList == null ) ? SystemInformation.SmallIconSize : ext.imageList.ImageSize;
				menuFont	= ( ext.useSystemFont || ext.menuFont == null ) ? SystemInformation.MenuFont : ext.menuFont;
				gfx			= graphics;
			}
			#endregion

			#region Public Members
			
 			public int CalcHeight()
			{
				if( IsSeperator() )
					return SEPERATOR_HEIGHT ;

				int itemHeight = ( menuFont.Height > iconSize.Height ) ? 
					menuFont.Height : iconSize.Height;

				return itemHeight + BORDER_VERTICAL;
			}
			
			public int CalcWidth()
			{
				StringFormat sf = new StringFormat() ;
				sf.HotkeyPrefix = System.Drawing.Text.HotkeyPrefix.Show ;
			
				int menuWidth = (int)Math.Ceiling( gfx.MeasureString( menuItem.Text, this.CurrentFont, 1000, sf).Width );
				int shortcutWidth = (int)Math.Ceiling( gfx.MeasureString( this.ShortcutText, this.CurrentFont, 1000, sf).Width );
				int arrowWidth = ( menuItem.IsParent ) ? ARROW_MARGIN : 0;

				if( IsTopLevel() )
					return menuWidth ;
				else
					return LEFT_MARGIN + iconSize.Width + RIGHT_MARGIN 
							+ menuWidth + SHORTCUT_MARGIN + shortcutWidth + arrowWidth;
			}
			
			public bool HasShortcut()
			{
				return ( menuItem.ShowShortcut && menuItem.Shortcut != Shortcut.None );
			}
			
			public bool IsSeperator()
			{
				return ( menuItem.Text == "-" );
			}
			
			public bool IsTopLevel()
			{
				return ( menuItem.Parent is MainMenu );
			}
			
			public string ShortcutText
			{
				get
				{
					if( menuItem.ShowShortcut && menuItem.Shortcut != Shortcut.None )
					{
						Keys keys = ( Keys )menuItem.Shortcut;
						return Convert.ToChar(Keys.Tab) + 
							System.ComponentModel.TypeDescriptor.GetConverter(keys.GetType()).ConvertToString(keys);
					}
					return null;
				}
			}

			public Font CurrentFont
			{
				get
				{
					if( menuItem.DefaultItem )
					{
						Font font;
						try
						{
							font = new Font( menuFont, menuFont.Style | System.Drawing.FontStyle.Bold );
						}
						catch
						{
							return menuFont;
						}
						return font;
					}
					return menuFont;
				}
				set { CurrentFont = value; }
			}

			public SolidBrush CurrentBrush( bool selected )
			{
				if( !menuItem.Enabled )
					return new SolidBrush( SystemColors.GrayText );

				SolidBrush menuBrush = null;
				if( selected )
					menuBrush = new SolidBrush( SystemColors.HighlightText );
				else
					menuBrush = new SolidBrush( SystemColors.MenuText );
				return menuBrush;
			}

			public void DrawBackground( Rectangle bounds, bool selected )
			{
				if( selected )
					gfx.FillRectangle( SystemBrushes.Highlight, bounds );
				else
					gfx.FillRectangle( SystemBrushes.Menu, bounds );
			}

			public void DrawMenu( Rectangle bounds, bool selected, int indexValue )
			{
				DrawMenuText( bounds, selected );
				
				if( menuItem.Checked )
					DrawCheckBox( bounds, selected );
				else
				{
					if( indexValue > -1 )
					{
						Image menuImage = extender.imageList.Images[ indexValue ];						
						DrawImage( menuImage, bounds );
					}
				}
			}
			
			public void DrawSeperator( Rectangle bounds )
			{
				Pen pen = new Pen( SystemColors.ControlDark );
				int yCenter	= bounds.Top  + (bounds.Height / 2);

				gfx.DrawLine(pen, bounds.Left, yCenter, (bounds.Left + bounds.Width), yCenter);
			}
			#endregion

			#region Private Members

			private void DrawMenuText( Rectangle bounds, bool selected )
			{
				SolidBrush menuBrush = CurrentBrush( selected );
				
				StringFormat sfMenu = new StringFormat();
				sfMenu.HotkeyPrefix = System.Drawing.Text.HotkeyPrefix.Show;

				int yPos = bounds.Top + (bounds.Height - this.CurrentFont.Height) / 2;

				gfx.DrawString( menuItem.Text, this.CurrentFont, menuBrush, 
					bounds.Left + LEFT_MARGIN + iconSize.Width + RIGHT_MARGIN, yPos, sfMenu );

				if( ! IsTopLevel() && HasShortcut() )
				{
					sfMenu.FormatFlags |= StringFormatFlags.DirectionRightToLeft;

					gfx.DrawString(this.ShortcutText, this.CurrentFont, menuBrush, 
						bounds.Width - ARROW_MARGIN, yPos, sfMenu);
				}
			}			
			
			private void DrawCheckBox( Rectangle bounds, bool selected )
			{
				Rectangle rectCheck = new Rectangle( bounds.Left, bounds.Top,
					SystemInformation.MenuCheckSize.Width,
					SystemInformation.MenuCheckSize.Height );

				rectCheck.X += ((LEFT_MARGIN + iconSize.Width + RIGHT_MARGIN) - rectCheck.Width) / 2;
				rectCheck.Y += (bounds.Height - rectCheck.Height) / 2;

				if( menuItem.RadioCheck )
					DrawBullet( rectCheck, selected );
				else
					DrawCheckMark( rectCheck, selected );
			}

			private void DrawBullet( Rectangle rect, bool selected )
			{
				SolidBrush menuBrush = CurrentBrush( selected );
				int x = rect.Left + ( rect.Width - BULLET_DIAMETER ) / 2;
				int y = rect.Top + ( rect.Height - BULLET_DIAMETER ) / 2;
				gfx.FillEllipse( menuBrush, x, y, BULLET_DIAMETER, BULLET_DIAMETER );
			}

			private void DrawCheckMark( Rectangle rect, bool selected )
			{
				SolidBrush brush = CurrentBrush( selected );
				Pen pen = new Pen( brush, 1 );

				int x = rect.Left + rect.Width  / 2;
				int y = rect.Top  + rect.Height / 2;
				Point[] points = new Point[] { new Point(x-4, y-1),
											   new Point(x-4, y+1),
											   new Point(x-2, y+3),
											   new Point(x+2, y-1),
											   new Point(x+2, y-3),
											   new Point(x-2, y+1),
											   new Point(x-4, y-1)
											 };
				gfx.FillPolygon( brush, points );
				gfx.DrawLines( pen, points );
			}
			
			private void DrawImage( Image menuImage, Rectangle bounds )
			{
				if( menuItem.Enabled )
					gfx.DrawImage( menuImage, bounds.Left + LEFT_MARGIN, 
						bounds.Top + ((bounds.Height - iconSize.Height) / 2), 
						iconSize.Width, iconSize.Height );	
				else
					ControlPaint.DrawImageDisabled(gfx, menuImage, 
						bounds.Left + LEFT_MARGIN, 
						bounds.Top + ((bounds.Height - iconSize.Height) / 2), 
						SystemColors.Menu );
			}
			#endregion
		}
		#endregion
	}

	#region ImageIndexEditor

	internal class ImageIndexEditor : UITypeEditor
	{
		public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context) 
		{
			if(context != null && context.Instance != null ) 
			{
				return UITypeEditorEditStyle.DropDown;
			}
			return base.GetEditStyle(context);
		}

		public override bool GetPaintValueSupported(ITypeDescriptorContext context) 
		{
			return true;
		}

		public override void PaintValue(PaintValueEventArgs pe) 
		{
			int imageIndex = -1;	// Do not display an image
			if( pe.Value != null )
			{
				try
				{
					imageIndex = (int)Convert.ToUInt16( pe.Value.ToString() ) ;
				}
				catch
				{
					imageIndex = -1;
				}
			}

			if( pe.Context.Instance == null || imageIndex < 0 )
				return;

			ImageList imageList = null;
			Component component = (Component)pe.Context.Instance;

			IExtenderListService extenderListService = (IExtenderListService)component.Site.GetService(typeof(IExtenderListService));
			if( extenderListService != null )
			{
				IExtenderProvider[] extenders = extenderListService.GetExtenderProviders();
				for( int i=0; i < extenders.Length; i++ )
				{
					if( extenders[i].GetType().FullName == "MenuExtender.MenuExtender" )
					{
						MenuExtender menuExtender = (MenuExtender)extenders[i];
						imageList = menuExtender.ImageList;
					}
				}
			}
			if( imageList == null || 
				imageList.Images.Empty || 
				imageIndex >= imageList.Images.Count )
				return;

			pe.Graphics.DrawImage( imageList.Images[imageIndex], pe.Bounds );
		}

		public override object EditValue(ITypeDescriptorContext context,
			IServiceProvider provider, object value)
		{
			IWindowsFormsEditorService wfes = (IWindowsFormsEditorService)provider.GetService(
				typeof(IWindowsFormsEditorService));

			if( wfes == null || context == null )
				return null;

			ImageList imageList = null;
			Component component = (Component)context.Instance;

			IExtenderListService extenderListService = (IExtenderListService)component.Site.GetService(typeof(IExtenderListService));
			if( extenderListService != null )
			{
				IExtenderProvider[] extenders = extenderListService.GetExtenderProviders();
				for( int i=0; i < extenders.Length; i++ )
				{
					if( extenders[i].GetType().FullName == "MenuExtender.MenuExtender" )
					{
						MenuExtender menuExtender = (MenuExtender)extenders[i];
						imageList = menuExtender.ImageList;
					}
				}
			}

			ImageSelector imageSelector = new ImageSelector( imageList, (int)value, wfes );

			wfes.DropDownControl( imageSelector );

			int imageIndex = -1;
			if( imageSelector.SelectedItems.Count != 0 )
			{
				try
				{
					imageIndex = (int)Convert.ToInt32( imageSelector.SelectedItems[0].Text );
				}
				catch
				{
					imageIndex = -1;
				}
			}
			return imageIndex;
		}

		internal class ImageSelector : System.Windows.Forms.ListView
		{
			private ImageList	imageList = null;
			private IWindowsFormsEditorService wfes = null;

			public ImageSelector( ImageList images, int selectedIndex, IWindowsFormsEditorService wfes )
			{
				this.wfes = wfes;
				this.BorderStyle = BorderStyle.None;
				this.View = View.Details;
				this.FullRowSelect = true;
				this.GridLines = false;
				this.Columns.Add("Index", -2, HorizontalAlignment.Left);
				this.HeaderStyle = ColumnHeaderStyle.None;
				this.MultiSelect = false;
				this.Click += new EventHandler( ImageSelector_Click );

				if( images != null )
				{
					// Scale images for the ListView if necessary.
					// You can assign this.imageList=images without this check.

					if( images.ImageSize == SystemInformation.SmallIconSize )
						this.imageList = images;
					else
					{
						this.imageList = new ImageList();
						this.imageList.ImageSize = SystemInformation.SmallIconSize;
						for( int i=0; i < images.Images.Count; i++ )
							this.imageList.Images.Add( images.Images[i] );
					}

					this.SmallImageList = imageList;
					for( int i=0; i < imageList.Images.Count; i++ )
					{
						this.Items.Add( i.ToString(), i );
					}
				}

				this.Items.Add( "(none)" );
				if( selectedIndex < 0 || selectedIndex >= this.Items.Count )
					selectedIndex = this.Items.Count - 1;

				this.Items[ selectedIndex ].Selected = true;
				this.Items[ selectedIndex ].EnsureVisible();
			}

			protected void ImageSelector_Click(object sender, EventArgs e)
			{
				if( wfes != null )
					wfes.CloseDropDown();
			}
		}
	}
	#endregion

	#region IndexConverter

	internal class IndexConverter : TypeConverter
	{
		public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType) 
		{
			if( sourceType == typeof(string) )
			{
				return true;
			}
			return base.CanConvertFrom(context, sourceType);
		}

		public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType) 
		{
			if( destinationType == typeof(string) ) 
				return true;
			return base.CanConvertTo(context, destinationType);
		}

		public override object ConvertFrom(ITypeDescriptorContext context, 
			CultureInfo culture, object value) 
		{
			if( value is string )
			{
				int imageIndex = 0;
				string indexValue = (string)value;
				if( value != null && indexValue.Length > 0 && indexValue != "(none)" )
				{
					try
					{
						imageIndex = (int)Convert.ToUInt16( indexValue );
					}
					catch
					{
						return -1;
					}
				}
				else
					return -1;

				return imageIndex;
			}
			return base.ConvertFrom(context, culture, value);
		}

		public override object ConvertTo(ITypeDescriptorContext context, 
			CultureInfo culture, object value, Type destinationType) 
		{
			if( destinationType == typeof(string) && value is int ) 
			{
				return ( (int)value > -1 ) ? value.ToString() : "(none)";
			}
			return base.ConvertTo(context, culture, value, destinationType);      
		}
	}
	#endregion
}
