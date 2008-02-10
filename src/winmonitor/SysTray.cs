using System;
using System.Windows.Forms;
using System.Drawing;

// http://blogs.msdn.com/abhinaba/archive/2005/09/12/464150.aspx
// by Abhinaba
namespace Abhinaba.SysTray
{
	/// <summary>
	/// SysTray class that can be used to display animated icons or text in the system tray
	/// </summary>
	public class SysTray : IDisposable
	{
		#region Constructor
		/// <summary>
		/// The constructor
		/// </summary>
		/// <param name="text">The toolip text</param>
		/// <param name="icon">The icon that will be shown by default, can be null</param>
		/// <param name="menu">The context menu to be opened on right clicking on the 
		///                    icon in the tray. This can be null.</param>
		public SysTray(string text, Icon icon, ContextMenu menu)
		{
			m_notifyIcon = new NotifyIcon();
			m_notifyIcon.Text = text; // tooltip text show over tray icon
			m_notifyIcon.Visible = true;
			m_notifyIcon.Icon = icon; // icon in the tray
			m_DefaultIcon = icon;
			m_notifyIcon.ContextMenu = menu; // context menu
			m_font = new Font("Helvetica", 8);

			m_timer = new System.Timers.Timer();
			m_timer.Interval = 100;
			m_timer.Elapsed += new System.Timers.ElapsedEventHandler(Tick);
		}
		#endregion // Constructor
		#region Public APIs
		/// <summary>
		/// Shows text instead of icon in the tray
		/// </summary>
		/// <param name="text">The text to be displayed on the tray. 
		///                    Make this only 1 or 2 characters. E.g. "23"</param>
		public void ShowText(string text)
		{
			ShowText(text, m_font, m_col);
		}

		/// <summary>
		/// Shows text instead of icon in the tray
		/// </summary>
		/// <param name="text">Same as above</param>
		/// <param name="col">Color to be used to display the text in the tray</param>
		public void ShowText(string text, Color col)
		{
			ShowText(text, m_font, col);
		}

		/// <summary>
		/// Shows text instead of icon in the tray
		/// </summary>
		/// <param name="text">Same as above</param>
		/// <param name="font">The default color will be used but in user given font</param>
		public void ShowText(string text, Font font)
		{
			ShowText(text, font, m_col);
		}

		/// <summary>
		/// Shows text instead of icon in the tray
		/// </summary>
		/// <param name="text">the text to be displayed</param>
		/// <param name="font">The font to be used</param>
		/// <param name="col">The color to be used</param>
		public void ShowText(string text, Font font, Color col)
		{
			Bitmap bitmap = new Bitmap(16, 16);//, System.Drawing.Imaging.PixelFormat.Max);
			Brush brush = new SolidBrush(col);
			Graphics graphics = Graphics.FromImage(bitmap);
			graphics.DrawString(text, m_font, brush, 0, 0);
			IntPtr hIcon = bitmap.GetHicon();
			Icon icon = Icon.FromHandle(hIcon);
			m_notifyIcon.Icon = icon;
		}

		/// <summary>
		/// Sets the animation clip that will be displayed in the system tray
		/// </summary>
		/// <param name="icons">The array of icons which forms each frame of the animation
		///                     This'll work by showing one icon after another in the array.
		///                     Each of the icons must be 16x16 pixels </param>
		public void SetAnimationClip(Icon[] icons)
		{
			m_animationIcons = icons;
		}

		/// <summary>
		/// Sets the animation clip that will be displayed in the system tray
		/// </summary>
		/// <param name="icons">The array of bitmaps which forms each frame of the animation
		///                     This'll work by showing one bitmap after another in the array.
		///                     Each of the bitmaps must be 16x16 pixels  </param>
		public void SetAnimationClip(Bitmap[] bitmap)
		{
			try
			{
				m_animationIcons = new Icon[bitmap.Length];
				for (int i = 0; i < bitmap.Length; i++)
				{
					m_animationIcons[i] = Icon.FromHandle(bitmap[i].GetHicon());
				}
			}
			catch (Exception)
			{
				// I was getting generic GDI+ exceptions from time to time
			}
		}

		/// <summary>
		/// Sets the animation clip that will be displayed in the system tray
		/// </summary>
		/// <param name="icons">The bitmap strip that contains the frames of animation.
		///                     This can be created by creating a image of size 16*n by 16 pixels
		///                     Where n is the number of frames. Then in the first 16x16 pixel put
		///                     first image and then from 16 to 32 pixel put the second image and so on</param>
		public void SetAnimationClip(Bitmap bitmapStrip)
		{
			try
			{
				m_animationIcons = new Icon[bitmapStrip.Width / 16];
				for (int i = 0; i < m_animationIcons.Length; i++)
				{
					Rectangle rect = new Rectangle(i * 16, 0, 16, 16);
					Bitmap bmp = bitmapStrip.Clone(rect, bitmapStrip.PixelFormat);
					m_animationIcons[i] = Icon.FromHandle(bmp.GetHicon());
				}
			}
			catch (Exception)
			{
				// I was getting generic GDI+ exceptions from time to time
			}
		}

		/// <summary>
		/// Start showing the animation. This needs to be called after 
		/// setting the clip using any of the above methods
		/// </summary>
		/// <param name="loop">whether to loop infinitely or stop after one iteration</param>
		/// <param name="interval">Interval in millisecond in between each frame. Typicall 100</param>
		public void StartAnimation(int interval, int loopCount)
		{
			if (m_animationIcons == null)
				throw new ApplicationException("Animation clip not set with SetAnimationClip");

			m_loopCount = loopCount;
			m_timer.Interval = interval;
			m_timer.Start();
		}

		/// <summary>
		/// Stop animation started with StartAnimation with loop = true
		/// </summary>
		public void StopAnimation()
		{
			m_timer.Stop();
		}
		#endregion // Public APIs
		#region Dispose
		public void Dispose()
		{
			m_notifyIcon.Dispose();
			if (m_font != null)
				m_font.Dispose();
		}
		#endregion
		#region Event handlers
		private void Tick(object sender, System.Timers.ElapsedEventArgs e)
		{
			if (m_currIndex < m_animationIcons.Length)
			{
				m_notifyIcon.Icon = m_animationIcons[m_currIndex];
				m_currIndex++;
			}
			else
			{
				m_currIndex = 0;
				if (m_loopCount <= 0)
				{
					m_timer.Stop();
					m_notifyIcon.Icon = m_DefaultIcon;
				}
				else
				{
					--m_loopCount;
				}
			}
		}
		#endregion // Event handlers
		#region private variables

		public NotifyIcon m_notifyIcon;
		private Font m_font;
		private Color m_col = Color.Black;
		private Icon[] m_animationIcons;
		private System.Timers.Timer m_timer;
		private int m_currIndex = 0;
		private int m_loopCount = 0;
		public Icon m_DefaultIcon;
		#endregion // private variables
	}
}
