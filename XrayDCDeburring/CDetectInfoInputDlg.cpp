// CDetectInfoInputDlg.cpp: 实现文件
//

#include "pch.h"
#include "XrayDCDeburring.h"
#include "CDetectInfoInputDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "XrayDCDeburringView.h"
// CDetectInfoInputDlg 对话框

IMPLEMENT_DYNAMIC(CDetectInfoInputDlg, CDialogEx)

CDetectInfoInputDlg::CDetectInfoInputDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DETECTINFO_INPUT, pParent)
	, m_Edit_Goback_Steps(0)
	, m_nRepeatTimes(1)
	, m_Edit_TubeSN(_T(""))
	, m_Edit_StartStep(1)
{
}

CDetectInfoInputDlg::~CDetectInfoInputDlg()
{
	//m_BkBrush.DeleteObject();
}

void CDetectInfoInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PROGSEL, m_ComboBox_ProgSel);
	DDX_Text(pDX, IDC_EDIT_GOBACK_STEPS, m_Edit_Goback_Steps);
	DDX_Text(pDX, IDC_EDIT_REPEATTIMES, m_nRepeatTimes);
	DDX_Control(pDX, IDC_LIST_PROGITEM, m_ListCtrl_ProgramItems);
	DDX_Control(pDX, IDC_SPIN_GOBACKSTEPS, m_Spin_GobackSteps);
	DDX_Control(pDX, IDC_SPIN_REPEATTIMES, m_SpinRepeatTimes);
	DDX_Control(pDX, IDC_STATIC_CURRENTSTEP, m_Static_CurrentStep);
	DDX_Control(pDX, IDC_STATIC_CURVE_VOLTAGE, m_Static_VoltageCurve);
	DDX_Control(pDX, IDC_STATIC_FINISHTIMES, m_Static_FinishTimes);
	DDX_Control(pDX, IDC_STATIC_HIGHTVOLTAGE, m_Static_HightVoltage);
	DDX_Control(pDX, IDC_STATIC_KEEPTIME_REAL, m_Static_KeepTime_Real);
	DDX_Control(pDX, IDC_STATIC_LOWVOLTAGE, m_Static_LowVoltage);
	DDX_Control(pDX, IDC_COMBO_INCREASESPEED, m_ComboBox_IncreaseSpeed);
	DDX_Text(pDX, IDC_EDIT_TUBE_SN, m_Edit_TubeSN);
	DDX_Text(pDX, IDC_EDIT_START_STEP, m_Edit_StartStep);
	DDX_Control(pDX, IDC_SPIN_START_STEP, m_SpinButtonCtrl_StartStep);
}


BEGIN_MESSAGE_MAP(CDetectInfoInputDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_START, &CDetectInfoInputDlg::OnBnClickedButtonStart)
	ON_CBN_SELCHANGE(IDC_COMBO_PROGSEL, &CDetectInfoInputDlg::OnCbnSelchangeComboProgsel)
//	ON_WM_TIMECHANGE()
ON_WM_TIMER()
ON_BN_CLICKED(IDC_BUTTON_CONTINUE, &CDetectInfoInputDlg::OnBnClickedButtonContinue)
ON_BN_CLICKED(IDC_BUTTON_STOP, &CDetectInfoInputDlg::OnBnClickedButtonStop)
END_MESSAGE_MAP()


// CDetectInfoInputDlg 消息处理程序


BOOL CDetectInfoInputDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect clientRect,modeRect;
	//m_Static_DetectMode.GetWindowRect(modeRect);
	GetClientRect(clientRect);
	clientRect.bottom = modeRect.Height();
	m_SpinRepeatTimes.SetRange(1, 100);
	m_Spin_GobackSteps.SetRange(0, 100);
	m_SpinButtonCtrl_StartStep.SetRange(1, 100);
	m_Edit_Goback_Steps = 1;
	m_Edit_StartStep = 1;
	m_ComboBox_IncreaseSpeed.SetCurSel(0);
	InitProgramSel();
	m_Static_VoltageCurve.Invalidate(FALSE);
	m_ListCtrl_ProgramItems.SetExtendedStyle(m_ListCtrl_ProgramItems.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	CRect ClientRect;
	m_ListCtrl_ProgramItems.GetClientRect(&ClientRect);
	m_ListCtrl_ProgramItems.InsertColumn(0, _T("步骤"), LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 20));
	m_ListCtrl_ProgramItems.InsertColumn(1, _T("电压"), LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 40));
	m_ListCtrl_ProgramItems.InsertColumn(2, _T("持续时间"), LVCFMT_LEFT, int(ClientRect.Width() / 100.0 * 40));
	UpdateProgramItem();
	SetTimer(1, 500, NULL);
	UpdateData(FALSE);
	//m_Static_DetectMode.MoveWindow(clientRect);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CDetectInfoInputDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		if ((pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))
		{
			return FALSE;
		}
		//return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}




