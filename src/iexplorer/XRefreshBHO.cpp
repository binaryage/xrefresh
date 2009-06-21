// XRefreshBHO.cpp : Implementation of CXRefreshBHO
#include "stdafx.h"
#include "XRefreshBHO.h"
#include "BrowserManager.h"
#include "ConnectionManager.h"
#include "Module.h"
#include "XRefreshHelperbar.h"

//#include "Debug.h"

CToolWindow CXRefreshBHO::m_IE7ToolWindow;

bool 
CToolWindow::TryToIntegrate(HWND hWnd)
{
	if (m_hWnd) return true; // this is singleton, integrate only first time

	CFindWnd finder(hWnd, TOOLBARCLASSNAME); // TODO: do it safer
	if (!finder.m_hWnd) return false;
	SubclassWindow(finder.m_hWnd);

	if (!::IsWindow(this->m_hWnd)) return true; // HACK
	CImageList il = GetImageList();
	m_IconBase = il.GetImageCount();
	il.AddIcon(LoadIcon(GetBaseModule().GetModuleInstance(), MAKEINTRESOURCE(IDI_XREFRESHA))); 
	il.AddIcon(LoadIcon(GetBaseModule().GetModuleInstance(), MAKEINTRESOURCE(IDI_XREFRESHZ))); 
	return true;
}

void 
CToolWindow::DetachFromIE()
{
	if (!m_hWnd) return;
	::DestroyWindow(UnsubclassWindow(TRUE)); // HACK: i shouldn't destroy IE's windows
}

void
CToolWindow::UpdateIcon()
{
	if (!m_Parent) return;
	if (!m_hWnd) return;
	CString bname = _T("XRefresh");
	int count = GetButtonCount();
	for (int i=0; i<count; i++)
	{
		TBBUTTON button;
		ZeroMemory(&button, sizeof(button));
		GetButton(i, &button);

		TBBUTTONINFO info;
		TCHAR buf[1000];
		ZeroMemory(&info, sizeof(info));
		info.cbSize = sizeof(info);
		info.dwMask = TBIF_TEXT|TBIF_IMAGE;
		info.cchText = 999;
		info.pszText = buf;
		GetButtonInfo(button.idCommand, &info);
		if (bname==buf)
		{
			if (m_Parent->IsConnected())
			{
				info.iImage = m_IconBase+0;
			}
			else
			{
				info.iImage = m_IconBase+1;
			}
			SetButtonInfo(button.idCommand, &info);
			break;
		}
	}
}

LRESULT 
CToolWindow::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	UpdateIcon();
	bHandled = FALSE;
	return S_OK;
}

CXRefreshBHO::CXRefreshBHO():
m_BrowserId(NULL_BROWSER),
m_Logger(NULL_BROWSER),
m_ConnectionManager(this),
m_Paused(false)
{
	InitRoot();
	DT(TRACE_I(FS(_T("BHO[%08X]: constructor"), this)));
}

CXRefreshBHO::~CXRefreshBHO()
{
	DT(TRACE_I(FS(_T("BHO[%08X]: destructor"), this)));
	DoneRoot();
}

