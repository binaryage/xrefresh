#include "stdafx.h"
#include "SitesDialog.h"

#define QUERY_BOX_HEIGHT                          85

//////////////////////////////////////////////////////////////////////////
// CSitesList

CSitesList::CSitesList(CSitesModel* model):
	m_Model(model)
{
}

CSitesList::~CSitesList()
{
	
}

BOOL
CSitesList::Initialise()
{
	CListImpl<CSitesList>::Initialise();

	AddColumn(_T("#"), 30, ITEM_IMAGE_NONE, FALSE, ITEM_FORMAT_EDIT, ITEM_FLAGS_RIGHT);
	AddColumn(_T(""), 20, ITEM_IMAGE_NONE, TRUE, ITEM_FORMAT_CHECKBOX, ITEM_FLAGS_LEFT);
	AddColumn(_T("Site"), 300, ITEM_IMAGE_NONE, FALSE, ITEM_FORMAT_EDIT, ITEM_FLAGS_LEFT);

	return TRUE;
}

int
CSitesList::GetItemCount() // required by CListImpl
{
	return m_Model->GetCount();
}

void
CSitesList::Swap(int a, int b)
{
	m_Model->Swap(a, b);
}

void
CSitesList::Remove(int i)
{
	m_Model->RemoveAt(i);
	DeleteItem(i);
}

CSiteRecord&
CSitesList::GetSite(int nItem)
{
	ATLASSERT(nItem >= 0 && nItem < GetItemCount());
	return m_Model->ItemAt(nItem);
}

CString
CSitesList::GetItemText(int nItem, int nSubItem) // required by CListImpl
{
	CSiteRecord& siteRecord = GetSite(nItem);
	switch (nSubItem) {
		case 0: return FS(_T("%d."), nItem+1);
		case 1: return siteRecord.GetActive()? _T( "1" ) : _T( "0" );
		case 2: return siteRecord.GetSite();
	}
	return _T("");
}

int
CSitesList::GetItemImage(int nItem, int nSubItem) // overrides CListImpl::GetItemImage
{
	if (nSubItem != 2) return -1;
	CSiteRecord& siteRecord = GetSite(nItem);
	return ((int)siteRecord.GetAction())+(siteRecord.GetActive()?0:2);
}

void
CSitesList::Add(TCHAR* url)
{
	m_Model->Add(url)?true:false;
	AddItem();
	Invalidate();
}

BOOL
CSitesList::SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
{
	if (nSubItem==2) return m_Model->UpdateSite(nItem, lpszText);
	return FALSE;
}

void
CSitesList::ToggleItemCheckState(int nItem, int nSubItem)
{
	if (nSubItem!=1) return;
	m_Model->ToggleActive(nItem);
	Invalidate();
}

BOOL 
CSitesList::GetItemColours(int nItem, int nSubItem, COLORREF& rgbBackground, COLORREF& rgbText)
{
	if (m_Model->IsActive(nItem))
	{
		rgbText = GetSysColor(COLOR_WINDOWTEXT);
	}
	else
	{
		rgbText = GetSysColor(COLOR_GRAYTEXT);
	}
	rgbBackground = m_rgbBackground;
	return TRUE;
}

