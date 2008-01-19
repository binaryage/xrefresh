#pragma once

#include "XRefreshBHO.h"
#include "XRefreshHelperbar.h"
#include "XRefreshToolbar.h"

#define BMM_BASE                                  (WM_APP+4000)
#define BMM_REQUEST_REFRESH                       (BMM_BASE+0)
#define BMM_REQUEST_LISTEN_FOR_RECONNECT          (BMM_BASE+1)
#define BMM_REQUEST_INFO_ABOUT_PAGE               (BMM_BASE+2)
#define BMM_REQUEST_PAUSE                         (BMM_BASE+3)
#define BMM_REQUEST_UNPAUSE                       (BMM_BASE+4)
#define BMM_REQUEST_UPDATE_ICON                   (BMM_BASE+5)
#define BMM_REQUEST_DISCONNECTED_NOTIFY           (BMM_BASE+6)
#define BMM_REQUEST_LOG                           (BMM_BASE+7)
#define BMM_REQUEST_RESET_LAST_SENT_TITLE         (BMM_BASE+8)
#define BMM_LAST                                  (BMM_BASE+100)

typedef CWinTraits<WS_OVERLAPPED, 0> TBrowserWindowTraits; // must be hidden

//////////////////////////////////////////////////////////////////////////
// CBrowserMessageWindow 
// CBrowserMessageWindow je okno vytvorene threadem prislusneho BHO/helperbaru
// na tomto okne budeme zpracovavat ukoly, ktere musi delat prislusny thread pro zbytek systemu
class CBrowserMessageWindow : public CWindowImpl<CBrowserMessageWindow, CWindow, TBrowserWindowTraits> {
public:
	CBrowserMessageWindow(IUnknown* browserInterface, CXRefreshBHO* pBHO, CXRefreshHelperbar* pHelperbar, CXRefreshToolbar* pToolbar);
	virtual ~CBrowserMessageWindow();

	DECLARE_WND_CLASS(BROWSER_MESSAGE_WINDOW_CLASS_NAME);
	BEGIN_MSG_MAP(CBrowserMessageWindow)
		MESSAGE_RANGE_HANDLER_EX(BMM_BASE, BMM_LAST, WindowProc)
	END_MSG_MAP()

	LRESULT                                       WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	CXRefreshBHO*                                 GetBHO() const { return m_BHO; }
	void                                          SetBHO(CXRefreshBHO* pBHO);
	CXRefreshHelperbar*                           GetHelperbar() const { return m_Helperbar; }
	void                                          SetHelperbar(CXRefreshHelperbar* pHelperbar) { m_Helperbar = pHelperbar; }
	CXRefreshToolbar*                             GetToolbar() const { return m_Toolbar; }
	void                                          SetToolbar(CXRefreshToolbar* pToolbar) { m_Toolbar = pToolbar; }

	unsigned int                                  AddRef() { return ++m_RefCount; }
	unsigned int                                  DecRef() { return --m_RefCount; }
	unsigned int                                  RefCount() const { return m_RefCount; }

	IUnknown*                                     GetBrowserInterface() const;
	DWORD                                         GetThreadId() { return m_ThreadId; }

protected:
	bool                                          CreateMessageWindow();
	bool                                          DestroyMessageWindow();

	unsigned int                                  m_RefCount;
	CXRefreshBHO*                                 m_BHO;
	CXRefreshHelperbar*                           m_Helperbar;
	CXRefreshToolbar*                             m_Toolbar;
	DWORD                                         m_ThreadId; ///< browser's thread
};

typedef hash_map<TBrowserId, CBrowserMessageWindow*> TBrowserMessageWindowMap;

//////////////////////////////////////////////////////////////////////////
// CBrowserManager
class CBrowserManager: public CResourceInit<SR_BROWSERMANAGER> {
public:
	CBrowserManager();
	~CBrowserManager();

	TBrowserId                                    AllocBrowserId(IUnknown* browserInterface, CXRefreshBHO* pBHO);
	TBrowserId                                    AllocBrowserId(IUnknown* browserInterface, CXRefreshHelperbar* pHelperbar);
	TBrowserId                                    AllocBrowserId(IUnknown* browserInterface, CXRefreshToolbar* pToolbar);
	bool                                          ReleaseBrowserId(TBrowserId browserId);
	CBrowserMessageWindow*                        FindBrowserMessageWindow(TBrowserId browserId);
	bool                                          IsBrowserThread(DWORD threadId, TBrowserId browserId);

protected:
	TBrowserId                                    AllocBrowserId(IUnknown* browserInterface, CXRefreshBHO* pBHO, CXRefreshHelperbar* pHelperbar, CXRefreshToolbar* pToolbar);
	TBrowserMessageWindowMap::iterator            FindBrowserId(IUnknown* browserInterface);

protected:
	TBrowserId                                    m_NextId;
	TBrowserMessageWindowMap                      m_Browsers;
};
