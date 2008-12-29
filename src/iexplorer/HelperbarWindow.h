#pragma once

#include "ConsoleWindow.h"

#define IDC_HELPERBAR_WORKSPACE                   1
#define ALT_MAP_HELPERBAR_TOOLBAR                 2
#define IDC_HB_TOOLBAR                            3

//////////////////////////////////////////////////////////////////////////
// CHelperbarWindow
class CHelperbarWindow: public CWindowImpl<CHelperbarWindow, CWindow> {
public:
	enum EDisplayMode {
		EDM_PINNED,
		EDM_FLOATING
	};

	CHelperbarWindow();
	virtual ~CHelperbarWindow();

	DECLARE_WND_CLASS(HELPERBAR_MAIN_CLASS_NAME)

	BEGIN_MSG_MAP(CHelperbarWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, OnWindowPosChanged)
		NOTIFY_HANDLER(IDC_HB_TOOLBAR, TBN_DROPDOWN, OnToolbarDropdown)
		NOTIFY_CODE_HANDLER(TTN_NEEDTEXT, OnToolbarNeedText)
		COMMAND_CODE_HANDLER(0, OnToolbarMenu) // 0 == from menu
		ALT_MSG_MAP(ALT_MAP_HELPERBAR_TOOLBAR)
	END_MSG_MAP()

	virtual LRESULT                               OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT                               OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT                               OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT                               OnToolbarNeedText(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	bool                                          SetBrowserId(TBrowserId browserId);
	bool                                          SetDisplayMode(EDisplayMode mode);

	virtual LRESULT                               OnToolbarDropdown(WPARAM wParam, LPNMHDR lParam, BOOL& bHandled);
	virtual bool                                  OnGeneralDropdown(LPNMTOOLBAR data);
	virtual LRESULT                               OnToolbarMenu(WORD wCode, WORD wId, HWND hWnd, BOOL& bHandled);

	void                                          Log(CString message, int icon);
	void                                          Update();

protected:
	virtual void                                  UpdateLayout();

	TBrowserId                                    m_BrowserId;
	CConsoleWindow                                m_Console;
	EDisplayMode                                  m_DisplayMode;
	CFont                                         m_Font;
	CContainedWindow                              m_Toolbar;

	int                                           m_ToolbarHeight;
	CAccelerator                                  m_Accelerator;
	CImageList                                    m_ImageList;
	CBitmap                                       m_ToolbarBitmap;
	CBitmap                                       m_ToolbarMaskBitmap;
};