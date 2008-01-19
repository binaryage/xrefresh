// XRefreshHelperbar.cpp : Implementation of CXRefreshHelperbar

#include "stdafx.h"
#include "XRefreshBHO.h"
#include "XRefreshHelperbar.h"
#include "BrowserManager.h"
#include "Module.h"

#include "Debug.h"

// CXRefreshHelperbar
CXRefreshHelperbar::CXRefreshHelperbar():
m_BandId(0), 
m_ViewMode(0),
m_BrowserId(NULL_BROWSER)
{
	InitRoot();
	DT(TRACE_I(FS(_T("Helperbar[%08X]: constructor"), this)));
}

CXRefreshHelperbar::~CXRefreshHelperbar()
{
	DT(TRACE_I(FS(_T("Helperbar[%08X]: destructor"), this)));
	DoneRoot();
}

STDMETHODIMP 
CXRefreshHelperbar::GetWindow(HWND *phwnd)
{
	DT(TRACE_I(FS(_T("Helperbar[%08X]: GetWindow(%08X)"), this, phwnd)));
	*phwnd = m_MainWindow;
	return S_OK;
}

STDMETHODIMP 
CXRefreshHelperbar::ContextSensitiveHelp(BOOL fEnterMode)
{
	DT(TRACE_I(FS(_T("Helperbar[%08X]: ContextSensitiveHelp(%s)"), this, fEnterMode?_T("TRUE"):_T("FALSE"))));
	return E_NOTIMPL;
}

STDMETHODIMP 
CXRefreshHelperbar::ShowDW(BOOL fShow)
{
	if (m_MainWindow)
	{
		m_MainWindow.ShowWindow(fShow ? SW_SHOW : SW_HIDE);
	}
	return S_OK;
}

STDMETHODIMP 
CXRefreshHelperbar::CloseDW(DWORD dwReserved)
{
	ShowDW(FALSE);
	// CloseDW purposely does not destroy the window. We leave that to the destructor.
	return S_OK;
}

STDMETHODIMP 
CXRefreshHelperbar::ResizeBorderDW(LPCRECT prcBorder, IUnknown *punkHelperbarSite, BOOL fReserved)
{
	return E_NOTIMPL;
}

STDMETHODIMP 
CXRefreshHelperbar::UIActivateIO(BOOL fActivate, LPMSG lpMsg)
{
	if( fActivate) SetFocus();

	return S_OK;
}

STDMETHODIMP 
CXRefreshHelperbar::HasFocusIO()
{
	// TODO: properly indicate when my UI has focus
	return S_FALSE;
}

STDMETHODIMP 
CXRefreshHelperbar::TranslateAcceleratorIO(LPMSG lpMsg)
{
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
CXRefreshHelperbar::SetSite(IUnknown *pUnknownSite)
{
	DT(TRACE_I(FS(_T("Helperbar[%08X]: SetSite(%08X)"), this, pUnknownSite)));
	
	if (!!pUnknownSite)
	{
		// get a WebBrowser reference
		CComPtr<IUnknown> site(pUnknownSite);
		CComQIPtr<IServiceProvider> serviceProvider(site);
		serviceProvider->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void**)&m_Browser);
		site->QueryInterface(IID_IInputObjectSite, (void**)&m_Site);

		// retrive browser id
		{
			BrowserManagerLock browserManager;
			m_BrowserId = browserManager->AllocBrowserId(m_Browser, this);
			ATLASSERT(m_BrowserId!=NULL_BROWSER);
		}

		// attach the window
		HWND hHelperbarWindow;
		CComQIPtr<IOleWindow> window(site);
		window->GetWindow(&hHelperbarWindow);
		if (!hHelperbarWindow) 
		{
			TRACE_E(FS(_T("Helperbar[%08X]: Cannot retrieve helpbar base window"), this));
			return E_FAIL;
		}
		SubclassWindow(hHelperbarWindow);

		// create main window
		CreateMainWindow();
	}
	else
	{
		BrowserManagerLock browserManager;
		CBrowserMessageWindow* bw = browserManager->FindBrowserMessageWindow(m_BrowserId);
		ATLASSERT(bw);
		bw->SetHelperbar(NULL);
		browserManager->ReleaseBrowserId(m_BrowserId);
		m_BrowserId = NULL_BROWSER;
	}

	return S_OK;
}

STDMETHODIMP 
CXRefreshHelperbar::GetSite(const IID &riid, void **ppvSite)
{
	if (!ppvSite) return E_INVALIDARG;
	*ppvSite = NULL;
	return m_Site->QueryInterface(riid, ppvSite);
}

STDMETHODIMP 
CXRefreshHelperbar::GetBandInfo(DWORD dwBandId, DWORD dwViewMode, DESKBANDINFO *pdbi)
{
	DT(TRACE_I(FS(_T("Helperbar[%08X]: GetBandInfo(...)"), this)));
	if (!pdbi) return E_INVALIDARG;
	m_BandId = dwBandId;
	m_ViewMode = dwViewMode;

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
		pdbi->dwModeFlags = DBIMF_NORMAL | DBIMF_VARIABLEHEIGHT | DBIMF_DEBOSSED;
	}

	if( pdbi->dwMask & DBIM_BKCOLOR )
	{
		// use the default background color by removing this flag.
		pdbi->dwMask &= ~DBIM_BKCOLOR;
	}
	return S_OK;
}

void 
CXRefreshHelperbar::CreateMainWindow()
{
	// only create the window if it doesn't exist yet.
	if (m_MainWindow) return;

	HWND parent = GetParent();
	if (!parent) throw CXRefreshRuntimeError(LoadStringResource(IDS_ERROR_NOPARENTWINDOW));

	ATLASSERT(m_BrowserId!=NULL_BROWSER);
	m_MainWindow.SetBrowserId(m_BrowserId);
	m_MainWindow.SetDisplayMode(CHelperbarWindow::EDM_PINNED);
	m_MainWindow.Create(parent, rcDefault, HELPERBAR_WINDOW_NAME);
}

CString 
CXRefreshHelperbar::GetTitle()
{
	DT(TRACE_I(FS(_T("Helperbar[%08X]: GetTitle()"), this)));
	return LoadStringResource(IDS_HELPERBAR_CAPTION);
}

CComPtr<IWebBrowser2> CXRefreshHelperbar::GetBrowser()
{
	return m_Browser;
}

POINTL CXRefreshHelperbar::GetMinSize() const
{
	POINTL pt = { -1, 100 };
	return pt;
}

POINTL CXRefreshHelperbar::GetMaxSize() const
{
	POINTL pt = { -1, -1 };
	return pt;
}

POINTL CXRefreshHelperbar::GetActualSize() const
{
	POINTL pt = { -1, -1 };
	return pt;
}

void
CXRefreshHelperbar::Log(CString message, int icon)
{
	m_MainWindow.Log(message, icon);
}
