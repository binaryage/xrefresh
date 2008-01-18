// XRefreshBHO.h : Declaration of the CXRefreshBHO

#pragma once
#include "resource.h"

#include "ConnectionManager.h"
#include "Logger.h"

class CXRefreshBHO;

//////////////////////////////////////////////////////////////////////////
// CToolWindow
class CToolWindow: public CWindowImpl<CToolWindow, CToolBarCtrl> {
public:
	CToolWindow(): m_Parent(NULL), m_IconBase(-1) { }
	virtual ~CToolWindow() {}

	BEGIN_MSG_MAP(CEditEnterAsTabT<T>)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

	LRESULT                                       OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void                                          SetParent(CXRefreshBHO* parent) { m_Parent = parent; }
	bool                                          TryToIntegrate(HWND hWnd);
	void                                          DetachFromIE();
	void                                          UpdateIcon();

	CXRefreshBHO*                                 m_Parent;
	int                                           m_IconBase;
};

//////////////////////////////////////////////////////////////////////////
// CXRefreshBHO
class ATL_NO_VTABLE CXRefreshBHO :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CXRefreshBHO, &CLSID_XRefreshBHO>,
	public IObjectWithSiteImpl<CXRefreshBHO>,
	public IDispatchImpl<IXRefreshBHO, &IID_IXRefreshBHO, &LIBID_XRefreshLib, 1, 0>,
	public IDispEventImpl<1, CXRefreshBHO, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 1>,
	public WinTrace
{
public:
	CXRefreshBHO();
	virtual ~CXRefreshBHO();

	DECLARE_CLASS_SIGNATURE(CXRefreshBHO)
	DECLARE_REGISTRY_RESOURCEID(IDR_XREFRESHBHO)
	DECLARE_NOT_AGGREGATABLE(CXRefreshBHO)

	BEGIN_COM_MAP(CXRefreshBHO)
		COM_INTERFACE_ENTRY(IXRefreshBHO)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IObjectWithSite)
	END_COM_MAP()

	BEGIN_SINK_MAP(CXRefreshBHO)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete2)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_SETPHISHINGFILTERSTATUS, OnDocumentReload)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, OnBeforeNavigate2)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_ONQUIT, OnQuit)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOWNLOADBEGIN, OnDownloadBegin)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOWNLOADCOMPLETE, OnDownloadComplete)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_WINDOWSTATECHANGED, OnWindowStateChanged)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_TITLECHANGE, OnWindowTitleChanged)
	END_SINK_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	// IObjectWithSite 
	STDMETHOD(SetSite)(IUnknown *pUnkSite);

	// DWebBrowserEvents2
	STDMETHOD(OnDocumentComplete)(IDispatch* pDisp, VARIANT* URL);
	STDMETHOD(OnNavigateComplete2)(IDispatch* pDisp, VARIANT* URL);
	STDMETHOD(OnDocumentReload)(IDispatch* pDisp);
	STDMETHOD(OnQuit)(void);
	STDMETHOD(OnProgressChange)(long Progress, long ProgressMax);
	STDMETHOD(OnCommandStateChange)(long Command, VARIANT_BOOL Enable);
	STDMETHOD(OnBeforeNavigate2)(IDispatch* pDisp, VARIANT*& url, VARIANT*& Flags, VARIANT*& TargetFrameName, VARIANT*& PostData, VARIANT*& Headers, VARIANT_BOOL*& Cancel);
	STDMETHOD(OnPropertyChange)(BSTR szProperty);
	STDMETHOD(OnDownloadBegin)();
	STDMETHOD(OnDownloadComplete)();
	STDMETHOD(OnWindowStateChanged)(DWORD dwFlags, DWORD dwValidFlagsMask);
	STDMETHOD(OnWindowTitleChanged)(BSTR bstrTitleText);

	// helpers
	CComPtr<IWebBrowser2>                         GetTopBrowser() const { return m_TopBrowser; }

	void                                          Log(CString message, int icon);

	void                                          PerformRefresh();
	void                                          ListenForReconnect();
	void                                          Connect();
	void                                          Disconnect();
	bool                                          IsConnected() { return m_ConnectionManager.IsConnected(); }
	CLogger*                                      GetLogger() { return &m_Logger; }
	TBrowserId                                    GetBrowserId() { return m_BrowserId; }
	void                                          SendInfoAboutPage();
	void                                          PauseXRefresh();
	void                                          UnpauseXRefresh();
	void                                          UpdateIcon();
	void                                          DisconnectedNotify();

private:
	// Helpers
	bool                                          SetupEnvironment();
	bool                                          OnRefreshStart();
	bool                                          OnRefreshEnd();

	// 
	HRESULT                                       ProcessDocument(IDispatch *pDisp, VARIANT *pvarURL);

	TBrowserId                                    m_BrowserId; ///< the id of this BHO 
	CComPtr<IWebBrowser2>                         m_TopBrowser; ///< top level browser window in BHO's tab

	bool                                          m_IsAdvised;
	bool                                          m_DownloadInProgress;

	CConnectionManager                            m_ConnectionManager; ///< XRefresh server connection
	CLogger                                       m_Logger;
	bool                                          m_Paused;
	static CToolWindow                            m_IE7ToolWindow;

	CString                                       m_LastSentTitle;
	CString                                       m_LastSentURL;
};

OBJECT_ENTRY_AUTO(__uuidof(XRefreshBHO), CXRefreshBHO)
