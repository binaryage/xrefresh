#include "StdAfx.h"
#include "BrowserManager.h"
#include "XRefreshBHO.h"
#include "XRefreshHelperbar.h"
#include "XRefreshToolbar.h"

//#include "Debug.h"

IUnknown*
CBrowserMessageWindow::GetBrowserInterface() const
{
	ATLASSERT(!!m_BHO || !!m_Helperbar || !!m_Toolbar);
	if (!!m_BHO) return m_BHO->GetBrowser(); 
	if (!!m_Helperbar) return m_Helperbar->GetBrowser(); 
	if (!!m_Toolbar) return m_Toolbar->GetBrowser(); 
	return NULL;
}

LRESULT
CBrowserMessageWindow::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!m_BHO) return S_OK;
	switch (uMsg) {
		case BMM_REQUEST_REFRESH: m_BHO->PerformRefresh(); break;
		case BMM_REQUEST_LISTEN_FOR_RECONNECT: m_BHO->ListenForReconnect(); break;
		case BMM_REQUEST_INFO_ABOUT_PAGE: m_BHO->SendInfoAboutPage(); break;
		case BMM_REQUEST_PAUSE: m_BHO->PauseXRefresh(); break;
		case BMM_REQUEST_UNPAUSE: m_BHO->UnpauseXRefresh(); break;
		case BMM_REQUEST_UPDATE_ICON: m_BHO->UpdateIcon(); break;
		case BMM_REQUEST_DISCONNECTED_NOTIFY: m_BHO->DisconnectedNotify(); break;
		case BMM_REQUEST_LOG: m_BHO->Log((LPCTSTR)lParam, wParam); break;
		case BMM_REQUEST_RESET_LAST_SENT_TITLE: m_BHO->ResetLastSentTitle(); break;
	}
	return S_OK;
}

CBrowserMessageWindow::CBrowserMessageWindow(IUnknown* browserInterface, CXRefreshBHO* pBHO, CXRefreshHelperbar* pHelperbar, CXRefreshToolbar* pToolbar):
	m_RefCount(1),
	m_BHO(NULL),
	m_Helperbar(NULL),
	m_Toolbar(NULL),
	m_ThreadId(0)
{
	CreateMessageWindow();
	SetBHO(pBHO);
	SetHelperbar(pHelperbar);
	SetToolbar(pToolbar);
}

CBrowserMessageWindow::~CBrowserMessageWindow()
{
	DestroyMessageWindow();
}

bool
CBrowserMessageWindow::CreateMessageWindow()
{
	ATLASSERT(!m_hWnd);
	CRect rcDefault(0,0,10,10);
	Create(NULL, rcDefault, BROWSER_MESSAGE_WINDOW_NAME);
	ATLASSERT(m_hWnd);
	m_ThreadId = GetCurrentThreadId();
	return true;
}

bool
CBrowserMessageWindow::DestroyMessageWindow()
{
	ATLASSERT(m_hWnd);
	::DestroyWindow(UnsubclassWindow(TRUE));
	ATLASSERT(!m_hWnd);
	m_ThreadId = 0;
	return true;
}

void 
CBrowserMessageWindow::SetBHO(CXRefreshBHO* pBHO)
{
	m_BHO = pBHO;
	if (!m_BHO) return;

	// re-parent message window
	//HWND hwnd;
	//CComPtr<IWebBrowser2> browser = m_BHO->GetBrowser();
	//ATLASSERT(!!browser);
	//CHECK(browser->get_HWND((LONG_PTR*)&hwnd));
	//SetParent(hwnd);
}
//////////////////////////////////////////////////////////////////////////

CBrowserManager::CBrowserManager():
	m_NextId(0)
{
	DT(CREATE_DEBUG_TRACE("BrowserManager"));
}

CBrowserManager::~CBrowserManager()
{
	DT(DELETE_DEBUG_TRACE());
}

TBrowserId
CBrowserManager::AllocBrowserId(IUnknown* browserInterface, CXRefreshBHO* pBHO)
{
	return AllocBrowserId(browserInterface, pBHO, NULL, NULL);
}

