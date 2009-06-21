#include "StdAfx.h"
#include "AboutBox.h"
#include "HelperbarWindow.h"
#include "BrowserManager.h"
#include "XRefreshBHO.h"
#include "SitesDialog.h"

//#include "Debug.h"

const wchar_t *g_wcHBToolbarWindowText = _T("XRefreshHBToolbarWindow");

#define IDC_HB_TOOL_BUTTON  100
#define IDC_HB_REFRESH_BUTTON  101
#define IDC_HB_CONNECT_BUTTON  102
#define IDC_HB_DISCONNECT_BUTTON  103

CHelperbarWindow::CHelperbarWindow():
	m_BrowserId(NULL_BROWSER),
	m_Toolbar(TOOLBARCLASSNAME, this, ALT_MAP_HELPERBAR_TOOLBAR)
{
	DT(TRACE_I(FS(_T("HelperbarMainWindow[%08X]: constructor"), this)));
	m_Font.CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,	OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY, FF_MODERN, NULL);

	m_ToolbarBitmap.LoadBitmap(MAKEINTRESOURCE(IDB_ICONS));
	if (m_ToolbarBitmap.IsNull()) throw CXRefreshWindowsError(GetLastError());
	CDPIHelper::ScaleBitmap(m_ToolbarBitmap);

	m_ToolbarMaskBitmap.LoadBitmap(MAKEINTRESOURCE(IDB_ICONS));
	if (m_ToolbarMaskBitmap.IsNull()) throw CXRefreshWindowsError(GetLastError());
	CDPIHelper::ScaleBitmap(m_ToolbarMaskBitmap);

	int width = (int)CDPIHelper::ScaleX(16);
	int height = (int)CDPIHelper::ScaleY(16);
	m_ImageList.Create(width, height, ILC_COLOR24 | ILC_MASK, 3, 3);
	if (m_ImageList.IsNull()) throw CXRefreshWindowsError(GetLastError());
	if (m_ImageList.Add(m_ToolbarBitmap, m_ToolbarMaskBitmap) == -1) throw CXRefreshWindowsError(GetLastError());
}

CHelperbarWindow::~CHelperbarWindow()
{
	DT(TRACE_I(FS(_T("HelperbarMainWindow[%08X]: destructor"), this)));
}

