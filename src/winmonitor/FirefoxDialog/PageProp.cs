/*--------------------------------------------------------------------------------------
 * Author: Rafey
 * 
 * Comments: Firefox Option Dialog User Control for .NET Win Apps
 * 
 * Email: syedrafey@gmail.com
 * 
 -------------------------------------------------------------------------------------*/

using System;
using System.Collections.Generic;
using System.Text;

namespace GUI.FirefoxDialog
{
	public class PageProp
	{
		private int imageIndex;

		public int ImageIndex
		{
			get { return imageIndex; }
			set { imageIndex = value; }
		}

		private string text;

		public string Text
		{
			get { return text; }
			set { text = value; }
		}

		private PropertyPage page;

		public PropertyPage Page
		{
			get { return page; }
			set { page = value; }
		}

		private Pabo.MozBar.MozItem mozItem;

		public Pabo.MozBar.MozItem MozItem
		{
			get { return mozItem; }
			set { mozItem = value; }
		}
	
	}
}
