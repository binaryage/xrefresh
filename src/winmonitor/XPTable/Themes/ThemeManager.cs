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
using System.Reflection;
using System.Resources;
using System.Runtime.InteropServices;
using System.Windows.Forms;

using XPTable.Win32;


namespace XPTable.Themes
{
	/// <summary>
	/// A class that contains methods for drawing Windows XP themed Control parts
	/// </summary>
	public abstract class ThemeManager
	{
		#region Constructor

		/// <summary>
		/// Initializes a new instance of the ThemeManager class with default settings
		/// </summary>
		protected ThemeManager()
		{
			
		}

		#endregion


		#region Methods

		#region Painting

		#region Button

		/// <summary>
		/// Draws a push button in the specified state, on the specified graphics 
		/// surface, and within the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="buttonRect">The Rectangle that represents the dimensions 
		/// of the button</param>
		/// <param name="state">A PushButtonStates value that specifies the 
		/// state to draw the button in</param>
		public static void DrawButton(Graphics g, Rectangle buttonRect, PushButtonStates state)
		{
			ThemeManager.DrawButton(g, buttonRect, buttonRect, state);
		}

        public static void DrawButton(Graphics g, Rectangle buttonRect, ComboBoxStates state)
        {
            ThemeManager.DrawButton(g, buttonRect, buttonRect, (PushButtonStates)state);
        }

		/// <summary>
		/// Draws a push button in the specified state, on the specified graphics 
		/// surface, and within the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="buttonRect">The Rectangle that represents the dimensions 
		/// of the button</param>
		/// <param name="clipRect">The Rectangle that represents the clipping area</param>
		/// <param name="state">A PushButtonStates value that specifies the 
		/// state to draw the button in</param>
		public static void DrawButton(Graphics g, Rectangle buttonRect, Rectangle clipRect, PushButtonStates state)
		{
			if (g == null || buttonRect.Width <= 0 || buttonRect.Height <= 0 || clipRect.Width <= 0 || clipRect.Height <= 0)
			{
				return;
			}

			if (ThemeManager.VisualStylesEnabled)
			{
				ThemeManager.DrawThemeBackground(g, ThemeClasses.Button, (int) ButtonParts.PushButton, (int) state, buttonRect, clipRect);
			}
			else
			{
				ControlPaint.DrawButton(g, buttonRect, ThemeManager.ConvertPushButtonStateToButtonState(state));
			}
		}


		/// <summary>
		/// Converts the specified PushButtonStates value to a ButtonState value
		/// </summary>
		/// <param name="state">The PushButtonStates value to be converted</param>
		/// <returns>A ButtonState value that represents the specified PushButtonStates 
		/// value</returns>
		private static ButtonState ConvertPushButtonStateToButtonState(PushButtonStates state)
		{
			switch (state)
			{
				case PushButtonStates.Pressed:
				{
					return ButtonState.Pushed;
				}

				case PushButtonStates.Disabled:
				{
					return ButtonState.Inactive;
				}
			}

			return ButtonState.Normal;
		}

		#endregion

		#region CheckBox

		/// <summary>
		/// Draws a check box in the specified state, on the specified graphics 
		/// surface, and within the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="checkRect">The Rectangle that represents the dimensions 
		/// of the check box</param>
		/// <param name="state">A CheckBoxStates value that specifies the 
		/// state to draw the check box in</param>
		public static void DrawCheck(Graphics g, Rectangle checkRect, CheckBoxStates state)
		{
			ThemeManager.DrawCheck(g, checkRect, checkRect, state);
		}


		/// <summary>
		/// Draws a check box in the specified state, on the specified graphics 
		/// surface, and within the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="checkRect">The Rectangle that represents the dimensions 
		/// of the check box</param>
		/// <param name="clipRect">The Rectangle that represents the clipping area</param>
		/// <param name="state">A CheckBoxStates value that specifies the 
		/// state to draw the check box in</param>
		public static void DrawCheck(Graphics g, Rectangle checkRect, Rectangle clipRect, CheckBoxStates state)
		{
			if (g == null || checkRect.Width <= 0 || checkRect.Height <= 0)
			{
				return;
			}

			if (ThemeManager.VisualStylesEnabled)
			{
				ThemeManager.DrawThemeBackground(g, ThemeClasses.Button, (int) ButtonParts.CheckBox, (int) state, checkRect, clipRect);
			}
			else
			{
				if (IsMixed(state))
				{
					ControlPaint.DrawMixedCheckBox(g, checkRect, ThemeManager.ConvertCheckBoxStateToButtonState(state));
				}
				else
				{
					ControlPaint.DrawCheckBox(g, checkRect, ThemeManager.ConvertCheckBoxStateToButtonState(state));
				}
			}
		}


