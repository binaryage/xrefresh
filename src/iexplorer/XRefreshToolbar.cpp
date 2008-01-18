// XRefreshToolbar.cpp : Implementation of CXRefreshToolbar

#include "stdafx.h"
#include "XRefreshToolbar.h"
#include "Services.h"
#include "AboutBox.h"
#include "SitesDialog.h"
#include "DialogManager.h"
#include "BrowserManager.h"
#include "XRefreshBHO.h"

#include "Debug.h"

const wchar_t *g_wcToolbarWindowText = _T("XRefreshToolbarWindow");

// CXRefreshToolbar
CXRefreshToolbar::CXRefreshToolbar():
m_Toolbar(TOOLBARCLASSNAME, this, ALT_MAP_TB_TOOLBAR),
m_dwBandId(0), 
m_dwViewMode(0), 
m_iToolbarHeight(22)
{
	InitRoot();
	DT(TRACE_I(FS(_T("Toolbar[%08X]: constructor"), this)));

	CBitmap toolbar;
	toolbar.LoadBitmap(MAKEINTRESOURCE(IDB_ICONS));
	if (toolbar.IsNull()) throw CXRefreshWindowsError(GetLastError());

	CBitmap mask;
	mask.LoadBitmap(MAKEINTRESOURCE(IDB_ICONS));
	if (mask.IsNull()) throw CXRefreshWindowsError(GetLastError());

	CDPIHelper::ScaleBitmap(toolbar);
	CDPIHelper::ScaleBitmap(mask);

	int width = (int)CDPIHelper::ScaleX(16);
	int height = (int)CDPIHelper::ScaleY(16);
	m_kImageList.Create(width, height, ILC_COLOR24 | ILC_MASK, 3, 3);
	if (m_kImageList.IsNull()) throw CXRefreshWindowsError(GetLastError());
	if (m_kImageList.Add(toolbar, mask) == -1) throw CXRefreshWindowsError(GetLastError());
}

CXRefreshToolbar::~CXRefreshToolbar()
{
	DT(TRACE_I(FS(_T("Toolbar[%08X]: destructor"), this)));
	m_Toolbar.DestroyWindow();
	UnsubclassWindow();
}

STDMETHODIMP 
CXRefreshToolbar::GetWindow(HWND *phwnd)
{
	DT(TRACE_I(FS(_T("Toolbar[%08X]: GetWindow(%08X)"), this, phwnd)));
	*phwnd = m_Toolbar;
	return S_OK;
}

STDMETHODIMP 
CXRefreshToolbar::ContextSensitiveHelp(BOOL fEnterMode)
{
	DT(TRACE_I(FS(_T("Toolbar[%08X]: ContextSensitiveHelp(%s)"), this, fEnterMode?_T("TRUE"):_T("FALSE"))));
	return E_NOTIMPL;
}

STDMETHODIMP 
CXRefreshToolbar::ShowDW(BOOL fShow)
{
	DT(TRACE_I(FS(_T("Toolbar[%08X]: ShowDW(%s)"), this, fShow?_T("TRUE"):_T("FALSE"))));
	if (!m_Toolbar) return E_FAIL;
	m_Toolbar.ShowWindow(fShow ? SW_SHOW : SW_HIDE);
	return S_OK;
}

STDMETHODIMP 
CXRefreshToolbar::CloseDW(DWORD dwReserved)
{
	DT(TRACE_I(FS(_T("Toolbar[%08X]: CloseDW(%08X)"), this, dwReserved)));
	ShowDW(FALSE);
	// CloseDW purposely does not destroy the window. We leave that to the destructor.
	return S_OK;
}

STDMETHODIMP 
CXRefreshToolbar::ResizeBorderDW(LPCRECT prcBorder, IUnknown *punkToolbarSite, BOOL fReserved)
{
	return E_NOTIMPL;
}

STDMETHODIMP 
CXRefreshToolbar::UIActivateIO(BOOL fActivate, LPMSG lpMsg)
{
	if (fActivate) m_Toolbar.SetFocus();
	return S_OK;
}

STDMETHODIMP 
CXRefreshToolbar::HasFocusIO()
{
	// TODO: properly indicate when my UI has focus
	return S_FALSE;
}

STDMETHODIMP 
CXRefreshToolbar::TranslateAcceleratorIO(LPMSG lpMsg)
{
	// generic implementation, override in base class to handle accelerators
	if( (lpMsg->message == WM_KEYDOWN || lpMsg->message == WM_KEYUP) && 
		(lpMsg->wParam == VK_TAB || lpMsg->wParam == VK_F6) )
		return S_FALSE;
	else
	{
		TranslateMessage(lpMsg);
		DispatchMessage(lpMsg);
		return S_OK;
	}
}

