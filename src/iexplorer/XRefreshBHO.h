// XRefreshBHO.h : Declaration of the CXRefreshBHO

#pragma once
#include "resource.h"       // main symbols

#include "ConnectionManager.h"
#include "Logger.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class CXRefreshBHO;

class CToolWindow : public CWindowImpl<CToolWindow, CToolBarCtrl> {
public:
	virtual ~CToolWindow() {}

	BEGIN_MSG_MAP(CEditEnterAsTabT< T >)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CToolWindow() : m_Parent(NULL) { }
	void SetParent(CXRefreshBHO* parent) { m_Parent = parent; }
	bool TryToIntegrate(HWND hWnd);
	void DetachFromIE();
	void UpdateIcon();

	CXRefreshBHO* m_Parent;
	int m_IconBase;
};

// CXRefreshBHO
class ATL_NO_VTABLE CXRefreshBHO :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CXRefreshBHO, &CLSID_XRefreshBHO>,
	public IObjectWithSiteImpl<CXRefreshBHO>,
	public IDispatchImpl<IXRefreshBHO, &IID_IXRefreshBHO, &LIBID_XRefreshLib, 1, 0>,
	public IDispEventImpl<1, CXRefreshBHO, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 1>,
//	public IDocHostUIHandler,
//	public IOleCommandTarget,
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
		//COM_INTERFACE_ENTRY(IDocHostUIHandler)
		//COM_INTERFACE_ENTRY(IOleCommandTarget)
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
		
		//SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_COMMANDSTATECHANGE, OnCommandStateChange)
		//SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_PROGRESSCHANGE, OnProgressChange) // needed for processing F5 (refresh)
		//SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_PROPERTYCHANGE, OnPropertyChange)
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

	//// IDocHostUIHandler
	//STDMETHOD(ShowContextMenu)(DWORD dwID, POINT FAR* ppt, IUnknown FAR* pcmdTarget, IDispatch FAR* pdispReserved);
	//STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject FAR* pActiveObject, IOleCommandTarget FAR* pCommandTarget, IOleInPlaceFrame  FAR* pFrame, IOleInPlaceUIWindow FAR* pDoc);
	//STDMETHOD(GetHostInfo)(DOCHOSTUIINFO FAR *pInfo);
	//STDMETHOD(HideUI)();
	//STDMETHOD(UpdateUI)();
	//STDMETHOD(EnableModeless)(BOOL fEnable);
	//STDMETHOD(OnDocWindowActivate)(BOOL fActivate);
	//STDMETHOD(OnFrameWindowActivate)(BOOL fActivate);
	//STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow FAR* pUIWindow, BOOL fFrameWindow);
	//STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID FAR* pguidCmdGroup, DWORD nCmdID);
	//STDMETHOD(GetOptionKeyPath)(LPOLESTR FAR* pchKey, DWORD dw);
	//STDMETHOD(GetDropTarget)(IDropTarget* pDropTarget, IDropTarget** ppDropTarget);
	//STDMETHOD(GetExternal)(IDispatch** ppDispatch);
	//STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut);
	//STDMETHOD(FilterDataObject)(IDataObject* pDO, IDataObject** ppDORet);

	//// IOleCommandTarget
	//STDMETHOD(QueryStatus)(/*[in]*/ const GUID *pguidCmdGroup, /*[in]*/ ULONG cCmds,	/*[in,out][size_is(cCmds)]*/ OLECMD *prgCmds, /*[in,out]*/ OLECMDTEXT *pCmdText);
	//STDMETHOD(Exec)(/*[in]*/ const GUID *pguidCmdGroup, /*[in]*/ DWORD nCmdID, /*[in]*/ DWORD nCmdExecOpt, /*[in]*/ VARIANTARG *pvaIn, /*[in,out]*/ VARIANTARG *pvaOut);

	// helpers
	CComPtr<IWebBrowser2>										GetTopBrowser() const { return m_TopBrowser; }

	void														Log(CString message, int icon);

	void														PerformRefresh();
	void														ListenForReconnect();
	void														Connect();
	void														Disconnect();
	bool														IsConnected() { return m_ConnectionManager.IsConnected(); }
	CLogger*													GetLogger() { return &m_Logger; }
	TBrowserId													GetBrowserId() { return m_BrowserId; }
	void														SendInfoAboutPage();
	void														PauseXRefresh();
	void														UnpauseXRefresh();
	void														UpdateIcon();
	void														DisconnectedNotify();

private:
	// Helpers
	bool														SetupEnvironment();
	bool														OnRefreshStart();
	bool														OnRefreshEnd();

	// 
	HRESULT														ProcessDocument(IDispatch *pDisp, VARIANT *pvarURL);

	TBrowserId													m_BrowserId; ///< Id of this BHO 
	CComPtr<IWebBrowser2>										m_TopBrowser; ///< top level browser window in BHO's tab

	// Default interface pointers
	//CComPtr<IDocHostUIHandler>									m_DefaultDocHostUIHandler;
	//CComPtr<IOleCommandTarget>									m_DefaultOleCommandTarget;
	//CComPtr<ICustomDoc>											m_HookedDoc;

	bool														m_IsAdvised; 
	//bool														m_RequestReload;
	bool														m_DownloadInProgress;

	CConnectionManager											m_ConnectionManager; ///< XRefresh server connection
	CLogger														m_Logger;
	bool														m_Paused;
	static CToolWindow											m_IE7ToolWindow;
};

OBJECT_ENTRY_AUTO(__uuidof(XRefreshBHO), CXRefreshBHO)
