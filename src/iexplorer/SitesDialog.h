#pragma once

#include "resource.h" // main symbols
#include <atlhost.h>
#include "ListCtrl.h"
#include "SitesModel.h"

#define SITE_EDIT_ID                              1
#define QUERY_EDIT_ID                             2

//////////////////////////////////////////////////////////////////////////
// CSitesList
class CSitesList : public CListImpl<CSitesList> {
	typedef CListImpl<CSitesList> super;
public:
	DECLARE_WND_CLASS(SITES_LIST_CLASS_NAME)

	CSitesList(CSitesModel* model);
	virtual ~CSitesList();

	BEGIN_MSG_MAP_EX(CSitesList)
		CHAIN_MSG_MAP(super)
	END_MSG_MAP_EX()

	BOOL                                          Initialise();
	int                                           GetItemCount(); // required by CListImpl
	CSiteRecord&                                  GetSite(int nItem);
	CString                                       GetItemText(int nItem, int nSubItem); // required by CListImpl
	int                                           GetItemImage(int nItem, int nSubItem); // overrides CListImpl::GetItemImage

	void                                          Add(TCHAR* url);

	void                                          ReverseItems() {}
	void                                          SortItems( int nColumn, BOOL bAscending ) { m_nSortColumn = NULL_COLUMN; }
	BOOL                                          SetItemText( int nItem, int nSubItem, LPCTSTR lpszText );
	void                                          ToggleItemCheckState(int nItem, int nSubItem);

	void                                          Swap(int a, int b);
	void                                          Remove(int i);

	BOOL                                          HitTest2(CPoint point, int& nItem, int& nSubItem, CPoint& rpos);
	virtual BOOL                                  EditItem(int nItem, int nSubItem = NULL_SUBITEM, CPoint point = CPoint());
	virtual BOOL                                  GetItemColours(int nItem, int nSubItem, COLORREF& rgbBackground, COLORREF& rgbText);

	CSitesModel*                                  m_Model;
	CListArray<CString>                           m_ActionList;
};

#define ID_SD_LIST                                20

//////////////////////////////////////////////////////////////////////////
// CSitesDialog
class CSitesDialog: public CDialogImpl<CSitesDialog>, public CDialogResize<CSitesDialog> {
public:
	CSitesDialog(CString defaultSite);
	~CSitesDialog();

	enum { IDD = IDD_SITESDIALOG };

	BEGIN_DLGRESIZE_MAP(CSitesDialog)
	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CSitesDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, OnWindowPosChanged)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		NOTIFY_HANDLER(ID_SD_LIST, LCN_SELECTED, OnListItemSelected)
		NOTIFY_HANDLER(ID_SD_LIST, LCN_CHANGED, OnListItemChanged)
		NOTIFY_HANDLER(ID_SD_LIST, LCN_MODIFIED, OnListItemModified)
		CHAIN_MSG_MAP(CDialogResize<CSitesDialog>)
	ALT_MSG_MAP(SITE_EDIT_ID)
//		MESSAGE_HANDLER(WM_GETDLGCODE, OnEditGetDlgCode)
//		MESSAGE_HANDLER(WM_KEYDOWN, OnEditKeyDown)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSiteEditSetFocus)
	ALT_MSG_MAP(QUERY_EDIT_ID)
		MESSAGE_HANDLER(WM_SETFOCUS, OnQueryEditSetFocus)
	END_MSG_MAP()

	LRESULT                                       OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT                                       OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT                                       OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT                                       OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT                                       OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT                                       OnListItemSelected(int wParam, LPNMHDR s, BOOL& bHandled);
	LRESULT                                       OnListItemChanged(int wParam, LPNMHDR s, BOOL& bHandled);
	LRESULT                                       OnListItemModified(int wParam, LPNMHDR s, BOOL& bHandled);

	LRESULT                                       OnEditGetDlgCode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT                                       OnEditKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT                                       OnSiteEditSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT                                       OnQueryEditSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void                                          Layout();
	void                                          InitList();
	void                                          UpdateButtons();
	void                                          ShowHelp();
	void                                          TestQuery();

	void                                          OnAdd();
	void                                          OnMoveUp();
	void                                          OnMoveDown();
	void                                          OnRemove();

private:
	CSitesModel*                                  m_SitesModel;
	CSitesList*                                   m_SitesList;
	CImageList                                    m_ImageList;
	int                                           m_LastSizeX;
	CContainedWindowT<CEdit>                      m_SiteEdit;
	CContainedWindowT<CEdit>                      m_QueryEdit;
	CHyperLink                                    m_Help;
	CString                                       m_DefaultSite;
};
