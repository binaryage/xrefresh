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

static int BHOCounter = 0;

CXRefreshBHO::CXRefreshBHO():
//m_RequestReload(false),
m_DownloadInProgress(false),
m_BrowserId(NULL_BROWSER),
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
	try {
		WATCH_EXITS;
		DTI(TRACE_I(FS(_T("BHO[%08X]: SetSite(%08X)"), this, pUnkSite)));
		if (pUnkSite != NULL)
		{
			// cache the pointer to IWebBrowser2
			CHECK(pUnkSite->QueryInterface(IID_IWebBrowser2, (void **)&m_TopBrowser));
			// register to sink events from DWebBrowserEvents2
			CHECK(DispEventAdvise(m_TopBrowser));
			m_IsAdvised = true;

			// allocate browser id
			BrowserManagerLock browserManager;
			m_BrowserId = browserManager->AllocBrowserId(m_TopBrowser, this);
			ATLASSERT(m_BrowserId!=NULL_BROWSER);

			HWND hwnd;
			HRESULT hr = m_TopBrowser->get_HWND((LONG_PTR*)&hwnd);
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
			//ATLASSERT(m_IE7ToolWindow.m_hWnd==NULL);

			// unregister event sink.
			if (m_IsAdvised)
			{
				DispEventUnadvise(m_TopBrowser);
				m_IsAdvised = false;
			}

			//if (!!m_HookedDoc) 
			//{
			//	m_HookedDoc->SetUIHandler(NULL);
			//	m_HookedDoc = NULL;
			//}

			BrowserManagerLock browserManager;
			browserManager->ReleaseBrowserId(m_BrowserId);
			m_BrowserId = NULL_BROWSER;

			// release cached pointers and other resources here.
			m_TopBrowser = NULL;
		}

		SendInfoAboutPage();
	}
	catch (...)
	{
	}

	// Call base class implementation.
	return IObjectWithSiteImpl<CXRefreshBHO>::SetSite(pUnkSite);
}

STDMETHODIMP 
CXRefreshBHO::OnDocumentReload(IDispatch *pDisp)
{
	return S_OK;
}

STDMETHODIMP
CXRefreshBHO::OnProgressChange(long Progress, long ProgressMax)
{
	WATCH_EXITS;
	DTI(TRACE_I(FS(_T("BHO[%08X]: OnProgressChange(Progress=%d, ProgressMax=%d)"), this, Progress, ProgressMax)));
	//if (Progress==0 && ProgressMax==0)
	//{
	//	if (m_RequestReload)
	//	{
	//		OnRefreshEnd();
	//	}
	//}
	return S_OK;
}

STDMETHODIMP 
CXRefreshBHO::OnCommandStateChange(long Command, VARIANT_BOOL Enable)
{
	WATCH_EXITS;
	DTI(TRACE_I(FS(_T("BHO[%08X]: OnCommandStateChange(Command=%d, Enable=%s)"), this, Command, Enable?_T("true"):_T("false"))));
	return S_OK;
}

bool
CXRefreshBHO::SetupEnvironment()
{
	//if (!!m_HookedDoc) 
	//{
	//	m_HookedDoc->SetUIHandler(NULL);
	//	m_HookedDoc = NULL;
	//}

	//HRESULT hr;

	// get the current document object from browser...
	//CComPtr<IDispatch> spDispDoc;
	//CHECK(m_TopBrowser->get_Document(&spDispDoc));

	// setup infrastructure
	// get pointers to default interfaces
	//CComQIPtr<IOleObject> spOleObject(spDispDoc);
	//if (spOleObject)
	//{
	//	CComPtr<IOleClientSite> spClientSite;
	//	hr = spOleObject->GetClientSite(&spClientSite);
	//	if (SUCCEEDED(hr) && spClientSite)
	//	{
	//		m_DefaultDocHostUIHandler = spClientSite;
	//		m_DefaultOleCommandTarget = spClientSite;
	//	}
	//}

	// set this class to be the IDocHostUIHandler
	//CComQIPtr<ICustomDoc> spCustomDoc(spDispDoc);
	//if (spCustomDoc)
	//{
	//	spCustomDoc->SetUIHandler(this);
	//	m_HookedDoc = spCustomDoc;
	//}

	// TODO: testing
	OnRefreshEnd();

	SendInfoAboutPage();
	return true;
}