		/// <summary>
		/// Converts the specified CheckBoxStates value to a ButtonState value
		/// </summary>
		/// <param name="state">The CheckBoxStates value to be converted</param>
		/// <returns>A ButtonState value that represents the specified CheckBoxStates 
		/// value</returns>
		private static ButtonState ConvertCheckBoxStateToButtonState(CheckBoxStates state)
		{
			switch (state)
			{
				case CheckBoxStates.UncheckedPressed:
				{
					return ButtonState.Pushed;
				}

				case CheckBoxStates.UncheckedDisabled:
				{
					return ButtonState.Inactive;
				}

				case CheckBoxStates.CheckedNormal:
				case CheckBoxStates.CheckedHot:
				{
					return ButtonState.Checked;
				}

				case CheckBoxStates.CheckedPressed:
				{
					return (ButtonState.Checked | ButtonState.Pushed);
				}

				case CheckBoxStates.CheckedDisabled:
				{
					return (ButtonState.Checked | ButtonState.Inactive);
				}

				case CheckBoxStates.MixedNormal:
				case CheckBoxStates.MixedHot:
				{
					return ButtonState.Checked;
				}

				case CheckBoxStates.MixedPressed:
				{
					return (ButtonState.Checked | ButtonState.Pushed);
				}

				case CheckBoxStates.MixedDisabled:
				{
					return (ButtonState.Checked | ButtonState.Inactive);
				}
			}

			return ButtonState.Normal;
		}


		/// <summary>
		/// Returns whether the specified CheckBoxStates value is in an 
		/// indeterminate state
		/// </summary>
		/// <param name="state">The CheckBoxStates value to be checked</param>
		/// <returns>true if the specified CheckBoxStates value is in an 
		/// indeterminate state, false otherwise</returns>
		private static bool IsMixed(CheckBoxStates state)
		{
			switch (state)
			{
				case CheckBoxStates.MixedNormal:
				case CheckBoxStates.MixedHot:
				case CheckBoxStates.MixedPressed:
				case CheckBoxStates.MixedDisabled:
				{
					return true;
				}
			}

			return false;
		}

		#endregion

		#region ColumnHeader

		/// <summary>
		/// Draws a column header in the specified state, on the specified graphics 
		/// surface, and within the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="headerRect">The Rectangle that represents the dimensions 
		/// of the column header</param>
		/// <param name="state">A ColumnHeaderStates value that specifies the 
		/// state to draw the column header in</param>
		public static void DrawColumnHeader(Graphics g, Rectangle headerRect, ColumnHeaderStates state)
		{
			ThemeManager.DrawColumnHeader(g, headerRect, headerRect, state);
		}


		/// <summary>
		/// Draws a column header in the specified state, on the specified graphics 
		/// surface, and within the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="headerRect">The Rectangle that represents the dimensions 
		/// of the column header</param>
		/// <param name="clipRect">The Rectangle that represents the clipping area</param>
		/// <param name="state">A ColumnHeaderStates value that specifies the 
		/// state to draw the column header in</param>
		public static void DrawColumnHeader(Graphics g, Rectangle headerRect, Rectangle clipRect, ColumnHeaderStates state)
		{
			if (g == null || headerRect.Width <= 0 || headerRect.Height <= 0 || clipRect.Width <= 0 || clipRect.Height <= 0)
			{
				return;
			}

			if (ThemeManager.VisualStylesEnabled)
			{
				ThemeManager.DrawThemeBackground(g, ThemeClasses.ColumnHeader, (int) ColumnHeaderParts.HeaderItem, (int) state, headerRect, clipRect);
			}
			else
			{
				g.FillRectangle(SystemBrushes.Control, headerRect);

				if (state == ColumnHeaderStates.Pressed)
				{
					g.DrawRectangle(SystemPens.ControlDark, headerRect.X, headerRect.Y, headerRect.Width-1, headerRect.Height-1);
				}
				else
				{
					ControlPaint.DrawBorder3D(g, headerRect.X, headerRect.Y, headerRect.Width, headerRect.Height, Border3DStyle.RaisedInner);
				}
			}
		}

		#endregion

		#region ComboBoxButton

