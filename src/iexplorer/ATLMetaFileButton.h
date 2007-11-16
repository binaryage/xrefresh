//
// Copyright © 1999 Ian Brumby
//
// This source code may be used in compiled form in any way you desire. 
// Source file(s) may be redistributed unmodified by any means PROVIDING
// they are not sold for profit without the authors expressed written consent,
// and providing that this notice and the authors name and all copyright
// notices remain intact.
//
// ==========================================================================  
// HISTORY:	  
// ==========================================================================  
//			1.20	21 Apr 1999	- Initial release.
// ==========================================================================  
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/31/99 11:04p $
 * $Revision: 6 $
 * $Archive: /CodeJock/CJLibrary/CJMetaFileButton.cpp $
 *
 * $History: CJMetaFileButton.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:04p
 * Updated in $/CodeJock/CJLibrary
 * Overrode OnEraseBkgnd(...) and OnPaint() for flicker free drawing.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 10/25/99   Time: 10:52p
 * Updated in $/CodeJock/CJLibrary
 * Modified resource include for static builds.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:01a
 * Updated in $/CodeJock/CJLibrary
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 12:41p
 * Updated in $/CodeJock/CJLibrary
 * Added source control history to file header.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////
// ==========================================================================  
// Port to ATL/WTL (ATLMetaFileButton.h)
//
// Rashid Thadha 03/02/2001
// rashidthadha@hotmail.com
// ==========================================================================  

#if !defined(ATL_METAFILEBUTTON_H)
#define ATL_METAFILEBUTTON_H

class CMetaFileButton : public CWindowImpl<CMetaFileButton, CButton>
{
    // Construction
public:
    CMetaFileButton() {};
    
    BEGIN_MSG_MAP(CMetaFileButton)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem)
        END_MSG_MAP()
        
        // Attributes
public:
    
    // Operations
public:
    virtual void SetMetaFiles(HENHMETAFILE hMetaFile,
        HENHMETAFILE hMetaFileSel = NULL,
        HENHMETAFILE hMetaFileFocus = NULL,
        HENHMETAFILE hMetaFileDisabled = NULL)
    {
        m_hEnhMetaFile         = hMetaFile;
        m_hEnhMetaFileSel      = hMetaFileSel;
        m_hEnhMetaFileFocus    = hMetaFileFocus;
        m_hEnhMetaFileDisabled = hMetaFileDisabled;
    }
    
    // Overrides
public:
    
    
    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
    {
        ATLASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON); 
        
        // define some temporary variables.
        CDCHandle dc = lpDrawItemStruct->hDC;		
        
        CRect rcItem = lpDrawItemStruct->rcItem;
        int   nState = lpDrawItemStruct->itemState;
        
        // Paint the background.
        dc.FillSolidRect(rcItem, ::GetSysColor(COLOR_3DFACE));
        
        // draw 3D button outline
        CPen penOutline;
        penOutline.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNTEXT));
        CPen penLight;
        penLight.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));
        CPen penDark;
        penDark.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
        HPEN pOldPen = NULL;
        
        if (nState & ODS_SELECTED)
        {
            pOldPen = dc.SelectPen(penDark);
            dc.MoveTo(0, rcItem.bottom - 1);
            dc.LineTo(0, 1);
            dc.LineTo(rcItem.right - 1, 1);
        }
        else
        {
            pOldPen = dc.SelectPen(penLight);
            dc.MoveTo(0, rcItem.bottom - 2);
            dc.LineTo(0, 1);
            dc.LineTo(rcItem.right - 2, 1);
            dc.SelectPen(penDark);
            dc.MoveTo(0, rcItem.bottom - 1);
            dc.LineTo(rcItem.right - 2, rcItem.bottom - 1);
            dc.LineTo(rcItem.right - 2, 0);
            dc.MoveTo(1, rcItem.bottom - 2);
            dc.LineTo(rcItem.right - 3, rcItem.bottom - 2);
            dc.LineTo(rcItem.right - 3, 1);
        }
        dc.SelectPen(penOutline);
        dc.MoveTo(0, 0);
        dc.LineTo(rcItem.right - 1, 0);
        dc.LineTo(rcItem.right - 1, rcItem.bottom);
        
        HENHMETAFILE hMetaFile = m_hEnhMetaFile;
        if ((nState & ODS_SELECTED) && m_hEnhMetaFileSel != NULL)
            hMetaFile = m_hEnhMetaFileSel;
        else if ((nState & ODS_FOCUS) && m_hEnhMetaFileFocus != NULL)
            hMetaFile = m_hEnhMetaFileFocus;   // third image for focused
        else if ((nState & ODS_DISABLED) && m_hEnhMetaFileDisabled != NULL)
            hMetaFile = m_hEnhMetaFileDisabled;   // last image for disabled
        
        ATLASSERT(hMetaFile != NULL);
        
        CRect rcBounds(5, 4, 0, 0);
        int iButtonHeight = rcItem.Height();
        if (iButtonHeight > 15)
            iButtonHeight = 15;
        else if (iButtonHeight < 11)
            iButtonHeight = 11;
        int iArrowWidth = (iButtonHeight / 2) - 3;
        rcBounds.right = rcBounds.left + iArrowWidth - 1;
        rcBounds.bottom = rcBounds.top + (iArrowWidth * 2) - 2;
        
        if (nState & ODS_SELECTED)
            rcBounds += CPoint(1, 1);
        dc.PlayMetaFile(hMetaFile, rcBounds);
        
        // fix potential resource leak - KStowell - 10-21-99
        dc.SelectPen(pOldPen);
        penOutline.DeleteObject();
        penLight.DeleteObject();
        penDark.DeleteObject();
    }
    
    // Implementation
