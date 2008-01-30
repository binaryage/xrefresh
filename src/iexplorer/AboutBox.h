// AboutBox.h : Declaration of the CAboutBox

#pragma once

#include "resource.h" // main symbols
#include <atlhost.h>

//////////////////////////////////////////////////////////////////////////
// CAboutBox
class CAboutBox: public CDialogImpl<CAboutBox> {
public:
	CAboutBox();
	~CAboutBox();

	enum { IDD = IDD_ABOUTBOX };

	BEGIN_MSG_MAP(CAboutBox)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedOK)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
	ALT_MSG_MAP(1) // version
	END_MSG_MAP()

	LRESULT                                       OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT                                       OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT                                       OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT                                       OnCtlColorStatic(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	CContainedWindow                              m_Version;
	CHyperLink                                    m_Homepage;
	CHyperLink                                    m_About;
	CHyperLink                                    m_Author;
};