		/// <summary>
		/// Draws a combobox button in the specified state, on the specified graphics 
		/// surface, and within the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="buttonRect">The Rectangle that represents the dimensions 
		/// of the combobox button</param>
		/// <param name="state">A ComboBoxStates value that specifies the 
		/// state to draw the combobox button in</param>
		public static void DrawComboBoxButton(Graphics g, Rectangle buttonRect, ComboBoxStates state)
		{
			ThemeManager.DrawComboBoxButton(g, buttonRect, buttonRect, state);
		}

		/// <summary>
		/// Draws a combobox button in the specified state, on the specified graphics 
		/// surface, and within the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="buttonRect">The Rectangle that represents the dimensions 
		/// of the button</param>
		/// <param name="clipRect">The Rectangle that represents the clipping area</param>
		/// <param name="state">A ComboBoxStates value that specifies the 
		/// state to draw the combobox button in</param>
		public static void DrawComboBoxButton(Graphics g, Rectangle buttonRect, Rectangle clipRect, ComboBoxStates state)
		{
			if (g == null || buttonRect.Width <= 0 || buttonRect.Height <= 0 || clipRect.Width <= 0 || clipRect.Height <= 0)
			{
				return;
			}

			if (ThemeManager.VisualStylesEnabled)
			{
				ThemeManager.DrawThemeBackground(g, ThemeClasses.ComboBox, (int) ComboBoxParts.DropDownButton, (int) state, buttonRect, clipRect);
			}
			else
			{
				ControlPaint.DrawComboButton(g, buttonRect, ThemeManager.ConvertComboBoxStateToButtonState(state));
			}
		}


		/// <summary>
		/// Converts the specified ComboBoxStates value to a ButtonState value
		/// </summary>
		/// <param name="state">The ComboBoxStates value to be converted</param>
		/// <returns>A ButtonState value that represents the specified ComboBoxStates 
		/// value</returns>
		private static ButtonState ConvertComboBoxStateToButtonState(ComboBoxStates state)
		{
			switch (state)
			{
				case ComboBoxStates.Pressed:
				{
					return ButtonState.Pushed;
				}

				case ComboBoxStates.Disabled:
				{
					return ButtonState.Inactive;
				}
			}

			return ButtonState.Normal;
		}

		#endregion

		#region ProgressBar

		/// <summary>
		/// Draws a ProgressBar on the specified graphics surface, and within 
		/// the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="drawRect">The Rectangle that represents the dimensions 
		/// of the ProgressBar</param>
		public static void DrawProgressBar(Graphics g, Rectangle drawRect)
		{
			ThemeManager.DrawProgressBar(g, drawRect, drawRect);
		}


		/// <summary>
		/// Draws a ProgressBar on the specified graphics surface, and within 
		/// the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="drawRect">The Rectangle that represents the dimensions 
		/// of the ProgressBar</param>
		/// <param name="clipRect">The Rectangle that represents the clipping area</param>
		public static void DrawProgressBar(Graphics g, Rectangle drawRect, Rectangle clipRect)
		{
			if (g == null || drawRect.Width <= 0 || drawRect.Height <= 0 || clipRect.Width <= 0 || clipRect.Height <= 0)
			{
				return;
			}

			if (ThemeManager.VisualStylesEnabled)
			{
				ThemeManager.DrawThemeBackground(g, ThemeClasses.ProgressBar, (int) ProgressBarParts.Bar, 0, drawRect, clipRect);
			}
			else
			{
				// background
				g.FillRectangle(Brushes.White, drawRect);

				// 3d border
				//ControlPaint.DrawBorder3D(g, drawRect, Border3DStyle.SunkenInner);
				
				// flat border
				g.DrawRectangle(SystemPens.ControlDark, drawRect.Left, drawRect.Top, drawRect.Width-1, drawRect.Height-1);
			}
		}


		/// <summary>
		/// Draws the ProgressBar's chunks on the specified graphics surface, and within 
		/// the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="drawRect">The Rectangle that represents the dimensions 
		/// of the ProgressBar</param>
		public static void DrawProgressBarChunks(Graphics g, Rectangle drawRect)
		{
			ThemeManager.DrawProgressBarChunks(g, drawRect, drawRect);
		}


