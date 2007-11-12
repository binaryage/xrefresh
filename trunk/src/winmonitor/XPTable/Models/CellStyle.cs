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
using System.Drawing;


namespace XPTable.Models
{
	/// <summary>
	/// Stores visual appearance related properties for a Cell
	/// </summary>
	public class CellStyle
	{
		#region Class Data

		/// <summary>
		/// The background color of the Cell
		/// </summary>
		private Color backColor;

		/// <summary>
		/// The foreground color of the Cell
		/// </summary>
		private Color foreColor;

		/// <summary>
		/// The font used to draw the text in the Cell
		/// </summary>
		private Font font;

		/// <summary>
		/// The amount of space between the Cells border and its contents
		/// </summary>
		private CellPadding padding;

        /// <summary>
        /// Whether the text can wrap (and force the cell's height to increase)
        /// </summary>
        private bool wordWrap;

		#endregion


		#region Constructor

		/// <summary>
		/// Initializes a new instance of the CellStyle class with default settings
		/// </summary>
		public CellStyle()
		{
			this.backColor = Color.Empty;
			this.foreColor = Color.Empty;
			this.font = null;
			this.padding = CellPadding.Empty;
            this.wordWrap = false;
		}

		#endregion


		#region Properties

		/// <summary>
		/// Gets or sets the Font used by the Cell
		/// </summary>
		[Category("Appearance"),
		Description("The font used to display text in the cell")]
		public Font Font
		{
			get
			{
				return this.font;
			}

			set
			{
				this.font = value;
			}
		}


		/// <summary>
		/// Gets or sets the background color for the Cell
		/// </summary>
		[Category("Appearance"),
		Description("The background color used to display text and graphics in the cell")]
		public Color BackColor
		{
			get
			{
				return this.backColor;
			}

			set
			{
				this.backColor = value;
			}
		}


		/// <summary>
		/// Gets or sets the foreground color for the Cell
		/// </summary>
		[Category("Appearance"),
		Description("The foreground color used to display text and graphics in the cell")]
		public Color ForeColor
		{
			get
			{
				return this.foreColor;
			}

			set
			{
				this.foreColor = value;
			}
		}


		/// <summary>
		/// Gets or sets the amount of space between the Cells Border and its contents
		/// </summary>
		[Category("Appearance"),
		Description("The amount of space between the cells border and its contents")]
		public CellPadding Padding
		{
			get
			{
				return this.padding;
			}

			set
			{
				this.padding = value;
			}
		}

		/// <summary>
		/// Gets of sets whether text can wrap in this cell (and force the cell's height to increase)
		/// </summary>
        [Category("Appearance"),
        Description("Whether the text can wrap (and force the cell's height to increase)")]
        public bool WordWrap
        {
            get
            {
                return this.wordWrap;
            }

            set
            {
                this.wordWrap = value;
            }
        }

		#endregion
	}
}
