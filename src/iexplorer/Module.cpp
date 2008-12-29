#include "stdafx.h"

#include "Module.h"
#include "BrowserManager.h"

//#include "Debug.h"

// here are shared resources !
class CXRefreshRootImpl : public CXRefreshRoot {

public:
	CXRefreshRootImpl();
	virtual ~CXRefreshRootImpl();

	virtual bool											Init();
	virtual bool											Done();
	virtual void*											Acquire(ESharedResourceId rid);
	virtual void											Release(ESharedResourceId rid);

	virtual bool											CheckThreadOwnership(ESharedResourceId rid);

private:
	bool													m_Inited;
	CCS														m_MainCS;

	// shared objects
	CBrowserManager											m_BrowserManager;

	static TCHAR											m_WorkspacePath[_MAX_PATH];
};

static CXRefreshRootImpl* gpXRefreshRoot = NULL;
static int giRootInitRefcount = 0;
CCS gRootCS;

void InitRoot()
{
	gRootCS.Enter();
	if (!giRootInitRefcount)
	{
		ATLASSERT(!gpXRefreshRoot);
		gpXRefreshRoot = new CXRefreshRootImpl();
		gpXRefreshRoot->Init();
	}
	giRootInitRefcount++;
	gRootCS.Leave();
}

void DoneRoot()
{
	gRootCS.Enter();
	giRootInitRefcount--;
	if (!giRootInitRefcount)
	{
		ATLASSERT(gpXRefreshRoot);
		gpXRefreshRoot->Done();
		delete gpXRefreshRoot;
	}
	gRootCS.Leave();
}

CXRefreshRoot& GetRoot() 
{ 
	ATLASSERT(gpXRefreshRoot);
	return *gpXRefreshRoot; 
}

static TCHAR* g_acResourceNames[SR_LAST] = {
	_T("BrowserManager")
};

CXRefreshRootImpl::CXRefreshRootImpl() : 
m_Inited(false)
{

}

CXRefreshRootImpl::~CXRefreshRootImpl()
{
	Done();
}

#define WSA_VERSION  MAKEWORD(2,0)

bool
CXRefreshRootImpl::Init()
{
	if (m_Inited) return true;

	// debug
#ifdef _DEBUG
	//GetDebugInterfacesModule().m_nIndexBreakAt = 39;
	//g_ComObjectBreak = 10;
#endif

#if (_WIN32_IE >= 0x0300)
	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize = sizeof(iccx);
	iccx.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_USEREX_CLASSES; 
	BOOL bRet = ::InitCommonControlsEx(&iccx);
	bRet;
	ATLASSERT(bRet);
#else
	::InitCommonControls();
#endif

	WSADATA WSAData = { 0 };
	if ( 0 != WSAStartup( WSA_VERSION, &WSAData ) )
	{
		// Tell the user that we could not find a usable
		// WinSock DLL.
		if ( LOBYTE( WSAData.wVersion ) != LOBYTE(WSA_VERSION) ||
			 HIBYTE( WSAData.wVersion ) != HIBYTE(WSA_VERSION) )
			 ::MessageBox(NULL, _T("Incorrect version of WS2_32.dll found"), _T("Error"), MB_OK);

		WSACleanup( );
		return false;
	}

	m_Inited = true;
	// CoInternetSetFeatureEnabled(FEATURE_LOCALMACHINE_LOCKDOWN, SET_FEATURE_ON_PROCESS, TRUE); 

	// other resources will be inited when they are first asked
	return true;
}

bool
CXRefreshRootImpl::Done()
{
	if (!m_Inited) return true;
	BrowserManagerLock()->Done();
	m_Inited = false;
	WSACleanup();
	return true;
}

static VOID CALLBACK APCProc(ULONG_PTR dwParam)
{
	DT(TRACE_I(FS(_T("signalled by %08x"), dwParam)));
}

// pri beznem provozu se nesmi TRACOVAT v Acquire a Release
// to vede k zmateni Loggeru, protoze Acquire a Release nejsou reentrantni
void*
CXRefreshRootImpl::Acquire(ESharedResourceId rid)
{
	// try to enter CS
	m_MainCS.Enter();
	switch (rid) {
	case SR_BROWSERMANAGER: return &m_BrowserManager;
	}
	ATLASSERT(0);
	return NULL;
}

void
CXRefreshRootImpl::Release(ESharedResourceId rid)
{
	m_MainCS.Leave();
}

bool
CXRefreshRootImpl::CheckThreadOwnership(ESharedResourceId rid)
{
	return true;
}