		/// <summary>
		/// Draws the ProgressBar's chunks on the specified graphics surface, and within 
		/// the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="drawRect">The Rectangle that represents the dimensions 
		/// of the ProgressBar</param>
		/// <param name="clipRect">The Rectangle that represents the clipping area</param>
		public static void DrawProgressBarChunks(Graphics g, Rectangle drawRect, Rectangle clipRect)
		{
			if (g == null || drawRect.Width <= 0 || drawRect.Height <= 0 || clipRect.Width <= 0 || clipRect.Height <= 0)
			{
				return;
			}

			if (ThemeManager.VisualStylesEnabled)
			{
				ThemeManager.DrawThemeBackground(g, ThemeClasses.ProgressBar, (int) ProgressBarParts.Chunk, 0, drawRect, clipRect);
			}
			else
			{
				g.FillRectangle(SystemBrushes.Highlight, drawRect);
			}
		}

		#endregion

		#region RadioButton

		/// <summary>
		/// Draws a RadioButton in the specified state, on the specified graphics 
		/// surface, and within the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="checkRect">The Rectangle that represents the dimensions 
		/// of the RadioButton</param>
		/// <param name="state">A RadioButtonStates value that specifies the 
		/// state to draw the RadioButton in</param>
		public static void DrawRadioButton(Graphics g, Rectangle checkRect, RadioButtonStates state)
		{
			ThemeManager.DrawRadioButton(g, checkRect, checkRect, state);
		}


		/// <summary>
		/// Draws a RadioButton in the specified state, on the specified graphics 
		/// surface, and within the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="checkRect">The Rectangle that represents the dimensions 
		/// of the RadioButton</param>
		/// <param name="clipRect">The Rectangle that represents the clipping area</param>
		/// <param name="state">A RadioButtonStates value that specifies the 
		/// state to draw the RadioButton in</param>
		public static void DrawRadioButton(Graphics g, Rectangle checkRect, Rectangle clipRect, RadioButtonStates state)
		{
			if (g == null || checkRect.Width <= 0 || checkRect.Height <= 0)
			{
				return;
			}

			if (ThemeManager.VisualStylesEnabled)
			{
				ThemeManager.DrawThemeBackground(g, ThemeClasses.Button, (int) ButtonParts.RadioButton, (int) state, checkRect, clipRect);
			}
			else
			{
				ControlPaint.DrawRadioButton(g, checkRect, ThemeManager.ConvertRadioButtonStateToButtonState(state));
			}
		}


		/// <summary>
		/// Converts the specified RadioButtonStates value to a ButtonState value
		/// </summary>
		/// <param name="state">The RadioButtonStates value to be converted</param>
		/// <returns>A ButtonState value that represents the specified RadioButtonStates 
		/// value</returns>
		private static ButtonState ConvertRadioButtonStateToButtonState(RadioButtonStates state)
		{
			switch (state)
			{
				case RadioButtonStates.UncheckedPressed:
				{
					return ButtonState.Pushed;
				}

				case RadioButtonStates.UncheckedDisabled:
				{
					return ButtonState.Inactive;
				}

				case RadioButtonStates.CheckedNormal:
				case RadioButtonStates.CheckedHot:
				{
					return ButtonState.Checked;
				}

				case RadioButtonStates.CheckedPressed:
				{
					return (ButtonState.Checked | ButtonState.Pushed);
				}

				case RadioButtonStates.CheckedDisabled:
				{
					return (ButtonState.Checked | ButtonState.Inactive);
				}
			}

			return ButtonState.Normal;
		}

		#endregion

		#region TabPage

		/// <summary>
		/// Draws a TabPage body on the specified graphics surface, and within the 
		/// specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="tabRect">The Rectangle that represents the dimensions 
		/// of the TabPage body</param>
		internal static void DrawTabPageBody(Graphics g, Rectangle tabRect)
		{
			ThemeManager.DrawTabPageBody(g, tabRect, tabRect);
		}

		
		/// <summary>
		/// Draws a TabPage body on the specified graphics surface, and within the 
		/// specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="tabRect">The Rectangle that represents the dimensions 
		/// of the TabPage body</param>
		/// <param name="clipRect">The Rectangle that represents the clipping area</param>
		internal static void DrawTabPageBody(Graphics g, Rectangle tabRect, Rectangle clipRect)
		{
			if (g == null || tabRect.Width <= 0 || tabRect.Height <= 0 || clipRect.Width <= 0 || clipRect.Height <= 0)
			{
				return;
			}

			if (ThemeManager.VisualStylesEnabled)
			{
				ThemeManager.DrawThemeBackground(g, ThemeClasses.TabControl, (int) TabParts.Body, 0, tabRect, clipRect);
			}
			else
			{
				g.FillRectangle(SystemBrushes.Control, Rectangle.Intersect(clipRect, tabRect));
			}
		}

