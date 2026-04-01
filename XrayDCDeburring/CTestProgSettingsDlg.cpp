// CTestProgSettingsDlg.cpp: 实现文件
//

#include "pch.h"
#include "XrayDCDeburring.h"
#include "CTestProgSettingsDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "CProductEditDlg.h"
#include "CProgramItemEditDlg.h"


// CTestProgSettingsDlg 对话框

IMPLEMENT_DYNAMIC(CTestProgSettingsDlg, CDialogEx)

CTestProgSettingsDlg::CTestProgSettingsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TESTPROG, pParent)
	, m_Edit_HightPower_Limit(_T(""))
	, m_Edit_LowpowerVoltage(_T(""))
{
	m_strCurrentProgram = L"";
}

CTestProgSettingsDlg::~CTestProgSettingsDlg()
{
}

void CTestProgSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_HIGHTPOWER_LIMIT, m_Edit_HightPower_Limit);
	DDX_Text(pDX, IDC_EDIT_LOWPOWER, m_Edit_LowpowerVoltage);
	DDX_Control(pDX, IDC_LIST_PROGRAMITEM, m_ListCtrl_ProgramItem);
	DDX_Control(pDX, IDC_TREE_PRODUCT, m_TreeCtrl_Product);
}


BEGIN_MESSAGE_MAP(CTestProgSettingsDlg, CDialogEx)
	ON_WM_INITMENUPOPUP()
	ON_NOTIFY(NM_RCLICK, IDC_TREE_PRODUCT, &CTestProgSettingsDlg::OnNMRClickTreeProduct)
	ON_COMMAND(ID_MENU_CREATEPROGRAM, &CTestProgSettingsDlg::OnMenuCreateprogram)
	ON_UPDATE_COMMAND_UI(ID_MENU_CREATEPROGRAM, &CTestProgSettingsDlg::OnUpdateMenuCreateprogram)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CTestProgSettingsDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, &CTestProgSettingsDlg::OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CTestProgSettingsDlg::OnBnClickedButtonDelete)
	ON_COMMAND(ID_MENU_MODIFYPROGRAM, &CTestProgSettingsDlg::OnMenuModifyprogram)
	ON_UPDATE_COMMAND_UI(ID_MENU_MODIFYPROGRAM, &CTestProgSettingsDlg::OnUpdateMenuModifyprogram)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PRODUCT, &CTestProgSettingsDlg::OnTvnSelchangedTreeProduct)
	ON_BN_CLICKED(IDOK, &CTestProgSettingsDlg::OnBnClickedOk)
	ON_COMMAND(ID_MENU_DELETPROGRAM, &CTestProgSettingsDlg::OnMenuDeletprogram)
	ON_UPDATE_COMMAND_UI(ID_MENU_DELETPROGRAM, &CTestProgSettingsDlg::OnUpdateMenuDeletprogram)
END_MESSAGE_MAP()


// CTestProgSettingsDlg 消息处理程序


