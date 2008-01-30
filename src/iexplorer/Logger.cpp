#include "StdAfx.h"
#include "time.h"
#include "Logger.h"
#include "BrowserManager.h"
#include "XRefreshBHO.h"

CLoggerConsole::CLoggerConsole():
	m_BrowserId(NULL_BROWSER)
{
}

CLoggerConsole::~CLoggerConsole()
{
}

CLoggerModel*
CLoggerConsole::GetModel()
{
	BrowserManagerLock browserManager;
	ATLASSERT(browserManager->IsBrowserThread(GetCurrentThreadId(), m_BrowserId));
	CBrowserMessageWindow* window = browserManager->FindBrowserMessageWindow(m_BrowserId);
	if (!window) return NULL;
	CXRefreshBHO* bho = window->GetBHO();
	if (!bho) return NULL;
	return bho->GetLogger();
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
	CLoggerModel* model = GetModel();
	if (!model) return 0;
	return model->m_Messages.GetSize();
}

BOOL
CLoggerConsole::GetLogMessage(int nItem, CLogMessage& logMessage)
{
	if (nItem < 0 || nItem >= GetItemCount()) return FALSE;
	CLoggerModel* model = GetModel();
	if (!model) return FALSE;
	logMessage = model->m_Messages[nItem];
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
	CLoggerModel* model = GetModel();
	if (!model) return;
	model->m_Messages.Sort(CompareItem((UserColumns)nColumn));
}

void
CLoggerConsole::ReverseItems() // overrides CListImpl::ReverseItems
{
	CLoggerModel* model = GetModel();
	if (!model) return;
	model->m_Messages.Reverse();
}

void
CLoggerConsole::Update()
{
	// track new item
	if (!m_hWnd) return; // HACK
	ResetScrollBars();
	Invalidate();
	EnsureItemVisible(GetItemCount());
}

//////////////////////////////////////////////////////////////////////////

bool
CLoggerModel::Log(LPCTSTR message, int icon)
{
	time_t ltime;
	time(&ltime);
	bool res = m_Messages.Add(CLogMessage(ltime, message, icon))?true:false;
	while (m_Messages.GetSize()>MAX_LOGGER_MESSAGES) m_Messages.RemoveAt(0); // keep the limits

	// notify console
	if (m_BrowserId==NULL_BROWSER) return false;
	BrowserManagerLock browserManager;
	ATLASSERT(browserManager->IsBrowserThread(GetCurrentThreadId(), m_BrowserId));
	CBrowserMessageWindow* window = browserManager->FindBrowserMessageWindow(m_BrowserId);
	if (!window) return false;
	CXRefreshHelperbar* bar = window->GetHelperbar();
	if (!bar) return false;

	bar->Update();
	return true;
}