void CDetectInfoInputDlg::SetWorkshift(CString strWorkshift)
{
	//for (int i = 0; i < m_ComboBox_Workshift.GetCount(); i++)
	//{
	//	CString strWorkshifttemp = L"";
	//	m_ComboBox_Workshift.GetLBText(i, strWorkshifttemp);
	//	if (strWorkshifttemp == strWorkshift)
	//	{
	//		m_ComboBox_Workshift.SetCurSel(i);
	//		break;
	//	}
	//}
}
void CDetectInfoInputDlg::SetMachineNum(CString strMachineNum)
{
	//for (int i = 0; i < m_ComboBox_MachineNum.GetCount(); i++)
	//{
	//	CString strMachineNumtemp = L"";
	//	m_ComboBox_MachineNum.GetLBText(i, strMachineNumtemp);
	//	if (strMachineNumtemp == strMachineNum)
	//	{
	//		m_ComboBox_MachineNum.SetCurSel(i);
	//		break;
	//	}
	//}
}
void CDetectInfoInputDlg::SetMeasureType(CString strMeasureType)
{
	//for (int i = 0; i < m_ComboBox_MeasureType.GetCount(); i++)
	//{
	//	CString strMeasureTypetemp = L"";
	//	m_ComboBox_MeasureType.GetLBText(i, strMeasureTypetemp);
	//	if (strMeasureTypetemp == strMeasureType)
	//	{
	//		m_ComboBox_MeasureType.SetCurSel(i);
	//		break;
	//	}
	//}
}

void CDetectInfoInputDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	//if (m_Static_DetectMode.GetSafeHwnd() == NULL)
	//{
	//	return;
	//}
	//CRect clientRect, modeRect;
	//m_Static_DetectMode.GetWindowRect(modeRect);
	//GetClientRect(clientRect);
	//clientRect.bottom = modeRect.Height();
	//m_Static_DetectMode.MoveWindow(clientRect);
	// TODO: 在此处添加消息处理程序代码
}

void CDetectInfoInputDlg::InitProgramSel()
{
	CMainFrame* pMainframe = (CMainFrame*)AfxGetMainWnd();
	m_ComboBox_ProgSel.ResetContent();
	int nSelIndex = -1;
	if (pMainframe->m_Vect_Program.size() <= 0)
	{
		return;
	}
	for (int i = 0; i < pMainframe->m_Vect_Program.size(); i++)
	{
		m_ComboBox_ProgSel.AddString(pMainframe->m_Vect_Program.at(i).strProgramName);
		if (pMainframe->m_Vect_Program.at(i).strProgramName == pMainframe->m_struct_CurrentProgram.strProgramName)
		{
			pMainframe->m_struct_CurrentProgram = pMainframe->m_Vect_Program.at(i);
			nSelIndex = i;
		}
	}
	if (nSelIndex != -1)
	{
		m_ComboBox_ProgSel.SetCurSel(nSelIndex);
		CString strShow = L"";
		strShow.Format(L"高压源限压: %.*f", DOT_BITNUMBER, pMainframe->m_Vect_Program.at(nSelIndex).fHightVoltage_Limit);
		m_Static_HightVoltage.SetWindowText(strShow);

		strShow.Format(L"低压源初始: %d", pMainframe->m_Vect_Program.at(nSelIndex).nLowVoltage);
		m_Static_LowVoltage.SetWindowText(strShow);

		m_Static_VoltageCurve.m_struct_Program = pMainframe->m_Vect_Program.at(nSelIndex);

	}
	else
	{
		m_ComboBox_ProgSel.SetCurSel(0);
		CString strShow = L"";
		strShow.Format(L"高压源限压: %.*f", DOT_BITNUMBER, pMainframe->m_Vect_Program.at(0).fHightVoltage_Limit);
		m_Static_HightVoltage.SetWindowText(strShow);

		strShow.Format(L"低压源初始: %d", pMainframe->m_Vect_Program.at(0).nLowVoltage);
		m_Static_LowVoltage.SetWindowText(strShow);
		pMainframe->m_struct_CurrentProgram = pMainframe->m_Vect_Program.at(0);
		m_Static_VoltageCurve.m_struct_Program = pMainframe->m_Vect_Program.at(0);
	}

}
void CDetectInfoInputDlg::UpdateProgramItem()
{
	CMainFrame* pMainframe = (CMainFrame*)AfxGetMainWnd();

	InitProgramSel();
	VECT_PROGRAMITEM vectProgramItem;
	vectProgramItem = pMainframe->m_struct_CurrentProgram.VectProgItem;
	m_ListCtrl_ProgramItems.DeleteAllItems();
	for (int i = 0; i < vectProgramItem.size(); i++)
	{
		m_ListCtrl_ProgramItems.InsertItem(i, L"");
		CString strtemp = L"";
		strtemp.Format(L"%d", vectProgramItem.at(i).nStep);
		m_ListCtrl_ProgramItems.SetItemText(i, 0, strtemp);

		strtemp.Format(L"%.*f", DOT_BITNUMBER,vectProgramItem.at(i).nHightVotage);
		m_ListCtrl_ProgramItems.SetItemText(i, 1, strtemp);

		strtemp.Format(L"%d", vectProgramItem.at(i).nKeepTime);
		m_ListCtrl_ProgramItems.SetItemText(i, 2, strtemp);


	}
}
void CDetectInfoInputDlg::InitDetectTypeInfo()
{
	//CMainFrame* pMainframe = (CMainFrame*)AfxGetMainWnd();
	//m_ComboBox_MeasureType.ResetContent();
	//for (int i = 0; i < pMainframe->m_Vect_DetectTypeInfo.size(); i++)
	//{
	//	m_ComboBox_MeasureType.AddString(pMainframe->m_Vect_DetectTypeInfo.at(i).strName);
	//	if (i == 0)
	//	{
	//		m_strMeasureType = pMainframe->m_Vect_DetectTypeInfo.at(i).strName;
	//	}
	//}
	//m_ComboBox_MeasureType.SetCurSel(0);
}




