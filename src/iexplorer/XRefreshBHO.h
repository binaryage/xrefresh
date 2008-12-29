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
	public IDispEventImpl<1, CXRefreshBHO, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 1>
#ifdef _DEBUG
	, public WinTrace
#endif
{
public:
	CXRefreshBHO();
	virtual ~CXRefreshBHO();

	DECLARE_REGISTRY_RESOURCEID(IDR_XREFRESHBHO)
	DECLARE_NOT_AGGREGATABLE(CXRefreshBHO)

	BEGIN_COM_MAP(CXRefreshBHO)
		COM_INTERFACE_ENTRY(IXRefreshBHO)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IObjectWithSite)
	END_COM_MAP()

	BEGIN_SINK_MAP(CXRefreshBHO)
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
	STDMETHOD(OnWindowStateChanged)(DWORD dwFlags, DWORD dwValidFlagsMask);
	STDMETHOD(OnWindowTitleChanged)(BSTR bstrTitleText);

	// helpers
	CComPtr<IWebBrowser2>                         GetBrowser() const { return m_Browser; }

	void                                          Log(LPCTSTR message, int icon);

	void                                          PerformRefresh();
	void                                          ListenForReconnect();
	void                                          Connect();
	void                                          Disconnect();
	bool                                          IsConnected() { return m_ConnectionManager.IsConnected(); }
	CLoggerModel*                                 GetLogger() { return &m_Logger; }
	TBrowserId                                    GetBrowserId() { return m_BrowserId; }
	void                                          SendInfoAboutPage();
	void                                          PauseXRefresh();
	void                                          UnpauseXRefresh();
	void                                          UpdateIcon();
	void                                          DisconnectedNotify();
	void                                          ResetLastSentTitle();

private:
	HRESULT                                       ProcessDocument(IDispatch *pDisp, VARIANT *pvarURL);

	TBrowserId                                    m_BrowserId; ///< the id of this BHO 
	CComPtr<IWebBrowser2>                         m_Browser; ///< top level browser window in BHO's tab

	bool                                          m_IsAdvised;

	CConnectionManager                            m_ConnectionManager; ///< XRefresh server connection
	CLoggerModel                                  m_Logger;
	bool                                          m_Paused;
	static CToolWindow                            m_IE7ToolWindow;

	CString                                       m_LastSentTitle;
	CString                                       m_LastSentURL;
};

OBJECT_ENTRY_AUTO(__uuidof(XRefreshBHO), CXRefreshBHO)
