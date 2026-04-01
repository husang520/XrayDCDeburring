// CFireRecordDlg.cpp: 实现文件
//

#include "pch.h"
#include "XrayDCDeburring.h"
#include "CFireRecordDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "XrayDCDeburringView.h"
// CFireRecordDlg 对话框

IMPLEMENT_DYNAMIC(CFireRecordDlg, CDialogEx)

CFireRecordDlg::CFireRecordDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FIRERECORD, pParent)
	, m_Edit_TubSn(_T(""))
{

}

CFireRecordDlg::~CFireRecordDlg()
{
}

void CFireRecordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_DataTime_Start);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_DataTime_End);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START, m_DataTime_Start);
	DDX_Control(pDX, IDC_LIST_INFORMATION, m_List_FireRecord);
	DDX_Text(pDX, IDC_EDIT_TUBESN, m_Edit_TubSn);
	DDX_Control(pDX, IDC_CHECK_TIME, m_Check_TIME);
	DDX_Control(pDX, IDC_CHECK_TUBESN, m_Check_TubeSn);
}


BEGIN_MESSAGE_MAP(CFireRecordDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &CFireRecordDlg::OnBnClickedButtonQuery)
END_MESSAGE_MAP()


// CFireRecordDlg 消息处理程序


void CFireRecordDlg::OnBnClickedButtonQuery()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!UpdateData())
	{
		return;
	}
	CTime timeStart,timeEnd;
	m_DataTime_Start.GetTime(timeStart);
	m_DataTime_End.GetTime(timeEnd);
	CString strTimeStart = timeStart.Format(L"%Y-%m-%d %H:%M:%S");
	CString strTimeEnd = timeEnd.Format(L"%Y-%m-%d %H:%M:%S");
	CString strSql = L"",strCondition1 = L"",strCondition2= L"";
	
	if (m_Check_TIME.GetCheck())
	{
		strCondition1.Format(L" where 日期 between '%s' and '%s' ", strTimeStart, strTimeEnd);
	}
	if (m_Check_TubeSn.GetCheck())
	{
		if (strCondition1 == L"")
		{
			strCondition2.Format(L" where 光管SN like '%%%s' ", m_Edit_TubSn);
		}
		else
		{
			strCondition2.Format(L" and 光管SN like '%%%s' ", m_Edit_TubSn);
		}
	}
	if (strCondition1 != L"" || strCondition2 != L"")
	{
		strSql.Format(L"select * from 打火记录表 %s%s order by ID desc", strCondition1, strCondition2);
	}
	SLOG1(strSql);

	UpdateListFire(strSql);
}


BOOL CFireRecordDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//CMainFrame* pMainframe = (CMainFrame*)AfxGetMainWnd();

	//CXrayDCDeburringView* pView = (CXrayDCDeburringView*)pMainframe->GetActiveView();
	//CRect ViewRect;
	//pView->GetClientRect(&ViewRect);
	//pView->ClientToScreen(&ViewRect);
	//ViewRect.bottom += 300;
	//MoveWindow(ViewRect);
	CRect ClientRect;
	m_List_FireRecord.SetExtendedStyle(m_List_FireRecord.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_List_FireRecord.GetClientRect(&ClientRect);
	m_List_FireRecord.InsertColumn(0, L"序号", LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 4));
	m_List_FireRecord.InsertColumn(1, L"产品", LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 8));
	m_List_FireRecord.InsertColumn(2, L"光管SN", LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 10));
	m_List_FireRecord.InsertColumn(3, L"时间", LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 10));
	m_List_FireRecord.InsertColumn(4, L"步骤", LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 4));
	m_List_FireRecord.InsertColumn(5, L"设置电压", LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 8));
	m_List_FireRecord.InsertColumn(6, L"打火电压", LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 8));
	m_List_FireRecord.InsertColumn(7, L"打火时刻", LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 8));
	m_List_FireRecord.InsertColumn(8, L"打火电流通道1", LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 10));
	m_List_FireRecord.InsertColumn(9, L"打火电流通道2", LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 10));
	m_List_FireRecord.InsertColumn(10, L"打火电流通道3", LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 10));
	m_List_FireRecord.InsertColumn(11, L"打火电流通道4", LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 10));
	m_DataTime_End.SetFormat(L"yyyy-MM-dd HH:mm:ss");
	m_DataTime_Start.SetFormat(L"yyyy-MM-dd HH:mm:ss");
	CTime timeCurrent = CTime::GetCurrentTime();
	CTime timeStart(timeCurrent.GetYear(), timeCurrent.GetMonth(), timeCurrent.GetDay(), 0, 0, 0);
	m_DataTime_Start.SetTime(&timeStart);
	UpdateListFire();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
void CFireRecordDlg::UpdateListFire(CString strSql)
{
	CMainFrame* pMianframe = (CMainFrame*)AfxGetMainWnd();
	VECT_FIRERECORD VectFireRecord;
	if (strSql == L"")
	{

		pMianframe->m_dbControl.ReadDataBase(COMMAND_TYPE_FIRERECORD_LOAD, NULL, &VectFireRecord, NULL);
	}
	else
	{
		pMianframe->m_dbControl.ReadDataBase(COMMAND_TYPE_FIRERECORD_LOAD, &strSql, &VectFireRecord, NULL);
	}
	m_List_FireRecord.DeleteAllItems();
	for (int i = 0; i < VectFireRecord.size(); i++)
	{
		m_List_FireRecord.InsertItem(i,L"");
		CString strtemp = L"";
		strtemp.Format(L"%d", i + 1);
		m_List_FireRecord.SetItemText(i, 0, strtemp);
		m_List_FireRecord.SetItemText(i, 1, VectFireRecord.at(i).strTubeType);
		m_List_FireRecord.SetItemText(i, 2, VectFireRecord.at(i).strTubeSn);
		m_List_FireRecord.SetItemText(i, 3, VectFireRecord.at(i).strTime);
		strtemp.Format(L"%d", VectFireRecord.at(i).nStep);
		m_List_FireRecord.SetItemText(i, 4, strtemp);
		m_List_FireRecord.SetItemText(i, 5, VectFireRecord.at(i).strVoltage);
		m_List_FireRecord.SetItemText(i, 6, VectFireRecord.at(i).strVoltageFire);
		m_List_FireRecord.SetItemText(i, 7, VectFireRecord.at(i).strFileMoment);
		m_List_FireRecord.SetItemText(i, 8, VectFireRecord.at(i).strFireCurrentCH[0]);
		m_List_FireRecord.SetItemText(i, 9, VectFireRecord.at(i).strFireCurrentCH[1]);
		m_List_FireRecord.SetItemText(i, 10, VectFireRecord.at(i).strFireCurrentCH[2]);
		m_List_FireRecord.SetItemText(i, 11, VectFireRecord.at(i).strFireCurrentCH[3]);
	}
}