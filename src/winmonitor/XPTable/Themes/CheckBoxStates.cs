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
	/// Represents the different states of a CheckBox
	/// </summary>
	public enum CheckBoxStates
	{
		/// <summary>
		/// The CheckBox is unchecked and in its normal state
		/// </summary>
		UncheckedNormal = 1,
		
		/// <summary>
		/// The CheckBox is unchecked and is currently highlighted
		/// </summary>
		UncheckedHot = 2,
		
		/// <summary>
		/// The CheckBox is unchecked and is currently pressed by 
		/// the mouse
		/// </summary>
		UncheckedPressed = 3,
		
		/// <summary>
		/// The CheckBox is unchecked and is disabled
		/// </summary>
		UncheckedDisabled = 4,
		
		/// <summary>
		/// The CheckBox is checked and in its normal state
		/// </summary>
		CheckedNormal = 5,
		
		/// <summary>
		/// The CheckBox is checked and is currently highlighted
		/// </summary>
		CheckedHot = 6,
		
		/// <summary>
		/// The CheckBox is checked and is currently pressed by the 
		/// mouse
		/// </summary>
		CheckedPressed = 7,
		
		/// <summary>
		/// The CheckBox is checked and is disabled
		/// </summary>
		CheckedDisabled = 8,
		
		/// <summary>
		/// The CheckBox is in an indeterminate state
		/// </summary>
		MixedNormal = 9,
		
		/// <summary>
		/// The CheckBox is in an indeterminate state and is currently 
		/// highlighted
		/// </summary>
		MixedHot = 10,
		
		/// <summary>
		/// The CheckBox is in an indeterminate state and is currently 
		/// pressed by the mouse
		/// </summary>
		MixedPressed = 11,
		
		/// <summary>
		/// The CheckBox is in an indeterminate state and is disabled
		/// </summary>
		MixedDisabled = 12
	}
}