HBRUSH CDetectInfoInputDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetSafeHwnd() == this->GetSafeHwnd())
	{
		return (HBRUSH)m_BkBrush.GetSafeHandle();
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)m_BkBrush.GetSafeHandle();
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CDetectInfoInputDlg::OnBnClickedButtonStart()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame* pMainframe = (CMainFrame*)AfxGetMainWnd(); 
	CXrayDCDeburringView* pView = (CXrayDCDeburringView*)pMainframe->GetActiveView();
	if (!UpdateData())
	{
		return;
	}
	int nProgIndex = m_ComboBox_ProgSel.GetCurSel();
	if (nProgIndex >= 0 && pMainframe->m_Vect_Program.size() > nProgIndex)
	{
		pMainframe->m_struct_CurrentProgram = pMainframe->m_Vect_Program.at(nProgIndex);
		pMainframe->m_Struct_BurningTest_Param.nCurrentStep = 1;
		pMainframe->m_Struct_BurningTest_Param.nFinishTimes = 0;
		pMainframe->m_Struct_BurningTest_Param.nFireBackSteps = m_Edit_Goback_Steps;
		pMainframe->m_Struct_BurningTest_Param.nIncreaseRate = m_ComboBox_IncreaseSpeed.GetCurSel();
		pMainframe->m_Struct_BurningTest_Param.nTimeLong = 0;
		CString strTubeSnOld = pMainframe->m_Struct_BurningTest_Param.strTubeSn;
		pMainframe->m_Struct_BurningTest_Param.strTubeSn = m_Edit_TubeSN;
		pMainframe->m_Struct_BurningTest_Param.strTubeType = pMainframe->m_struct_CurrentProgram.strProgramName;
		pMainframe->m_Struct_BurningTest_Param.nRepeatTest_Times = m_nRepeatTimes;
		pMainframe->m_Struct_BurningTest_Param.nStartStep = m_Edit_StartStep;
		pView->m_Static_Curve.m_fMaxVoltage = pMainframe->m_struct_CurrentProgram.fHightVoltage_Limit;
//		pMainframe->m_wndProperties.m_pDetectInfoInputDlg->m_Static_VoltageCurve.m_fMaxVoltage = pMainframe->m_struct_CurrentProgram.fHightVoltage_Limit;
		SLOG1(L"InitializeTest start");
		if (pMainframe->InitializeTest())
		{
			
			pMainframe->m_Struct_BurningTest_Param.nFinishTimes = 0;
            
			if (strTubeSnOld != pMainframe->m_Struct_BurningTest_Param.strTubeSn)
			{
				pMainframe->m_Vect_FireRecord.clear();
				pMainframe->m_Vect_CurveValue.clear();
				pView->m_Static_Curve.m_vect_pixelvalue.clear();
				pMainframe->m_wndOutput.GetFireOutPutWnd()->PostMessage(WM_UPDATE_FIRERECORD, 2, NULL);
			}
			else
			{
				if (AfxMessageBox(L"是否清除历史记录", MB_YESNO) == IDYES)
				{
					pMainframe->m_Vect_FireRecord.clear();
					pMainframe->m_Vect_CurveValue.clear();
					pView->m_Static_Curve.m_vect_pixelvalue.clear();
					pMainframe->m_wndOutput.GetFireOutPutWnd()->PostMessage(WM_UPDATE_FIRERECORD, 2, NULL);

				}
			}

//			pMainframe->m_wndProperties.m_pDetectInfoInputDlg->m_Static_VoltageCurve.m_vect_pixelvalue.clear();

			pMainframe->StartTestThread();
			pView->m_Static_WorkStatus.SetWindowText(L"0");
			
		}	
		else
		{
			AfxMessageBox(L"初始化检测参数失败!");
		}
		SLOG1(L"InitializeTest End");
	}
}