void
CXRefreshBHO::SendInfoAboutPage()
{
	// send message
	if (!m_TopBrowser) return;
	CString title = GetLocationName(m_TopBrowser);
	CString url = GetLocationURL(m_TopBrowser);
	m_ConnectionManager.SendSetPage(title, url);
}

bool 
CXRefreshBHO::OnRefreshStart()
{
	return true;
}

bool 
CXRefreshBHO::OnRefreshEnd()
{
	//m_RequestReload = false;
	return true;
}

STDMETHODIMP
CXRefreshBHO::OnPropertyChange(BSTR szProperty)
{
	WATCH_EXITS;
	return S_OK;
}

STDMETHODIMP
CXRefreshBHO::OnBeforeNavigate2(IDispatch *pDisp, VARIANT *&url, VARIANT *&Flags, VARIANT *&TargetFrameName, VARIANT *&PostData, VARIANT *&Headers, VARIANT_BOOL *&Cancel)
{
	WATCH_EXITS;
	CString sUrl = VariantToString((VARIANT*)&url);
	DTI(TRACE_LI(FS(_T("BHO[%08X]: OnBeforeNavigate2(pDisp=%08X, url='%s', ...)"), this, pDisp, sUrl)));
	return S_OK;
}

HRESULT
CXRefreshBHO::ProcessDocument(IDispatch *pDisp, VARIANT *pvarURL)
{
	CString sUrl = VariantToString(pvarURL);
	DTI(TRACE_LI(FS(_T("BHO[%08X]: ProcessDocument(%08p, '%s')"), this, pDisp, sUrl)));

	// test for top-level window and do some environment setup
	// retrieve the top-level window from the site.
	HWND hwnd;
	HRESULT hr = m_TopBrowser->get_HWND((LONG_PTR*)&hwnd);
	if (SUCCEEDED(hr))
	{
		HRESULT hr = S_OK;

		// query for the IWebBrowser2 interface.
		CComQIPtr<IWebBrowser2> spTempWebBrowser = pDisp;

		// is this event associated with the top-level browser?
		if (spTempWebBrowser && m_TopBrowser &&
			m_TopBrowser.IsEqualObject(spTempWebBrowser))
		{
			DT(TRACE_I(_T("This is top level browser => setup environment")));
			SetupEnvironment();
		}
	}
	return S_OK;
}

STDMETHODIMP 
CXRefreshBHO::OnDocumentComplete(IDispatch *pDisp, VARIANT *pvarURL)
{
	WATCH_EXITS;
	CString sUrl = VariantToString(pvarURL);
	DTI(TRACE_LI(FS(_T("BHO[%08X]: OnDocumentComplete(%08p, '%s')"), this, pDisp, sUrl)));

	if (m_DownloadInProgress)
	{
		// hack for IE7 - it gives false OnDocumentComplete events during download
		DTI(TRACE_LI(FS(_T("BHO[%08X]: ignoring ... download in progress"), this)));
		return S_OK;
	}

	ProcessDocument(pDisp, pvarURL);
	return S_OK;
}

STDMETHODIMP 
CXRefreshBHO::OnNavigateComplete2(IDispatch *pDisp, VARIANT *pvarURL)
{
	WATCH_EXITS;
	CString sUrl = VariantToString(pvarURL);
	DTI(TRACE_LI(FS(_T("BHO[%08X]: OnNavigateComplete2(%08p, '%s')"), this, pDisp, sUrl)));
	return S_OK;
}

STDMETHODIMP 
CXRefreshBHO::OnQuit()
{
	WATCH_EXITS;
	DTI(TRACE_I(FS(_T("BHO[%08X]: OnQuit()"), this)));
	return S_OK;
}

STDMETHODIMP
CXRefreshBHO::OnDownloadBegin()
{
	DTI(TRACE_LI(FS(_T("BHO[%08X]: OnDownloadBegin()"), this)));
	ATLASSERT(!m_DownloadInProgress);
	m_DownloadInProgress = true;
	return S_OK;
}

STDMETHODIMP
CXRefreshBHO::OnDownloadComplete()
{
	DTI(TRACE_LI(FS(_T("BHO[%08X]: OnDownloadComplete()"), this)));
	ATLASSERT(m_DownloadInProgress);
	m_DownloadInProgress = false;
	return S_OK;
}

