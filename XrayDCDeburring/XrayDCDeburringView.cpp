
// XrayDCDeburringView.cpp: CXrayDCDeburringView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "XrayDCDeburring.h"
#endif

#include "XrayDCDeburringDoc.h"
#include "XrayDCDeburringView.h"
#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern std::mutex g_GetValueMutex;
// CXrayDCDeburringView

IMPLEMENT_DYNCREATE(CXrayDCDeburringView, CFormView)

BEGIN_MESSAGE_MAP(CXrayDCDeburringView, CFormView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CXrayDCDeburringView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CXrayDCDeburringView 构造/析构

CXrayDCDeburringView::CXrayDCDeburringView() noexcept
	: CFormView(IDD_XRAYDCDEBURRING_FORM)
{
	// TODO: 在此处添加构造代码

}

CXrayDCDeburringView::~CXrayDCDeburringView()
{
}

void CXrayDCDeburringView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_WORK_STATUS, m_Static_WorkStatus);
	DDX_Control(pDX, IDC_STATIC_SHIBOQI_STATUS, m_Static_Shiboqi_Satus);
	DDX_Control(pDX, IDC_STATIC_MCU_STATUS, m_Static_Mcu_Status);
	DDX_Control(pDX, IDC_STATIC_BREAKDOWN, m_Static_Breakdown_Status);
	DDX_Control(pDX, IDC_STATIC_LOWPOWER_STATUS, m_Static_Lowpower_Status);
	DDX_Control(pDX, IDC_STATIC_CURVE, m_Static_Curve);
	DDX_Control(pDX, IDC_STATIC_LOUCURRENT_CH1, m_Static_LouCurrent_Ch1);
	DDX_Control(pDX, IDC_STATIC_LOUCURRENT_CH2, m_Static_LouCurrent_Ch2);
	DDX_Control(pDX, IDC_STATIC_LOUCURRENT_CH3, m_Static_LouCurrent_Ch3);
	DDX_Control(pDX, IDC_STATIC_LOUCURRENT_CH4, m_Static_LouCurrent_Ch4);
	DDX_Control(pDX, IDC_STATIC_JICHAN_CURRENT, m_Static_Jichuan_Current);
	DDX_Control(pDX, IDC_STATIC_JICHUAN_CH, m_Static_Jichuan_CH);
	DDX_Control(pDX, IDC_STATIC_LOWER_CURRENT, m_Static_LowPower_Current);
	DDX_Control(pDX, IDC_STATIC_LOWER_VALTAGE, m_Static_LowPower_Voltage);
	DDX_Control(pDX, IDC_STATIC_SPLITLINE, m_Static_SplitLine);
	DDX_Control(pDX, IDC_STATIC_HIPOWER_STATUS, m_Static_HiPowerStatus);
	DDX_Control(pDX, IDC_STATIC_HIGHT_VALTAGE, m_Static_Hight_Voltage);
}

BOOL CXrayDCDeburringView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CFormView::PreCreateWindow(cs);
}

void CXrayDCDeburringView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	m_Font = new CFont;
	m_Font->CreateFont(30,0, 0, 0, 100,
		FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_SWISS, _T("方正姚体"));
	DWORD StaticID[] = { IDC_STATIC_LOUCURRENT_CH1,IDC_STATIC_LOUCURRENT_CH2, IDC_STATIC_LOUCURRENT_CH3 ,IDC_STATIC_LOUCURRENT_CH4 ,IDC_STATIC_JICHAN_CURRENT ,
	IDC_STATIC_JICHUAN_CH ,IDC_STATIC_LOWER_VALTAGE ,IDC_STATIC_LOWER_CURRENT,IDC_STATIC_WORK_STATUS,IDC_STATIC_HIGHT_VALTAGE };
	for (int i = 0; i < 10; i++)
	{
		CStatic* pStatic = (CStatic*)GetDlgItem(StaticID[i]);
		pStatic->SetFont(m_Font);
	}
	m_Static_Breakdown_Status.SetStatus(LIGHT_STATUS_GOOD);
	m_Static_Shiboqi_Satus .SetStatus(LIGHT_STATUS_NG);
	m_Static_Mcu_Status.SetStatus(LIGHT_STATUS_NG);
	m_Static_Lowpower_Status.SetStatus(LIGHT_STATUS_NG);

	m_Static_Breakdown_Status.SetTimer(1, 1000, NULL);
	m_Static_Shiboqi_Satus.SetTimer(1, 1000, NULL);
	m_Static_Mcu_Status.SetTimer(1, 1000, NULL);
	m_Static_Lowpower_Status.SetTimer(1, 1000, NULL);

	SetTimer(1, 200, NULL);

}