public:
    virtual ~CMetaFileButton()
    {
    }
    
    // Generated message map functions
protected:
    
    LRESULT OnPaint(UINT Msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CPaintDC dc(m_hWnd); // device context for painting
        
        // KStowell - Get the client rect.
        CRect rcClient, rcClip;
        dc.GetClipBox(&rcClip);
        GetClientRect(&rcClient);
        
        // KStowell - Create a memory device-context. This is done to help reduce
        // screen flicker, since we will paint the entire control to the
        // off screen device context first.
        CDC memDC;
        CBitmap bitmap;
        memDC.CreateCompatibleDC(dc.m_hDC);
        bitmap.CreateCompatibleBitmap(dc.m_hDC, rcClient.Width(), rcClient.Height());
        HBITMAP pOldBitmap = memDC.SelectBitmap(bitmap);
        
        // KStowell - Repaint the background.
        memDC.FillSolidRect(rcClient, ::GetSysColor(COLOR_WINDOW));
        
        // let the control do its default drawing.
        CWindowImpl<CMetaFileButton, CButton>::DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, 0);
        
        // KStowell - Copy the memory device context back into the original DC via BitBlt().
        dc.BitBlt(rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), memDC.m_hDC, 
            rcClip.left, rcClip.top, SRCCOPY);
        
        // KStowell - Cleanup resources.
        memDC.SelectBitmap(pOldBitmap);
        memDC.DeleteDC();
        bitmap.DeleteObject();
        
        return 0;
    }
    
    LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        DrawItem((LPDRAWITEMSTRUCT)lParam);
        return 0;
    }
    
    LRESULT OnLButtonDblClk(UINT Msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        // Fix for the double click problem with owner drawn buttons
        POINT pt;
        ::GetCursorPos(&pt);
        ::MapWindowPoints(NULL, (HWND)(m_hWnd), &pt, 1);
        ::SendMessage((HWND)(m_hWnd), WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));	
        
        return 0;
    }
    
    LRESULT OnEraseBkgnd(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
    {
        // KStowell - overridden for flicker-free drawing.
        //@@UNUSED_ALWAYS((HDC)wParam);
        // How do you port this to ATL ??
        return 0;
    }
    
private:
    HENHMETAFILE m_hEnhMetaFile;
    HENHMETAFILE m_hEnhMetaFileSel;
    HENHMETAFILE m_hEnhMetaFileFocus;
    HENHMETAFILE m_hEnhMetaFileDisabled;
};

#endif