STDMETHODIMP
CXRefreshBHO::OnWindowStateChanged(DWORD dwFlags, DWORD dwValidFlagsMask)
{
	DTI(TRACE_LI(FS(_T("BHO[%08X]: OnWindowStateChanged()"), this)));
	m_IE7ToolWindow.SetParent(this);
	return S_OK;
}
/*
// IDocHostUIHandler
HRESULT 
CXRefreshBHO::ShowContextMenu(DWORD dwID,
										POINT *ppt,
										IUnknown *pcmdTarget,
										IDispatch *pdispObject) 
{
	return S_FALSE; 
}

STDMETHODIMP
CXRefreshBHO::ShowUI(DWORD dwID, 
						 IOleInPlaceActiveObject FAR* pActiveObject,
						 IOleCommandTarget FAR* pCommandTarget,
						 IOleInPlaceFrame  FAR* pFrame,
						 IOleInPlaceUIWindow FAR* pDoc)
{
	DTI(TRACE_LI(FS(_T("BHO[%08X]: ShowUI"), this)));
	if (m_DefaultDocHostUIHandler)	return m_DefaultDocHostUIHandler->ShowUI(dwID, pActiveObject, pCommandTarget, pFrame, pDoc);
	return S_FALSE;
}

STDMETHODIMP
CXRefreshBHO::GetHostInfo(DOCHOSTUIINFO FAR *pInfo)
{
	DTI(TRACE_LI(FS(_T("BHO[%08X]: GetHostInfo"), this)));
	if (m_DefaultDocHostUIHandler)	return m_DefaultDocHostUIHandler->GetHostInfo(pInfo);
	return S_OK;
}

STDMETHODIMP
CXRefreshBHO::HideUI()
{
	DTI(TRACE_LI(FS(_T("BHO[%08X]: HideUI"), this)));
	if (m_DefaultDocHostUIHandler)	return m_DefaultDocHostUIHandler->HideUI();
	return S_OK;
}

STDMETHODIMP
CXRefreshBHO::UpdateUI()
{
	//DTI(TRACE_LI(FS(_T("BHO[%08X]: UpdateUI"), this)));
	if (m_DefaultDocHostUIHandler)	return m_DefaultDocHostUIHandler->UpdateUI();
	return S_OK;
}

STDMETHODIMP
CXRefreshBHO::EnableModeless(BOOL fEnable)
{
	DTI(TRACE_LI(FS(_T("BHO[%08X]: EnableModeless"), this)));
	if (m_DefaultDocHostUIHandler)	return m_DefaultDocHostUIHandler->EnableModeless(fEnable);
	return S_OK;
}

STDMETHODIMP
CXRefreshBHO::OnDocWindowActivate(BOOL fActivate)
{
	DTI(TRACE_LI(FS(_T("BHO[%08X]: OnDocWindowActivate"), this)));
	if (m_DefaultDocHostUIHandler)	return m_DefaultDocHostUIHandler->OnDocWindowActivate(fActivate);
	return S_OK;
}

STDMETHODIMP
CXRefreshBHO::OnFrameWindowActivate(BOOL fActivate)
{
	DTI(TRACE_LI(FS(_T("BHO[%08X]: OnFrameWindowActivate"), this)));
	if (m_DefaultDocHostUIHandler)	return m_DefaultDocHostUIHandler->OnFrameWindowActivate(fActivate);
	return S_OK;
}

STDMETHODIMP
CXRefreshBHO::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow FAR* pUIWindow, BOOL fFrameWindow)
{
	if (m_DefaultDocHostUIHandler)	return m_DefaultDocHostUIHandler->ResizeBorder(prcBorder, pUIWindow, fFrameWindow);
	return S_OK;
}

STDMETHODIMP
CXRefreshBHO::TranslateAccelerator(LPMSG lpMsg, const GUID FAR* pguidCmdGroup, DWORD nCmdID)
{
	if (m_DefaultDocHostUIHandler)	return m_DefaultDocHostUIHandler->TranslateAccelerator(lpMsg, pguidCmdGroup, nCmdID);
	return E_NOTIMPL;
}

STDMETHODIMP
CXRefreshBHO::GetOptionKeyPath(LPOLESTR FAR* pchKey, DWORD dw)
{
	if (m_DefaultDocHostUIHandler)	return m_DefaultDocHostUIHandler->GetOptionKeyPath(pchKey, dw);
	return E_FAIL;
}

STDMETHODIMP
CXRefreshBHO::GetDropTarget(IDropTarget* pDropTarget, IDropTarget** ppDropTarget)
{
	if (m_DefaultDocHostUIHandler)	return m_DefaultDocHostUIHandler->GetDropTarget(pDropTarget, ppDropTarget);
	return S_OK;
}

STDMETHODIMP
CXRefreshBHO::GetExternal(IDispatch** ppDispatch)
{
	if (m_DefaultDocHostUIHandler)	return m_DefaultDocHostUIHandler->GetExternal(ppDispatch);
	return S_FALSE;
}

STDMETHODIMP
CXRefreshBHO::TranslateUrl(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut)
{
	if (m_DefaultDocHostUIHandler)	return m_DefaultDocHostUIHandler->TranslateUrl(dwTranslate, pchURLIn, ppchURLOut);
	return S_FALSE;
}

STDMETHODIMP
CXRefreshBHO::FilterDataObject(IDataObject* pDO, IDataObject** ppDORet)
{
	if (m_DefaultDocHostUIHandler)	return m_DefaultDocHostUIHandler->FilterDataObject(pDO, ppDORet);
	return S_FALSE;
}
*/
//
// IOleCommandTarget
//
//STDMETHODIMP
//CXRefreshBHO::QueryStatus(/*[in]*/ const GUID *pguidCmdGroup, 
//								/*[in]*/ ULONG cCmds,
//								/*[in,out][size_is(cCmds)]*/ OLECMD *prgCmds,
//								/*[in,out]*/ OLECMDTEXT *pCmdText)
//{
//	DTI(TRACE_LI(FS(_T("BHO[%08X]: QueryStatus"), this)));
//	return m_DefaultOleCommandTarget->QueryStatus(pguidCmdGroup, cCmds, prgCmds, pCmdText);
//}
//
//STDMETHODIMP
//CXRefreshBHO::Exec(/*[in]*/ const GUID *pguidCmdGroup,
//					  /*[in]*/ DWORD nCmdID,
//					  /*[in]*/ DWORD nCmdExecOpt,
//					  /*[in]*/ VARIANTARG *pvaIn,
//					  /*[in,out]*/ VARIANTARG *pvaOut)
//{
//	DTI(TRACE_LI(FS(_T("BHO[%08X]: Exec"), this)));
//	// see http://www.codeproject.com/internet/detecting_the_ie_refresh.asp
//	if(nCmdID==2300||nCmdID==6041||nCmdID==6042)
//	{
//		m_RequestReload = true;
//		OnRefreshStart();
//	}
//	if (nCmdID == OLECMDID_SHOWSCRIPTERROR)
//	{
//		// Don't show the error dialog, but
//		// continue running scripts on the page.
//		(*pvaOut).vt = VT_BOOL;
//		(*pvaOut).boolVal = VARIANT_TRUE;
//		return S_OK;
//	}
//	return m_DefaultOleCommandTarget->Exec(pguidCmdGroup, nCmdID, nCmdExecOpt, pvaIn, pvaOut);
//}

void
CXRefreshBHO::PerformRefresh()
{
	if (!m_TopBrowser) return;
	CComVariant v(REFRESH_COMPLETELY);
	m_TopBrowser->Refresh2(&v);
}

void
CXRefreshBHO::ListenForReconnect()
{
	m_ConnectionManager.StartReconnectListener();
}

void
CXRefreshBHO::Connect()
{
	if (!m_TopBrowser) return;
	m_ConnectionManager.Connect();
}

void
CXRefreshBHO::Disconnect()
{
	if (!m_TopBrowser) return;
	m_ConnectionManager.Disconnect();
}

void
CXRefreshBHO::DisconnectedNotify()
{
	Log(_T("Disconnected from XRefresh Monitor"), ICON_DISCONNECTED);
	UpdateIcon();
}

void
CXRefreshBHO::Log(CString message, int icon)
{
	m_Logger.Log(message, icon);
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

