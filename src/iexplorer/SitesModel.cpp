#include "stdafx.h"
#include "SitesModel.h"

using namespace regex;

//////////////////////////////////////////////////////////////////////////
// CSitesRecord

CSiteRecord::CSiteRecord(CString site, ESiteAction action, bool active):
	m_Active(active),
	m_Site(site),
	m_Action(action),
	m_Pattern(NULL)
{
}

CSiteRecord::~CSiteRecord() 
{
	delete m_Pattern;
}

CSiteRecord::CSiteRecord(const CSiteRecord& r):
	m_Active(r.m_Active),
	m_Site(r.m_Site),
	m_Action(r.m_Action),
	m_Pattern(NULL) // do not copy pattern cache!
{
}

CSiteRecord&
CSiteRecord::operator=(const CSiteRecord& r)
{
	m_Active = r.m_Active;
	m_Site = r.m_Site;
	m_Action = r.m_Action;
	delete m_Pattern; // release pattern cache
	m_Pattern = NULL; // do not copy pattern cache!
	return *this;
}

bool
CSiteRecord::Test(tstring& s)
{
	match_results results;
	if (!m_Pattern) m_Pattern = new rpattern(tstring(m_Site), NOCASE);
	return m_Pattern->match(s, results).matched;
}

CString
CSiteRecord::GetSite()
{
	return m_Site;
}

void
CSiteRecord::SetSite(LPCTSTR site)
{
	if (m_Pattern)
	{
		delete m_Pattern;
		m_Pattern = NULL;
	}
	m_Site = site;
}

ESiteAction
CSiteRecord::GetAction()
{
	return m_Action;
}

void
CSiteRecord::SetAction(ESiteAction action)
{
	m_Action = action;
}

bool
CSiteRecord::GetActive()
{
	return m_Active;
}

void
CSiteRecord::SetActive(bool active)
{
	m_Active = active;
}

//////////////////////////////////////////////////////////////////////////
// CSitesModel

bool
CSitesModel::Add(CString message, ESiteAction action)
{
	m_Sites.push_back(CSiteRecord(message, action));
	return true;
}

bool
CSitesModel::UpdateActive(int index, bool active)
{
	m_Sites[index].SetActive(active);
	return true;
}

bool
CSitesModel::ToggleActive(int index)
{
	m_Sites[index].SetActive(!m_Sites[index].GetActive());
	return true;
}

bool
CSitesModel::IsActive(int index)
{
	return m_Sites[index].GetActive();
}

bool
CSitesModel::UpdateSite(int index, LPCTSTR message)
{
	m_Sites[index].SetSite(message);
	return true;
}

bool
CSitesModel::UpdateAction(int index, ESiteAction action)
{
	m_Sites[index].SetAction(action);
	return true;
}

bool
CSitesModel::CycleAction(int index)
{
	int a = m_Sites[index].GetAction(); a++;
	if (a==E_LAST) a = E_ALLOW;
	m_Sites[index].SetAction((ESiteAction)a);
	return true;
}

void
CSitesModel::Swap(int a, int b)
{
	CSiteRecord tmp(m_Sites[a]);
	m_Sites[a] = m_Sites[b];
	m_Sites[b] = tmp;
}

bool
CSitesModel::LoadRecord(HKEY hKey, int index, CSiteRecord& record)
{
	DWORD dwType;
	DWORD dwSize;
	LONG lRes;
	
	TCHAR buf[1024];
	dwSize = 1024*sizeof(TCHAR);
	lRes = RegQueryValueEx(hKey, FS(_T("%ds"), index), NULL, &dwType, (LPBYTE)buf, &dwSize);
	if (lRes!=ERROR_SUCCESS) return false;
	record.SetSite(buf);

	DWORD action = E_ALLOW;
	dwSize = sizeof(action);
	lRes = RegQueryValueEx(hKey, FS(_T("%dx"), index), NULL, &dwType, (LPBYTE)&action, &dwSize);
	record.SetAction((ESiteAction)action);

	DWORD active = 1;
	dwSize = sizeof(active);
	lRes = RegQueryValueEx(hKey, FS(_T("%da"), index), NULL, &dwType, (LPBYTE)&active, &dwSize);
	record.SetActive(active?true:false);

	return true;
}