BOOL CTestProgSettingsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// TODO:  在此添加额外的初始化
	m_TreeCtrl_Product.ModifyStyle(0, TVS_HASBUTTONS | TVS_HASLINES);
	InitTreeCtrl();

	m_ListCtrl_ProgramItem.SetExtendedStyle(m_ListCtrl_ProgramItem.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	CRect ClientRect;
	m_ListCtrl_ProgramItem.GetClientRect(&ClientRect);
	m_ListCtrl_ProgramItem.InsertColumn(0, _T("步号"), LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 20));
	m_ListCtrl_ProgramItem.InsertColumn(1, _T("电压"), LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 40));
	m_ListCtrl_ProgramItem.InsertColumn(2, _T("持续时间"), LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 40));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CTestProgSettingsDlg::InitTreeCtrl()
{
	CMainFrame* pMainframe = (CMainFrame*)AfxGetMainWnd();
	CBitmap bmp;
	if (!bmp.LoadBitmap(IDB_BITMAP_FILETREE))
	{
		TRACE(_T("无法加载位图:  %x\n"), IDB_BITMAP_FILETREE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= ILC_COLOR24;
	m_Imagelist.DeleteImageList();
	m_Imagelist.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_Imagelist.Add(&bmp, RGB(255, 0, 255));

	m_TreeCtrl_Product.SetImageList(&m_Imagelist, TVSIL_NORMAL);

	//程序选择初始化
	HTREEITEM hRootProgSel;
	CString str = L"程序选择";
	hRootProgSel = m_TreeCtrl_Product.InsertItem(str);
	m_TreeCtrl_Product.SetItemState(hRootProgSel, TVIS_BOLD, TVIS_BOLD);
	for (int i = 0; i < m_Vect_Program.size(); i++)
	{
		
			HTREEITEM hChilditem = m_TreeCtrl_Product.InsertItem(m_Vect_Program.at(i).strProgramName, 1, 1, hRootProgSel);
			m_TreeCtrl_Product.SetItemData(hChilditem, m_Vect_Program.at(i).nProgramID + ID_DETECTSET_PROGSTART);
			if (m_Vect_Program.at(i).strProgramName == m_strCurrentProgram)
			{
				m_TreeCtrl_Product.SetFocus();
				m_TreeCtrl_Product.SelectItem(hChilditem);
			}
		
	}

	m_TreeCtrl_Product.Expand(hRootProgSel, TVE_EXPAND);
}

void CTestProgSettingsDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CDialogEx::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// Determine if menu is popup in top-level menu and set m_pOther to 
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup). 
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
		state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup. 
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = this;
		// Child windows don't have menus--need to go to the top! 
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
				{
					// When popup is found, m_pParentMenu is containing menu. 
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // Menu separator or invalid cmd - ignore it. 

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// Possibly a popup menu, route to first item of that popup. 
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // First item of popup can't be routed to. 
			}
			state.DoUpdate(this, TRUE);   // Popups are never auto disabled. 
		}
		else
		{
			// Normal menu item. 
			// Auto enable/disable if frame window has m_bAutoMenuEnable 
			// set and command is _not_ a system command. 
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, FALSE);
		}

		// Adjust for menu deletions and additions. 
		UINT nCount = pPopupMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
	// TODO: 在此处添加消息处理程序代码
}


void CTestProgSettingsDlg::OnNMRClickTreeProduct(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码

	CMainFrame* pMainframe = (CMainFrame*)AfxGetMainWnd();

	*pResult = 0;

	CPoint pt;
	GetCursorPos(&pt);
	m_TreeCtrl_Product.ScreenToClient(&pt);
	HTREEITEM item = m_TreeCtrl_Product.HitTest(pt);
	DWORD NewItemData = 0;
	if (item != NULL)
	{
		NewItemData = m_TreeCtrl_Product.GetItemData(item);
	}
	if (NewItemData >= ID_DETECTSET_PROGSTART)
	{
		for (int i = 0; i < m_Vect_Program.size(); i++)
		{
			//切换到下一个程序
			if (m_Vect_Program.at(i).nProgramID == NewItemData - ID_DETECTSET_PROGSTART)
			{
				CString strNewProgName = _T("");
				strNewProgName.Format(_T("%s"), m_Vect_Program.at(i).strProgramName);
				if (m_strCurrentProgram != strNewProgName)
				{
					m_strCurrentProgram = strNewProgName;
					RefreshDetectInfoShow(i);
					CString strTemp = L"程序设定-";
					strTemp += strNewProgName;
					SetWindowText(strTemp);
				}
				break;
			}

		}
		CMenu menu, * popmenu;
		//装在菜单，资源ID为IDR_POPMENU——你编辑的菜单的ID
		menu.LoadMenu(IDR_MENU_POPUP_PROGRAM);
		popmenu = menu.GetSubMenu(0);
		//popmenu->RemoveMenu(5, MF_BYPOSITION);
		popmenu->RemoveMenu(0, MF_BYPOSITION);
		CPoint myPoint;
		GetCursorPos(&myPoint);  //鼠标位置
		popmenu->TrackPopupMenu(TPM_LEFTALIGN, myPoint.x, myPoint.y, this);
	}
	else
	{
		HTREEITEM hRootitem = m_TreeCtrl_Product.GetRootItem();
		if (hRootitem == item)
		{
			CMenu menu, * popmenu;
			//装在菜单，资源ID为IDR_POPMENU——你编辑的菜单的ID
			menu.LoadMenu(IDR_MENU_POPUP_PROGRAM);
			popmenu = menu.GetSubMenu(0);
			//popmenu->RemoveMenu(4, MF_BYPOSITION);
			//popmenu->RemoveMenu(3, MF_BYPOSITION);
			popmenu->RemoveMenu(2, MF_BYPOSITION);
			popmenu->RemoveMenu(1, MF_BYPOSITION);
			CPoint myPoint;
			GetCursorPos(&myPoint);  //鼠标位置
			popmenu->TrackPopupMenu(TPM_LEFTALIGN, myPoint.x, myPoint.y, this);
		}
	}

	*pResult = 0;
}


