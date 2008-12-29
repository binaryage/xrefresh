#pragma once

// resource accessors
#define BrowserManagerLock                        CResourceLock<CBrowserManager> 

enum ESharedResourceId {
	SR_BROWSERMANAGER,
	SR_LAST
};

//////////////////////////////////////////////////////////////////////////
// CResourceLock
template <class T>
class CResourceLock {
public:
	CResourceLock(): m_T((T*)GetRoot().Acquire(T::GetResourceId())) {}
	~CResourceLock() { GetRoot().Release(T::GetResourceId()); }
	T* operator->() { m_T->Init(); GetRoot().CheckThreadOwnership(T::GetResourceId()); return m_T; }

private:
	// do not allow copy
	CResourceLock(const CResourceLock& rs) {}
	CResourceLock&                                operator=(const CResourceLock&) { return *this; }

	T*                                            m_T;
};

//////////////////////////////////////////////////////////////////////////
// CResourceInit
template<ESharedResourceId rid>
class CResourceInit {
public:
	CResourceInit(): m_Inited(false), m_Failed(false) {};
	virtual ~CResourceInit() {};

	virtual bool                                  Init() { return true; } 
	virtual bool                                  Done() { return true; } 
	static ESharedResourceId                      GetResourceId() { return rid; }

private:
	// do not allow copy
	CResourceInit(const CResourceInit&) {}
	CResourceInit&                                operator=(const CResourceInit&) { return *this; }

protected:
	bool                                          m_Inited;
	bool                                          m_Failed;
};

//////////////////////////////////////////////////////////////////////////
// CXRefreshRoot
class CXRefreshRoot {
public:
	virtual bool                                  Init() = 0;
	virtual bool                                  Done() = 0;
	virtual void*                                 Acquire(ESharedResourceId rid) = 0;
	virtual void                                  Release(ESharedResourceId rid) = 0;

	virtual bool                                  CheckThreadOwnership(ESharedResourceId rid) = 0;
};

//////////////////////////////////////////////////////////////////////////

void InitRoot();
void DoneRoot();

CXRefreshRoot& GetRoot();