LRESULT
CHelperbarWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ATLASSERT(m_BrowserId!=NULL_BROWSER);

	DWORD consoleStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN;
	m_Console.Create(m_hWnd, CRect(0, 0, 800, 300), CONSOLE_WINDOW_NAME, consoleStyle);
	m_Console.SetFont(m_Font);

	DWORD style = WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_TABSTOP | CCS_TOP | CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN | TBSTYLE_FLAT | TBSTYLE_LIST | TBSTYLE_TRANSPARENT | TBSTYLE_TOOLTIPS;
	m_Toolbar.Create(m_hWnd, rcDefault, MAIN_TOOLBAR_WINDOW_NAME, style, 0, IDC_HB_TOOLBAR);
	RECT  rc;
	GetClientRect(&rc);
	
	m_Toolbar.SetWindowText(g_wcHBToolbarWindowText);
	m_Toolbar.SendMessage(TB_SETEXTENDEDSTYLE, 0, SendMessage(m_Toolbar, TB_GETEXTENDEDSTYLE, 0, 0) | TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_MIXEDBUTTONS);
	m_Toolbar.SendMessage(TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);
	m_Toolbar.SendMessage(TB_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(static_cast<HIMAGELIST>(m_ImageList)));

	TBBUTTON button[6];
	ZeroMemory(button, sizeof(button));

	int i = 0;
	button[i].iBitmap = 11;
	button[i].idCommand = IDC_HB_TOOL_BUTTON;
	button[i].fsState = TBSTATE_ENABLED;
	button[i].fsStyle = BTNS_DROPDOWN | BTNS_WHOLEDROPDOWN | BTNS_AUTOSIZE | BTNS_SHOWTEXT;
	button[i].iString = -1;

	i++;
	button[i].iBitmap = 0;
	button[i].idCommand = 0;
	button[i].fsState = TBSTATE_ENABLED;
	button[i].fsStyle = BTNS_SEP;
	button[i].iString = -1;

	i++;
	button[i].iBitmap = 8;
	button[i].idCommand = IDC_HB_REFRESH_BUTTON;
	button[i].fsState = TBSTATE_ENABLED;
	button[i].fsStyle = BTNS_AUTOSIZE|BTNS_SHOWTEXT;
	button[i].iString = m_Toolbar.SendMessage((UINT)TB_ADDSTRING,(WPARAM)0,(LPARAM)_T("Refresh"));

	i++;
	button[i].iBitmap = 0;
	button[i].idCommand = 0;
	button[i].fsState = TBSTATE_ENABLED;
	button[i].fsStyle = BTNS_SEP;
	button[i].iString = 0;

	i++;
	button[i].iBitmap = 2;
	button[i].idCommand = IDC_HB_CONNECT_BUTTON;
	button[i].fsState = TBSTATE_ENABLED;
	button[i].fsStyle = BTNS_AUTOSIZE|BTNS_SHOWTEXT;
	button[i].iString = m_Toolbar.SendMessage((UINT)TB_ADDSTRING,(WPARAM)0,(LPARAM)_T("Connect"));

	i++;
	button[i].iBitmap = 4;
	button[i].idCommand = IDC_HB_DISCONNECT_BUTTON;
	button[i].fsState = TBSTATE_ENABLED;
	button[i].fsStyle = BTNS_AUTOSIZE| BTNS_SHOWTEXT;
	button[i].iString = m_Toolbar.SendMessage((UINT)TB_ADDSTRING,(WPARAM)0,(LPARAM)_T("Disconnect"));

	// add buttons
	if (!m_Toolbar.SendMessage(TB_ADDBUTTONS, sizeof(button) / sizeof(TBBUTTON), reinterpret_cast<LPARAM>(button))) 
		throw CXRefreshWindowsError(GetLastError());

	// in order to make sure the edit control has the right height regardless of font size settings,
	// we give it the same height as the toolbar buttons
	LRESULT size = m_Toolbar.SendMessage(TB_GETBUTTONSIZE, 0, 0);
	int height = HIWORD(size);
	int width = LOWORD(size);
	m_ToolbarHeight = height;

	UpdateLayout();
	return 0;
}

LRESULT
CHelperbarWindow::OnToolbarDropdown(WPARAM wParam, LPNMHDR lParam, BOOL& bHandled)
{
	// dispatcher for button popup menus
	LPNMTOOLBAR data = (LPNMTOOLBAR)lParam;
	switch (data->iItem) {
		case IDC_HB_TOOL_BUTTON: OnGeneralDropdown(data); break;
		default: ATLASSERT(0);
	}
	bHandled = TRUE;
	return 0;
}

bool
CHelperbarWindow::OnGeneralDropdown(LPNMTOOLBAR data)
{
	RECT rect;
	m_Toolbar.SendMessage(TB_GETRECT, data->iItem, reinterpret_cast<LPARAM>(&rect));
	m_Toolbar.MapWindowPoints(HWND_DESKTOP, reinterpret_cast<LPPOINT>(&rect), 2);
	TPMPARAMS tpm;
	tpm.cbSize = sizeof(tpm);
	tpm.rcExclude = rect;

	CMenu menu;
	menu.LoadMenu(MAKEINTRESOURCE(IDR_TOOLMENU));
	if (menu.IsNull()) throw CXRefreshWindowsError(GetLastError());

	CMenuHandle popupMenu = menu.GetSubMenu(0);
	if (popupMenu.IsNull()) throw CXRefreshWindowsError(GetLastError());

	TrackPopupMenuEx(popupMenu, TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL, rect.left, rect.bottom, m_Toolbar, &tpm);
	return true;
}