void CTestProgSettingsDlg::OnMenuCreateprogram()
{
	// TODO: 在此添加命令处理程序代码
	CProductEditDlg dlg;
	dlg.m_bModify = FALSE;
	dlg.m_Vect_Program = m_Vect_Program;
	if (dlg.DoModal() == IDOK)
	{
		m_Vect_Program = dlg.m_Vect_Program;
		RefreshTreeListProgsel();
		if (m_Vect_Program.size() > 0)
		{
			m_strCurrentProgram = m_Vect_Program.at(m_Vect_Program.size() - 1).strProgramName;
			m_Edit_HightPower_Limit.Format(L"%.*f", DOT_BITNUMBER,m_Vect_Program.at(m_Vect_Program.size() - 1).fHightVoltage_Limit);
			m_Edit_LowpowerVoltage.Format(L"%d", m_Vect_Program.at(m_Vect_Program.size() - 1).nLowVoltage) ;
			RefreshDetectInfoShow(m_Vect_Program.size() - 1);
			UpdateData(FALSE);
		}
		
	}
}


void CTestProgSettingsDlg::OnUpdateMenuCreateprogram(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable();
}

void CTestProgSettingsDlg::RefreshTreeListProgsel()
{
	//程序选择初始化
	m_TreeCtrl_Product.DeleteAllItems();
	HTREEITEM hRootProgSel;
	CString str = L"程序选择";
	hRootProgSel = m_TreeCtrl_Product.InsertItem(str);
	m_TreeCtrl_Product.SetItemState(hRootProgSel, TVIS_BOLD, TVIS_BOLD);
	for (int i = 0; i < m_Vect_Program.size(); i++)
	{

		HTREEITEM hChilditem = m_TreeCtrl_Product.InsertItem(m_Vect_Program.at(i).strProgramName, 1, 1, hRootProgSel);
		m_TreeCtrl_Product.SetItemData(hChilditem, m_Vect_Program.at(i).nProgramID + ID_DETECTSET_PROGSTART);

		if (m_Vect_Program.at(i).strProgramName == m_strCurrentProgram)
		{
			m_TreeCtrl_Product.SetFocus();
			m_TreeCtrl_Product.SelectItem(hChilditem);
		}

	}
	m_TreeCtrl_Product.Expand(hRootProgSel, TVE_EXPAND);
}

void CTestProgSettingsDlg::RefreshDetectInfoShow(int nProgIndex, int nProgitemIndex)
{
	if (m_Vect_Program.size() > nProgIndex)
	{
		m_Edit_HightPower_Limit.Format(L"%.*f", DOT_BITNUMBER, m_Vect_Program.at(nProgIndex).fHightVoltage_Limit);
		m_Edit_LowpowerVoltage.Format(L"%d",m_Vect_Program.at(nProgIndex).nLowVoltage);
		if (nProgitemIndex == -1)
		{
			m_ListCtrl_ProgramItem.DeleteAllItems();
			for (int i = 0; i < m_Vect_Program.at(nProgIndex).VectProgItem.size(); i++)
			{
				m_ListCtrl_ProgramItem.InsertItem(i, L"");
				CString strtemp = L"";
				strtemp.Format(L"%d", m_Vect_Program.at(nProgIndex).VectProgItem.at(i).nStep);
				m_ListCtrl_ProgramItem.SetItemText(i, 0, strtemp);
				strtemp.Format(L"%.*f", DOT_BITNUMBER, m_Vect_Program.at(nProgIndex).VectProgItem.at(i).nHightVotage);
				m_ListCtrl_ProgramItem.SetItemText(i, 1, strtemp);
				strtemp.Format(L"%d", m_Vect_Program.at(nProgIndex).VectProgItem.at(i).nKeepTime);
				m_ListCtrl_ProgramItem.SetItemText(i, 2, strtemp);
			}
		}
		else
		{
			if (m_ListCtrl_ProgramItem.GetItemCount() > nProgitemIndex)
			{
				CString strtemp = L"";
				strtemp.Format(L"%d", m_Vect_Program.at(nProgIndex).VectProgItem.at(nProgitemIndex).nStep);
				m_ListCtrl_ProgramItem.SetItemText(nProgitemIndex, 0, strtemp);
				strtemp.Format(L"%.*f", DOT_BITNUMBER, m_Vect_Program.at(nProgIndex).VectProgItem.at(nProgitemIndex).nHightVotage);
				m_ListCtrl_ProgramItem.SetItemText(nProgitemIndex, 1, strtemp);
				strtemp.Format(L"%d", m_Vect_Program.at(nProgIndex).VectProgItem.at(nProgitemIndex).nKeepTime);
				m_ListCtrl_ProgramItem.SetItemText(nProgitemIndex, 2, strtemp);
			}
			else {
				m_ListCtrl_ProgramItem.InsertItem(nProgitemIndex, L"");
				CString strtemp = L"";
				strtemp.Format(L"%d", m_Vect_Program.at(nProgIndex).VectProgItem.at(nProgitemIndex).nStep);
				m_ListCtrl_ProgramItem.SetItemText(nProgitemIndex, 0, strtemp);
				strtemp.Format(L"%.*f", DOT_BITNUMBER, m_Vect_Program.at(nProgIndex).VectProgItem.at(nProgitemIndex).nHightVotage);
				m_ListCtrl_ProgramItem.SetItemText(nProgitemIndex, 1, strtemp);
				strtemp.Format(L"%d", m_Vect_Program.at(nProgIndex).VectProgItem.at(nProgitemIndex).nKeepTime);
				m_ListCtrl_ProgramItem.SetItemText(nProgitemIndex, 2, strtemp);
			}
		}
		m_ListCtrl_ProgramItem.DeleteAllItems();
		for (int i = 0; i < m_Vect_Program.at(nProgIndex).VectProgItem.size(); i++)
		{
			m_ListCtrl_ProgramItem.InsertItem(i,L"");
			CString strtemp = L"";
			strtemp.Format(L"%d", m_Vect_Program.at(nProgIndex).VectProgItem.at(i).nStep);
			m_ListCtrl_ProgramItem.SetItemText(i, 0, strtemp);
			strtemp.Format(L"%.*f", DOT_BITNUMBER, m_Vect_Program.at(nProgIndex).VectProgItem.at(i).nHightVotage);
			m_ListCtrl_ProgramItem.SetItemText(i, 1, strtemp);
			strtemp.Format(L"%d", m_Vect_Program.at(nProgIndex).VectProgItem.at(i).nKeepTime);
			m_ListCtrl_ProgramItem.SetItemText(i, 2, strtemp);
		}
	}
	else
	{
		m_ListCtrl_ProgramItem.DeleteAllItems();
	}
}