bool
CSitesModel::Load(LPCTSTR key)
{
	m_Sites.clear();

	HKEY hKey;
	LONG lRes = RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_READ, &hKey);
	if (lRes != ERROR_SUCCESS) return true; // it is correct when key does not exist

	int index = 1;
	while (index<1000) // safety cap
	{
		CSiteRecord record(_T(""), E_ALLOW, false);
		if (!LoadRecord(hKey, index, record)) break;
		m_Sites.push_back(record);
		index++;
	}
	RegCloseKey(hKey);
	return true;
}

bool
CSitesModel::SaveRecord(HKEY hKey, int index, CSiteRecord& record)
{
	DWORD dwType;
	DWORD dwSize;
	LONG lRes;

	dwType = REG_SZ;
	CString site = record.GetSite();
	LPTSTR buf = site.LockBuffer();
	dwSize = (DWORD)(_tcslen(buf)+1)*sizeof(TCHAR);
	lRes = RegSetValueEx(hKey, FS(_T("%ds"), index), NULL, dwType, (LPBYTE)buf, dwSize);
	site.UnlockBuffer();
	if (lRes!=ERROR_SUCCESS) return false;

	dwType = REG_DWORD;
	DWORD action = record.GetAction();
	dwSize = (DWORD)sizeof(action);
	lRes = RegSetValueEx(hKey, FS(_T("%dx"), index), NULL, dwType, (LPBYTE)&action, dwSize);
	if (lRes!=ERROR_SUCCESS) return false;

	dwType = REG_DWORD;
	DWORD active = record.GetActive()?1:0;
	dwSize = (DWORD)sizeof(active);
	lRes = RegSetValueEx(hKey, FS(_T("%da"), index), NULL, dwType, (LPBYTE)&active, dwSize);
	if (lRes!=ERROR_SUCCESS) return false;

	return true;
}

bool
CSitesModel::Save(LPCTSTR key)
{
	HKEY hKey;
	SHDeleteKey(HKEY_CURRENT_USER, key);
	if (RegCreateKey(HKEY_CURRENT_USER, key, &hKey)) return false;

	T_SitesContainer::iterator i = m_Sites.begin();
	int index = 1;
	while (i!=m_Sites.end())
	{
		if (!SaveRecord(hKey, index, *i)) 
		{
			RegCloseKey(hKey);
			return false;
		}
		index++;
		i++;
	}
	RegCloseKey(hKey);
	return true;
}

bool
CSitesModel::Test(LPCTSTR v)
{
	bool accept = false;
	tstring s(v);
	T_SitesContainer::iterator i = m_Sites.begin();
	while (i!=m_Sites.end())
	{
		CSiteRecord& r(*i);
		if (r.GetActive())
		{
			if (accept)
			{
				if (r.GetAction()==E_BLOCK)
				{
					bool q = r.Test(s);
					if (q) return false; // blocked by this record
				}
			}
			else
			{
				if (r.GetAction()==E_ALLOW)
				{
					bool q = r.Test(s);
					if (q) accept = true; // accepted by this record
				}
			}
		}
		i++;
	}
	return accept;
}

CString
CSitesModel::TestWithReason(LPCTSTR v)
{
	CString reason = _T("NOT ALLOWED: no rule has mathed this url");
	tstring s(v);
	T_SitesContainer::iterator i = m_Sites.begin();
	int index = 1;
	bool accept = false;
	while (i!=m_Sites.end())
	{
		CSiteRecord& r(*i);
		if (r.GetActive())
		{
			if (accept)
			{
				if (r.GetAction()==E_BLOCK)
				{
					bool q = r.Test(s);
					if (q) 
					{
						return FS(_T("NOT ALLOWED: block rule #%d has mathed this url"), index);
					}
				}
			}
			else
			{
				if (r.GetAction()==E_ALLOW)
				{
					bool q = r.Test(s);
					if (q)
					{
						accept = true;
						reason = FS(_T("ALLOWED: allow rule #%d has mathed this url"), index);
					}
				}
			}
		}
		index++;
		i++;
	}
	return reason;
}

int
CSitesModel::GetCount()
{
	return (int)m_Sites.size();
}

bool
CSitesModel::RemoveAt(int index)
{
	ATLASSERT(index>=0 && index<GetCount());
	T_SitesContainer::iterator i = m_Sites.begin();
	std::advance(i,index);
	m_Sites.erase(i);
	return true;
}

CSiteRecord&
CSitesModel::ItemAt(int index)
{
	ATLASSERT(index>=0 && index<GetCount());
	return m_Sites[index];
}
