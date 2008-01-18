#pragma once
#include "greta/regexpr2.h"

#define ICON_ALLOW                                0
#define ICON_BLOCK                                1

class CSitesList;

enum ESiteAction {
	E_ALLOW                                       = ICON_ALLOW,
	E_BLOCK                                       = ICON_BLOCK,
	E_LAST
};

//////////////////////////////////////////////////////////////////////////
// CSiteRecord
class CSiteRecord {
public:
	CSiteRecord();
	CSiteRecord(CString site, ESiteAction action, bool active = true);
	~CSiteRecord();

	bool                                          Test(tstring& s);
	CString                                       GetSite();
	void                                          SetSite(LPCTSTR site);
	ESiteAction                                   GetAction();
	void                                          SetAction(ESiteAction action);
	bool                                          GetActive();
	void                                          SetActive(bool active);

private:
	bool                                          m_Active;
	ESiteAction                                   m_Action;
	CString                                       m_Site; // regexp
	regex::rpattern*                              m_Pattern; // cached pattern
};

//////////////////////////////////////////////////////////////////////////
// CSitesModel
class CSitesModel {
public:
	CSitesModel() {}
	bool                                          Add(CString message, ESiteAction action = E_ALLOW);
	bool                                          UpdateActive(int index, bool active);
	bool                                          ToggleActive(int index);
	bool                                          IsActive(int index);
	bool                                          UpdateSite(int index, LPCTSTR message);
	bool                                          UpdateAction(int index, ESiteAction action);
	bool                                          CycleAction(int index);
	void                                          Swap(int a, int b);

	bool                                          Load(LPCTSTR key);
	bool                                          Save(LPCTSTR key);

	bool                                          Test(LPCTSTR v);
	CString                                       TestWithReason(LPCTSTR v);

	int                                           GetCount();
	bool                                          RemoveAt(int index);
	CSiteRecord&                                  ItemAt(int index);

protected:
	bool                                          SaveRecord(HKEY key, int index, CSiteRecord& record);
	bool                                          LoadRecord(HKEY key, int index, CSiteRecord& record);

	typedef std::vector<CSiteRecord>              T_SitesContainer;
	T_SitesContainer                              m_Sites;
};
