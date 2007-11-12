#pragma once

enum ESharedResourceId {
	SR_SERVICES,
	SR_BROWSERMANAGER,
	SR_DIALOGMANAGER,
	SR_LAST
};

template <class T>
class CResourceLock {
public:
	CResourceLock() :	m_T((T*)GetRoot().Acquire(T::GetResourceId())) {}
	~CResourceLock() { GetRoot().Release(T::GetResourceId()); }
	T* operator->() { m_T->Init(); GetRoot().CheckThreadOwnership(T::GetResourceId()); return m_T; }

private:
	// do not allow copy
	CResourceLock& operator=(const CResourceLock&) { return *this; }
	CResourceLock(const CResourceLock& rs) {}

	T*														m_T;
};

template<ESharedResourceId rid>
class CResourceInit {
public:
	CResourceInit() : m_Inited(false), m_Failed(false) {};
	virtual ~CResourceInit() {};

	virtual bool											Init() { return true; } 
	virtual bool											Done() { return true; } 
	static ESharedResourceId								GetResourceId() { return rid; }

private:
	// do not allow copy
	CResourceInit(const CResourceInit&) {}
	CResourceInit& operator=(const CResourceInit&) { return *this; }

protected:
	bool													m_Inited;
	bool													m_Failed;
};

template <ESharedResourceId rid> 
class CResourceUnlock {
public:
	CResourceUnlock() : m_Count(GetRoot().ReleaseAll(rid)) {}
	~CResourceUnlock() { GetRoot().AcquireMany(rid, m_Count); }

	int														m_Count;
};

// resource accessors
#define ServicesLock										CResourceLock<CServices> 
#define BrowserManagerLock									CResourceLock<CBrowserManager> 
#define DialogManagerLock									CResourceLock<CDialogManager> 

#define ResourceUnlock(rid)									CResourceUnlock<rid> __unlocker__##rid;

class CXRefreshRoot {
public:
	virtual bool											Init() = 0;
	virtual bool											Done() = 0;
	virtual void*											Acquire(ESharedResourceId rid) = 0;
	virtual void											Release(ESharedResourceId rid) = 0;

	virtual bool											CheckThreadOwnership(ESharedResourceId rid) = 0;

	virtual int												ReleaseAll(ESharedResourceId rid) = 0;
	virtual void*											AcquireMany(ESharedResourceId rid, int count) = 0;
};

void InitRoot();
void DoneRoot();

CXRefreshRoot& GetRoot();
