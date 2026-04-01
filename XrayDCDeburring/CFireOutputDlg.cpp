// CFireOutputDlg.cpp: 实现文件
//

#include "pch.h"
#include "XrayDCDeburring.h"
#include "CFireOutputDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"

// CFireOutputDlg 对话框

IMPLEMENT_DYNAMIC(CFireOutputDlg, CDialogEx)

CFireOutputDlg::CFireOutputDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FIRE_OUTPUT, pParent)
{

}

CFireOutputDlg::~CFireOutputDlg()
{
}

void CFireOutputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FIREOUTPUT, m_ListCtrl_FireOutput);
	DDX_Control(pDX, IDC_BUTTON_REPORT, m_Button_Report);
}


BEGIN_MESSAGE_MAP(CFireOutputDlg, CDialogEx)
	ON_WM_SIZE()
	ON_MESSAGE(WM_UPDATE_FIRERECORD, &CFireOutputDlg::OnUpdateFireRecord)
	
	ON_BN_CLICKED(IDC_BUTTON_REPORT, &CFireOutputDlg::OnBnClickedButtonReport)
END_MESSAGE_MAP()


// CFireOutputDlg 消息处理程序


BOOL CFireOutputDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ListCtrl_FireOutput.SetExtendedStyle(m_ListCtrl_FireOutput.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	CRect rClientRect,ListRect,ButtonRect;
	GetClientRect(rClientRect);
	ListRect = rClientRect;

	ButtonRect.top = rClientRect.top;
	ButtonRect.bottom = rClientRect.top + 25;
	ButtonRect.left = rClientRect.right - 110;
	ButtonRect.right = rClientRect.right - 10;
	m_Button_Report.MoveWindow(ButtonRect);
	ListRect.top = rClientRect.top + 25;
	m_ListCtrl_FireOutput.MoveWindow(ListRect);
	CRect ClientRect;
	m_ListCtrl_FireOutput.GetClientRect(&ClientRect);
	m_ListCtrl_FireOutput.InsertColumn(0, _T("步骤"), LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 4));
	m_ListCtrl_FireOutput.InsertColumn(1, _T("记录时间"), LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 12));
	m_ListCtrl_FireOutput.InsertColumn(2, _T("输出电压"), LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 12));
	m_ListCtrl_FireOutput.InsertColumn(3, _T("打火电压"), LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 12));
	m_ListCtrl_FireOutput.InsertColumn(4, _T("打火时刻(S)"), LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 12));
	m_ListCtrl_FireOutput.InsertColumn(5, _T("CH1打火电流"), LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 12));
	m_ListCtrl_FireOutput.InsertColumn(6, _T("CH2打火电流"), LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 12));
	m_ListCtrl_FireOutput.InsertColumn(7, _T("CH3打火电流"), LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 12));
	m_ListCtrl_FireOutput.InsertColumn(8, _T("CH4打火电流"), LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 12));


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CFireOutputDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (m_ListCtrl_FireOutput.GetSafeHwnd() != NULL)
	{
		CRect rClientRect, ListRect, ButtonRect;
		GetClientRect(rClientRect);
		ListRect = rClientRect;

		ButtonRect.top = rClientRect.top;
		ButtonRect.bottom = rClientRect.top + 25;
		ButtonRect.left = rClientRect.right - 110;
		ButtonRect.right = rClientRect.right - 10;
		m_Button_Report.MoveWindow(ButtonRect);
		ListRect.top = rClientRect.top + 25;
		m_ListCtrl_FireOutput.MoveWindow(ListRect);

		CRect ClientRect;
		m_ListCtrl_FireOutput.GetClientRect(&ClientRect);
		m_ListCtrl_FireOutput.SetColumnWidth(0, int(ClientRect.Width() / 100.0 * 4));
		m_ListCtrl_FireOutput.SetColumnWidth(1, int(ClientRect.Width() / 100.0 * 12));
		m_ListCtrl_FireOutput.SetColumnWidth(2, int(ClientRect.Width() / 100.0 * 12));
		m_ListCtrl_FireOutput.SetColumnWidth(3, int(ClientRect.Width() / 100.0 * 12));
		m_ListCtrl_FireOutput.SetColumnWidth(4, int(ClientRect.Width() / 100.0 * 12));
		m_ListCtrl_FireOutput.SetColumnWidth(5, int(ClientRect.Width() / 100.0 * 12));
		m_ListCtrl_FireOutput.SetColumnWidth(6, int(ClientRect.Width() / 100.0 * 12));
		m_ListCtrl_FireOutput.SetColumnWidth(7, int(ClientRect.Width() / 100.0 * 12));
		m_ListCtrl_FireOutput.SetColumnWidth(8, int(ClientRect.Width() / 100.0 * 12));
	}
	// TODO: 在此处添加消息处理程序代码
}

LRESULT CFireOutputDlg::OnUpdateFireRecord(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 1)
	{//ADD
		SLOG1(L"Add Record");
		CMainFrame* pMainframe = (CMainFrame*)AfxGetMainWnd();
		int nCount = m_ListCtrl_FireOutput.GetItemCount();
		for (int i = nCount; i < pMainframe->m_Vect_FireRecord.size(); i++)
		{
			CString strtemp = L"";
			m_ListCtrl_FireOutput.InsertItem(i, L"");
			strtemp.Format(L"%d", pMainframe->m_Vect_FireRecord.at(i).nStep);
			m_ListCtrl_FireOutput.SetItemText(i, 0,strtemp);

			m_ListCtrl_FireOutput.SetItemText(i, 1, pMainframe->m_Vect_FireRecord.at(i).strTime);
			m_ListCtrl_FireOutput.SetItemText(i, 2, pMainframe->m_Vect_FireRecord.at(i).strVoltage);
			m_ListCtrl_FireOutput.SetItemText(i, 3, pMainframe->m_Vect_FireRecord.at(i).strVoltageFire);
			m_ListCtrl_FireOutput.SetItemText(i, 4, pMainframe->m_Vect_FireRecord.at(i).strFileMoment);
			m_ListCtrl_FireOutput.SetItemText(i, 5, pMainframe->m_Vect_FireRecord.at(i).strFireCurrentCH[0]);
			m_ListCtrl_FireOutput.SetItemText(i, 6, pMainframe->m_Vect_FireRecord.at(i).strFireCurrentCH[1]);
			m_ListCtrl_FireOutput.SetItemText(i, 7, pMainframe->m_Vect_FireRecord.at(i).strFireCurrentCH[2]);
			m_ListCtrl_FireOutput.SetItemText(i, 8, pMainframe->m_Vect_FireRecord.at(i).strFireCurrentCH[3]);
		}
	}
	else if (wParam == 2)
	{//delete ALL
		m_ListCtrl_FireOutput.DeleteAllItems();
	}
	return 0;
}

void CFireOutputDlg::OnBnClickedButtonReport()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame* pMainframe = (CMainFrame*)AfxGetMainWnd();
	pMainframe->SaveTestRecode();
}