TBrowserId
CBrowserManager::AllocBrowserId(IUnknown* browserInterface, CXRefreshHelperbar* pHelperbar)
{
	return AllocBrowserId(browserInterface, NULL, pHelperbar, NULL);
}

TBrowserId
CBrowserManager::AllocBrowserId(IUnknown* browserInterface, CXRefreshToolbar* pToolbar)
{
	return AllocBrowserId(browserInterface, NULL, NULL, pToolbar);
}

TBrowserId
CBrowserManager::AllocBrowserId(IUnknown* browserInterface, CXRefreshBHO* pBHO, CXRefreshHelperbar* pHelperbar, CXRefreshToolbar* pToolbar)
{
	DT(TRACE_LI(FS(_T("AllocBrowserId(browserInterface=%08X, BHO=%08X, Helperbar=%08X, Toolbar=%08X)"), browserInterface, pBHO, pHelperbar, pToolbar)));
	CHECK_THREAD_OWNERSHIP;
	TBrowserMessageWindowMap::iterator l = FindBrowserId(browserInterface);
	if (l!=m_Browsers.end()) 
	{
		// increase refcount
		l->second->AddRef();

		// fill missing data
		if (pBHO) l->second->SetBHO(pBHO);
		if (pHelperbar) l->second->SetHelperbar(pHelperbar);
		if (pToolbar) l->second->SetToolbar(pToolbar);

		DT(TRACE_LI(FS(_T("... addref to %d and return %d"), l->second->RefCount(), l->first)));
		return l->first;
	}

	// allocate a new browser browserId
	CHECK_THREAD_OWNERSHIP;
	++m_NextId;
	m_Browsers.insert(make_pair(m_NextId, new CBrowserMessageWindow(browserInterface, pBHO, pHelperbar, pToolbar)));
	DT(TRACE_LI(FS(_T("... created %d"), m_NextId)));
	return m_NextId;	
}

bool
CBrowserManager::ReleaseBrowserId(TBrowserId browserId)
{
	DT(TRACE_LI(FS(_T("ReleaseBrowserId(browserId=%d)"), browserId)));
	CHECK_THREAD_OWNERSHIP;
	TBrowserMessageWindowMap::iterator l = m_Browsers.find(browserId);
	ATLASSERT(l!=m_Browsers.end());
	ATLASSERT(l->second->RefCount()>0);
	DT(TRACE_LI(FS(_T("... refcount %d"), l->second->RefCount())));
	if (l->second->DecRef()==0) 
	{
		delete l->second;
		m_Browsers.erase(l);
		DT(TRACE_LI(FS(_T("....... deleted"))));
	}
	return true;
}

TBrowserMessageWindowMap::iterator
CBrowserManager::FindBrowserId(IUnknown* browserInterface)
{
	DT(TRACE_LI(FS(_T("FindBrowserId(browserId=%08X)"), browserInterface)));
	CHECK_THREAD_OWNERSHIP;
	TBrowserMessageWindowMap::iterator i = m_Browsers.begin();
	while (i!=m_Browsers.end())
	{
		if (i->second->GetBrowserInterface()==browserInterface) break;
		++i;
	}
	return i;
}

CBrowserMessageWindow*
CBrowserManager::FindBrowserMessageWindow(TBrowserId browserId)
{
	CHECK_THREAD_OWNERSHIP;
	TBrowserMessageWindowMap::iterator i = m_Browsers.find(browserId);
	if (i==m_Browsers.end()) return NULL;
	return i->second;
}

bool 
CBrowserManager::IsBrowserThread(DWORD threadId, TBrowserId browserId)
{
	CHECK_THREAD_OWNERSHIP;
	CBrowserMessageWindow* pBrowserMessageWindow = FindBrowserMessageWindow(browserId);
	ATLASSERT(pBrowserMessageWindow);
	if (!pBrowserMessageWindow) return false;
	return pBrowserMessageWindow->GetThreadId()==threadId;
}
