/*--------------------------------------------------------------------------------------
 * Author: Rafey
 * 
 * Comments: Firefox Option Dialog User Control for .NET Win Apps
 * 
 * Email: syedrafey@gmail.com
 * 
 -------------------------------------------------------------------------------------*/
using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace GUI.FirefoxDialog
{
	public class PropertyPage : System.Windows.Forms.UserControl
	{
		private System.ComponentModel.Container components = null;

		protected bool isInit = false;

		public bool IsInit
		{
			get { return isInit; }
			set { isInit = value; }
		}
	
		public PropertyPage()
		{
			InitComponent();
		}

		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Component Designer generated code
		private void InitComponent()
		{
			components = new System.ComponentModel.Container();
		}
		#endregion

		#region Overridables
		public new virtual string Text
		{
			get { return this.GetType().Name; }
		}

		public virtual Image Image
		{
			get { return null; }
		}

		public virtual void OnInit()
		{
			this.isInit = true;
		}

		public virtual void OnSetActive()
		{
		}

		public virtual void OnApply()
		{
		}


		#endregion
	}
}
