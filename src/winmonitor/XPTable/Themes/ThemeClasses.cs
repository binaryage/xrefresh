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

namespace XPTable.Themes
{
	/// <summary>
	/// Represents the different types of objects that can be 
	/// drawn by the Windows XP theme engine
	/// </summary>
	public sealed class ThemeClasses
	{
		#region Constructor

		/// <summary>
		/// Private constructor so that the class can't be instantiated
		/// </summary>
		private ThemeClasses()
		{
			
		}

		#endregion


		#region Properties

		/// <summary>
		/// Button objects (Button, CheckBox, RadioButton)
		/// </summary>
		public static string Button
		{
			get
			{
				return "BUTTON";
			}
		}


		/// <summary>
		/// ComboBox objects
		/// </summary>
		public static string ComboBox
		{
			get
			{
				return "COMBOBOX";
			}
		}


		/// <summary>
		/// TextBox objects
		/// </summary>
		public static string TextBox
		{
			get
			{
				return "EDIT";
			}
		}


		/// <summary>
		/// ColumnHeader objects
		/// </summary>
		public static string ColumnHeader
		{
			get
			{
				return "HEADER";
			}
		}


		/// <summary>
		/// ListView objects
		/// </summary>
		public static string ListView
		{
			get
			{
				return "LISTVIEW";
			}
		}


		/// <summary>
		/// ProgressBar objects
		/// </summary>
		public static string ProgressBar
		{
			get
			{
				return "PROGRESS";
			}
		}


		/// <summary>
		/// TabControl objects
		/// </summary>
		internal static string TabControl
		{
			get
			{
				return "TAB";
			}
		}


		/// <summary>
		/// UpDown objects
		/// </summary>
		public static string UpDown
		{
			get
			{
				return "SPIN";
			}
		}

		#endregion
	}
}
