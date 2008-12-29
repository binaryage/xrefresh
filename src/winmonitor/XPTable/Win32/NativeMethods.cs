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
using System.Runtime.InteropServices;

using XPTable.Themes;


namespace XPTable.Win32
{
	/// <summary>
	/// A class that contains methods for P/Invoking the Win32 API
	/// </summary>
	internal abstract class NativeMethods
	{
		/// <summary>
		/// Implemented by many of the Microsoft® Windows® Shell dynamic-link libraries 
		/// (DLLs) to allow applications to obtain DLL-specific version information
		/// </summary>
		/// <param name="pdvi">Pointer to a DLLVERSIONINFO structure that receives the 
		/// version information. The cbSize member must be filled in before calling 
		/// the function</param>
		/// <returns>Returns NOERROR if successful, or an OLE-defined error value otherwise</returns>
		[DllImport("Comctl32.dll", CharSet=CharSet.Auto, SetLastError=true)] 
		internal static extern int DllGetVersion(ref DLLVERSIONINFO pdvi);


		/// <summary>
		/// Tests if a visual style for the current application is active
		/// </summary>
		/// <returns>TRUE if a visual style is enabled, and windows with 
		/// visual styles applied should call OpenThemeData to start using 
		/// theme drawing services, FALSE otherwise</returns>
		[DllImport("UxTheme.dll", CharSet=CharSet.Auto, SetLastError=true)]
		internal static extern bool IsThemeActive();


		/// <summary>
		/// Reports whether the current application's user interface 
		/// displays using visual styles
		/// </summary>
		/// <returns>TRUE if the application has a visual style applied,
		/// FALSE otherwise</returns>
		[DllImport("UxTheme.dll", CharSet=CharSet.Auto, SetLastError=true)]
		internal static extern bool IsAppThemed();


		/// <summary>
		/// Opens the theme data for a window and its associated class
		/// </summary>
		/// <param name="hwnd">Handle of the window for which theme data 
		/// is required</param>
		/// <param name="pszClassList">Pointer to a string that contains 
		/// a semicolon-separated list of classes</param>
		/// <returns>OpenThemeData tries to match each class, one at a 
		/// time, to a class data section in the active theme. If a match 
		/// is found, an associated HTHEME handle is returned. If no match 
		/// is found NULL is returned</returns>
		[DllImport("UxTheme.dll", CharSet=CharSet.Auto, SetLastError=true)]
		internal static extern IntPtr OpenThemeData(IntPtr hwnd, [MarshalAs(UnmanagedType.LPTStr)] string pszClassList);


		/// <summary>
		/// Closes the theme data handle
		/// </summary>
		/// <param name="hTheme">Handle to a window's specified theme data. 
		/// Use OpenThemeData to create an HTHEME</param>
		/// <returns>Returns S_OK if successful, or an error value otherwise</returns>
		[DllImport("UxTheme.dll", CharSet=CharSet.Auto, SetLastError=true)]
		internal static extern int CloseThemeData(IntPtr hTheme);


		/// <summary>
		/// Draws the background image defined by the visual style for the 
		/// specified control part
		/// </summary>
		/// <param name="hTheme">Handle to a window's specified theme data. 
		/// Use OpenThemeData to create an HTHEME</param>
		/// <param name="hdc">Handle to a device context (HDC) used for 
		/// drawing the theme-defined background image</param>
		/// <param name="iPartId">Value of type int that specifies the part 
		/// to draw</param>
		/// <param name="iStateId">Value of type int that specifies the state 
		/// of the part to draw</param>
		/// <param name="pRect">Pointer to a RECT structure that contains the 
		/// rectangle, in logical coordinates, in which the background image 
		/// is drawn</param>
		/// <param name="pClipRect">Pointer to a RECT structure that contains 
		/// a clipping rectangle. This parameter may be set to NULL</param>
		/// <returns>Returns S_OK if successful, or an error value otherwise</returns>
		[DllImport("UxTheme.dll", CharSet=CharSet.Auto, SetLastError=true)]
		internal static extern int DrawThemeBackground(IntPtr hTheme, IntPtr hdc, int iPartId, int iStateId, ref RECT pRect, ref RECT pClipRect);


