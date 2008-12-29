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
	/// Represents the different states of a ScrollBar control's buttons and track
	/// </summary>
	internal enum ScrollBarStates
	{
		/// <summary>
		/// 
		/// </summary>
		Normal = 1,
		
		/// <summary>
		/// 
		/// </summary>
		Hot = 2,
		
		/// <summary>
		/// 
		/// </summary>
		Pressed = 3,
		
		/// <summary>
		/// 
		/// </summary>
		Disabled = 4,

		/// <summary>
		/// 
		/// </summary>
		UpNormal = 1,
		
		/// <summary>
		/// 
		/// </summary>
		UpHot = 2,
		
		/// <summary>
		/// 
		/// </summary>
		UpPressed = 3,
		
		/// <summary>
		/// 
		/// </summary>
		UpDisabled = 4,

		/// <summary>
		/// 
		/// </summary>
		DownNormal = 5,
		
		/// <summary>
		/// 
		/// </summary>
		DownHot = 6,
		
		/// <summary>
		/// 
		/// </summary>
		DownPressed = 7,
		
		/// <summary>
		/// 
		/// </summary>
		DownDisabled = 8,

		/// <summary>
		/// 
		/// </summary>
		LeftNormal = 9,
		
		/// <summary>
		/// 
		/// </summary>
		LeftHot = 10,
		
		/// <summary>
		/// 
		/// </summary>
		LeftPressed = 11,
		
		/// <summary>
		/// 
		/// </summary>
		LeftDisabled = 12,

		/// <summary>
		/// 
		/// </summary>
		RightNormal = 13,
		
		/// <summary>
		/// 
		/// </summary>
		RightHot = 14,
		
		/// <summary>
		/// 
		/// </summary>
		RightPressed = 15,
		
		/// <summary>
		/// 
		/// </summary>
		RightDisabled = 16,

		/// <summary>
		/// 
		/// </summary>
		RightAlign = 1,

		/// <summary>
		/// 
		/// </summary>
		LeftAlign = 2
	}
}
