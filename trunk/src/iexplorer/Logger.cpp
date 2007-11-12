#include "StdAfx.h"
#include "time.h"
#include "Logger.h"
#include "BrowserManager.h"
#include "XRefreshBHO.h"

CLoggerConsole::CLoggerConsole():
	m_BrowserId(NULL_BROWSER),
	m_Logger(NULL)
{
}

CLoggerConsole::~CLoggerConsole()
{
	if (m_Logger) m_Logger->m_Console = NULL;
}

BOOL
CLoggerConsole::Initialise()
{
	CListImpl<CLoggerConsole>::Initialise();

	AddColumn(_T("Time"), 70);
	AddColumn(_T("Message"), 1000);

	return TRUE;
}

int
CLoggerConsole::GetItemCount() // required by CListImpl
{
	if (!CacheLogger()) return 0;
	return m_Logger->m_Messages.GetSize();
}

BOOL
CLoggerConsole::GetLogMessage(int nItem, CLogMessage& logMessage)
{
	if (!CacheLogger()) return FALSE;
	if (nItem < 0 || nItem >= GetItemCount()) return FALSE;
	logMessage = m_Logger->m_Messages[nItem];
	return TRUE;
}

CString
CLoggerConsole::GetItemText(int nItem, int nSubItem) // required by CListImpl
{
	CLogMessage logMessage;
	if (!GetLogMessage(nItem, logMessage)) return _T("");
	switch ((UserColumns)nSubItem) {
		case E_TIME:	   
		{
			CString s;
			struct tm t;
			errno_t err = _localtime64_s(&t, &logMessage.m_Time);
			if (err) return _T("[?]");
			s.Format(_T("[%02d:%02d:%02d]"), t.tm_hour, t.tm_min, t.tm_sec);
			return s;
		}
		case E_MESSAGE:	return logMessage.m_Message;
	}
	return _T("");
}

int
CLoggerConsole::GetItemImage(int nItem, int nSubItem) // overrides CListImpl::GetItemImage
{
	if ((UserColumns)nSubItem == E_TIME) return -1;
	CLogMessage logMessage;
	if (!GetLogMessage(nItem, logMessage)) return -1;
	return logMessage.m_Icon;
}

void
CLoggerConsole::SortItems(int nColumn, BOOL bAscending) // overrides CListImpl::SortItems
{
	if (!CacheLogger()) return;
	m_Logger->m_Messages.Sort(CompareItem((UserColumns)nColumn));
}

void
CLoggerConsole::ReverseItems() // overrides CListImpl::ReverseItems
{
	if (!CacheLogger()) return;
	m_Logger->m_Messages.Reverse();
}

void
CLoggerConsole::MessageAdded()
{
	// track new item
	if (!m_hWnd) return; // HACK
	ResetScrollBars();
	Invalidate();
	EnsureItemVisible(GetItemCount());
}

bool 
CLoggerConsole::CacheLogger()
{
	if (m_Logger) return true;
	if (m_BrowserId==NULL_BROWSER) return false;
	BrowserManagerLock browserManager;
	CBrowserMessageWindow* window = browserManager->FindBrowserMessageWindow(m_BrowserId);
	if (!window) return false;
	CXRefreshBHO* BHO = window->GetBHO();
	if (!BHO) return false;
	m_Logger = BHO->GetLogger();
	m_Logger->m_Console = this;
	return m_Logger!=NULL;
}

//////////////////////////////////////////////////////////////////////////

bool											
CLogger::Log(CString message, int icon)
{
	time_t ltime;
	time(&ltime);
	bool res = m_Messages.Add(CLogMessage(ltime, message, icon))?true:false;
	if (m_Console) m_Console->MessageAdded();
	return true;
}