		#endregion

		#region TextBox

		/// <summary>
		/// Draws a TextBox in the specified state, on the specified graphics 
		/// surface, and within the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="textRect">The Rectangle that represents the dimensions 
		/// of the TextBox</param>
		/// <param name="state">A TextBoxStates value that specifies the 
		/// state to draw the TextBox in</param>
		public static void DrawTextBox(Graphics g, Rectangle textRect, TextBoxStates state)
		{
			ThemeManager.DrawTextBox(g, textRect, textRect, state);
		}


		/// <summary>
		/// Draws a TextBox in the specified state, on the specified graphics 
		/// surface, and within the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="textRect">The Rectangle that represents the dimensions 
		/// of the TextBox</param>
		/// <param name="clipRect">The Rectangle that represents the clipping area</param>
		/// <param name="state">A TextBoxStates value that specifies the 
		/// state to draw the TextBox in</param>
		public static void DrawTextBox(Graphics g, Rectangle textRect, Rectangle clipRect, TextBoxStates state)
		{
			if (g == null || textRect.Width <= 0 || textRect.Height <= 0 || clipRect.Width <= 0 || clipRect.Height <= 0)
			{
				return;
			}

			if (ThemeManager.VisualStylesEnabled)
			{
				ThemeManager.DrawThemeBackground(g, ThemeClasses.TextBox, (int) TextBoxParts.EditText, (int) state, textRect, clipRect);
			}
			else
			{
				ControlPaint.DrawBorder3D(g, textRect, Border3DStyle.Sunken);
			}
		}

		#endregion

		#region UpDown

		/// <summary>
		/// Draws an UpDown's up and down buttons in the specified state, on the specified 
		/// graphics surface, and within the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="upButtonRect">The Rectangle that represents the dimensions 
		/// of the up button</param>
		/// <param name="upButtonState">An UpDownStates value that specifies the 
		/// state to draw the up button in</param>
		/// <param name="downButtonRect">The Rectangle that represents the dimensions 
		/// of the down button</param>
		/// <param name="downButtonState">An UpDownStates value that specifies the 
		/// state to draw the down button in</param>
		public static void DrawUpDownButtons(Graphics g, Rectangle upButtonRect, UpDownStates upButtonState, Rectangle downButtonRect, UpDownStates downButtonState)
		{
			ThemeManager.DrawUpDownButtons(g, upButtonRect, upButtonRect, upButtonState, downButtonRect, downButtonRect, downButtonState);
		}


		/// <summary>
		/// Draws an UpDown's up and down buttons in the specified state, on the specified 
		/// graphics surface, and within the specified bounds
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="upButtonRect">The Rectangle that represents the dimensions 
		/// of the up button</param>
		/// <param name="upButtonClipRect">The Rectangle that represents the clipping area
		/// for the up button</param>
		/// <param name="upButtonState">An UpDownStates value that specifies the 
		/// state to draw the up button in</param>
		/// <param name="downButtonRect">The Rectangle that represents the dimensions 
		/// of the down button</param>
		/// <param name="downButtonClipRect">The Rectangle that represents the clipping area
		/// for the down button</param>
		/// <param name="downButtonState">An UpDownStates value that specifies the 
		/// state to draw the down button in</param>
		public static void DrawUpDownButtons(Graphics g, Rectangle upButtonRect, Rectangle upButtonClipRect, UpDownStates upButtonState, Rectangle downButtonRect, Rectangle downButtonClipRect, UpDownStates downButtonState)
		{
			if (g == null)
			{
				return;
			}

			if (upButtonRect.Width > 0 && upButtonRect.Height > 0 && upButtonClipRect.Width > 0 && upButtonClipRect.Height > 0)
			{
				if (ThemeManager.VisualStylesEnabled)
				{
					ThemeManager.DrawThemeBackground(g, ThemeClasses.UpDown, (int) UpDownParts.Up, (int) upButtonState, upButtonRect, upButtonClipRect);
				}
				else
				{
					ControlPaint.DrawScrollButton(g, upButtonRect, ScrollButton.Up, ThemeManager.ConvertUpDownStateToButtonState(upButtonState));
				}
			}

			if (downButtonRect.Width > 0 && downButtonRect.Height > 0 && downButtonClipRect.Width > 0 && downButtonClipRect.Height > 0)
			{
				if (ThemeManager.VisualStylesEnabled)
				{
					ThemeManager.DrawThemeBackground(g, ThemeClasses.UpDown, (int) UpDownParts.Down, (int) downButtonState, downButtonRect, downButtonClipRect);
				}
				else
				{
					ControlPaint.DrawScrollButton(g, downButtonRect, ScrollButton.Down, ThemeManager.ConvertUpDownStateToButtonState(downButtonState));
				}
			}
		}