STDMETHODIMP 
CXRefreshToolbar::SetSite(IUnknown *pUnknownSite)
{
	DT(TRACE_I(FS(_T("Toolbar[%08X]: SetSite(%08X)"), this, pUnknownSite)));
	try {
		if (!!pUnknownSite)
		{
			// attach the window
			HWND hWnd;
			CComPtr<IUnknown> site(pUnknownSite);
			CComQIPtr<IOleWindow> window(site);
			window->GetWindow(&hWnd);
			if (!hWnd) 
			{
				TRACE_E(FS(_T("Toolbar[%08X]: Cannot retrieve toolbar base window"), this));
				return E_FAIL;
			}
			SubclassWindow(hWnd);

			// get a WebBrowser reference
			CComQIPtr<IServiceProvider> serviceProvider(site);
			serviceProvider->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void**)&m_Browser);
			site->QueryInterface(IID_IInputObjectSite, (void**)&m_Site);

			// retrive browser id
			{
				BrowserManagerLock browserManager;
				m_BrowserId = browserManager->AllocBrowserId(m_Browser, this);
				ATLASSERT(m_BrowserId!=NULL_BROWSER);
			}

			// create main window
			CreateMainWindow();
		}
	}
	catch (CXRefreshRuntimeError &ex)
	{
		HandleError(ex.ErrorMessage());
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP 
CXRefreshToolbar::GetSite(const IID &riid, void **ppvSite)
{
	DT(TRACE_I(FS(_T("Toolbar[%08X]: GetSite(...)"), this)));
	if (!ppvSite) return E_INVALIDARG;
	*ppvSite = NULL;
	return m_Site->QueryInterface(riid, ppvSite);
}

STDMETHODIMP 
CXRefreshToolbar::GetBandInfo(DWORD dwBandId, DWORD dwViewMode, DESKBANDINFO *pdbi)
{
	DT(TRACE_I(FS(_T("Toolbar[%08X]: GetBandInfo(...)"), this)));
	try
	{
		if( pdbi == NULL )
			return E_INVALIDARG;

		m_dwBandId = dwBandId;
		m_dwViewMode = dwViewMode;

		if( pdbi->dwMask & DBIM_MINSIZE )
		{
			pdbi->ptMinSize = GetMinSize();
		}

		if( pdbi->dwMask & DBIM_MAXSIZE )
		{
			pdbi->ptMaxSize = GetMaxSize();
		}

		if( pdbi->dwMask & DBIM_INTEGRAL )
		{
			pdbi->ptIntegral.x = 1;
			pdbi->ptIntegral.y = 1;
		}

		if( pdbi->dwMask & DBIM_ACTUAL )
		{
			pdbi->ptActual = GetActualSize();
		}

		if( pdbi->dwMask & DBIM_TITLE )
		{
			wcscpy_s(pdbi->wszTitle, GetTitle());
		}

		if( pdbi->dwMask & DBIM_MODEFLAGS )
		{
			pdbi->dwModeFlags = DBIMF_NORMAL | DBIMF_USECHEVRON;
		}

		if( pdbi->dwMask & DBIM_BKCOLOR )
		{
			// use the default background color by removing this flag
			pdbi->dwMask &= ~DBIM_BKCOLOR;
		}

		return S_OK;
	}
	catch (CXRefreshRuntimeError &ex)
	{
		HandleError(ex.ErrorMessage());
		return E_FAIL;
	}
}

void 
CXRefreshToolbar::HandleError(const CString &errorMessage)
{
	CString message = LoadStringResource(IDS_ERROR_BASEMESSAGE);
	message += errorMessage;
	::MessageBox(NULL, message, LoadStringResource(IDS_TOOLBAR_NAME), MB_OK | MB_ICONERROR);
}

void 
CXRefreshToolbar::CreateMainWindow()
{
	DWORD style = WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_TABSTOP | CCS_TOP | CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN | TBSTYLE_FLAT | TBSTYLE_LIST | TBSTYLE_TRANSPARENT | TBSTYLE_TOOLTIPS;
	m_Toolbar.Create(*this, rcDefault, MAIN_TOOLBAR_WINDOW_NAME, style, 0, IDC_TB_TOOLBAR);
	RECT  rc;
	GetClientRect(&rc);
	
	m_Toolbar.SetWindowText(g_wcToolbarWindowText);
	m_Toolbar.SendMessage(TB_SETEXTENDEDSTYLE, 0, SendMessage(m_Toolbar, TB_GETEXTENDEDSTYLE, 0, 0) | TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_MIXEDBUTTONS);
	m_Toolbar.SendMessage(TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);
	m_Toolbar.SendMessage(TB_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(static_cast<HIMAGELIST>(m_kImageList)));

	TBBUTTON button[1];
	ZeroMemory(button, sizeof(button));

	button[0].iBitmap = 11;
	button[0].idCommand = IDC_TOOLBUTTON;
	button[0].fsState = TBSTATE_ENABLED;
	button[0].fsStyle = BTNS_DROPDOWN | BTNS_AUTOSIZE | BTNS_WHOLEDROPDOWN;
	button[0].iString = m_Toolbar.SendMessage((UINT)TB_ADDSTRING,(WPARAM)0,(LPARAM)_T("XRefresh"));

	// Add the buttons
	if (!m_Toolbar.SendMessage(TB_ADDBUTTONS, sizeof(button) / sizeof(TBBUTTON), reinterpret_cast<LPARAM>(button))) 
		throw CXRefreshWindowsError(GetLastError());

	// In order to make sure the edit control has the right height regardless of font size settings,
	// we give it the same height as the toolbar buttons.
	LRESULT size = m_Toolbar.SendMessage(TB_GETBUTTONSIZE, 0, 0);
	int height = HIWORD(size);
	int width = LOWORD(size);
	m_iToolbarHeight = height;
}

LRESULT 
CXRefreshToolbar::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// notify IE of the focus change
	DT(TRACE_I(FS(_T("Toolbar[%08X]: Focus gained"), this)));
	m_Site->OnFocusChangeIS(static_cast<IDockingWindow*>(this), TRUE);
	return S_OK;
}