// CXrayDCDeburringView 打印


void CXrayDCDeburringView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CXrayDCDeburringView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CXrayDCDeburringView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CXrayDCDeburringView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CXrayDCDeburringView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: 在此处添加自定义打印代码
}

void CXrayDCDeburringView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CXrayDCDeburringView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CXrayDCDeburringView 诊断

#ifdef _DEBUG
void CXrayDCDeburringView::AssertValid() const
{
	CFormView::AssertValid();
}

void CXrayDCDeburringView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CXrayDCDeburringDoc* CXrayDCDeburringView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CXrayDCDeburringDoc)));
	return (CXrayDCDeburringDoc*)m_pDocument;
}
#endif //_DEBUG


// CXrayDCDeburringView 消息处理程序


//LRESULT CXrayDCDeburringView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//
//	return CFormView::WindowProc(message, wParam, lParam);
//}


void CXrayDCDeburringView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	if (m_Static_SplitLine.GetSafeHwnd() == NULL)
	{
		return;
	}
	CRect clientRect, splitRect,curveRect;
	GetClientRect(clientRect);
	m_Static_SplitLine.GetWindowRect(splitRect);
	m_Static_Curve.GetWindowRect(curveRect);
	ScreenToClient(splitRect);
	ScreenToClient(curveRect);
	CRect recttemp(clientRect.left, splitRect.top, clientRect.right, splitRect.top + 1);
	m_Static_SplitLine.MoveWindow(recttemp);
	
	m_Static_Curve.MoveWindow(CRect(clientRect.left, curveRect.top, clientRect.right, clientRect.bottom));

	// TODO: 在此处添加消息处理程序代码
}