void CTestProgSettingsDlg::OnBnClickedButtonAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	int nprogindex = -1;
	for (int i = 0; i < m_Vect_Program.size(); i++)
	{
		if (m_Vect_Program.at(i).strProgramName == m_strCurrentProgram)
		{
			nprogindex = i;
			break;
		}
	}
	if (nprogindex != -1)
	{
		CProgramItemEditDlg dlg;
		dlg.m_VectProgItem = m_Vect_Program.at(nprogindex).VectProgItem;
		dlg.m_bModify = FALSE;
		if (dlg.DoModal() == IDOK)
		{
			m_Vect_Program.at(nprogindex).VectProgItem = dlg.m_VectProgItem;
			RefreshDetectInfoShow(nprogindex, m_Vect_Program.at(nprogindex).VectProgItem.size() - 1);
		}
	}
	else
	{
		AfxMessageBox(L"请选择一个程序");
	}
}


void CTestProgSettingsDlg::OnBnClickedButtonModify()
{
	// TODO: 在此添加控件通知处理程序代码
	int nprogindex = -1;
	for (int i = 0; i < m_Vect_Program.size(); i++)
	{
		if (m_Vect_Program.at(i).strProgramName == m_strCurrentProgram)
		{
			nprogindex = i;
			break;
		}
	}
	if (nprogindex != -1)
	{
		int nItemIndex = m_ListCtrl_ProgramItem.GetSelectionMark();
		if (nItemIndex != -1)
		{
			CProgramItemEditDlg dlg;
			dlg.m_VectProgItem = m_Vect_Program.at(nprogindex).VectProgItem;
			dlg.m_bModify = TRUE;
			dlg.m_nModifyIndex = nItemIndex;
			if (dlg.DoModal() == IDOK)
			{
				m_Vect_Program.at(nprogindex).VectProgItem = dlg.m_VectProgItem;
				RefreshDetectInfoShow(nprogindex, nItemIndex);
			}

		}
		else
		{
			AfxMessageBox(L"请选择一个程序子项");
		}
	}
	else
	{
		AfxMessageBox(L"请选择一个程序");
	}
}