BOOL
CSitesList::HitTest2(CPoint point, int& nItem, int& nSubItem, CPoint& rpos)
{
	// are we over the header?
	if ( point.y < ( m_bShowHeader ? m_nHeaderHeight : 0 ) )
		return FALSE;
	
	// calculate hit test item
	nItem = GetTopItem() + (int)( ( point.y - ( m_bShowHeader ? m_nHeaderHeight : 0 ) ) / m_nItemHeight );
	
	if ( nItem < 0 || nItem >= GetItemCount() )
		return FALSE;
	
	int nTotalWidth = 0;
	int nColumnCount = GetColumnCount();

	// get hit-test subitem
	for ( nSubItem = 0; nSubItem < nColumnCount; nSubItem++ )
	{
		int nColumnWidth = GetColumnWidth( nSubItem );
		nTotalWidth += nColumnWidth;

		// offset position with current scroll position
		int nRelativePos = nTotalWidth - GetScrollPos( SB_HORZ );

		// are we over a subitem?
		if ( point.x > nRelativePos - nColumnWidth && point.x < nRelativePos )
		{
			rpos.x = point.x - (nRelativePos - nColumnWidth);
			rpos.y = (int)( ( point.y - ( m_bShowHeader ? m_nHeaderHeight : 0 ) ) % m_nItemHeight );
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL
CSitesList::EditItem(int nItem, int nSubItem, CPoint point)
{
	if (nSubItem==0) return FALSE;
	if (nSubItem!=2 || (point.x==0 && point.y==0)) return super::EditItem(nItem, nSubItem);

	if (!EnsureItemVisible( nItem, nSubItem ))
		return FALSE;
	
	if (GetFocus() != m_hWnd)
		return FALSE;
	
	CPoint rpos;
	HitTest2(point, nItem, nSubItem, rpos);

	if (rpos.x>20) return super::EditItem(nItem, nSubItem);
	m_Model->CycleAction(nItem);
	Invalidate();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CSitesDialog

CSitesDialog::CSitesDialog(CString defaultSite):
m_DefaultSite(defaultSite),
m_LastSizeX(-1),
m_SiteEdit(this, SITE_EDIT_ID),
m_QueryEdit(this, QUERY_EDIT_ID),
m_SitesList(NULL),
m_SitesModel(NULL)
{
}

CSitesDialog::~CSitesDialog()
{
	delete m_SitesModel;
	delete m_SitesList;
}

LRESULT 
CSitesDialog::OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (!m_SitesModel->Save(REGISTRY_ROOT_KEY_SITES))
	{
		::MessageBox(NULL, FS(_T("Unable to save sites settings into registry key: HKCU\\%s\nThe registry key may be opened by different application."), REGISTRY_ROOT_KEY_SITES), _T("Error"), MB_OK);
	}
	EndDialog(wID);
	return 0;
}

LRESULT 
CSitesDialog::OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}

void
CSitesDialog::ShowHelp()
{
	SHELLEXECUTEINFO shExeInfo = { sizeof(SHELLEXECUTEINFO), 0, 0, L"open", ALLOWED_SITES_HELP_LINK, 0, 0, SW_SHOWNORMAL, 0, 0, 0, 0, 0, 0, 0 };
	::ShellExecuteEx(&shExeInfo);
}

void
CSitesDialog::TestQuery()
{
	TCHAR buf[1024];
	m_QueryEdit.GetWindowTextW(buf, 1024);
	CString reason = m_SitesModel->TestWithReason(buf);
	::SetWindowText(GetDlgItem(IDC_QUERY_RESULT), (LPCTSTR)reason);
}

LRESULT 
CSitesDialog::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (LOWORD(wParam)){
		case ID_ADD_SITE: OnAdd(); break;
		case IDC_MOVE_UP: OnMoveUp(); break;
		case IDC_MOVE_DOWN: OnMoveDown(); break;
		case IDC_REMOVE: OnRemove(); break;
		case IDC_HELP2: ShowHelp(); break;
		case ID_TEST_QUERY: TestQuery(); break;
	}
	return S_OK;
}

void
CSitesDialog::OnAdd()
{
	TCHAR buf[1024];
	m_SiteEdit.GetWindowText(buf, 1024);
	m_SitesList->Add(buf);
	m_SiteEdit.SetSel(0, -1, TRUE);
}

LRESULT 
CSitesDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DlgResize_Init();

	m_SiteEdit.SubclassWindow(GetDlgItem(IDC_SITE_EDIT));
	m_SiteEdit.SetWindowTextW(m_DefaultSite);

	m_QueryEdit.SubclassWindow(GetDlgItem(IDC_QUERY_EDIT));
	m_QueryEdit.SetWindowTextW(m_DefaultSite);

	m_SitesModel = new CSitesModel();
	if (!m_SitesModel->Load(REGISTRY_ROOT_KEY_SITES))
	{
		::MessageBox(NULL, FS(_T("Unable to load sites settings from registry key: HKCU\\%s"), REGISTRY_ROOT_KEY_SITES), _T("Error"), MB_OK);
	}
	m_SitesList = new CSitesList(m_SitesModel);

	DWORD sitesStyle = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER;
	m_SitesList->Create(*this, rcDefault, SITES_LIST_WINDOW_NAME, sitesStyle, 0, ID_SD_LIST);

	CBitmap toolbar;
	toolbar.LoadBitmap(MAKEINTRESOURCE(IDB_STATES));
	if (toolbar.IsNull()) throw CXRefreshWindowsError(GetLastError());

	CBitmap mask;
	mask.LoadBitmap(MAKEINTRESOURCE(IDB_STATES));
	if (mask.IsNull()) throw CXRefreshWindowsError(GetLastError());

	CDPIHelper::ScaleBitmap(toolbar);
	CDPIHelper::ScaleBitmap(mask);

	int width = (int)CDPIHelper::ScaleX(16);
	int height = (int)CDPIHelper::ScaleY(16);
	m_ImageList.Create(width, height, ILC_COLOR24 | ILC_MASK, 3, 3);
	if (m_ImageList.IsNull()) throw CXRefreshWindowsError(GetLastError());
	if (m_ImageList.Add(toolbar, mask) == -1) throw CXRefreshWindowsError(GetLastError());

	m_Help.SetHyperLinkExtendedStyle(HLINK_COMMANDBUTTON);
	m_Help.SetToolTipText(_T("Visit xrefresh.com for manual page"));
	m_Help.SubclassWindow(GetDlgItem(IDC_HELP2));

	::SetWindowText(GetDlgItem(IDC_QUERY_RESULT), _T("> enter test query and hit [Test] button..."));

	InitList();
	Layout();
	UpdateButtons();

	bHandled = TRUE;
	return S_OK;
}

