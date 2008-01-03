// AboutBox.cpp : Implementation of CAboutBox

#include "stdafx.h"
#include "AboutBox.h"
#include "verinfo.h"

// CAboutBox
CAboutBox::CAboutBox():m_Version(this, 1)
{
}

CAboutBox::~CAboutBox()
{
}

LRESULT 
CAboutBox::OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}

LRESULT 
CAboutBox::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TCHAR* hyperlink;
	switch (LOWORD(wParam)) {
		case IDC_HOMEPAGE: hyperlink = _T("http://xrefresh.com"); break;
		case IDC_PEOPLE: hyperlink = _T("http://xrefresh.com/people"); break;
		case IDC_DONATE: hyperlink = _T("http://xrefresh.com/donate"); break;
		case IDC_AUTHOR: hyperlink = _T("mailto:antonin@hildebrand.cz"); break;
		default: return S_OK;
	}

	SHELLEXECUTEINFO shExeInfo = { sizeof(SHELLEXECUTEINFO), 0, 0, L"open", hyperlink, 0, 0, SW_SHOWNORMAL, 0, 0, 0, 0, 0, 0, 0 };
	::ShellExecuteEx(&shExeInfo);
	return S_OK;
}

LRESULT 
CAboutBox::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_Version.SubclassWindow(GetDlgItem(IDC_VERSION));
	DLLVERSIONINFO dllVerInfo;
	ZeroMemory(&dllVerInfo, sizeof(DLLVERSIONINFO));
	dllVerInfo.cbSize  = sizeof(DLLVERSIONINFO);
	DllGetVersion(GetBaseModule().GetModuleInstance(), &dllVerInfo);
	TCHAR version[100];
	_stprintf_s(version, _T("%d.%d"), dllVerInfo.dwMajorVersion, dllVerInfo.dwMinorVersion);
	m_Version.SetWindowText(version);
	m_Version.Invalidate();

	m_Homepage.SetLabel(_T("http://xrefresh.com"));
	m_Homepage.SetHyperLinkExtendedStyle(HLINK_COMMANDBUTTON);
	m_Homepage.SetToolTipText(_T("Visit XRefresh homepage"));
	m_Homepage.SubclassWindow(GetDlgItem(IDC_HOMEPAGE));

	m_People.SetLabel(_T("http://xrefresh.com/people"));
	m_People.SetHyperLinkExtendedStyle(HLINK_COMMANDBUTTON);
	m_People.SetToolTipText(_T("See XRefresh contributors"));
	m_People.SubclassWindow(GetDlgItem(IDC_PEOPLE));

	m_Donate.SetLabel(_T("http://xrefresh.com/donate"));
	m_Donate.SetHyperLinkExtendedStyle(HLINK_COMMANDBUTTON);
	m_Donate.SetToolTipText(_T("See XRefresh donators"));
	m_Donate.SubclassWindow(GetDlgItem(IDC_DONATE));

	m_Author.SetLabel(_T("Antonin Hildebrand"));
	m_Author.SetHyperLinkExtendedStyle(HLINK_COMMANDBUTTON);
	m_Author.SetToolTipText(_T("Send mail to author"));
	m_Author.SubclassWindow(GetDlgItem(IDC_AUTHOR));

	bHandled = TRUE;
	return S_OK;
}

LRESULT 
CAboutBox::OnCtlColorStatic(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hDC = (HDC)wParam;
	SetBkMode(hDC, TRANSPARENT);
	bHandled = TRUE;
	return (LRESULT)(HBRUSH)GetStockObject(NULL_BRUSH);
}