void CTestProgSettingsDlg::OnBnClickedButtonDelete()
{
	// TODO: 在此添加控件通知处理程序代码
	int nprogindex = -1;
	for (int i = 0; i < m_Vect_Program.size(); i++)
	{
		if (m_Vect_Program.at(i).strProgramName == m_strCurrentProgram)
		{
			nprogindex = i;
			break;
		}
	}
	if (nprogindex != -1)
	{
		int nItemIndex = m_ListCtrl_ProgramItem.GetSelectionMark();
		if (nItemIndex != -1)
		{
			if (AfxMessageBox(L"是否要删除该选择项目？", MB_YESNO) == IDYES)
			{
				m_Vect_Program.at(nprogindex).VectProgItem.erase(m_Vect_Program.at(nprogindex).VectProgItem.begin() + nItemIndex);
				m_ListCtrl_ProgramItem.DeleteItem(nItemIndex);
			}
		}
		else
		{
			AfxMessageBox(L"请选择一个程序子项");
		}
	}
	else
	{
	AfxMessageBox(L"请选择一个程序");
	}
}


void CTestProgSettingsDlg::OnMenuModifyprogram()
{
	// TODO: 在此添加命令处理程序代码
	CProductEditDlg dlg;
	dlg.m_bModify = TRUE;
	dlg.m_Vect_Program = m_Vect_Program;

	for (int i = 0; i < m_Vect_Program.size(); i++)
	{
		if (m_Vect_Program.at(i).strProgramName == m_strCurrentProgram)
		{
			dlg.m_nModifyIndex = i;
			break;
		}
	}

	if (dlg.DoModal() == IDOK)
	{
		m_Vect_Program = dlg.m_Vect_Program;
		RefreshTreeListProgsel();
		if (m_Vect_Program.size() > 0)
		{
			m_strCurrentProgram = m_Vect_Program.at(dlg.m_nModifyIndex).strProgramName;
			m_Edit_HightPower_Limit.Format(L"%.*f", DOT_BITNUMBER, m_Vect_Program.at(dlg.m_nModifyIndex).fHightVoltage_Limit);
			m_Edit_LowpowerVoltage.Format(L"%d", m_Vect_Program.at(dlg.m_nModifyIndex).nLowVoltage);
			UpdateData(FALSE);
		}
	}
}


void CTestProgSettingsDlg::OnUpdateMenuModifyprogram(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable();
}



void CTestProgSettingsDlg::OnTvnSelchangedTreeProduct(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	DWORD NewItemData = 0;
	if (pNMTreeView->itemNew.hItem)
	{
		NewItemData = m_TreeCtrl_Product.GetItemData(pNMTreeView->itemNew.hItem);
	}
	if (NewItemData >= ID_DETECTSET_PROGSTART)
	{
		for (int i = 0; i < m_Vect_Program.size(); i++)
		{
			//切换到下一个程序
			if (m_Vect_Program.at(i).nProgramID == NewItemData - ID_DETECTSET_PROGSTART)
			{
				CString strNewProgName = _T("");
				strNewProgName.Format(_T("%s"), m_Vect_Program.at(i).strProgramName);
				if (m_strCurrentProgram != strNewProgName)
				{
					m_strCurrentProgram = strNewProgName;
					RefreshDetectInfoShow(i);
					m_Edit_HightPower_Limit.Format(L"%.*f", DOT_BITNUMBER, m_Vect_Program.at(i).fHightVoltage_Limit);
					m_Edit_LowpowerVoltage.Format(L"%d", m_Vect_Program.at(i).nLowVoltage);
					CString strTemp = L"程序设定-";
					strTemp += strNewProgName;
					SetWindowText(strTemp);
					UpdateData(FALSE);
				}
			}

		}
	}

}


void CTestProgSettingsDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	CDialogEx::OnOK();
}


void CTestProgSettingsDlg::OnMenuDeletprogram()
{
	// TODO: 在此添加命令处理程序代码
	for (int i = 0; i < m_Vect_Program.size(); i++)
	{
		if (m_Vect_Program.at(i).strProgramName == m_strCurrentProgram)
		{
			if (AfxMessageBox(L"是否删除该程序?", MB_YESNO) == IDYES)
			{
				m_Vect_Program.erase(m_Vect_Program.begin() + i);
				RefreshTreeListProgsel();
				if (m_Vect_Program.size() > 0)
				{
					m_strCurrentProgram = m_Vect_Program.at(0).strProgramName;
					m_Edit_HightPower_Limit.Format(L"%.*f", DOT_BITNUMBER, m_Vect_Program.at(0).fHightVoltage_Limit);
					m_Edit_LowpowerVoltage.Format(L"%d", m_Vect_Program.at(0).nLowVoltage);
					RefreshDetectInfoShow(0);
					UpdateData(FALSE);
				}
			}
			break;
		}
	}

}


void CTestProgSettingsDlg::OnUpdateMenuDeletprogram(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable();
}