LRESULT
CSitesDialog::OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	WINDOWPOS * lpwndpos = (WINDOWPOS *)lParam;
	if (!(lpwndpos->flags & SWP_NOSIZE))
	{
		Layout();
	}
	bHandled = FALSE;
	return S_OK;
}

void
CSitesDialog::Layout()
{
	RECT  rc;
	GetClientRect(&rc);

	HDWP hdwp = BeginDeferWindowPos(15);
	hdwp = m_SiteEdit.DeferWindowPos(hdwp, NULL, 5, 5, rc.right-50, 20, SWP_NOZORDER);
	hdwp = ::DeferWindowPos(hdwp, GetDlgItem(ID_ADD_SITE), NULL, rc.right-40, 5, 35, 20, SWP_NOZORDER);
	hdwp = m_SitesList->DeferWindowPos(hdwp, NULL, 5, 35, rc.right-10, rc.bottom-100-QUERY_BOX_HEIGHT, SWP_NOZORDER);
	// buttons ok, cancel
	hdwp = ::DeferWindowPos(hdwp, GetDlgItem(IDOK), NULL, rc.right-130, rc.bottom-30, 60, 22, SWP_NOZORDER);
	hdwp = ::DeferWindowPos(hdwp, GetDlgItem(IDCANCEL), NULL, rc.right-65, rc.bottom-30, 60, 22, SWP_NOZORDER);
	// buttons for list
	hdwp = ::DeferWindowPos(hdwp, GetDlgItem(IDC_MOVE_UP), NULL, 5, rc.bottom-60-QUERY_BOX_HEIGHT, 70, 22, SWP_NOZORDER);
	hdwp = ::DeferWindowPos(hdwp, GetDlgItem(IDC_MOVE_DOWN), NULL, 80, rc.bottom-60-QUERY_BOX_HEIGHT, 70, 22, SWP_NOZORDER);
	hdwp = ::DeferWindowPos(hdwp, GetDlgItem(IDC_REMOVE), NULL, 155, rc.bottom-60-QUERY_BOX_HEIGHT, 70, 22, SWP_NOZORDER);
	// help
	WINDOWPLACEMENT wp;
	ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
	wp.length = sizeof(WINDOWPLACEMENT);
	::GetWindowPlacement(GetDlgItem(IDC_HELP1), &wp);
	hdwp = ::DeferWindowPos(hdwp, GetDlgItem(IDC_HELP1), NULL, rc.right-10-(wp.rcNormalPosition.right-wp.rcNormalPosition.left), rc.bottom-60-QUERY_BOX_HEIGHT, 70, 22, SWP_NOZORDER|SWP_NOSIZE);
	ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
	wp.length = sizeof(WINDOWPLACEMENT);
	::GetWindowPlacement(GetDlgItem(IDC_HELP2), &wp);
	hdwp = ::DeferWindowPos(hdwp, GetDlgItem(IDC_HELP2), NULL, rc.right-10-(wp.rcNormalPosition.right-wp.rcNormalPosition.left), rc.bottom-60-QUERY_BOX_HEIGHT+16, 70, 22, SWP_NOZORDER|SWP_NOSIZE);
	// query box
	int iQueryBoxY = rc.bottom-60-QUERY_BOX_HEIGHT+25;
	hdwp = ::DeferWindowPos(hdwp, GetDlgItem(IDC_QUERY_SEP), NULL, 5, iQueryBoxY+12, rc.right-10, 2, SWP_NOZORDER);
	hdwp = ::DeferWindowPos(hdwp, GetDlgItem(IDC_QUERY_TITLE), NULL, 30, iQueryBoxY+5, rc.right-10, 1, SWP_NOZORDER|SWP_NOSIZE);
	hdwp = m_QueryEdit.DeferWindowPos(hdwp, NULL, 5, iQueryBoxY+30, rc.right-50, 20, SWP_NOZORDER);
	hdwp = ::DeferWindowPos(hdwp, GetDlgItem(ID_TEST_QUERY), NULL, rc.right-40, iQueryBoxY+30, 35, 20, SWP_NOZORDER);
	hdwp = ::DeferWindowPos(hdwp, GetDlgItem(IDC_QUERY_RESULT), NULL, 10, iQueryBoxY+56, rc.right-20, 20, SWP_NOZORDER);
	EndDeferWindowPos(hdwp);

	// apply column size constraints
	int sizeX = rc.right - rc.left;
	if (m_LastSizeX!=-1)
	{
		int diff = sizeX - m_LastSizeX;
		int lastWidth = m_SitesList->GetColumnWidth(2);
		int newWidth = lastWidth + diff;
		if (newWidth>=20)
		{
			m_SitesList->SetColumnWidth(2, newWidth);
		}
	}
	m_LastSizeX = sizeX;
} 