		/// <summary>
		/// The SendMessage function sends the specified message to a 
		/// window or windows. It calls the window procedure for the 
		/// specified window and does not return until the window 
		/// procedure has processed the message
		/// </summary>
		/// <param name="hwnd">Handle to the window whose window procedure will 
		/// receive the message</param>
		/// <param name="msg">Specifies the message to be sent</param>
		/// <param name="wParam">Specifies additional message-specific information</param>
		/// <param name="lParam">Specifies additional message-specific information</param>
		/// <returns>The return value specifies the result of the message processing; 
		/// it depends on the message sent</returns>
		[DllImport("User32.dll", CharSet=CharSet.Auto, SetLastError=true)] 
		internal static extern int SendMessage(IntPtr hwnd, int msg, int wParam, int lParam);


		/// <summary>
		/// The TrackMouseEvent function posts messages when the mouse pointer 
		/// leaves a window or hovers over a window for a specified amount of time
		/// </summary>
		/// <param name="tme">A TRACKMOUSEEVENT structure that contains tracking 
		/// information</param>
		/// <returns>true if the function succeeds, false otherwise</returns>
		[DllImport("User32.dll", CharSet=CharSet.Auto, SetLastError=true)]
		internal static extern bool TrackMouseEvent(TRACKMOUSEEVENT tme);


		/// <summary>
		/// The PostMessage function places (posts) a message in the message queue associated 
		/// with the thread that created the specified window and returns without waiting for 
		/// the thread to process the message
		/// </summary>
		/// <param name="hwnd">Handle to the window whose window procedure is to receive the 
		/// message</param>
		/// <param name="msg">Specifies the message to be posted</param>
		/// <param name="wparam">Specifies additional message-specific information</param>
		/// <param name="lparam">Specifies additional message-specific information</param>
		/// <returns>If the function succeeds, the return value is nonzero. If the function 
		/// fails, the return value is zero</returns>
		[DllImport("User32.dll", CharSet=CharSet.Auto, SetLastError=true)]
		internal static extern IntPtr PostMessage(IntPtr hwnd, int msg, int wparam, int lparam);


		/// <summary>
		/// The MessageBeep function plays a waveform sound. The waveform sound for each 
		/// sound type is identified by an entry in the registry
		/// </summary>
		/// <param name="type">Sound type, as identified by an entry in the registry</param>
		/// <returns>If the function succeeds, the return value is nonzero. If the function 
		/// fails, the return value is zero</returns>
		[DllImport("User32.dll", CharSet=CharSet.Auto, SetLastError=true)]
		internal static extern bool MessageBeep(int type);


		/// <summary>
		/// The NotifyWinEvent function signals the system that a predefined event occurred. 
		/// If any client applications have registered a hook function for the event, the 
		/// system calls the client's hook function
		/// </summary>
		/// <param name="winEvent">Specifies the event that occurred</param>
		/// <param name="hwnd">Handle to the window that contains the object that generated 
		/// the event</param>
		/// <param name="objType">Identifies the kind of object that generated the event</param>
		/// <param name="objID">Identifies whether the event was generated by an object or 
		/// by a child element of the object. If this value is CHILDID_SELF, the event was 
		/// generated by the object itself. If not, this value is the child ID of the element 
		/// that generated the event</param>
		[DllImport("User32.dll", CharSet=CharSet.Auto, SetLastError=true)]
		internal static extern void NotifyWinEvent(int winEvent, IntPtr hwnd, int objType, int objID);


		/// <summary>
		/// The ScrollWindow function scrolls the contents of the specified window's client area
		/// </summary>
		/// <param name="hWnd">Handle to the window where the client area is to be scrolled</param>
		/// <param name="XAmount">Specifies the amount, in device units, of horizontal scrolling. 
		/// This parameter must be a negative value to scroll the content of the window to the left</param>
		/// <param name="YAmount">Specifies the amount, in device units, of vertical scrolling. 
		/// This parameter must be a negative value to scroll the content of the window up</param>
		/// <param name="lpRect">Pointer to the RECT structure specifying the portion of the 
		/// client area to be scrolled. If this parameter is NULL, the entire client area is 
		/// scrolled</param>
		/// <param name="lpClipRect">Pointer to the RECT structure containing the coordinates 
		/// of the clipping rectangle. Only device bits within the clipping rectangle are affected. 
		/// Bits scrolled from the outside of the rectangle to the inside are painted; bits scrolled 
		/// from the inside of the rectangle to the outside are not painted</param>
		/// <returns>If the function succeeds, the return value is nonzero. If the function fails, 
		/// the return value is zero</returns>
		[DllImport("User32.dll", CharSet=CharSet.Auto, SetLastError=true)]
		internal static extern bool ScrollWindow(IntPtr hWnd, int XAmount, int YAmount, ref RECT lpRect, ref RECT lpClipRect);


