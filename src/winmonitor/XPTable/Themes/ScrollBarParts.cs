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
	/// Represents the different parts of a ScrollBar control that can be 
	/// drawn by the Windows XP theme engine
	/// </summary>
	internal enum ScrollBarParts
	{
		/// <summary>
		/// 
		/// </summary>
		ArrowBtn = 1,
		
		/// <summary>
		/// 
		/// </summary>
		ThumbBtnHorz = 2,
		
		/// <summary>
		/// 
		/// </summary>
		ThumbBtnVert = 3,
		
		/// <summary>
		/// 
		/// </summary>
		LowerTrackHorz = 4,
		
		/// <summary>
		/// 
		/// </summary>
		UpperTrackHorz = 5,
		
		/// <summary>
		/// 
		/// </summary>
		LowerTrackVert = 6,
		
		/// <summary>
		/// 
		/// </summary>
		UpperTrackVert = 7,
		
		/// <summary>
		/// 
		/// </summary>
		GripperHorz = 8,
		
		/// <summary>
		/// 
		/// </summary>
		GripperVert = 9,
		
		/// <summary>
		/// 
		/// </summary>
		SizeBox = 10,
	}
}