void
CSitesDialog::InitList()
{
	m_SitesList->SetSmoothScroll(FALSE);
	m_SitesList->SetSingleSelect(TRUE);
	m_SitesList->SetDragDrop(FALSE);

	m_SitesList->SetImageList(m_ImageList);
}

LRESULT
CSitesDialog::OnListItemSelected(int wParam, LPNMHDR s, BOOL& bHandled)
{
	bHandled = TRUE;
	UpdateButtons();
	return S_OK;
}

LRESULT
CSitesDialog::OnListItemChanged(int wParam, LPNMHDR s, BOOL& bHandled)
{
	bHandled = TRUE;
	UpdateButtons();
	return S_OK;
}

LRESULT
CSitesDialog::OnListItemModified(int wParam, LPNMHDR s, BOOL& bHandled)
{
	CListNotify* n = (CListNotify*)s;
	bHandled = TRUE;
	m_SitesList->ToggleItemCheckState(n->m_nItem, n->m_nSubItem);
	return S_OK;
}

LRESULT
CSitesDialog::OnEditGetDlgCode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (lParam)
	{
		LPMSG lpmsg = (LPMSG)lParam;
		if (lpmsg->message == WM_KEYDOWN)
		{
			if (lpmsg->wParam == 13)
			{
				bHandled = TRUE;
				return DLGC_WANTMESSAGE;
			}
		}
	}
	bHandled = FALSE;
	return 0;
}

LRESULT
CSitesDialog::OnEditKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_KEYDOWN)
	{
		if (wParam == 13)
		{
			OnAdd();
			return S_OK;
		}
	}
	bHandled = FALSE;
	return 0;
}

LRESULT
CSitesDialog::OnSiteEditSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_SiteEdit.SetSel(0, -1, TRUE);
	bHandled = false;
	return 0;
}

LRESULT
CSitesDialog::OnQueryEditSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_QueryEdit.SetSel(0, -1, TRUE);
	bHandled = false;
	return 0;
}

void
CSitesDialog::UpdateButtons()
{
	int count = m_SitesList->GetItemCount();
	CListArray<int> ids;
	m_SitesList->GetSelectedItems(ids);
	if (!count || ids.GetSize()!=1) 
	{
		::EnableWindow(GetDlgItem(IDC_MOVE_UP), FALSE);
		::EnableWindow(GetDlgItem(IDC_MOVE_DOWN), FALSE);
		::EnableWindow(GetDlgItem(IDC_REMOVE), FALSE);
		return;
	}
	
	::EnableWindow(GetDlgItem(IDC_REMOVE), TRUE);
	::EnableWindow(GetDlgItem(IDC_MOVE_UP), ids[0]!=0);
	::EnableWindow(GetDlgItem(IDC_MOVE_DOWN), ids[0]!=count-1);
}

void
CSitesDialog::OnMoveUp()
{
	int count = m_SitesList->GetItemCount();
	CListArray<int> ids;
	m_SitesList->GetSelectedItems(ids);
	if (ids.GetSize()!=1) return;

	m_SitesList->Swap(ids[0], ids[0]-1);
	m_SitesList->SelectItem(ids[0]-1);
	m_SitesList->SetFocus();
	m_SitesList->Invalidate();
}

void
CSitesDialog::OnMoveDown()
{
	int count = m_SitesList->GetItemCount();
	CListArray<int> ids;
	m_SitesList->GetSelectedItems(ids);
	if (ids.GetSize()!=1) return;

	m_SitesList->Swap(ids[0], ids[0]+1);
	m_SitesList->SelectItem(ids[0]+1);
	m_SitesList->SetFocus();
	m_SitesList->Invalidate();
}

void
CSitesDialog::OnRemove()
{
	int count = m_SitesList->GetItemCount();
	CListArray<int> ids;
	m_SitesList->GetSelectedItems(ids);
	if (ids.GetSize()!=1) return;

	m_SitesList->Remove(ids[0]);
	if (ids[0]<count-1) m_SitesList->SelectItem(ids[0]); 
	else if (ids[0]!=0) m_SitesList->SelectItem(ids[0]-1);
	m_SitesList->SetFocus();
	m_SitesList->Invalidate();
}