LRESULT
CHelperbarWindow::OnToolbarMenu(WORD wCode, WORD wId, HWND hWnd, BOOL& bHandled)
{
	CXRefreshBHO* BHO;
	CBrowserMessageWindow* window;
	{
		BrowserManagerLock browserManager;
		window = browserManager->FindBrowserMessageWindow(m_BrowserId);
		if (!window) return 0;
		BHO = window->GetBHO();
		if (!BHO) return 0;
	}

	switch (wId) {
		case IDC_HB_TOOL_BUTTON:
			break;
		case IDC_HB_REFRESH_BUTTON:
			Log(_T("Manual refresh performed by user"), ICON_REFRESH);
			BHO->PerformRefresh();
			break;
		case IDC_HB_CONNECT_BUTTON:
			Log(_T("Connection requested by user"), ICON_CONNECTED_BTN);
			BHO->Connect();
			break;
		case IDC_HB_DISCONNECT_BUTTON:
			Log(_T("Disconnection requested by user"), ICON_DISCONNECTED_BTN);
			BHO->Disconnect();
			break;
		case ID_POPUPMENU_VISITSITE:
			{
				CComPtr<IWebBrowser2> browser = BHO->GetBrowser();
				if (!!browser)
				{
					CComBSTR url = _T("http://xrefresh.binaryage.com");
					CComVariant target = _T("_blank");
					CComVariant flags = navOpenInNewTab;
					browser->Navigate(url, &flags, &target, NULL, NULL);
				}
			}
			break;
		case ID_POPUPMENU_ABOUT:
			{
				CAboutBox kAboutBox;
				kAboutBox.DoModal();
			}
			break;
		case ID_POPUPMENU_ALLOWEDSITES:
			{
				CComQIPtr<IWebBrowser2> browser;
				{
					BrowserManagerLock browserManager;
					CBrowserMessageWindow* window = browserManager->FindBrowserMessageWindow(m_BrowserId);
					if (!window) return 0;
					browser = window->GetBrowserInterface();
					if (!browser) return 0;
				}
				CSitesDialog kSitesDialog(GetSiteRootUrl(browser));
				kSitesDialog.DoModal();
			}
			break;
	}
	return 0;
}

LRESULT
CHelperbarWindow::OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return FALSE;
}

LRESULT 
CHelperbarWindow::OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	WINDOWPOS * lpwndpos = (WINDOWPOS *) lParam;
	if (!(lpwndpos->flags & SWP_NOSIZE))
	{
		UpdateLayout();
		bHandled = TRUE;
	}
	return 0;
}

void
CHelperbarWindow::UpdateLayout()
{
	RECT  rc;
	GetClientRect(&rc);

	HDWP hdwp = BeginDeferWindowPos(2);
	hdwp = m_Toolbar.DeferWindowPos(hdwp, NULL, 0, 0, rc.right, m_ToolbarHeight, SWP_NOZORDER);
	hdwp = m_Console.DeferWindowPos(hdwp, NULL, 0, m_ToolbarHeight, rc.right, rc.bottom - m_ToolbarHeight, SWP_NOZORDER);
	EndDeferWindowPos(hdwp);
}

bool
CHelperbarWindow::SetBrowserId( TBrowserId browserId )
{
	m_BrowserId = browserId;
	m_Console.SetBrowserId(browserId);
	return true;
}

bool
CHelperbarWindow::SetDisplayMode(CHelperbarWindow::EDisplayMode mode)
{
	m_DisplayMode = mode;
	return true;
}

void
CHelperbarWindow::Log(CString message, int icon)
{
	BrowserManagerLock browserManager;
	CBrowserMessageWindow* window = browserManager->FindBrowserMessageWindow(m_BrowserId);
	if (!window) return;
	CXRefreshBHO* BHO = window->GetBHO();
	if (!BHO) return;
	BHO->Log(message, icon);
}

LRESULT 
CHelperbarWindow::OnToolbarNeedText(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	LPNMTTDISPINFO pttdi = reinterpret_cast<LPNMTTDISPINFO>(pnmh);
	
	switch (idCtrl) {
		case IDC_HB_CONNECT_BUTTON: pttdi->lpszText = _T("Connect to XRefresh Monitor"); break;
		case IDC_HB_DISCONNECT_BUTTON: pttdi->lpszText = _T("Disconnect from XRefresh Monitor"); break;
		case IDC_HB_REFRESH_BUTTON: pttdi->lpszText = _T("Perform manual refresh"); break;
		case IDC_HB_TOOL_BUTTON: pttdi->lpszText = _T("XRefresh Menu"); break;
		default:
			bHandled = FALSE;
			return 0;
	}
	
	pttdi->hinst = NULL;
	pttdi->uFlags = TTF_DI_SETITEM;

	//-- message processed
	return 0;
}

void
CHelperbarWindow::Update()
{
	m_Console.Update();
}