		/// <summary>
		/// The keybd_event function synthesizes a keystroke. The system can use such a synthesized 
		/// keystroke to generate a WM_KEYUP or WM_KEYDOWN message. The keyboard driver's interrupt 
		/// handler calls the keybd_event function
		/// </summary>
		/// <param name="bVk">Specifies a virtual-key code</param>
		/// <param name="bScan">This parameter is not used</param>
		/// <param name="dwFlags">Specifies various aspects of function operation</param>
		/// <param name="dwExtraInfo"></param>
		[DllImport("User32.dll", CharSet=CharSet.Auto, SetLastError=true)]
		internal extern static void keybd_event(byte bVk, byte bScan, KeyEventFFlags dwFlags, int dwExtraInfo);


		/// <summary>
		/// The PeekMessage function dispatches incoming sent messages, checks the thread message 
		/// queue for a posted message, and retrieves the message (if any exist).
		/// </summary>
		/// <param name="msg">Pointer to an MSG structure that receives message information</param>
		/// <param name="hwnd">Handle to the window whose messages are to be examined. The window 
		/// must belong to the current thread. If hWnd is NULL, PeekMessage retrieves messages for 
		/// any window that belongs to the current thread. If hWnd is INVALID_HANDLE_VALUE, 
		/// PeekMessage retrieves messages whose hWnd value is NULL, as posted by the PostThreadMessage 
		/// function</param>
		/// <param name="msgMin">Specifies the value of the first message in the range of messages 
		/// to be examined. Use WM_KEYFIRST to specify the first keyboard message or WM_MOUSEFIRST 
		/// to specify the first mouse message. If wMsgFilterMin and wMsgFilterMax are both zero, 
		/// PeekMessage returns all available messages (that is, no range filtering is performed).</param>
		/// <param name="msgMax">Specifies the value of the last message in the range of messages 
		/// to be examined. Use WM_KEYLAST to specify the first keyboard message or WM_MOUSELAST 
		/// to specify the last mouse message. If wMsgFilterMin and wMsgFilterMax are both zero, 
		/// PeekMessage returns all available messages (that is, no range filtering is performed).</param>
		/// <param name="remove">Specifies how messages are handled</param>
		/// <returns>If a message is available, the return value is nonzero. If no messages are 
		/// available, the return value is zero</returns>
		[DllImport("User32.dll", CharSet=CharSet.Auto, SetLastError=true)]
		internal static extern bool PeekMessage(ref MSG msg, IntPtr hwnd, int msgMin, int msgMax, int remove);


		/// <summary>
		/// The TranslateMessage function translates virtual-key messages into character messages. 
		/// The character messages are posted to the calling thread's message queue, to be read the 
		/// next time the thread calls the GetMessage or PeekMessage function
		/// </summary>
		/// <param name="msg">Pointer to an MSG structure that contains message information retrieved 
		/// from the calling thread's message queue by using the GetMessage or PeekMessage function</param>
		/// <returns>If the message is translated (that is, a character message is posted to the 
		/// thread's message queue), the return value is nonzero.If the message is WM_KEYDOWN, 
		/// WM_KEYUP, WM_SYSKEYDOWN, or WM_SYSKEYUP, the return value is nonzero, regardless of 
		/// the translation. If the message is not translated (that is, a character message is not 
		/// posted to the thread's message queue), the return value is zero</returns>
		[DllImport("User32.dll", CharSet=CharSet.Auto, SetLastError=true)]
		internal static extern bool TranslateMessage(ref MSG msg);


		/// <summary>
		/// The DispatchMessage function dispatches a message to a window procedure. It is typically 
		/// used to dispatch a message retrieved by the GetMessage funct
		/// </summary>
		/// <param name="msg">Pointer to an MSG structure that contains the message</param>
		/// <returns>The return value specifies the value returned by the window procedure. Although 
		/// its meaning depends on the message being dispatched, the return value generally is ignored</returns>
		[DllImport("User32.dll", CharSet=CharSet.Auto, SetLastError=true)]
		internal static extern int DispatchMessage(ref MSG msg);
	}
}
