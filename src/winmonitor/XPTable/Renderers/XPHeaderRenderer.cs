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
using System.Drawing;
using System.Windows.Forms;

using XPTable.Events;
using XPTable.Models;
using XPTable.Themes;


namespace XPTable.Renderers
{
	/// <summary>
	/// A HeaderRenderer that draws Windows XP themed Column headers
	/// </summary>
	public class XPHeaderRenderer : HeaderRenderer
	{
		#region Constructor
		
		/// <summary>
		/// Initializes a new instance of the XPHeaderRenderer class 
		/// with default settings
		/// </summary>
		public XPHeaderRenderer() : base()
		{
			
		}

		#endregion


		#region Events

		#region Paint

		/// <summary>
		/// Raises the PaintBackground event
		/// </summary>
		/// <param name="e">A PaintHeaderEventArgs that contains the event data</param>
		protected override void OnPaintBackground(PaintHeaderEventArgs e)
		{
			base.OnPaintBackground(e);

			if (e.Column == null)
			{
				ThemeManager.DrawColumnHeader(e.Graphics, e.HeaderRect, ColumnHeaderStates.Normal);
			}
			else
			{
				ThemeManager.DrawColumnHeader(e.Graphics, e.HeaderRect, (ColumnHeaderStates) e.Column.ColumnState);
			}
		}


		/// <summary>
		/// Raises the Paint event
		/// </summary>
		/// <param name="e">A PaintHeaderEventArgs that contains the event data</param>
		protected override void OnPaint(PaintHeaderEventArgs e)
		{
			base.OnPaint(e);

			if (e.Column == null)
			{
				return;
			}

			Rectangle textRect = this.ClientRectangle;
			Rectangle imageRect = Rectangle.Empty;

			if (e.Column.Image != null)
			{
				imageRect = this.CalcImageRect();

				textRect.Width -= imageRect.Width;
				textRect.X += imageRect.Width;

				if (e.Column.ImageOnRight)
				{
					imageRect.X = this.ClientRectangle.Right - imageRect.Width;
					textRect.X = this.ClientRectangle.X;
				}

				if (!ThemeManager.VisualStylesEnabled && e.Column.ColumnState == ColumnState.Pressed)
				{
					imageRect.X += 1;
					imageRect.Y += 1;
				}

				this.DrawColumnHeaderImage(e.Graphics, e.Column.Image, imageRect, e.Column.Enabled);
			}

			if (!ThemeManager.VisualStylesEnabled && e.Column.ColumnState == ColumnState.Pressed)
			{
				textRect.X += 1;
				textRect.Y += 1;
			}

			if (e.Column.SortOrder != SortOrder.None)
			{
				Rectangle arrowRect = this.CalcSortArrowRect();
				
				arrowRect.X = textRect.Right - arrowRect.Width;
				textRect.Width -= arrowRect.Width;

				this.DrawSortArrow(e.Graphics, arrowRect, e.Column.SortOrder, e.Column.Enabled);
			}

			if (e.Column.Text == null)
			{
				return;
			}

			if (e.Column.Text.Length > 0 && textRect.Width > 0)
			{
				if (e.Column.Enabled)
				{
					e.Graphics.DrawString(e.Column.Text, this.Font, this.ForeBrush, textRect, this.StringFormat);
				}
				else
				{
					using (SolidBrush brush = new SolidBrush(SystemPens.GrayText.Color))
					{
						e.Graphics.DrawString(e.Column.Text, this.Font, brush, textRect, this.StringFormat);
					}
				}
			}
		}

		#endregion

		#endregion
	}
}