// CXRefreshBHO
STDMETHODIMP 
CXRefreshBHO::SetSite(IUnknown* pUnkSite)
{
	WATCH_EXITS;
	DTI(TRACE_I(FS(_T("BHO[%08X]: SetSite(%08X)"), this, pUnkSite)));
	if (pUnkSite != NULL)
	{
		// cache the pointer to IWebBrowser2
		pUnkSite->QueryInterface(IID_IWebBrowser2, (void **)&m_Browser);
		// register to sink events from DWebBrowserEvents2
		DispEventAdvise(m_Browser);
		m_IsAdvised = true;

		// allocate browser id
		BrowserManagerLock browserManager;
		m_BrowserId = browserManager->AllocBrowserId(m_Browser, this);
		m_Logger.m_BrowserId = m_BrowserId;
		ATLASSERT(m_BrowserId!=NULL_BROWSER);

		HWND hwnd;
		HRESULT hr = m_Browser->get_HWND((LONG_PTR*)&hwnd);
		if (SUCCEEDED(hr))
		{
			m_IE7ToolWindow.TryToIntegrate(hwnd);
		}

		m_ConnectionManager.Connect();
	}
	else
	{
		m_ConnectionManager.StopReconnectListener();
		m_ConnectionManager.Disconnect();

		m_IE7ToolWindow.DetachFromIE();

		// unregister event sink.
		if (m_IsAdvised)
		{
			DispEventUnadvise(m_Browser);
			m_IsAdvised = false;
		}

		BrowserManagerLock browserManager;
		CBrowserMessageWindow* bw = browserManager->FindBrowserMessageWindow(m_BrowserId);
		ATLASSERT(bw);
		bw->SetBHO(NULL);
		browserManager->ReleaseBrowserId(m_BrowserId);
		m_BrowserId = NULL_BROWSER;
		m_Logger.m_BrowserId = NULL_BROWSER;

		// release cached pointers and other resources here.
		m_Browser = NULL;
	}

	SendInfoAboutPage();

	// Call base class implementation.
	return IObjectWithSiteImpl<CXRefreshBHO>::SetSite(pUnkSite);
}

void
CXRefreshBHO::ResetLastSentTitle()
{
	m_LastSentTitle = "";
	m_LastSentURL = "";
}

void
CXRefreshBHO::SendInfoAboutPage()
{
	// send message
	if (!m_Browser) return;
	if (!m_ConnectionManager.IsConnected()) return;
	
	CString title = GetTitle(m_Browser);
	CString url = GetURL(m_Browser);
	if (m_LastSentTitle==title && m_LastSentURL==url) return; // prevent duplicit page info messages

	// send message
	m_LastSentTitle = title;
	m_LastSentURL = url;
	m_ConnectionManager.SendSetPage(title, url);
}

STDMETHODIMP
CXRefreshBHO::OnWindowStateChanged(DWORD dwFlags, DWORD dwValidFlagsMask)
{
	DTI(TRACE_LI(FS(_T("BHO[%08X]: OnWindowStateChanged()"), this)));
	m_IE7ToolWindow.SetParent(this);
	return S_OK;
}

STDMETHODIMP
CXRefreshBHO::OnWindowTitleChanged(BSTR bstrTitleText)
{
	DTI(TRACE_LI(FS(_T("BHO[%08X]: OnWindowTitleChanged()"), this)));
	SendInfoAboutPage();
	return S_OK;
}

void
CXRefreshBHO::PerformRefresh()
{
	if (!m_Browser) return;
	CComVariant v(REFRESH_COMPLETELY);
	m_Browser->Refresh2(&v);
}

void
CXRefreshBHO::ListenForReconnect()
{
	m_ConnectionManager.StartReconnectListener();
}

void
CXRefreshBHO::Connect()
{
	if (!m_Browser) return;
	m_ConnectionManager.Connect();
}

void
CXRefreshBHO::Disconnect()
{
	if (!m_Browser) return;
	m_ConnectionManager.Disconnect();
}

void
CXRefreshBHO::DisconnectedNotify()
{
	Log(_T("Disconnected from XRefresh Monitor"), ICON_DISCONNECTED);
	UpdateIcon();
}

void
CXRefreshBHO::Log(LPCTSTR message, int icon)
{
	BrowserManagerLock browserManager;
	if (browserManager->IsBrowserThread(GetCurrentThreadId(), m_BrowserId))
	{
		m_Logger.Log(message, icon);
		return;
	}
	CBrowserMessageWindow* window = browserManager->FindBrowserMessageWindow(m_BrowserId);
	if (!window) return;
	window->PostMessage(BMM_REQUEST_LOG, icon, (LPARAM)FS(_T("%s"), message));
}

void
CXRefreshBHO::PauseXRefresh()
{
	Disconnect();
	m_Paused = true;
}

void
CXRefreshBHO::UnpauseXRefresh()
{
	Connect();
	m_Paused = false;
}

void
CXRefreshBHO::UpdateIcon()
{
	m_IE7ToolWindow.UpdateIcon();
}