		/// <summary>
		/// Converts the specified UpDownStates value to a ButtonState value
		/// </summary>
		/// <param name="state">The UpDownStates value to be converted</param>
		/// <returns>A ButtonState value that represents the specified UpDownStates 
		/// value</returns>
		private static ButtonState ConvertUpDownStateToButtonState(UpDownStates state)
		{
			switch (state)
			{
				case UpDownStates.Pressed:
				{
					return ButtonState.Pushed;
				}

				case UpDownStates.Disabled:
				{
					return ButtonState.Inactive;
				}
			}

			return ButtonState.Normal;
		}

		#endregion

		#region Theme Background

		/// <summary>
		/// Draws the background image defined by the visual style for the specified control part
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="windowClass">The class of the part to draw</param>
		/// <param name="part">The part to draw</param>
		/// <param name="partState">The state of the part to draw</param>
		/// <param name="drawRect">The Rectangle in which the part is drawn</param>
		public static void DrawThemeBackground(Graphics g, string windowClass, int part, int partState, Rectangle drawRect)
		{
			//
			ThemeManager.DrawThemeBackground(g, windowClass, part, partState, drawRect, drawRect);
		}
		

		/// <summary>
		/// Draws the background image defined by the visual style for the specified control part
		/// </summary>
		/// <param name="g">The Graphics to draw on</param>
		/// <param name="windowClass">The class of the part to draw</param>
		/// <param name="part">The part to draw</param>
		/// <param name="partState">The state of the part to draw</param>
		/// <param name="drawRect">The Rectangle in which the part is drawn</param>
		/// <param name="clipRect">The Rectangle that represents the clipping area for the part</param>
		public static void DrawThemeBackground(Graphics g, string windowClass, int part, int partState, Rectangle drawRect, Rectangle clipRect)
		{
			if (g == null || drawRect.Width <= 0 || drawRect.Height <= 0 || clipRect.Width <= 0 || clipRect.Height <= 0)
			{
				return;
			}

			// open theme data
			IntPtr hTheme = IntPtr.Zero;
			hTheme = NativeMethods.OpenThemeData(hTheme, windowClass);

			// make sure we have a valid handle
			if (hTheme != IntPtr.Zero)
			{
				// get a graphics object the UxTheme can draw into
				IntPtr hdc = g.GetHdc();

				// get the draw and clipping rectangles
				RECT dRect = RECT.FromRectangle(drawRect);
				RECT cRect = RECT.FromRectangle(clipRect);

				// draw the themed background
				NativeMethods.DrawThemeBackground(hTheme, hdc, part, partState, ref dRect, ref cRect);

				// clean up resources
				g.ReleaseHdc(hdc);
			}

			// close the theme handle
			NativeMethods.CloseThemeData(hTheme);
		}

		#endregion

		#endregion

		#endregion


		#region Properties

		/// <summary>
		/// Gets whether Visual Styles are supported by the system
		/// </summary>
		public static bool VisualStylesSupported
		{
			get
			{
				return OSFeature.Feature.IsPresent(OSFeature.Themes);
			}
		}


		/// <summary>
		/// Gets whether Visual Styles are enabled for the application
		/// </summary>
		public static bool VisualStylesEnabled
		{
			get
			{
				if (VisualStylesSupported)
				{
					// are themes enabled
					if (NativeMethods.IsThemeActive() && NativeMethods.IsAppThemed())
					{
						return GetComctlVersion().Major >= 6;
					}
				}

				return false;
			}
		}


		/// <summary>
		/// Returns a Version object that contains information about the verion 
		/// of the CommonControls that the application is using
		/// </summary>
		/// <returns>A Version object that contains information about the verion 
		/// of the CommonControls that the application is using</returns>
		private static Version GetComctlVersion()
		{
			DLLVERSIONINFO comctlVersion = new DLLVERSIONINFO();
			comctlVersion.cbSize = Marshal.SizeOf(typeof(DLLVERSIONINFO));

			if (NativeMethods.DllGetVersion(ref comctlVersion) == 0)
			{
				return new Version(comctlVersion.dwMajorVersion, comctlVersion.dwMinorVersion, comctlVersion.dwBuildNumber);
			}

			return new Version();
		}

		#endregion
	}
}
