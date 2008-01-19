#include "StdAfx.h"
#include "ConsoleWindow.h"

//////////////////////////////////////////////////////////////////////////
// CConsoleWindow

CConsoleWindow::CConsoleWindow()
{
}

CConsoleWindow::~CConsoleWindow()
{
}

LRESULT
CConsoleWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DWORD consoleStyle = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
	m_LoggerConsole.Create(*this, rcDefault, CONSOLE_LIST_WINDOW_NAME, consoleStyle, 0, 1);

	CBitmap toolbar;
	toolbar.LoadBitmap(MAKEINTRESOURCE(IDB_ICONS));
	if (toolbar.IsNull()) throw CXRefreshWindowsError(GetLastError());

	CBitmap mask;
	mask.LoadBitmap(MAKEINTRESOURCE(IDB_ICONS));
	if (mask.IsNull()) throw CXRefreshWindowsError(GetLastError());

	CDPIHelper::ScaleBitmap(toolbar);
	CDPIHelper::ScaleBitmap(mask);

	int width = (int)CDPIHelper::ScaleX(16);
	int height = (int)CDPIHelper::ScaleY(16);
	m_ImageList.Create(width, height, ILC_COLOR24 | ILC_MASK, 3, 3);
	if (m_ImageList.IsNull()) throw CXRefreshWindowsError(GetLastError());
	if (m_ImageList.Add(toolbar, mask) == -1) throw CXRefreshWindowsError(GetLastError());

	InitList();
	return TRUE;
}

void
CConsoleWindow::Layout()
{
	RECT  rc;
	GetClientRect(&rc);

	HDWP hdwp = BeginDeferWindowPos(1);
	hdwp = m_LoggerConsole.DeferWindowPos(hdwp, NULL, 0, 0, rc.right-10, rc.bottom, SWP_NOZORDER);
	EndDeferWindowPos(hdwp);
} 

LRESULT
CConsoleWindow::OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	WINDOWPOS * lpwndpos = (WINDOWPOS *)lParam;
	if (!(lpwndpos->flags & SWP_NOSIZE))
	{
		Layout();
		bHandled = TRUE;
	}
	return 0;
}

void
CConsoleWindow::InitList()
{
	LOGFONT logFont = { 0 };
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = 90;
	lstrcpy(logFont.lfFaceName, _T("Courier New"));
	logFont.lfWeight = FW_NORMAL;
	logFont.lfItalic = (BYTE)FALSE;
	m_Font.CreatePointFontIndirect(&logFont);

	m_LoggerConsole.SetSmoothScroll(FALSE);
	m_LoggerConsole.SetSingleSelect(TRUE);
	m_LoggerConsole.SetDragDrop(FALSE);

	m_LoggerConsole.SetImageList(m_ImageList);
}

void
CConsoleWindow::Update()
{
	m_LoggerConsole.Update();
}