void CXrayDCDeburringView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{//刷新界面
		CMainFrame* pMainframe = (CMainFrame*)AfxGetMainWnd();
		CString strtemp = L"";
		strtemp.Format(L"%.1f", pMainframe->m_Struct_CommRet.fLowPowerVoltage);
		m_Static_LowPower_Voltage.SetWindowText(strtemp);

		strtemp.Format(L"%.1f", pMainframe->m_Struct_CommRet.fHiPowerVoltage);
		m_Static_Hight_Voltage.SetWindowText(strtemp);

		strtemp.Format(L"%.1f", pMainframe->m_Struct_CommRet.fLowPowerCurrent);
		m_Static_LowPower_Current.SetWindowText(strtemp);

		strtemp.Format(L"%d", pMainframe->m_Struct_CommRet.fShiboqiCurrent);
		m_Static_Jichuan_Current.SetWindowText(strtemp);

		strtemp.Format(L"%d", pMainframe->m_Struct_CommRet.nShiboqiChannel);
		m_Static_Jichuan_CH.SetWindowText(strtemp);

		strtemp.Format(L"%d", pMainframe->m_Struct_CommRet.nMcuCurrent[0]);
		m_Static_LouCurrent_Ch1.SetWindowText(strtemp);

		strtemp.Format(L"%d", pMainframe->m_Struct_CommRet.nMcuCurrent[1]);
		m_Static_LouCurrent_Ch2.SetWindowText(strtemp);

		strtemp.Format(L"%d", pMainframe->m_Struct_CommRet.nMcuCurrent[3]);
		m_Static_LouCurrent_Ch3.SetWindowText(strtemp);

		strtemp.Format(L"%d", pMainframe->m_Struct_CommRet.nMcuCurrent[4]);
		m_Static_LouCurrent_Ch4.SetWindowText(strtemp);

		if (pMainframe->m_bBeginGetValue == TRUE)
		{
			CTime tCurrent = CTime::GetCurrentTime();
			if (pMainframe->m_struct_CurrentProgram.VectProgItem.size() > 0)
			{
				CTimeSpan tSpan = tCurrent - pMainframe->m_struct_CurrentProgram.VectProgItem.at(0).tStartTime;
				CString strTime = L"";
				strTime.Format(L"%d", tSpan.GetTotalSeconds());
				m_Static_WorkStatus.SetWindowText(strTime);
			}
		}
			
		if ((pMainframe->m_Struct_CommRet.nHiPowerStatus > 0)  && (pMainframe->m_Struct_CommRet.nHiPowerStatus& STATUS_HIGHT_UNDERVOLTAGE != STATUS_HIGHT_UNDERVOLTAGE) && (pMainframe->m_Struct_CommRet.nHiPowerStatus& STATUS_BUCK_COMMNICATIONFAULT != STATUS_BUCK_COMMNICATIONFAULT) && (m_StructCommRet.nHiPowerStatus == STATUS_OK))
		{

			//STATUS_HIGHT_OVERVOLTAGE = 0x01,
			//	STATUS_HIGHT_OVERCURRENT = 0x02,
			//	STATUS_HIGHT_UNDERVOLTAGE = 0x04,
			//	STATUS_HIGHT_SOFTSTARTFAULT = 0x08,
			//	STATUS_BUCK_OVERVOLTAGE = 0x10,
			//	STATUS_BUCK_OVERCURRENT = 0x20,
			//	STATUS_BUCK_UNDERVOLTAGE = 0x40,
			//	STATUS_BUCK_SOFTSTARTFAULT = 0x80,

			//	STATUS_REC_UNDERVOLTAGEFAULT = 0x100,
			//	STATUS_REC_OVERVOLTAGEFAULT = 0x200,
			//	STATUS_IGNITIONFAULT = 0x400,
			//	STATUS_BUCK_COMMNICATIONFAULT = 0x800,
			if ((pMainframe->m_Struct_CommRet.nHiPowerStatus & STATUS_HIGHT_OVERVOLTAGE) == STATUS_HIGHT_OVERVOLTAGE)
			{
				SLOG1("高压过压故障");
			}
			else if ((pMainframe->m_Struct_CommRet.nHiPowerStatus & STATUS_HIGHT_OVERCURRENT) == STATUS_HIGHT_OVERCURRENT)
			{
				SLOG1("高压过流故障");
			}
			else if ((pMainframe->m_Struct_CommRet.nHiPowerStatus & STATUS_HIGHT_UNDERVOLTAGE) == STATUS_HIGHT_UNDERVOLTAGE)
			{
				SLOG1("高压欠压故障");
			}
			else if ((pMainframe->m_Struct_CommRet.nHiPowerStatus & STATUS_HIGHT_SOFTSTARTFAULT) == STATUS_HIGHT_SOFTSTARTFAULT)
			{
				SLOG1("高压软启故障");
			}
			else if ((pMainframe->m_Struct_CommRet.nHiPowerStatus & STATUS_BUCK_OVERVOLTAGE) == STATUS_BUCK_OVERVOLTAGE)
			{
				SLOG1("buck过压故障");
			}
			else if ((pMainframe->m_Struct_CommRet.nHiPowerStatus & STATUS_BUCK_OVERCURRENT) == STATUS_BUCK_OVERCURRENT)
			{
				SLOG1("buck过流故障");
			}
			else if ((pMainframe->m_Struct_CommRet.nHiPowerStatus & STATUS_BUCK_UNDERVOLTAGE) == STATUS_BUCK_UNDERVOLTAGE)
			{
				SLOG1("buck欠压故障");
			}
			else if ((pMainframe->m_Struct_CommRet.nHiPowerStatus & STATUS_BUCK_SOFTSTARTFAULT) == STATUS_BUCK_SOFTSTARTFAULT)
			{
				SLOG1("buck软启故障");
			}
			else if ((pMainframe->m_Struct_CommRet.nHiPowerStatus & STATUS_REC_UNDERVOLTAGEFAULT) == STATUS_REC_UNDERVOLTAGEFAULT)
			{
				SLOG1("整流欠压故障");
			}
			else if ((pMainframe->m_Struct_CommRet.nHiPowerStatus & STATUS_REC_OVERVOLTAGEFAULT) == STATUS_REC_OVERVOLTAGEFAULT)
			{
				SLOG1("整流过压故障");
			}
			else if ((pMainframe->m_Struct_CommRet.nHiPowerStatus & STATUS_IGNITIONFAULT) == STATUS_IGNITIONFAULT)
			{
				SLOG1("打火故障");
			}
			else if ((pMainframe->m_Struct_CommRet.nHiPowerStatus & STATUS_BUCK_COMMNICATIONFAULT) == STATUS_BUCK_COMMNICATIONFAULT)
			{
				SLOG1("通信故障");
			}
			
			m_Static_Breakdown_Status.SetStatus(LIGHT_STATUS_BAD);
			pMainframe->PostMessage(WM_EXECUTECOMMAND, EXECUTE_COMMAND_STOPTEST);
		}
		else
		{
			m_Static_Breakdown_Status.SetStatus(LIGHT_STATUS_GOOD);
		}

		m_StructCommRet = pMainframe->m_Struct_CommRet;

		if (pMainframe->m_Struct_CommStatus.nHightPowerStatus == 1)
		{
			m_Static_HiPowerStatus.SetStatus(LIGHT_STATUS_OK);
		}
		else
		{
			m_Static_HiPowerStatus.SetStatus(LIGHT_STATUS_NG);
		}

		if (pMainframe->m_Struct_CommStatus.nLowPowerstatus == 1)
		{
			m_Static_Lowpower_Status.SetStatus(LIGHT_STATUS_OK);
		}
		else
		{
			m_Static_Lowpower_Status.SetStatus(LIGHT_STATUS_NG);
		}

		if (pMainframe->m_Struct_CommStatus.nMcuStatus == 1)
		{
			m_Static_Mcu_Status.SetStatus(LIGHT_STATUS_OK);
		}
		else
		{
			m_Static_Mcu_Status.SetStatus(LIGHT_STATUS_NG);
		}

		if (pMainframe->m_Struct_CommStatus.nShiboqiStatus == 1)
		{
			m_Static_Shiboqi_Satus.SetStatus(LIGHT_STATUS_OK);
		}
		else
		{
			m_Static_Shiboqi_Satus.SetStatus(LIGHT_STATUS_NG);
		}
		//刷新曲线
		g_GetValueMutex.lock();
		for (int i = m_Static_Curve.m_vect_pixelvalue.size(); i < pMainframe->m_Vect_CurveValue.size(); i++)
		{
			m_Static_Curve.m_vect_pixelvalue.push_back(pMainframe->m_Vect_CurveValue.at(i).fHiVoltageValue);
//			pMainframe->m_wndProperties.m_pDetectInfoInputDlg->m_Static_VoltageCurve.m_vect_pixelvalue.push_back(pMainframe->m_Vect_CurveValue.at(i));
		}
		g_GetValueMutex.unlock();
		m_Static_Curve.Invalidate(FALSE);
		pMainframe->m_wndProperties.m_pDetectInfoInputDlg->m_Static_VoltageCurve.Invalidate(FALSE);
		m_Static_HiPowerStatus.Invalidate(FALSE);
		m_Static_Lowpower_Status.Invalidate(FALSE);
		m_Static_Mcu_Status.Invalidate(FALSE);
		m_Static_Shiboqi_Satus.Invalidate(FALSE);
	}
	CFormView::OnTimer(nIDEvent);
}