void CDetectInfoInputDlg::OnCbnSelchangeComboProgsel()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame* pMainframe = (CMainFrame*)AfxGetMainWnd();
	int nProgIndex = m_ComboBox_ProgSel.GetCurSel();
	if (nProgIndex >= 0 && pMainframe->m_Vect_Program.size() > nProgIndex)
	{
		CString strShow = L"";
		strShow.Format(L"高压源限压: %.*f", DOT_BITNUMBER, pMainframe->m_Vect_Program.at(nProgIndex).fHightVoltage_Limit);
		m_Static_HightVoltage.SetWindowText(strShow);

		strShow.Format(L"低压源初始: %d", pMainframe->m_Vect_Program.at(nProgIndex).nLowVoltage);
		m_Static_LowVoltage.SetWindowText(strShow);
		pMainframe->m_struct_CurrentProgram = pMainframe->m_Vect_Program.at(nProgIndex);
		m_Static_VoltageCurve.m_struct_Program = pMainframe->m_Vect_Program.at(nProgIndex);
		m_Static_VoltageCurve.Invalidate(FALSE);
		UpdateProgramItem();
	}

}


//void CDetectInfoInputDlg::OnTimeChange()
//{
//	CDialogEx::OnTimeChange();
//
//	// TODO: 在此处添加消息处理程序代码
//}


void CDetectInfoInputDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		CMainFrame* pMainframe = (CMainFrame*)AfxGetMainWnd();
		CString strFinishTimes = L"", strCurrentSteps = L"", strTimeLong = L"";
		strFinishTimes.Format(L"完成:%d次", pMainframe->m_Struct_BurningTest_Param.nFinishTimes);
		strCurrentSteps.Format(L"步骤:%d", pMainframe->m_Struct_BurningTest_Param.nCurrentStep);
		m_Static_VoltageCurve.m_nStep = pMainframe->m_Struct_BurningTest_Param.nCurrentStep;
		strTimeLong.Format(L"时间:%dS", pMainframe->m_Struct_BurningTest_Param.nTimeLong);
		m_Static_KeepTime_Real.SetWindowText(strTimeLong);
		m_Static_FinishTimes.SetWindowText(strFinishTimes);
		m_Static_CurrentStep.SetWindowText(strCurrentSteps);
		m_Static_VoltageCurve.Invalidate(FALSE);
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CDetectInfoInputDlg::OnBnClickedButtonContinue()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame* pMainframe = (CMainFrame*)AfxGetMainWnd();
	if (pMainframe->m_bBeginGetValue == TRUE)
	{
		pMainframe->SendMessage(WM_EXECUTECOMMAND, EXECUTE_COMMAND_SUSPEND);
		GetDlgItem(IDC_BUTTON_CONTINUE)->SetWindowText(L"继续");
	}
	else
	{
		pMainframe->SendMessage(WM_EXECUTECOMMAND, EXECUTE_COMMAND_CONTINUE);
		GetDlgItem(IDC_BUTTON_CONTINUE)->SetWindowText(L"暂停");
	}

}


void CDetectInfoInputDlg::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame* pMainframe = (CMainFrame*)AfxGetMainWnd();
	CXrayDCDeburringView* pView = (CXrayDCDeburringView*)pMainframe->GetActiveView();
	if (pMainframe->m_pThread_TestThread != NULL && pMainframe->m_pThread_TestThread->m_hThread != NULL)
	{
		pMainframe->PostMessage(WM_EXECUTECOMMAND, EXECUTE_COMMAND_STOPTEST);
	}
}