LRESULT 
CXRefreshToolbar::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// notify IE of the focus change
	DT(TRACE_I(FS(_T("Toolbar[%08X]: Focus lost"), this)));
	m_Site->OnFocusChangeIS(static_cast<IDockingWindow*>(this), FALSE);
	return S_OK;
}

LRESULT 
CXRefreshToolbar::OnToolbarDropdown(WPARAM wParam, LPNMHDR lParam, BOOL& bHandled)
{
	// dispatcher for button popup menus
	LPNMTOOLBAR data = (LPNMTOOLBAR)lParam;
	switch (data->iItem) {
		case IDC_TOOLBUTTON: OnGeneralDropdown(data); break;
		default: ATLASSERT(0);
	}
	bHandled = TRUE;
	return 0;
}

bool
CXRefreshToolbar::OnGeneralDropdown(LPNMTOOLBAR data)
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

CString 
CXRefreshToolbar::GetTitle()
{
	return LoadStringResource(IDS_TOOLBAR_CAPTION);
}

LRESULT
CXRefreshToolbar::OnToolbarMenu(WORD wCode, WORD wId, HWND hWnd, BOOL& bHandled)
{
	switch (wId) {
		case ID_POPUPMENU_OPTIONS:
			// bring options dialog
			{
				ServicesLock services;
				services->OpenSettingsDialog();
			}
			break;
		case ID_POPUPMENU_VISITSITE:
			{
				if (!!m_Browser)
				{
					CComBSTR url = _T("http://xrefresh.com");
					CComVariant target = _T("_blank");
					CComVariant flags = navOpenInNewTab;
					m_Browser->Navigate(url, &flags, &target, NULL, NULL);
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
				CSitesDialog kSitesDialog(GetSiteRootUrl(m_Browser));
				kSitesDialog.DoModal();
			}
			break;
		case ID_POPUPMENU_GENERATECRASH:
			{
				int* p = 0;
				*p = 1;
			}
			break;
		case ID_POPUPMENU_TESTDIALOG:
			{
				DialogManagerLock dialogManager;
				dialogManager->TestDialog();
			}
			break;
		case ID_POPUPMENU_CHECKFORUPDATES:
			{
				//ComPtr<IWebBrowser2> browser = GetBrowser();
				//BSTR url = SysAllocString(menu == ID_POPUPMENU_VISITOOKII ? LoadStringResource(IDS_URL_OOKII).c_str() : LoadStringResource(IDS_URL_UPDATE).c_str());
				//_variant_t target = _T("_blank");
				//browser->Navigate(url, NULL, &target, NULL, NULL);
				//SysFreeString(url);
				return true;
			}
	}
	return 0;
}

LRESULT
CXRefreshToolbar::OnToolbarNeedText(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	LPNMTTDISPINFO pttdi = reinterpret_cast<LPNMTTDISPINFO>(pnmh);
	
	switch (idCtrl) {
		case IDC_TOOLBUTTON: pttdi->lpszText = _T("XRefresh Menu"); break;
		default:
			bHandled = FALSE;
			return 0;
	}
	
	pttdi->hinst = NULL;
	pttdi->uFlags = TTF_DI_SETITEM;

	//-- message processed
	return 0;
}

CComPtr<IWebBrowser2>
CXRefreshToolbar::GetBrowser()
{
	return m_Browser;
}

POINTL
CXRefreshToolbar::GetMinSize() const
{
	POINTL pt = { 20, m_iToolbarHeight };
	return pt;
}

POINTL
CXRefreshToolbar::GetMaxSize() const
{
	POINTL pt = { -1, m_iToolbarHeight };
	return pt;
}

POINTL
CXRefreshToolbar::GetActualSize() const
{
	POINTL pt = { -1, m_iToolbarHeight };
	return pt;
}
