
// MainFrm.cpp: CMainFrame 类的实现
//

#include "pch.h"
#include "framework.h"
#include "XrayDCDeburring.h"

#include "MainFrm.h"
#include "CCommSetDlg.h"
#include "CTestProgSettingsDlg.h"
#include "XrayDCDeburringView.h"
#include "CFireRecordDlg.h"
#include <thread>
#include "CApplication.h"
#include "CWorkbook.h"
#include "CWorksheet.h"
#include "CWorkbooks.h"
#include "CWorksheets.h"
#include "CRange.h"
#include "Cnterior.h"
#include "CFont0.h"
#include <algorithm>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;
std::mutex g_comm_hipowerthread_mutex;
std::mutex g_comm_lowpowerthread_mutex;
std::mutex g_comm_shiboqithread_mutex;
std::mutex g_comm_mcuthread_mutex;
std::mutex g_GetValueMutex;
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_TOOLBAR_COMMSET, &CMainFrame::OnToolbarCommset)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_COMMSET, &CMainFrame::OnUpdateToolbarCommset)
	ON_COMMAND(ID_TOOLBAR_TESTPLAN, &CMainFrame::OnToolbarTestplan)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_TESTPLAN, &CMainFrame::OnUpdateToolbarTestplan)
	ON_COMMAND(ID_TOOLBAR_DEVICE_LINK, &CMainFrame::OnToolbarDeviceLink)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_DEVICE_LINK, &CMainFrame::OnUpdateToolbarDeviceLink)
	ON_MESSAGE(WM_EXECUTECOMMAND, &CMainFrame::OnExecuteCommand)
	ON_WM_CLOSE()
	ON_COMMAND(ID_TOOLBAR_FIRERECORD, &CMainFrame::OnToolbarFirerecord)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_FIRERECORD, &CMainFrame::OnUpdateToolbarFirerecord)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame() noexcept
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLUE);
	m_bFireFlag = FALSE;
	m_pThread_HightPower = NULL;
	m_pThread_LowPower = NULL;
	m_pThread_Mcu = NULL;
	m_pThread_Shiboqi = NULL;
	m_pThread_TestThread = NULL;

	m_bStopHightPowerThread = FALSE;
	m_bStopLowPowerThread = FALSE;
	m_bStopMcuThread = FALSE;
	m_bStopShiboqiThread = FALSE;
	m_bStopTestThread = FALSE;
	m_strHightPower_Com = L"";
	m_strLowPower_Com = L"";
	m_strMcu_Com = L"";
	m_strShiboqi_Com = L"";
	m_strShiboqi_IP = L"";
	m_nBaudRate_MCU = 9600;
	m_nBaudRate_HightPower = 9600;
	m_nBaudRate_LowPower = 9600;
	m_nBaudRate_Shiboqi = 9600;
	m_bBeginGetValue = FALSE;
	m_Vect_FireRecord.clear();
	m_strUser = L"";
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;


	//union convertUnion
	//{
	//	float fValue;
	//	BYTE byteValue[4];
	//};
	//convertUnion convert;
	//convert.fValue = 1100;


	BOOL bNameValid;
	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME, 0, 0, TRUE, 0, 0, IDB_BITMAP_MAINTOOLBAR))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}
	m_wndToolBar.EnableTextLabels();
	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	//// 允许用户定义的工具栏操作: 
	//InitUserToolbars(nullptr, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	CString strDbPath = ((CXrayDCDeburringApp*)AfxGetApp())->m_strRootPath + _T("XrayDCDeburring.SL3");
	if (m_dbControl.GetInstance(strDbPath) == NULL)
	{
		AfxMessageBox(L"数据库初始化失败!");
	}
	ReadIniConfig();
	m_dbControl.ReadDataBase(COMMAND_TYPE_PRODUCTINFO_LOAD,NULL,&m_Vect_Program,NULL);

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}

	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);

	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	// 启用工具栏和停靠窗口菜单替换
	//EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	//CMFCToolBar::EnableQuickCustomization();

	//if (CMFCToolBar::GetUserImages() == nullptr)
	//{
	//	// 加载用户定义的工具栏图像
	//	if (m_UserImages.Load(_T(".\\UserImages.bmp")))
	//	{
	//		CMFCToolBar::SetUserImages(&m_UserImages);
	//	}
	//}

	// 启用菜单个性化(最近使用的命令)
	// TODO: 定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。
	CList<UINT, UINT> lstBasicCommands;

	//lstBasicCommands.AddTail(ID_FILE_NEW);
	//lstBasicCommands.AddTail(ID_FILE_OPEN);
	//lstBasicCommands.AddTail(ID_FILE_SAVE);
	//lstBasicCommands.AddTail(ID_FILE_PRINT);
	//lstBasicCommands.AddTail(ID_APP_EXIT);
	//lstBasicCommands.AddTail(ID_EDIT_CUT);
	//lstBasicCommands.AddTail(ID_EDIT_PASTE);
	//lstBasicCommands.AddTail(ID_EDIT_UNDO);
	//lstBasicCommands.AddTail(ID_APP_ABOUT);
	//lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	//lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	//lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	//lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	//lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	//lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	//lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	//lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	//lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);

	//STRUCT_FIRERECORD structRecord;
	//structRecord.nStep = 1;
	//CTime tCurrentTime = CTime::GetCurrentTime();
	//	
	//structRecord.strFileMoment.Format(L"%d", 20);
	////structRecord.strVoltage.Format(pMainframe->m_struct_CurrentProgram.VectProgItem.at(structRecord.nStep - 1).nHightVotage);
	//structRecord.strVoltage.Format(L"%.1f/%.1f", 50.0, 48.8);
	//structRecord.strVoltageFire.Format(L"%.1f", 48.8);
	//structRecord.strTime = tCurrentTime.Format(L"%Y-%m-%d %H:%M:%S");
	//for (int j = 0; j < 4; j++)
	//{
	//	structRecord.strFireCurrentCH[j].Format(L"%.1f", 20.2);
	//}
	//m_struct_CurrentProgram = m_Vect_Program.at(0);
	//m_struct_CurrentProgram.strDataSavePath = L"";
	//structRecord.strTubeSn = L"CH11";
	//structRecord.strTubeType = L"130KV";
	//m_Vect_FireRecord.push_back(structRecord);
	//m_Struct_BurningTest_Param.strTubeSn = L"CH11";
	//SaveTestRecode();

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	// 创建输出窗口
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 500), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建输出窗口\n");
		return FALSE; // 未能创建
	}
	m_wndOutput.SetControlBarStyle(~AFX_CBRS_CLOSE);
	// 创建属性窗口
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 300, 300), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“属性”窗口\n");
		return FALSE; // 未能创建
	}
	m_wndProperties.SetControlBarStyle(~AFX_CBRS_CLOSE);


	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	m_wndOutput.UpdateFonts();
	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	// 基类将执行真正的工作

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != nullptr)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}


LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CFrameWndEx::WindowProc(message, wParam, lParam);
}


void CMainFrame::OnToolbarCommset()
{
	// TODO: 在此添加命令处理程序代码
	CCommSetDlg dlg;
	if (dlg.DoModal() == IDOK)
	{

	}
}


void CMainFrame::OnUpdateToolbarCommset(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable();
}
BOOL  CompareProgItem(STRUCT_PROGRAMITEM a, STRUCT_PROGRAMITEM b)
{
	if (a.nStep > b.nStep)
	{
		return FALSE;
	}
	return TRUE;
}

void CMainFrame::OnToolbarTestplan()
{
	// TODO: 在此添加命令处理程序代码
	CTestProgSettingsDlg dlg;
	dlg.m_Vect_Program = m_Vect_Program;
	if (dlg.DoModal() == IDOK)
	{
		m_Vect_Program = dlg.m_Vect_Program;
		for (int i = 0; i < dlg.m_Vect_Program.size(); i++)
		{
			std::sort(m_Vect_Program.at(i).VectProgItem.begin(), m_Vect_Program.at(i).VectProgItem.end(), CompareProgItem);
		}
		
		CXrayDCDeburringView* pView = (CXrayDCDeburringView*)GetActiveView();
		m_wndProperties.UpdateProgramItem();
		m_dbControl.WriteDataBase(COMMAND_TYPE_PRODUCTINFO_SAVE, &m_Vect_Program, NULL);
	}
}


void CMainFrame::OnUpdateToolbarTestplan(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable();
}


void CMainFrame::ReadIniConfig()
{
	CString strIniPath = theApp.m_strRootPath + _T("SystemConfig.ini");
	TCHAR Ipbuffer[200];
	memset(Ipbuffer, 0, 200 * sizeof(TCHAR));
	GetPrivateProfileString(_T("SystemSettings"), _T("HIGHT_POWER_COM"), _T("COM1"), Ipbuffer, 200, strIniPath);
	m_strHightPower_Com.Format(_T("%s"), Ipbuffer);

	memset(Ipbuffer, 0, 200 * sizeof(TCHAR));
	GetPrivateProfileString(_T("SystemSettings"), _T("LOW_POWER_COM"), _T("COM1"), Ipbuffer, 200, strIniPath);
	m_strLowPower_Com.Format(_T("%s"), Ipbuffer);

	memset(Ipbuffer, 0, 200 * sizeof(TCHAR));
	GetPrivateProfileString(_T("SystemSettings"), _T("MCU_COM"), _T("COM1"), Ipbuffer, 200, strIniPath);
	m_strMcu_Com.Format(_T("%s"), Ipbuffer);

	memset(Ipbuffer, 0, 200 * sizeof(TCHAR));
	GetPrivateProfileString(_T("SystemSettings"), _T("SHIBOQI_IP"), _T("COM1"), Ipbuffer, 200, strIniPath);
	m_strShiboqi_Com.Format(_T("%s"), Ipbuffer);
	m_nBaudRate_HightPower = GetPrivateProfileInt(_T("SystemSettings"), _T("HIGHT_POWER_BAUDRATE"), 9600, strIniPath);
	m_nBaudRate_LowPower = GetPrivateProfileInt(_T("SystemSettings"), _T("LOW_POWER_BAUDRATE"), 9600, strIniPath);
	m_nBaudRate_MCU = GetPrivateProfileInt(_T("SystemSettings"), _T("MCU_BAUDRATE"), 9600, strIniPath);
	m_nBaudRate_Shiboqi = GetPrivateProfileInt(_T("SystemSettings"), _T("SHIBOQI_BAUDRATE"), 9600, strIniPath);
}

unsigned short CMainFrame::CRC16(const unsigned char* puchMsg, unsigned char usDataLen)
{
	unsigned char ucCRCHi = 0xFF;        /* CRC的高字节初始化 */
	unsigned char ucCRCLo = 0xFF;        /* CRC的低字节初始化 */
	unsigned char uIndex;                    /* CRC查询表索引 */
	while (usDataLen--)                  /* 完成整个报文缓冲区 */
	{
		uIndex = ucCRCLo ^ *puchMsg++;            /* 计算CRC */
		ucCRCLo = ucCRCHi ^ aucCRCHi[uIndex];
		ucCRCHi = aucCRCLo[uIndex];
	};
	unsigned short val;
	val = ucCRCHi << 8 | ucCRCLo;
	return val;
}
UINT MonitorHightPowerTread(LPVOID lParam)
{
	CMainFrame* pMainframe = (CMainFrame*)lParam;
	CXrayDCDeburringView* pView = (CXrayDCDeburringView * )pMainframe->GetActiveView();

	while (true)
	{
		if (pMainframe->m_bStopHightPowerThread)
		{
			break;
		}
		this_thread::sleep_for(50ms);
		//读电压
		g_comm_hipowerthread_mutex.lock();
		BYTE SendByte[500];
		memset(SendByte, 0, 500);
		SendByte[0] = 0x55;
		SendByte[1] = 0x01;
		SendByte[2] = 0x10;
		SendByte[3] = 0x00;
		WORD nWord = pMainframe->get_check_sum(SendByte, 4);
		SendByte[4] = nWord % 0x0100;
		SendByte[5] = 0x66;
		DWORD dwWritten = 0;
		LONG sendnum = pMainframe->m_Serial_HightPower.Write(SendByte, 6, &dwWritten, 0, 500);
		if (dwWritten <= 0)
		{
			g_comm_hipowerthread_mutex.unlock();
			return -1;
		}

		DWORD nReadNum = 0, nReadNumTemp = 0;
		/*static*/
		int ntimes = 0;
		BYTE bBuff[64];
		memset(bBuff, 0x00, 64);
		LONG    lLastError = ERROR_SUCCESS;
		BOOL bFirstTime = TRUE;
		while ((ntimes < 20/*最大延迟0.5s*/)/* && (nReadNum > 2)*/)
		{
			lLastError = pMainframe->m_Serial_HightPower.Read(&bBuff[nReadNum], 64, &nReadNumTemp);
			nReadNum += nReadNumTemp;
			CString strreceive = _T(""), strtemp = _T("");
			if (nReadNum <= 0)
			{
				this_thread::sleep_for(10ms);
				ntimes++;
				continue;
			}
			if (nReadNum < 10)
			{
				ntimes++;
				if (bFirstTime)
				{
					ntimes = 1;
					bFirstTime = FALSE;
				}
				if (nReadNum > 2)
				{
					if (bBuff[0] != 0x55)
					{//读取失败
						pMainframe->m_Serial_HightPower.Purge();
						break;
					}
				}
				this_thread::sleep_for(50ms);
				continue;
			}
			else if (nReadNum == 10)
			{//55 81 10 04 12 00 00 00 92 66
				if (bBuff[0] == 0x55 && bBuff[9] == 0x66)
				{//读取成功
					CTime tCurrentTime = CTime::GetCurrentTime();
					CTimeSpan tTimeSpan = tCurrentTime - pMainframe->m_tBeginGetValue;
					double fvalue = (bBuff[7] * 0x1000000 + bBuff[6] * 0x10000 + bBuff[5] * 0x100 + bBuff[4])/10.0;
					pMainframe->m_Struct_CommRet.fHiPowerVoltage = fvalue;
					STRUCT_CURVEVALUE structtemp;
					structtemp.fHiVoltageValue = fvalue;
					structtemp.nTimeSecond = tTimeSpan.GetTotalSeconds();
					if (pMainframe->m_bBeginGetValue)
					{
						g_GetValueMutex.lock();
//#ifdef SHOW_LOG
//						CString strLog = L"";
//						strLog.Format(L"%f", fvalue);
//						SLOG1(strLog);
//#endif
						pMainframe->m_Vect_CurveValue.push_back(structtemp);
						g_GetValueMutex.unlock();
					}

					break;
				}
			}
		}
		//读状态55 01 09 00 17 66 

		memset(SendByte, 0, 500);
		SendByte[0] = 0x55;
		SendByte[1] = 0x01;
		SendByte[2] = 0x09;
		SendByte[3] = 0x00;
		nWord = pMainframe->get_check_sum(SendByte, 4);
		SendByte[4] = nWord % 0x0100;
		SendByte[5] = 0x66;
		dwWritten = 0;
		sendnum = pMainframe->m_Serial_HightPower.Write(SendByte, 6, &dwWritten, 0, 500);
		if (dwWritten <= 0)
		{
			g_comm_hipowerthread_mutex.unlock();
			return -1;
		}
		nReadNum = 0, nReadNumTemp = 0;
		/*static*/
		ntimes = 0;
		memset(bBuff, 0x00, 64);
		lLastError = ERROR_SUCCESS;
		bFirstTime = TRUE;
		while ((ntimes < 20/*最大延迟0.5s*/)/* && (nReadNum > 2)*/)
		{
			lLastError = pMainframe->m_Serial_HightPower.Read(&bBuff[nReadNum], 64, &nReadNumTemp);
			nReadNum += nReadNumTemp;
			CString strreceive = _T(""), strtemp = _T("");
			if (nReadNum <= 0)
			{
				this_thread::sleep_for(10ms);
				ntimes++;
				continue;
			}
			if (nReadNum < 10)
			{
				ntimes++;
				if (bFirstTime)
				{
					ntimes = 1;
					bFirstTime = FALSE;
				}
				if (nReadNum > 2)
				{
					if (bBuff[0] != 0x55)
					{//读取失败
						pMainframe->m_Serial_HightPower.Purge();
						break;
					}
				}
				this_thread::sleep_for(50ms);
				continue;
			}
			else if (nReadNum == 10)
			{//55 81 09 04 00 00 00 00 47 66
				if (bBuff[0] == 0x55 && bBuff[9] == 0x66)
				{//读取成功
					pMainframe->m_Struct_CommRet.nHiPowerStatus = bBuff[7] * 0x1000000 + bBuff[6] * 0x10000 + bBuff[5] * 0x100 + bBuff[4];
					break;
				}
			}
		}
		g_comm_hipowerthread_mutex.unlock();
	}
	return 0;
}

UINT MonitorLowPowerTread(LPVOID lParam)
{
	CMainFrame* pMainframe = (CMainFrame*)lParam;
	CXrayDCDeburringView* pView = (CXrayDCDeburringView*)pMainframe->GetActiveView();

	while (true)
	{
		if (pMainframe->m_bStopLowPowerThread)
		{
			break;
		}
//#ifdef SHOW_LOG
//		SLOG1(L"MonitorLowPowerTread-读电流电压 start");
//#endif
		this_thread::sleep_for(50ms);
		g_comm_lowpowerthread_mutex.lock();
		//读电流电压
		BYTE SendByte[500];
		memset(SendByte, 0, 500);
		SendByte[0] = 0x01;
		SendByte[1] = 0x03;
		SendByte[2] = 0x0B;
		SendByte[3] = 0x00;
		SendByte[4] = 0x00;
		SendByte[5] = 0x04;
		WORD nWord = pMainframe->CRC16(SendByte, 6);
		SendByte[6] = nWord % 0x0100;
		SendByte[7] = nWord / 0x0100;
		DWORD dwWritten = 0;
		LONG sendnum = pMainframe->m_Serial_LowPower.Write(SendByte, 8, &dwWritten, 0, 500);
		if (dwWritten <= 0)
		{
			g_comm_lowpowerthread_mutex.unlock();
			return -1;
		}
		DWORD nReadNum = 0,nReadNumTemp = 0;
		/*static*/
		int ntimes = 0;
		BYTE bBuff[64];
		memset(bBuff, 0x00, 64);
		LONG    lLastError = ERROR_SUCCESS;
		BOOL bFirstTime = TRUE;
		BOOL bSuccess = FALSE;
		while ((ntimes < 20/*最大延迟0.5s*/)/* && (nReadNum > 2)*/)
		{
			lLastError = pMainframe->m_Serial_LowPower.Read(&bBuff[nReadNum], 64, &nReadNumTemp);
			nReadNum += nReadNumTemp;
			CString strreceive = _T(""), strtemp = _T("");
			if (nReadNum <= 0)
			{
				ntimes++;
				this_thread::sleep_for(10ms);
				continue;
			}
			if (nReadNum < 13)
			{
				ntimes++;
				if (bFirstTime)
				{
					ntimes = 1;
					bFirstTime = FALSE;
				}
				if (nReadNum > 2)
				{
					if (bBuff[1] != 0x03)
					{//读取失败
						pMainframe->m_Serial_LowPower.Purge();
						break;
					}
				}
				this_thread::sleep_for(50ms);
				continue;
			}
			else if(nReadNum == 13)
			{
				if (bBuff[1] == 0x03)
				{//读取成功
					union convertUnion
					{
						float fValue;
						BYTE byteValue[4];
					};
					convertUnion ConvertUnion;
					ConvertUnion.byteValue[0] = bBuff[6];
					ConvertUnion.byteValue[1] = bBuff[5];
					ConvertUnion.byteValue[2] = bBuff[4];
					ConvertUnion.byteValue[3] = bBuff[3];
					pMainframe->m_Struct_CommRet.fLowPowerVoltage = ConvertUnion.fValue;
					//CString strLog = L"";
					//strLog.Format(L"电压%.1f", ConvertUnion.fValue);
					//SLOG1(strLog);
					ConvertUnion.byteValue[0] = bBuff[10];
					ConvertUnion.byteValue[1] = bBuff[9];
					ConvertUnion.byteValue[2] = bBuff[8];
					ConvertUnion.byteValue[3] = bBuff[7];
					pMainframe->m_Struct_CommRet.fLowPowerCurrent = ConvertUnion.fValue;
					//strLog = L"";
					//strLog.Format(L"电流%.1f", ConvertUnion.fValue);
					//SLOG1(strLog);

					bSuccess = TRUE;
					break;
				}
			}
		}
		//读打火标记线圈
		if (bSuccess == FALSE)
		{
			SLOG1(L"低压源读取电流电压失败");
			g_comm_lowpowerthread_mutex.unlock();
			continue;
		}
//#ifdef SHOW_LOG
//		SLOG1(L"MonitorLowPowerTread-读电流电压end");
//		SLOG1(L"MonitorLowPowerTread-读取低压源输出状态Start");
//#endif
		//this_thread::sleep_for(30ms);
		//读取低压源输出状态
		g_comm_lowpowerthread_mutex.unlock();
		this_thread::sleep_for(50ms);
		g_comm_lowpowerthread_mutex.lock();
		memset(SendByte, 0, 500);
		SendByte[0] = 0x01;
		SendByte[1] = 0x01;
		SendByte[2] = 0x05;
		SendByte[3] = 0x13;
		SendByte[4] = 0x00;
		SendByte[5] = 0x01;
		nWord = pMainframe->CRC16(SendByte, 6);
		SendByte[6] = nWord % 0x0100;
		SendByte[7] = nWord / 0x0100;
		dwWritten = 0;
		bSuccess = FALSE;
		sendnum = pMainframe->m_Serial_LowPower.Write(SendByte, 8, &dwWritten, 0, 500);
		if (dwWritten <= 0)
		{
			g_comm_lowpowerthread_mutex.unlock();
			return -1;
		}
		nReadNum = 0, nReadNumTemp = 0;
		/*static*/
		ntimes = 0;
		memset(bBuff, 0x00, 64);
		lLastError = ERROR_SUCCESS;
		bFirstTime = TRUE;
		while ((ntimes < 20/*最大延迟0.5s*/)/* && (nReadNum > 2)*/)
		{
			lLastError = pMainframe->m_Serial_LowPower.Read(&bBuff[nReadNum], 64, &nReadNumTemp);
			nReadNum += nReadNumTemp;
			CString strreceive = _T(""), strtemp = _T("");
			if (nReadNum <= 0)
			{//100ms不回
				ntimes++;
				this_thread::sleep_for(10ms);
				continue;
			}
			if (nReadNum < 6)
			{
				ntimes++;
				if (bFirstTime)
				{
					ntimes = 1;
					bFirstTime = FALSE;
				}
				if (nReadNum > 2)
				{
					if (bBuff[1] != 0x01)
					{//读取失败
						pMainframe->m_Serial_LowPower.Purge();
						break;
					}
				}
				this_thread::sleep_for(50ms);
				continue;
			}
			else if (nReadNum == 6)
			{
				if (bBuff[1] == 0x01)
				{//读取成功
					if ((bBuff[3] & 0x01) == 0x01)
					{//输出状态关断
						pMainframe->m_Struct_CommRet.bLowPowerStop = TRUE;
					}
					else
					{
						pMainframe->m_Struct_CommRet.bLowPowerStop = FALSE;
					}
					bSuccess = TRUE;
					break;
				}
			}
		}

		if (bSuccess == FALSE)
		{
			SLOG1(L"读低压源状态失败");
		}
#ifdef SHOW_LOG
		//SLOG1(L"MonitorLowPowerTread-读取低压源输出状态End");
#endif
		g_comm_lowpowerthread_mutex.unlock();
	}
	return 0;
}

UINT MonitorMcuTread(LPVOID lParam)
{
	CMainFrame* pMainframe = (CMainFrame*)lParam;
	CXrayDCDeburringView* pView = (CXrayDCDeburringView*)pMainframe->GetActiveView();

	while (true)
	{
		if (pMainframe->m_bStopMcuThread)
		{
			break;
		}
		this_thread::sleep_for(100ms);
		g_comm_mcuthread_mutex.lock();

		//读电流电压01 03 00 00 00 0A C5 CD
		BYTE SendByte[500];
		memset(SendByte, 0, 500);
		SendByte[0] = 0x01;
		SendByte[1] = 0x03;
		SendByte[2] = 0x00;
		SendByte[3] = 0x00;
		SendByte[4] = 0x00;
		SendByte[5] = 0x0A;
		WORD nWord = pMainframe->CRC16(SendByte, 6);
		SendByte[6] = nWord % 0x0100;
		SendByte[7] = nWord / 0x0100;
		DWORD dwWritten = 0;
		LONG sendnum = pMainframe->m_Serial_MCU.Write(SendByte, 8, &dwWritten, 0, 500);
		if (dwWritten <= 0)
		{
			g_comm_mcuthread_mutex.unlock();
			return -1;
		}
		DWORD nReadNum = 0, nReadNumTemp = 0;
		/*static*/
		int ntimes = 0;
		BYTE bBuff[64];
		memset(bBuff, 0x00, 64);
		LONG    lLastError = ERROR_SUCCESS;
		BOOL bFirstTime = TRUE;
		//01 03 14 55 55 55 55 55 55 55 55 55 55 55 55 00 00 00 00 00 00 00 00 6F 61
		while ((ntimes < 20/*最大延迟0.5s*/)/* && (nReadNum > 2)*/)
		{
			lLastError = pMainframe->m_Serial_MCU.Read(&bBuff[nReadNum], 64, &nReadNumTemp);
			nReadNum += nReadNumTemp;
			CString strreceive = _T(""), strtemp = _T("");
			if (nReadNum <= 0)
			{
				this_thread::sleep_for(10ms);
				ntimes++;
				continue;
			}
			if (nReadNum < 25)
			{
				ntimes++;
				if (bFirstTime)
				{
					ntimes = 1;
					bFirstTime = FALSE;
				}
				if (nReadNum > 2)
				{
					if (bBuff[1] != 0x03)
					{//读取失败
						pMainframe->m_Serial_LowPower.Purge();
						break;
					}
				}
				this_thread::sleep_for(50ms);
				continue;
			}
			else if (nReadNum == 25)
			{
				if (bBuff[1] == 0x03)
				{//读取成功
					for (int i = 0; i < 4; i++)
					{
						if (bBuff[9 + i] == 0xAA)
						{
							pMainframe->m_Struct_CommRet.bMcuFire[i] = TRUE;
						}
						else
						{
							pMainframe->m_Struct_CommRet.bMcuFire[i] = FALSE;
						}
						//pMainframe->m_Struct_CommRet.bMcuFire[i] = (bBuff[9 + i] == 0xAA ? TRUE : FALSE);
						pMainframe->m_Struct_CommRet.nMcuCurrent[i] = bBuff[15 + 2 * i] * 0x100 + bBuff[16 + 2 * i];
						if (pMainframe->m_Struct_CommRet.bMcuFire[i] == TRUE && pMainframe->m_Struct_CommRetOld.bMcuFire[i] == FALSE && pMainframe->m_bFireFlag == FALSE)
						{//发生打火
					
							//保存数据
							STRUCT_FIRERECORD structRecord;
							structRecord.nStep = pMainframe->m_Struct_BurningTest_Param.nCurrentStep;
							CTime tCurrentTime = CTime::GetCurrentTime();
							if (structRecord.nStep>=1 && (pMainframe->m_struct_CurrentProgram.VectProgItem.size() > structRecord.nStep - 1) && pMainframe->m_pThread_TestThread!=NULL && pMainframe->m_pThread_TestThread->m_hThread != NULL)
							{
								CTimeSpan timespan = tCurrentTime - pMainframe->m_struct_CurrentProgram.VectProgItem.at(structRecord.nStep - 1).tStartTime;
								structRecord.strFileMoment.Format(L"%d", timespan.GetTotalSeconds());
								//structRecord.strVoltage.Format(pMainframe->m_struct_CurrentProgram.VectProgItem.at(structRecord.nStep - 1).nHightVotage);
								structRecord.strVoltage.Format(L"%.1f/%.1f", pMainframe->m_struct_CurrentProgram.VectProgItem.at(structRecord.nStep - 1).nHightVotage ,pMainframe->m_Struct_CommRetOld.fHiPowerVoltage);
								structRecord.strVoltageFire.Format(L"%.1f", pMainframe->m_Struct_CommRetOld.fHiPowerVoltage);
								structRecord.strTime = tCurrentTime.Format(L"%Y-%m-%d %H:%M:%S");
								for (int j = 0; j < 4; j++)
								{
									structRecord.strFireCurrentCH[j].Format(L"%d", pMainframe->m_Struct_CommRet.nMcuCurrent[j]);
								}

								structRecord.strTubeSn = pMainframe->m_Struct_BurningTest_Param.strTubeSn;
								structRecord.strTubeType = pMainframe->m_Struct_BurningTest_Param.strTubeType;
								pMainframe->m_Vect_FireRecord.push_back(structRecord);
								pMainframe->m_dbControl.WriteDataBase(COMMAND_TYPE_FIRERECORD_SAVE, &structRecord, NULL);
								pMainframe->m_bFireFlag = TRUE;
								pMainframe->PostMessage(WM_EXECUTECOMMAND, EXECUTE_COMMAND_RESETFIRE);
								pMainframe->m_wndOutput.GetFireOutPutWnd()->PostMessage(WM_UPDATE_FIRERECORD, 1, NULL);
							}

							break;
						}

					}

					pMainframe->m_Struct_CommRetOld = pMainframe->m_Struct_CommRet;
					break;
				}
			}
		}
		g_comm_mcuthread_mutex.unlock();

	}
	return 0;
}

UINT MonitorShiboqiTread(LPVOID lParam)
{
	CMainFrame* pMainframe = (CMainFrame*)lParam;
	CXrayDCDeburringView* pView = (CXrayDCDeburringView*)pMainframe->GetActiveView();
	while (true)
	{
		this_thread::sleep_for(200ms);
		if (pMainframe->m_bStopShiboqiThread)
		{
			break;
		}
	}
	return 0;
}

BOOL CMainFrame::InitializeHightPower()
{
	LONG    lLastError = ERROR_SUCCESS;
	
	CString strCom = m_strHightPower_Com;
	if (m_strHightPower_Com.GetLength()>4)
	{
		strCom.Format(_T("\\\\.\\%s"), m_strHightPower_Com);
	}
	if (m_Serial_HightPower.Open(strCom, NULL, NULL, TRUE) != ERROR_SUCCESS)
	{
		return FALSE;
	}
	// 关闭默认打开时就监听
// 	if (m_Serial.StopListener(200) != ERROR_SUCCESS)
// 	{
// 		return COM_ERROR_STOPLISTEN;
// 	}
	lLastError = m_Serial_HightPower.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
	if (lLastError != ERROR_SUCCESS)
	{
		return FALSE;
	}
	//lLastError = m_Serial.Setup(CSerial::EBaud115200, CSerial::EData8, CSerial::EParNone, CSerial::EStop1);
	lLastError = m_Serial_HightPower.Setup((CSerial::EBaudrate)m_nBaudRate_HightPower,
		(CSerial::EDataBits)8,
		(CSerial::EParity)CSerial::EParNone,
		CSerial::EStop1);
	if (lLastError != ERROR_SUCCESS)
	{
		return FALSE;
	}

	DWORD dwEvents = 0;
	dwEvents |= CSerial::EEventBreak;
	// 	dwEvents |= CSerial::EEventCTS;
	// 	dwEvents |= CSerial::EEventDSR;
	dwEvents |= CSerial::EEventError;
	dwEvents |= CSerial::EEventRcvEv;
	dwEvents |= CSerial::EEventRecv;
	// 	dwEvents |= CSerial::EEventRing;
	// 	dwEvents |= CSerial::EEventRLSD;
	dwEvents |= CSerial::EEventSend;
	// Setup handshaking
	if (m_Serial_HightPower.SetMask(dwEvents))
		return FALSE;
	return TRUE;
}
BOOL CMainFrame::InitializeLowPower()
{
	LONG    lLastError = ERROR_SUCCESS;

	CString strCom = m_strLowPower_Com;
	if (m_strLowPower_Com.GetLength() > 4)
	{
		strCom.Format(_T("\\\\.\\%s"), m_strLowPower_Com);
	}

	if (m_Serial_LowPower.Open(strCom, NULL, NULL, TRUE) != ERROR_SUCCESS)
	{
		SLOG1(strCom + L"打开失败");
		return FALSE;
	}
	// 关闭默认打开时就监听
// 	if (m_Serial.StopListener(200) != ERROR_SUCCESS)
// 	{
// 		return COM_ERROR_STOPLISTEN;
// 	}
	lLastError = m_Serial_LowPower.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
	if (lLastError != ERROR_SUCCESS)
	{
		SLOG1(strCom + L"SetupReadTimeouts失败");
		return FALSE;
	}
	//lLastError = m_Serial.Setup(CSerial::EBaud115200, CSerial::EData8, CSerial::EParNone, CSerial::EStop1);
	lLastError = m_Serial_LowPower.Setup((CSerial::EBaudrate)m_nBaudRate_LowPower,
		(CSerial::EDataBits)8,
		(CSerial::EParity)CSerial::EParNone,
		CSerial::EStop1);
	if (lLastError != ERROR_SUCCESS)
	{
		SLOG1(strCom + L"Setup失败");
		return FALSE;
	}

	DWORD dwEvents = 0;
	dwEvents |= CSerial::EEventBreak;
	// 	dwEvents |= CSerial::EEventCTS;
	// 	dwEvents |= CSerial::EEventDSR;
	dwEvents |= CSerial::EEventError;
	dwEvents |= CSerial::EEventRcvEv;
	dwEvents |= CSerial::EEventRecv;
	// 	dwEvents |= CSerial::EEventRing;
	// 	dwEvents |= CSerial::EEventRLSD;
	dwEvents |= CSerial::EEventSend;
	// Setup handshaking
	if (m_Serial_LowPower.SetMask(dwEvents))
		return FALSE;
	return TRUE;
}
BOOL CMainFrame::InitializeMcu()
{
	LONG    lLastError = ERROR_SUCCESS;

	CString strCom = m_strMcu_Com;
	if (m_strMcu_Com.GetLength() > 4)
	{
		strCom.Format(_T("\\\\.\\%s"), m_strMcu_Com);
	}



	if (m_Serial_MCU.Open(strCom, NULL, NULL, TRUE) != ERROR_SUCCESS)
	{
		return FALSE;
	}
	// 关闭默认打开时就监听
// 	if (m_Serial.StopListener(200) != ERROR_SUCCESS)
// 	{
// 		return COM_ERROR_STOPLISTEN;
// 	}
	lLastError = m_Serial_MCU.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
	if (lLastError != ERROR_SUCCESS)
	{
		return FALSE;
	}
	//lLastError = m_Serial.Setup(CSerial::EBaud115200, CSerial::EData8, CSerial::EParNone, CSerial::EStop1);
	lLastError = m_Serial_MCU.Setup((CSerial::EBaudrate)m_nBaudRate_MCU,
		(CSerial::EDataBits)8,
		(CSerial::EParity)CSerial::EParNone,
		CSerial::EStop1);
	if (lLastError != ERROR_SUCCESS)
	{
		return FALSE;
	}

	DWORD dwEvents = 0;
	dwEvents |= CSerial::EEventBreak;
	// 	dwEvents |= CSerial::EEventCTS;
	// 	dwEvents |= CSerial::EEventDSR;
	dwEvents |= CSerial::EEventError;
	dwEvents |= CSerial::EEventRcvEv;
	dwEvents |= CSerial::EEventRecv;
	// 	dwEvents |= CSerial::EEventRing;
	// 	dwEvents |= CSerial::EEventRLSD;
	dwEvents |= CSerial::EEventSend;
	// Setup handshaking
	if (m_Serial_MCU.SetMask(dwEvents))
		return FALSE;
	return TRUE;
}
BOOL CMainFrame::InitializeShiboqi()
{
	LONG    lLastError = ERROR_SUCCESS;

	CString strCom = m_strShiboqi_Com;
	if (m_strShiboqi_Com.GetLength() > 4)
	{
		strCom.Format(_T("\\\\.\\%s"), m_strShiboqi_Com);
	}
	if (m_Serial_Shiboqi.Open(strCom, NULL, NULL, TRUE) != ERROR_SUCCESS)
	{
		return FALSE;
	}
	// 关闭默认打开时就监听
// 	if (m_Serial.StopListener(200) != ERROR_SUCCESS)
// 	{
// 		return COM_ERROR_STOPLISTEN;
// 	}
	lLastError = m_Serial_Shiboqi.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
	if (lLastError != ERROR_SUCCESS)
	{
		return FALSE;
	}
	//lLastError = m_Serial.Setup(CSerial::EBaud115200, CSerial::EData8, CSerial::EParNone, CSerial::EStop1);
	lLastError = m_Serial_Shiboqi.Setup((CSerial::EBaudrate)m_nBaudRate_Shiboqi,
		(CSerial::EDataBits)8,
		(CSerial::EParity)CSerial::EParNone,
		CSerial::EStop1);
	if (lLastError != ERROR_SUCCESS)
	{
		return FALSE;
	}

	DWORD dwEvents = 0;
	dwEvents |= CSerial::EEventBreak;
	// 	dwEvents |= CSerial::EEventCTS;
	// 	dwEvents |= CSerial::EEventDSR;
	dwEvents |= CSerial::EEventError;
	dwEvents |= CSerial::EEventRcvEv;
	dwEvents |= CSerial::EEventRecv;
	// 	dwEvents |= CSerial::EEventRing;
	// 	dwEvents |= CSerial::EEventRLSD;
	dwEvents |= CSerial::EEventSend;
	// Setup handshaking
	if (m_Serial_Shiboqi.SetMask(dwEvents))
		return FALSE;
}
BOOL CMainFrame::CreateCommThreads()
{
	StopCommThreads();
	m_pThread_HightPower = AfxBeginThread(MonitorHightPowerTread, this);
	m_pThread_HightPower->m_bAutoDelete = FALSE;
	m_pThread_LowPower = AfxBeginThread(MonitorLowPowerTread, this);
	m_pThread_LowPower->m_bAutoDelete = FALSE;
	m_pThread_Mcu = AfxBeginThread(MonitorMcuTread, this);
	m_pThread_Mcu->m_bAutoDelete = FALSE;
	m_pThread_Shiboqi = AfxBeginThread(MonitorShiboqiTread, this);
	m_pThread_Shiboqi->m_bAutoDelete = FALSE;
	return TRUE;
}
BOOL CMainFrame::StopCommThreads()
{
	m_bStopHightPowerThread = TRUE;
	if (m_pThread_HightPower != NULL)
	{
		::WaitForSingleObject(m_pThread_HightPower->m_hThread, INFINITE);

		delete  m_pThread_HightPower;
		m_pThread_HightPower = NULL;
	}
	m_bStopHightPowerThread = FALSE;
	
	m_bStopLowPowerThread = TRUE;
	if (m_pThread_LowPower != NULL)
	{
		::WaitForSingleObject(m_pThread_LowPower->m_hThread, INFINITE);

		delete  m_pThread_LowPower;
		m_pThread_LowPower = NULL;
	}
	m_bStopLowPowerThread = FALSE;

	m_bStopMcuThread = TRUE;
	if (m_pThread_Mcu != NULL)
	{
		::WaitForSingleObject(m_pThread_Mcu->m_hThread, INFINITE);

		delete  m_pThread_Mcu;
		m_pThread_Mcu = NULL;
	}
	m_bStopMcuThread = FALSE;

	m_bStopShiboqiThread = TRUE;
	if (m_pThread_Shiboqi != NULL)
	{
		::WaitForSingleObject(m_pThread_Shiboqi->m_hThread, INFINITE);

		delete  m_pThread_Shiboqi;
		m_pThread_Shiboqi = NULL;
	}
	m_bStopShiboqiThread = FALSE;
	return TRUE;
}

void CMainFrame::OnToolbarDeviceLink()
{
	// TODO: 在此添加命令处理程序代码

	if (m_Struct_CommStatus.nHightPowerStatus == 1)
	{
		if (m_pThread_TestThread != NULL && m_pThread_TestThread->m_hThread != NULL&&m_bBeginGetValue)
		{
			if (AfxMessageBox(L"正在测试是否结束？", MB_YESNO) == IDYES)
			{
				SLOG1(L"停止测试");
				for (int i = 0; i < 3; i++)
				{
					if (StopLowpowerOut())
					{
						break;
					}
				}
				for (int i = 0; i < 3; i++)
				{
					if (StopHipowerOut())
					{
						break;
					}
				}
				StopTestThread();
			}
			else
			{
				return;
			}

			if (AfxMessageBox(L"是否保存测试记录？", MB_YESNO) == IDYES)
			{
				SaveTestRecode();
			}
		}
		StopCommThreads();
		if (m_Serial_HightPower.IsOpen())
		{
			m_Serial_HightPower.Close();
		}
		if (m_Serial_LowPower.IsOpen())
		{
			m_Serial_LowPower.Close();
		}
		if (m_Serial_MCU.IsOpen())
		{
			m_Serial_MCU.Close();
		}
		m_Struct_CommStatus.nHightPowerStatus = 0;
		m_Struct_CommStatus.nLowPowerstatus = 0;
		m_Struct_CommStatus.nMcuStatus = 0;
	}
	else
	{
		if (!InitializeHightPower())
		{
			AfxMessageBox(L"高压源连接失败!");
			return;
		}
		m_Struct_CommStatus.nHightPowerStatus = 1;
		if (!InitializeLowPower())
		{
			AfxMessageBox(L"低压源连接失败!");
			if (m_Serial_HightPower.IsOpen())
			{
				m_Serial_HightPower.Close();
			}
			m_Struct_CommStatus.nHightPowerStatus = 0;
			return;
		}
		m_Struct_CommStatus.nLowPowerstatus = 1;
		if (!InitializeMcu())
		{
			AfxMessageBox(L"MCU连接失败!");
			if (m_Serial_HightPower.IsOpen())
			{
				m_Serial_HightPower.Close();
			}
			if (m_Serial_LowPower.IsOpen())
			{
				m_Serial_LowPower.Close();
			}
			m_Struct_CommStatus.nHightPowerStatus = 0;
			m_Struct_CommStatus.nLowPowerstatus = 0;
			return;
		}
		m_Struct_CommStatus.nMcuStatus = 1;
		//if (!InitializeShiboqi())
		//{
		//	AfxMessageBox(L"示波器连接失败!");
		//	if (m_Serial_HightPower.IsOpen())
		//	{
		//		m_Serial_HightPower.Close();
		//	}
		//	if (m_Serial_LowPower.IsOpen())
		//	{
		//		m_Serial_LowPower.Close();
		//	}

		//	if (m_Serial_MCU.IsOpen())
		//	{
		//		m_Serial_MCU.Close();
		//	}
		//	return;
		//}
		m_Struct_CommStatus.nShiboqiStatus = 1;
		CreateCommThreads();
	}



}


void CMainFrame::OnUpdateToolbarDeviceLink(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable();
	if (m_Struct_CommStatus.nMcuStatus == 1 && m_Struct_CommStatus.nShiboqiStatus == 1 &&
		m_Struct_CommStatus.nHightPowerStatus && m_Struct_CommStatus.nLowPowerstatus == 1)
	{
		pCmdUI->SetCheck(TRUE);
		pCmdUI->SetText(L"断开连接");
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->SetText(L"设备连接");
	}
}
uint8_t CMainFrame::get_check_sum(uint8_t* buf, uint8_t len)
{
	uint8_t i = 0;
	uint8_t chk_sum = 0;
	for (i = 0; i < len; i++)
	{
		chk_sum += (buf[i] ^ rnd_tbl[buf[i]]);
	} 
	return chk_sum;
}

UINT BurningTestTread(LPVOID lParam)
{

	CMainFrame* pMainframe = (CMainFrame*)lParam;
	if (pMainframe == NULL)
	{
		return 0;
	}
	VECT_PROGRAMITEM *VectProgItem = &pMainframe->m_struct_CurrentProgram.VectProgItem;		// 获取老化流程表格中的数据
//#ifdef SHOW_LOG
	//SLOG1(L"BurningTestTread");
//#endif
	CTime tCurrentTime = CTime::GetCurrentTime();   //记录当前的时间
	for (int i = 0;i<VectProgItem->size();i++)
	{
		if (VectProgItem->at(i).nStep < pMainframe->m_Struct_BurningTest_Param.nStartStep)
		{
			VectProgItem->at(i).tStartTime = tCurrentTime;
			VectProgItem->at(i).bFinish = TRUE;
		}
		else if (VectProgItem->at(i).nStep == pMainframe->m_Struct_BurningTest_Param.nStartStep)
		{
			VectProgItem->at(i).tStartTime = tCurrentTime;
			int nvoltage = VectProgItem->at(i).nHightVotage * 10;
			pMainframe->SetHipowerVoltage(nvoltage);
			pMainframe->m_Struct_BurningTest_Param.nCurrentStep = VectProgItem->at(i).nStep;
			VectProgItem->at(i).bFinish = FALSE;
		}
		else
		{
			VectProgItem->at(i).bFinish = FALSE;
		}
	}
	while (TRUE)
	{
		bool over = false;
		this_thread::sleep_for(1000ms);
		if (pMainframe->m_bStopTestThread)        
		{
			pMainframe->m_bStopTestThread = FALSE;  
			break;
		}
		//if (pMainframe->m_bAgingAuto)             
		{
			CTime tCurrentTime = CTime::GetCurrentTime();   //记录当前的时间
			int setsize = VectProgItem->size();             //记录老化的条数
			if (setsize <= 0)
			{
				pMainframe->m_bStopTestThread = FALSE;
				break;
			}

			for (int i = 0; i < VectProgItem->size(); i++)
			{
				if (pMainframe->m_bStopTestThread)        //
				{
					pMainframe->m_bStopTestThread = FALSE;  
					break;
				}
				int size = VectProgItem->size();    //得到老化的条数
				if (VectProgItem->at(i).bFinish == FALSE)   //判断第 i 个是否已经老化完成 没有完成就进入老化
				{
					if (pMainframe->m_bFireFlag)
					{
						int nGobackTo = 0;
						for (int j = 0; j < pMainframe->m_Struct_BurningTest_Param.nFireBackSteps; j++)
						{
							if (i-j-1 >= 0)
							{
								VectProgItem->at(i - 1 - j).bFinish = FALSE;
								VectProgItem->at(i - 1 - j).tStartTime = tCurrentTime;
								nGobackTo = i - 1 - j;
							}
						}
						if (VectProgItem->size() > nGobackTo)
						{
							//设置高压
							int nvoltage = VectProgItem->at(nGobackTo).nHightVotage * 10;
							pMainframe->StopHipowerOut();
							pMainframe->StopLowpowerOut();
							pMainframe->InitializeTest();
							pMainframe->SetHipowerVoltage(nvoltage);
							pMainframe->m_Struct_BurningTest_Param.nCurrentStep = VectProgItem->at(nGobackTo).nStep;
							pMainframe->m_Struct_BurningTest_Param.nTimeLong = 0;
						}
						pMainframe->m_bFireFlag = FALSE;
						break;
					}
					CTimeSpan timespan = tCurrentTime - VectProgItem->at(i).tStartTime;
                    if (timespan.GetTotalSeconds() >= VectProgItem->at(i).nKeepTime)
					{
						//保存数据
						STRUCT_FIRERECORD structRecord;
						structRecord.nStep = pMainframe->m_Struct_BurningTest_Param.nCurrentStep;
						CTime tCurrentTime = CTime::GetCurrentTime();
						if (structRecord.nStep >= 1 && pMainframe->m_pThread_TestThread != NULL && pMainframe->m_pThread_TestThread->m_hThread != NULL)
						{
							CTimeSpan timespan = tCurrentTime - pMainframe->m_struct_CurrentProgram.VectProgItem.at(i).tStartTime;
							structRecord.strFileMoment=L"";
							//structRecord.strVoltage.Format(pMainframe->m_struct_CurrentProgram.VectProgItem.at(i).nHightVotage);
							structRecord.strVoltage.Format(L"%.1f/%.1f", pMainframe->m_struct_CurrentProgram.VectProgItem.at(i).nHightVotage, pMainframe->m_Struct_CommRetOld.fHiPowerVoltage);
							structRecord.strVoltageFire=L"";
							structRecord.strTime = tCurrentTime.Format(L"%Y-%m-%d %H:%M:%S");
							for (int j = 0; j < 4; j++)
							{
								structRecord.strFireCurrentCH[j].Format(L"%d", pMainframe->m_Struct_CommRetOld.nMcuCurrent[j]);
							}

							structRecord.strTubeSn = pMainframe->m_Struct_BurningTest_Param.strTubeSn;
							structRecord.strTubeType = pMainframe->m_Struct_BurningTest_Param.strTubeType;
							pMainframe->m_Vect_FireRecord.push_back(structRecord);
							pMainframe->m_dbControl.WriteDataBase(COMMAND_TYPE_FIRERECORD_SAVE, &structRecord, NULL);
							pMainframe->m_wndOutput.GetFireOutPutWnd()->PostMessage(WM_UPDATE_FIRERECORD,1,NULL );
						}



						VectProgItem->at(i).bFinish = TRUE;
						if (i == VectProgItem->size() - 1)
						{
							pMainframe->m_Struct_BurningTest_Param.nFinishTimes += 1;
							if (pMainframe->m_Struct_BurningTest_Param.nFinishTimes >= pMainframe->m_Struct_BurningTest_Param.nRepeatTest_Times)
							{
								over = true;
								SLOG1(L"完成测试");
								pMainframe->m_bBeginGetValue = FALSE;
								break;
							}
							else
							{
								for (int k = 0; k < VectProgItem->size(); k++)
								{
									if (VectProgItem->at(k).nStep < pMainframe->m_Struct_BurningTest_Param.nStartStep)
									{
										VectProgItem->at(k).tStartTime = tCurrentTime;
										VectProgItem->at(k).bFinish = TRUE;
									}
									else if (VectProgItem->at(k).nStep == pMainframe->m_Struct_BurningTest_Param.nStartStep)
									{
										VectProgItem->at(k).bFinish = FALSE;
										VectProgItem->at(k).tStartTime = tCurrentTime;
										int nvoltage = VectProgItem->at(k).nHightVotage * 10;
										pMainframe->SetHipowerVoltage(nvoltage);
										pMainframe->m_Struct_BurningTest_Param.nCurrentStep = VectProgItem->at(k).nStep;
									}
									else
									{
										VectProgItem->at(k).bFinish = FALSE;
									}
									
								}
								
								break;
							}
						}
						if (VectProgItem->size() > i + 1) //
						{
							VectProgItem->at(i+1).tStartTime = tCurrentTime;
                            //设置高压
							int nvoltage = VectProgItem->at(i+1).nHightVotage * 10;
							pMainframe->SetHipowerVoltage(nvoltage);
							pMainframe->m_Struct_BurningTest_Param.nCurrentStep = VectProgItem->at(i+1).nStep;
							timespan = tCurrentTime - VectProgItem->at(i+1).tStartTime;
							pMainframe->m_Struct_BurningTest_Param.nTimeLong = timespan.GetTotalSeconds();
						}
					
					}
					else
					{
						pMainframe->m_Struct_BurningTest_Param.nTimeLong = timespan.GetTotalSeconds();
						//CString strLog = L"";
						//strLog.Format(L"nTimeLong = %d", pMainframe->m_Struct_BurningTest_Param.nTimeLong);
						//SLOG1(strLog);
					}
					
					break;

		
				}
				else
				{

				}
			}
		}
		if (over == true)
		{
			pMainframe->PostMessage(WM_EXECUTECOMMAND, EXECUTE_COMMAND_FINISH);
			break;
		}
	}

	return 0;
}


BOOL CMainFrame::SetLowpowerLimit(int nLowpowerLimit)
{//01 10 0A 01 00 02 04 44 96 00 00 B8 1F 
	g_comm_lowpowerthread_mutex.lock();

	BOOL bSuccess = FALSE;
	//设置电流电压
	BYTE SendByte[500];
	memset(SendByte, 0, 500);
	SendByte[0] = 0x01;
	SendByte[1] = 0x10;
	SendByte[2] = 0x0A;
	SendByte[3] = 0x01;
	SendByte[4] = 0x00;
	SendByte[5] = 0x02;
	SendByte[6] = 0x04;

	union convertUnion
	{
		float fValue;
		BYTE byteValue[4];
	};
	convertUnion convert;
	convert.fValue = nLowpowerLimit;

	SendByte[7] = convert.byteValue[3];
	SendByte[8] = convert.byteValue[2];
	SendByte[9] = convert.byteValue[1];
	SendByte[10] = convert.byteValue[0];
	WORD nWord = CRC16(SendByte, 11);
	SendByte[11] = nWord % 0x0100;
	SendByte[12] = nWord / 0x0100;

	//CString strLog = L"";
	//for (int i = 0;i < 13; i++)
	//{
	//	CString strTemp = L"";
	//	strTemp.Format(L"%02x", SendByte[i]);
	//	if (strLog == L"")
	//	{
	//		strLog = strTemp;
	//	}
	//	else
	//	{
	//		strLog += L","+strTemp;
	//	}

	//}
	//SLOG1(strLog);
	DWORD dwWritten = 0;
	LONG sendnum = m_Serial_LowPower.Write(SendByte, 13, &dwWritten, 0, 500);
	if (dwWritten <= 0)
	{
		g_comm_lowpowerthread_mutex.unlock();
		return FALSE;
	}
	DWORD nReadNum = 0, nReadNumTemp = 0;
	/*static*/
	int ntimes = 0;
	BYTE bBuff[64];
	memset(bBuff, 0x00, 64);
	LONG    lLastError = ERROR_SUCCESS;
	BOOL bFirstTime = TRUE;


	//01 10 0A 01 00 02 13 D0 
	while ((ntimes < 20/*最大延迟0.5s*/)/* && (nReadNum > 2)*/)
	{
		lLastError = m_Serial_LowPower.Read(&bBuff[nReadNum], 64, &nReadNumTemp);
		nReadNum += nReadNumTemp;
		CString strreceive = _T(""), strtemp = _T("");
		if (nReadNum <= 0)
		{
			ntimes++;
			this_thread::sleep_for(10ms);
			continue;
		}
		if (nReadNum < 8)
		{
			ntimes++;
			if (bFirstTime)
			{
				ntimes = 1;
				bFirstTime = FALSE;
			}
			if (nReadNum > 2)
			{
				if (bBuff[1] != 0x10)
				{//读取失败
					m_Serial_LowPower.Purge();
					break;
				}
			}
			this_thread::sleep_for(50ms);
			continue;
		}
		else if (nReadNum == 8)
		{
			if (bBuff[1] == 0x10)
			{//读取成功
				bSuccess = TRUE;
				break;
			}
		}
	}

	//strLog = L"";
	//for (int i = 0; i < 8; i++)
	//{
	//	CString strTemp = L"";
	//	strTemp.Format(L"%02x", bBuff[i]);
	//	if (strLog == L"")
	//	{
	//		strLog = strTemp;
	//	}
	//	else
	//	{
	//		strLog += L"," + strTemp;
	//	}

	//}
	//SLOG1(strLog);

	g_comm_lowpowerthread_mutex.unlock();
	return bSuccess;
}
BOOL CMainFrame::SetLowpowerCurrent(int nLowpowerCurrent)
{
	g_comm_lowpowerthread_mutex.lock();

	BOOL bSuccess = FALSE;
	//读电流电压
	BYTE SendByte[500];
	memset(SendByte, 0, 500);
	SendByte[0] = 0x01;
	SendByte[1] = 0x10;
	SendByte[2] = 0x0A;
	SendByte[3] = 0x07;
	SendByte[4] = 0x00;
	SendByte[5] = 0x02;
	SendByte[6] = 0x04;

	union convertUnion
	{
		float fValue;
		BYTE byteValue[4];
	};
	convertUnion convert;
	convert.fValue = nLowpowerCurrent;
	SendByte[7] = convert.byteValue[3];
	SendByte[8] = convert.byteValue[2];
	SendByte[9] = convert.byteValue[1];
	SendByte[10] = convert.byteValue[0];
	WORD nWord = CRC16(SendByte, 11);
	SendByte[11] = nWord % 0x0100;
	SendByte[12] = nWord / 0x0100;
	DWORD dwWritten = 0;
	LONG sendnum = m_Serial_LowPower.Write(SendByte, 13, &dwWritten, 0, 500);
	if (dwWritten <= 0)
	{
		g_comm_lowpowerthread_mutex.unlock();
		return FALSE;
	}
	DWORD nReadNum = 0, nReadNumTemp = 0;
	/*static*/
	int ntimes = 0;
	BYTE bBuff[64];
	memset(bBuff, 0x00, 64);
	LONG    lLastError = ERROR_SUCCESS;
	BOOL bFirstTime = TRUE;


	//01 10 0A 01 00 02 13 D0 
	while ((ntimes < 20/*最大延迟0.5s*/)/* && (nReadNum > 2)*/)
	{
		lLastError = m_Serial_LowPower.Read(&bBuff[nReadNum], 64, &nReadNumTemp);
		nReadNum += nReadNumTemp;
		CString strreceive = _T(""), strtemp = _T("");
		if (nReadNum <= 0)
		{
			ntimes++;
			this_thread::sleep_for(10ms);
			continue;
		}
		if (nReadNum < 8)
		{
			ntimes++;
			if (bFirstTime)
			{
				ntimes = 1;
				bFirstTime = FALSE;
			}
			if (nReadNum > 2)
			{
				if (bBuff[1] != 0x10)
				{//读取失败
					m_Serial_LowPower.Purge();
					break;
				}
			}
			this_thread::sleep_for(50ms);
			continue;
		}
		else if (nReadNum == 8)
		{
			if (bBuff[1] == 0x10)
			{//读取成功
				bSuccess = TRUE;
				break;
			}
		}
	}

	if (bSuccess == FALSE)
	{
		SLOG1(L"设置低压源电压失败!");
		g_comm_lowpowerthread_mutex.unlock();
		return FALSE;
	}
	//使能电压01 10 0A 00 00 01 02 00 06 8C 52
	//01 10 0A 00 00 01 02 11
	bSuccess = FALSE;
	memset(SendByte, 0, 500);
	SendByte[0] = 0x01;
	SendByte[1] = 0x10;
	SendByte[2] = 0x0A;
	SendByte[3] = 0x00;
	SendByte[4] = 0x00;
	SendByte[5] = 0x01;
	SendByte[6] = 0x02;
	SendByte[7] = 0x00;
	SendByte[8] = 0x02;
	nWord = CRC16(SendByte, 9);
	SendByte[9] = nWord % 0x0100;
	SendByte[10] = nWord / 0x0100;
	dwWritten = 0;
	sendnum = m_Serial_LowPower.Write(SendByte, 11, &dwWritten, 0, 500);
	if (dwWritten <= 0)
	{
		g_comm_lowpowerthread_mutex.unlock();
		return FALSE;
	}
	nReadNum = 0, nReadNumTemp = 0;
	/*static*/
	ntimes = 0;
	bBuff[64];
	memset(bBuff, 0x00, 64);
	lLastError = ERROR_SUCCESS;
	bFirstTime = TRUE;


	//01 10 0A 01 00 02 13 D0 
	while ((ntimes < 20/*最大延迟0.5s*/)/* && (nReadNum > 2)*/)
	{
		lLastError = m_Serial_LowPower.Read(&bBuff[nReadNum], 64, &nReadNumTemp);
		nReadNum += nReadNumTemp;
		CString strreceive = _T(""), strtemp = _T("");
		if (nReadNum <= 0)
		{
			ntimes++;
			this_thread::sleep_for(10ms);
			continue;
		}
		if (nReadNum < 8)
		{
			ntimes++;
			if (bFirstTime)
			{
				ntimes = 1;
				bFirstTime = FALSE;
			}
			if (nReadNum > 2)
			{
				if (bBuff[1] != 0x10)
				{//读取失败
					m_Serial_LowPower.Purge();
					break;
				}
			}
			this_thread::sleep_for(50ms);
			continue;
		}
		else if (nReadNum == 8)
		{
			if (bBuff[1] == 0x10)
			{//读取成功
				bSuccess = TRUE;
				break;
			}
		}
	}
	if (bSuccess == FALSE)
	{
		SLOG1(L"使能低压源电压失败!");
	}
	g_comm_lowpowerthread_mutex.unlock();
	return bSuccess;
}
BOOL CMainFrame::SetLowpowerVoltage(int nLowpowerVoltage)
{
	g_comm_lowpowerthread_mutex.lock();

	BOOL bSuccess = FALSE;
	//读电流电压
	BYTE SendByte[500];
	memset(SendByte, 0, 500);
	SendByte[0] = 0x01;
	SendByte[1] = 0x10;
	SendByte[2] = 0x0A;
	SendByte[3] = 0x05;
	SendByte[4] = 0x00;
	SendByte[5] = 0x02;
	SendByte[6] = 0x04;
	union convertUnion
	{
		float fValue;
		BYTE byteValue[4];
	};
	convertUnion convert;
	convert.fValue = nLowpowerVoltage;

	SendByte[7] = convert.byteValue[3];
	SendByte[8] = convert.byteValue[2];
	SendByte[9] = convert.byteValue[1];
	SendByte[10] = convert.byteValue[0];
	WORD nWord = CRC16(SendByte, 11);
	SendByte[11] = nWord % 0x0100;
	SendByte[12] = nWord / 0x0100;
	DWORD dwWritten = 0;
	LONG sendnum = m_Serial_LowPower.Write(SendByte, 13, &dwWritten, 0, 500);
	if (dwWritten <= 0)
	{
		g_comm_lowpowerthread_mutex.unlock();
		return FALSE;
	}
	DWORD nReadNum = 0, nReadNumTemp = 0;
	/*static*/
	int ntimes = 0;
	BYTE bBuff[64];
	memset(bBuff, 0x00, 64);
	LONG    lLastError = ERROR_SUCCESS;
	BOOL bFirstTime = TRUE;


	//01 10 0A 01 00 02 13 D0 
	while ((ntimes < 20/*最大延迟0.5s*/)/* && (nReadNum > 2)*/)
	{
		lLastError = m_Serial_LowPower.Read(&bBuff[nReadNum], 64, &nReadNumTemp);
		nReadNum += nReadNumTemp;
		CString strreceive = _T(""), strtemp = _T("");
		if (nReadNum <= 0)
		{
			ntimes++;
			this_thread::sleep_for(10ms);
			continue;
		}
		if (nReadNum < 8)
		{
			ntimes++;
			if (bFirstTime)
			{
				ntimes = 1;
				bFirstTime = FALSE;
			}
			if (nReadNum > 2)
			{
				if (bBuff[1] != 0x10)
				{//读取失败
					m_Serial_LowPower.Purge();
					break;
				}
			}
			this_thread::sleep_for(50ms);
			continue;
		}
		else if (nReadNum == 8)
		{
			if (bBuff[1] == 0x10)
			{//读取成功
				bSuccess = TRUE;
				break;
			}
		}
	}
	//使能电压01 10 0A 00 00 01 02 00 06 8C 52
	//01 10 0A 00 00 01 02 11
	bSuccess = FALSE;
	memset(SendByte, 0, 500);
	SendByte[0] = 0x01;
	SendByte[1] = 0x10;
	SendByte[2] = 0x0A;
	SendByte[3] = 0x00;
	SendByte[4] = 0x00;
	SendByte[5] = 0x01;
	SendByte[6] = 0x02;
	SendByte[7] = 0x00;
	SendByte[8] = 0x06;
	nWord = CRC16(SendByte, 9);
	SendByte[9] = nWord % 0x0100;
	SendByte[10] = nWord / 0x0100;
	dwWritten = 0;
	sendnum = m_Serial_LowPower.Write(SendByte, 11, &dwWritten, 0, 500);
	if (dwWritten <= 0)
	{
		g_comm_lowpowerthread_mutex.unlock();
		return FALSE;
	}
	nReadNum = 0, nReadNumTemp = 0;
	/*static*/
	ntimes = 0;
	bBuff[64];
	memset(bBuff, 0x00, 64);
	lLastError = ERROR_SUCCESS;
	bFirstTime = TRUE;


	//01 10 0A 01 00 02 13 D0 
	while ((ntimes < 20/*最大延迟0.5s*/)/* && (nReadNum > 2)*/)
	{
		lLastError = m_Serial_LowPower.Read(&bBuff[nReadNum], 64, &nReadNumTemp);
		nReadNum += nReadNumTemp;
		CString strreceive = _T(""), strtemp = _T("");
		if (nReadNum <= 0)
		{
			ntimes++;
			this_thread::sleep_for(10ms);
			continue;
		}
		if (nReadNum < 8)
		{
			ntimes++;
			if (bFirstTime)
			{
				ntimes = 1;
				bFirstTime = FALSE;
			}
			if (nReadNum > 2)
			{
				if (bBuff[1] != 0x10)
				{//读取失败
					m_Serial_LowPower.Purge();
					break;
				}
			}
			this_thread::sleep_for(50ms);
			continue;
		}
		else if (nReadNum == 8)
		{
			if (bBuff[1] == 0x10)
			{//读取成功
				bSuccess = TRUE;
				break;
			}
		}
	}

	g_comm_lowpowerthread_mutex.unlock();
	return bSuccess;
}

BOOL CMainFrame::SetHipowerVoltage(int nHipowerVoltage)
{//55 02 01 05 E8 03 00 00 00 1A 66---55 82 FF 00 6A 66  ,55 82 00 00 SC 66
	g_comm_hipowerthread_mutex.lock();
	BOOL bSuccess = FALSE;
	BYTE SendByte[500];
	memset(SendByte, 0, 500);
	SendByte[0] = 0x55;
	SendByte[1] = 0x02;
	SendByte[2] = 0x01;
	SendByte[3] = 0x05;
	SendByte[4] = nHipowerVoltage%0x100;
	SendByte[5] = (nHipowerVoltage/0x100) % 0x100;
	SendByte[6] = (nHipowerVoltage / 0x10000) % 0x100;
	SendByte[7] = (nHipowerVoltage / 0x1000000) % 0x100;
	SendByte[8] = m_Struct_BurningTest_Param.nIncreaseRate;
	WORD nWord = get_check_sum(SendByte, 9);
	SendByte[9] = nWord % 0x0100;
	SendByte[10] = 0x66;
	DWORD dwWritten = 0;
	LONG sendnum = m_Serial_HightPower.Write(SendByte, 11, &dwWritten, 0, 500);
	if (dwWritten <= 0)
	{
		g_comm_hipowerthread_mutex.unlock();
		SLOG1(L"set highpower Write fail");
		return -1;
	}

	DWORD nReadNum = 0, nReadNumTemp = 0;
	/*static*/
	int ntimes = 0;
	BYTE bBuff[64];
	memset(bBuff, 0x00, 64);
	LONG    lLastError = ERROR_SUCCESS;
	BOOL bFirstTime = TRUE;
	while ((ntimes < 20/*最大延迟0.5s*/)/* && (nReadNum > 2)*/)
	{
		lLastError = m_Serial_HightPower.Read(&bBuff[nReadNum], 64, &nReadNumTemp);
		nReadNum += nReadNumTemp;
		CString strreceive = _T(""), strtemp = _T("");
		if (nReadNum <= 0)
		{
			this_thread::sleep_for(10ms);
			ntimes++;
			continue;
		}
		if (nReadNum < 6)
		{
			ntimes++;
			if (bFirstTime)
			{
				ntimes = 1;
				bFirstTime = FALSE;
			}
			if (nReadNum > 2)
			{
				if (bBuff[0] != 0x55)
				{//读取失败
					m_Serial_HightPower.Purge();
					break;
				}
			}
			this_thread::sleep_for(50ms);
			continue;
		}
		else if (nReadNum == 6)
		{//55 82 FF 00 6A 66
			if (bBuff[0] == 0x55 && bBuff[5] == 0x66)
			{//读取成功
				if (bBuff[2] == 0xFF)
				{//设置成功
					bSuccess = TRUE;
					CString strLog = L"";
					strLog.Format(L"set highpower %d success", nHipowerVoltage);
					SLOG1(strLog);
				}
				else
				{
					bSuccess = FALSE;
					SLOG1(L"set highpower fail");
				}
				break;
			}
		}
	}
	g_comm_hipowerthread_mutex.unlock(); 
	return bSuccess;
}

BOOL CMainFrame::StopHipowerOut()
{//55 05 01 00 69 66---55 85 ff 00 SC 66 /55 85 00 00 SC 66
	if (!m_Serial_HightPower.IsOpen())
	{
		return FALSE;
	}
	g_comm_hipowerthread_mutex.lock();
	BOOL bSuccess = FALSE;
	BYTE SendByte[500];
	memset(SendByte, 0, 500);
	SendByte[0] = 0x55;
	SendByte[1] = 0x05;
	SendByte[2] = 0x01;
	SendByte[3] = 0x00;
	SendByte[4] = 0x69;
	SendByte[5] = 0x66;
	DWORD dwWritten = 0;
	LONG sendnum = m_Serial_HightPower.Write(SendByte, 6, &dwWritten, 0, 500);
	if (dwWritten <= 0)
	{
		g_comm_hipowerthread_mutex.unlock();
		return -1;
	}

	DWORD nReadNum = 0, nReadNumTemp = 0;
	/*static*/
	int ntimes = 0;
	BYTE bBuff[64];
	memset(bBuff, 0x00, 64);
	LONG    lLastError = ERROR_SUCCESS;
	BOOL bFirstTime = TRUE;
	while ((ntimes < 20/*最大延迟0.5s*/)/* && (nReadNum > 2)*/)
	{
		lLastError = m_Serial_HightPower.Read(&bBuff[nReadNum], 64, &nReadNumTemp);
		nReadNum += nReadNumTemp;
		CString strreceive = _T(""), strtemp = _T("");
		if (nReadNum <= 0)
		{
			this_thread::sleep_for(10ms);
			ntimes++;
			continue;
		}
		if (nReadNum < 6)
		{
			ntimes++;
			if (bFirstTime)
			{
				ntimes = 1;
				bFirstTime = FALSE;
			}
			if (nReadNum > 2)
			{
				if (bBuff[0] != 0x55)
				{//读取失败
					m_Serial_HightPower.Purge();
					break;
				}
			}
			this_thread::sleep_for(50ms);
			continue;
		}
		else if (nReadNum == 6)
		{//55 81 10 04 12 00 00 00 92 66
			if (bBuff[0] == 0x55 && bBuff[5] == 0x66)
			{//读取成功
				if (bBuff[2] == 0xFF)
				{//设置成功
					bSuccess = TRUE;
				}
				else
				{
					bSuccess = FALSE;
				}
				break;
			}
		}
	}
	g_comm_hipowerthread_mutex.unlock();
	return bSuccess;

}
BOOL CMainFrame::StopLowpowerOut()
{//01 10 0A 00 00 01 02 00 07 4D 92 

	if (!m_Serial_LowPower.IsOpen())
	{
		return FALSE;
	}
	g_comm_lowpowerthread_mutex.lock();

	BOOL bSuccess = FALSE;
	//读电流电压
	BYTE SendByte[500];
	memset(SendByte, 0, 500);
	SendByte[0] = 0x01;
	SendByte[1] = 0x10;
	SendByte[2] = 0x0A;
	SendByte[3] = 0x00;
	SendByte[4] = 0x00;
	SendByte[5] = 0x01;
	SendByte[6] = 0x02;
	SendByte[7] = 0x00;
	SendByte[8] = 0x07;
	SendByte[9] = 0x4D;
	SendByte[10] = 0x92;
	DWORD dwWritten = 0;
	LONG sendnum = m_Serial_LowPower.Write(SendByte, 11, &dwWritten, 0, 500);
	if (dwWritten <= 0)
	{
		g_comm_lowpowerthread_mutex.unlock();
		return FALSE;
	}
	DWORD nReadNum = 0, nReadNumTemp = 0;
	/*static*/
	int ntimes = 0;
	BYTE bBuff[64];
	memset(bBuff, 0x00, 64);
	LONG    lLastError = ERROR_SUCCESS;
	BOOL bFirstTime = TRUE;


	//01 10 0A 00 00 01 02 11 
	while ((ntimes < 20/*最大延迟0.5s*/)/* && (nReadNum > 2)*/)
	{
		lLastError = m_Serial_LowPower.Read(&bBuff[nReadNum], 64, &nReadNumTemp);
		nReadNum += nReadNumTemp;
		CString strreceive = _T(""), strtemp = _T("");
		if (nReadNum <= 0)
		{
			ntimes++;
			this_thread::sleep_for(10ms);
			continue;
		}
		if (nReadNum < 8)
		{
			ntimes++;
			if (bFirstTime)
			{
				ntimes = 1;
				bFirstTime = FALSE;
			}
			if (nReadNum > 2)
			{
				if (bBuff[1] != 0x10)
				{//读取失败
					m_Serial_LowPower.Purge();
					break;
				}
			}
			this_thread::sleep_for(50ms);
			continue;
		}
		else if (nReadNum == 8)
		{
			if (bBuff[1] == 0x10)
			{//读取成功
				bSuccess = TRUE;
				break;
			}
		}
	}
	g_comm_lowpowerthread_mutex.unlock();
	return bSuccess;
}
BOOL CMainFrame::StartLowpowerOut()
{


	if (!m_Serial_LowPower.IsOpen())
	{
		return FALSE;
	}
	g_comm_lowpowerthread_mutex.lock();

	BOOL bSuccess = FALSE;
	//读电流电压
	BYTE SendByte[500];
	memset(SendByte, 0, 500);
	SendByte[0] = 0x01;
	SendByte[1] = 0x10;
	SendByte[2] = 0x0A;
	SendByte[3] = 0x00;
	SendByte[4] = 0x00;
	SendByte[5] = 0x01;
	SendByte[6] = 0x02;
	SendByte[7] = 0x00;
	SendByte[8] = 0x07;
	SendByte[9] = 0x4D;
	SendByte[10] = 0x92;
	DWORD dwWritten = 0;
	LONG sendnum = m_Serial_LowPower.Write(SendByte, 11, &dwWritten, 0, 500);
	if (dwWritten <= 0)
	{
		g_comm_lowpowerthread_mutex.unlock();
		return FALSE;
	}
	DWORD nReadNum = 0, nReadNumTemp = 0;
	/*static*/
	int ntimes = 0;
	BYTE bBuff[64];
	memset(bBuff, 0x00, 64);
	LONG    lLastError = ERROR_SUCCESS;
	BOOL bFirstTime = TRUE;


	//01 10 0A 00 00 01 02 11 
	while ((ntimes < 20/*最大延迟0.5s*/)/* && (nReadNum > 2)*/)
	{
		lLastError = m_Serial_LowPower.Read(&bBuff[nReadNum], 64, &nReadNumTemp);
		nReadNum += nReadNumTemp;
		CString strreceive = _T(""), strtemp = _T("");
		if (nReadNum <= 0)
		{
			ntimes++;
			this_thread::sleep_for(10ms);
			continue;
		}
		if (nReadNum < 8)
		{
			ntimes++;
			if (bFirstTime)
			{
				ntimes = 1;
				bFirstTime = FALSE;
			}
			if (nReadNum > 2)
			{
				if (bBuff[1] != 0x10)
				{//读取失败
					m_Serial_LowPower.Purge();
					break;
				}
			}
			this_thread::sleep_for(50ms);
			continue;
		}
		else if (nReadNum == 8)
		{
			if (bBuff[1] == 0x10)
			{//读取成功
				bSuccess = TRUE;
				break;
			}
		}
	}
	g_comm_lowpowerthread_mutex.unlock();
	return bSuccess;
}

BOOL CMainFrame::ResetMcu()
{//01 10 00 00 00 03 06 55 55 55 55 AA 55 59 CA 	
	if (!m_Serial_MCU.IsOpen())
	{
		return FALSE;
	}
	g_comm_mcuthread_mutex.lock();
	BOOL bSuccess = FALSE;
	BYTE SendByte[500];
	memset(SendByte, 0, 500);
	SendByte[0] = 0x01;
	SendByte[1] = 0x10;
	SendByte[2] = 0x00;
	SendByte[3] = 0x00;
	SendByte[4] = 0x00;
	SendByte[5] = 0x03;
	SendByte[6] = 0x06;
	SendByte[7] = 0x55;
	SendByte[8] = 0x55;
	SendByte[9] = 0x55;
	SendByte[10] = 0x55;
	SendByte[11] = 0xAA;
	SendByte[12] = 0x55;
	SendByte[13] = 0x59;
	SendByte[14] = 0xCA;
	DWORD dwWritten = 0;
	LONG sendnum = m_Serial_MCU.Write(SendByte, 15, &dwWritten, 0, 500);
	if (dwWritten <= 0)
	{
		g_comm_mcuthread_mutex.unlock();
		return FALSE;
	}

	DWORD nReadNum = 0, nReadNumTemp = 0;
	/*static*/
	int ntimes = 0;
	BYTE bBuff[64];
	memset(bBuff, 0x00, 64);
	LONG    lLastError = ERROR_SUCCESS;
	BOOL bFirstTime = TRUE;
	while ((ntimes < 20/*最大延迟0.5s*/)/* && (nReadNum > 2)*/)
	{//01 10 00 00 00 03 80 08 
		lLastError = m_Serial_MCU.Read(&bBuff[nReadNum], 64, &nReadNumTemp);
		nReadNum += nReadNumTemp;
		CString strreceive = _T(""), strtemp = _T("");
		if (nReadNum <= 0)
		{
			this_thread::sleep_for(10ms);
			ntimes++;
			continue;
		}
		if (nReadNum < 8)
		{
			ntimes++;
			if (bFirstTime)
			{
				ntimes = 1;
				bFirstTime = FALSE;
			}
			if (nReadNum > 2)
			{
				if (bBuff[1] != 0x10)
				{//读取失败
					m_Serial_MCU.Purge();
					SLOG1(L"Reset Mcu fail");
					break;
				}
			}
			this_thread::sleep_for(50ms);
			continue;
		}
		else if (nReadNum == 8)
		{//01 10 00 00 00 03 80 08
			if (bBuff[1] == 0x10)
			{//读取成功
				bSuccess = TRUE;
				SLOG1(L"Reset Mcu success");
				break;
			}
		}
	}
	g_comm_mcuthread_mutex.unlock();
	return bSuccess;
}
BOOL CMainFrame::InitializeTest()
{
	//设置低压限压
	if (!SetLowpowerLimit(m_struct_CurrentProgram.nLowValtageLimit))
	{
		SLOG1(L"设置低压限压失败");
		return FALSE;
	}
	//SLOG1(L"111111111");
    //设置低压电流
	if (!SetLowpowerCurrent(m_struct_CurrentProgram.nLowCurrent))
	{
		SLOG1(L"设置低压电流失败");
		return FALSE;
	}
	//SLOG1(L"2222222222222");
	//设置低压电压
	if (!SetLowpowerVoltage(m_struct_CurrentProgram.nLowVoltage))
	{
		SLOG1(L"设置低压电压失败");
		return FALSE;
	}
	//SLOG1(L"3333333333");
	return TRUE;
}

BOOL CMainFrame::StartTestThread()
{
	StopTestThread();
	m_pThread_TestThread = AfxBeginThread(BurningTestTread, this);
	m_pThread_TestThread->m_bAutoDelete = FALSE;

	m_bBeginGetValue = TRUE;
	m_tBeginGetValue = CTime::GetCurrentTime();
	return TRUE;
}
BOOL CMainFrame::StopTestThread()
{
	m_bStopTestThread = TRUE;
	m_bBeginGetValue = FALSE;
	if (m_pThread_TestThread != NULL && m_pThread_TestThread->m_hThread!=NULL)
	{
		::WaitForSingleObject(m_pThread_TestThread->m_hThread, INFINITE);

		delete  m_pThread_TestThread;
		m_pThread_TestThread = NULL;
	}
	m_bStopTestThread = FALSE;
	return TRUE;
}

LRESULT CMainFrame::OnExecuteCommand(WPARAM wParam, LPARAM lParam)
{

	if (wParam == EXECUTE_COMMAND_STOPTEST)
	{//
		SLOG1(L"停止测试");
		for (int i = 0; i < 3; i++)
		{
			if (StopLowpowerOut())
			{
				break;
			}
		}
		for (int i = 0; i < 3; i++)
		{
			if (StopHipowerOut())
			{
				break;
			}
		}
		StopTestThread();
		if (AfxMessageBox(L"是否保存测试记录？", MB_YESNO) == IDYES)
		{
			SaveTestRecode();
		}
	}
	else if (wParam == EXECUTE_COMMAND_SUSPEND)
	{//

		SLOG1(L"暂停测试");
		for (int i = 0; i < 3; i++)
		{
			if (StopLowpowerOut())
			{
				break;
			}
		}
		for (int i = 0; i < 3; i++)
		{
			if (StopHipowerOut())
			{
				break;
			}
		}
		m_bBeginGetValue = FALSE;
		if (m_pThread_TestThread != NULL && m_pThread_TestThread->m_hThread != NULL)
		{
			m_pThread_TestThread->SuspendThread();
		}
	}
	else if (wParam == EXECUTE_COMMAND_CONTINUE)
	{//
		SLOG1(L"继续测试");
		for (int i = 0; i < 3; i++)
		{
			if (StartLowpowerOut())
			{
				break;
			}
		}
		for (int i = 0; i < m_struct_CurrentProgram.VectProgItem.size(); i++)
		{
			if (m_struct_CurrentProgram.VectProgItem.at(i).bFinish == FALSE)
			{
				CTimeSpan TimeSpan(m_Struct_BurningTest_Param.nTimeLong / 24 * 3600, m_Struct_BurningTest_Param.nTimeLong / 3600 % 24, m_Struct_BurningTest_Param.nTimeLong / 60 % 60, m_Struct_BurningTest_Param.nTimeLong % 60);
				CTime tTimeStart = CTime::GetCurrentTime() - TimeSpan;
				m_struct_CurrentProgram.VectProgItem.at(i).tStartTime = tTimeStart;
				int nvoltage = m_struct_CurrentProgram.VectProgItem.at(i).nHightVotage * 10;
				SetHipowerVoltage(nvoltage);
				break;
			}
		}
	
		if (m_pThread_TestThread != NULL && m_pThread_TestThread->m_hThread != NULL)
		{
			m_pThread_TestThread->ResumeThread();
			m_bBeginGetValue = TRUE;
		}

	}
	else if (wParam == EXECUTE_COMMAND_RESETFIRE)
	{
		SLOG1(L"reset mcu");
		for (int i = 0; i < 3; i++)
		{
			if (ResetMcu())
			{
				break;
			}
		}
	}
	else if (wParam == EXECUTE_COMMAND_FINISH)
	{
		SLOG1(L"完成测试");
		for (int i = 0; i < 3; i++)
		{
			if (StopLowpowerOut())
			{
				break;
			}
		}
		for (int i = 0; i < 3; i++)
		{
			if (StopHipowerOut())
			{
				break;
			}
		}
		StopTestThread();
		//提示保存报告
		if (AfxMessageBox(L"是否保存测试记录？", MB_YESNO) == IDYES)
		{
			SaveTestRecode();
		}
	}
	
	return 0;
}

void CMainFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_Struct_CommStatus.nHightPowerStatus == 1)
	{
		if (m_pThread_TestThread != NULL && m_pThread_TestThread->m_hThread != NULL && m_bBeginGetValue)
		{
			if (AfxMessageBox(L"正在测试是否结束？", MB_YESNO) == IDYES)
			{
				SLOG1(L"停止测试");
				for (int i = 0; i < 3; i++)
				{
					if (StopLowpowerOut())
					{
						break;
					}
				}
				for (int i = 0; i < 3; i++)
				{
					if (StopHipowerOut())
					{
						break;
					}
				}
				StopTestThread();
			}
			else
			{
				return;
			}

			if (AfxMessageBox(L"是否保存测试记录？", MB_YESNO) == IDYES)
			{
				SaveTestRecode();
			}
		}
		StopCommThreads();
		if (m_Serial_HightPower.IsOpen())
		{
			m_Serial_HightPower.Close();
		}
		if (m_Serial_LowPower.IsOpen())
		{
			m_Serial_LowPower.Close();
		}
		if (m_Serial_MCU.IsOpen())
		{
			m_Serial_MCU.Close();
		}
		m_Struct_CommStatus.nHightPowerStatus = 0;
		m_Struct_CommStatus.nLowPowerstatus = 0;
		m_Struct_CommStatus.nMcuStatus = 0;
	}
	CFrameWndEx::OnClose();
}


void CMainFrame::OnToolbarFirerecord()
{
	// TODO: 在此添加命令处理程序代码
	CFireRecordDlg dlg;
	if (dlg.DoModal() == IDOK)
	{

	}
}


void CMainFrame::OnUpdateToolbarFirerecord(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable();
}
SAFEARRAY* CMainFrame::GetSafeArrayForExport(void* workshet)
{
	CWorksheet* pWorksheet = (CWorksheet*)workshet;
	VARTYPE vt = VT_VARIANT; /*数组元素的类型，long*/
	SAFEARRAYBOUND sab[2]; /*用于定义数组的维数和下标的起始值*/


	sab[0].cElements = m_Vect_FireRecord.size() + 7;
	sab[0].lLbound = 0;
	sab[1].cElements = 9;
	sab[1].lLbound = 0;
	/*创建一个2*2的类型为long的二维数组*/
	SAFEARRAY* psa = SafeArrayCreate(vt, sizeof(sab) / sizeof(SAFEARRAYBOUND), sab);
	if (NULL == psa)
	{
		throw;
	}

	/*对二维数组的元素进行逐个赋值*/
	long index[2] = { 0, 0 };
	long lFirstLBound = 0;
	long lFirstUBound = 0;
	long lSecondLBound = 0;
	long lSecondUBound = 0;
	SafeArrayGetLBound(psa, 1, &lFirstLBound);
	SafeArrayGetUBound(psa, 1, &lFirstUBound);
	SafeArrayGetLBound(psa, 2, &lSecondLBound);
	SafeArrayGetUBound(psa, 2, &lSecondUBound);
	_variant_t strTitle[2000] = { L"高压产品试验报告", L"试验单位：", L"无锡日联", L"操作人：", L"型号：", L"规格：", L"编号：", L"测试名称：", L"试验记录", L"步骤"
	, L"记录时间" , L"输出电压(kV)" , L"打火电压(KV)" , L"打火时刻(s)" , L"CH1 电流/打火电流" , L"CH2 电流/打火电流" , L"CH3 电流/打火电流" , L"CH4 电流/打火电流" };
	int nMaxVoltage = 0;
	for (int i = 0; i < m_struct_CurrentProgram.VectProgItem.size(); i++)
	{
		if (m_struct_CurrentProgram.VectProgItem.at(i).nHightVotage > nMaxVoltage)
		{
			nMaxVoltage = m_struct_CurrentProgram.VectProgItem.at(i).nHightVotage;
		}
	}

	for (long i = lFirstLBound; i <= lFirstUBound; i++)
	{
		index[0] = i;
		int paramindex = 0;
		for (long j = lSecondLBound; j <= lSecondUBound; j++)
		{
			index[1] = j;


			if (i == lFirstLBound)
			{//标题

				HRESULT hRet = SafeArrayPutElement(psa, index, &strTitle[0]);
				if (FAILED(hRet))
				{
					throw;
				}
				break;
			}
			else if (i == lFirstLBound + 1)
			{
				if (j == lSecondLBound)
				{
					HRESULT hRet = SafeArrayPutElement(psa, index, &strTitle[1]);
					if (FAILED(hRet))
					{
						throw;
					}
				}
				else if (j == lSecondLBound + 1)
				{
					HRESULT hRet = SafeArrayPutElement(psa, index, &strTitle[2]);
					if (FAILED(hRet))
					{
						throw;
					}
				}
				else if (j == lSecondLBound + 4)
				{
					HRESULT hRet = SafeArrayPutElement(psa, index, &strTitle[3]);
					if (FAILED(hRet))
					{
						throw;
					}
				}
				else if (j == lSecondLBound + 5)
				{
					HRESULT hRet = SafeArrayPutElement(psa, index, &_variant_t(m_strUser));
					if (FAILED(hRet))
					{
						throw;
					}
					break;
				}

			}
			else if (i == lFirstLBound + 2)
			{
				if (j == lSecondLBound)
				{
					HRESULT hRet = SafeArrayPutElement(psa, index, &strTitle[4]);
					if (FAILED(hRet))
					{
						throw;
					}
				}
				else if (j == lSecondLBound + 1)
				{
					HRESULT hRet = SafeArrayPutElement(psa, index, &_variant_t(m_struct_CurrentProgram.strProgramName));
					if (FAILED(hRet))
					{
						throw;
					}
				}
				else if (j == lSecondLBound + 4)
				{
					HRESULT hRet = SafeArrayPutElement(psa, index, &strTitle[5]);
					if (FAILED(hRet))
					{
						throw;
					}
				}
				else if (j == lSecondLBound + 5)
				{
					HRESULT hRet = SafeArrayPutElement(psa, index, &_variant_t(m_struct_CurrentProgram.strMemo));
					if (FAILED(hRet))
					{
						throw;
					}
					break;
				}

			}
			else if (i == lFirstLBound + 3)
			{
				if (j == lSecondLBound)
				{
					HRESULT hRet = SafeArrayPutElement(psa, index, &strTitle[6]);
					if (FAILED(hRet))
					{
						throw;
					}
				}
				else if (j == lSecondLBound + 1)
				{
					HRESULT hRet = SafeArrayPutElement(psa, index, &_variant_t(m_Struct_BurningTest_Param.strTubeSn));
					if (FAILED(hRet))
					{
						throw;
					}
				}
				else if (j == lSecondLBound + 4)
				{
					HRESULT hRet = SafeArrayPutElement(psa, index, &strTitle[7]);
					if (FAILED(hRet))
					{
						throw;
					}
				}
				else if (j == lSecondLBound + 5)
				{

					HRESULT hRet = SafeArrayPutElement(psa, index, &strTitle[8]);
					if (FAILED(hRet))
					{
						throw;
					}
					break;
				}

			}
			else if (i == lFirstLBound + 4)
			{
				break;
			}
			else if (i == lFirstLBound + 5)
			{
				if (j == lSecondLBound)
				{
					CString strTestName = L"";
					strTestName.Format(L"%s-%dKV", m_struct_CurrentProgram.strProgramName, nMaxVoltage);
					HRESULT hRet = SafeArrayPutElement(psa, index, &_variant_t(strTestName));
					if (FAILED(hRet))
					{
						throw;
					}
				}
				break;
			}
			else if (i == lFirstLBound + 6)
			{
				HRESULT hRet = SafeArrayPutElement(psa, index, &strTitle[9 + j - lSecondLBound]);
				if (FAILED(hRet))
				{
					throw;
				}
			}
			else
			{//数据

				if (m_Vect_FireRecord.size() > i - lFirstLBound-7)
				{
					CString strtemp = L"";
					switch (j - lSecondLBound)
					{
					case 0:
						strtemp.Format(L"%d", m_Vect_FireRecord.at(i - lFirstLBound - 7).nStep);
						break;
					case 1:
						strtemp = m_Vect_FireRecord.at(i - lFirstLBound - 7).strTime;
						break;
					case 2:
						strtemp = m_Vect_FireRecord.at(i - lFirstLBound - 7).strVoltage;
						break;
					case 3:
						strtemp = m_Vect_FireRecord.at(i - lFirstLBound - 7).strVoltageFire;
						break;
					case 4:
						strtemp = m_Vect_FireRecord.at(i - lFirstLBound - 7).strFileMoment;
						break;
					case 5:
						strtemp = m_Vect_FireRecord.at(i - lFirstLBound - 7).strFireCurrentCH[0]+L"uA";
						break;
					case 6:
						strtemp = m_Vect_FireRecord.at(i - lFirstLBound - 7).strFireCurrentCH[1] + L"uA";
						break;
					case 7:
						strtemp = m_Vect_FireRecord.at(i - lFirstLBound - 7).strFireCurrentCH[2] + L"uA";
						break;
					case 8:
						strtemp = m_Vect_FireRecord.at(i - lFirstLBound - 7).strFireCurrentCH[3] + L"uA";
						break;

					default:
						break;
					}
					HRESULT hRet = SafeArrayPutElement(psa, index, &_variant_t(strtemp));
					if (FAILED(hRet))
					{
						throw;
					}

				}
			}
		}
	}

	return psa;
}
void CMainFrame::SaveTestRecode()
{
	CTime Curtime = CTime::GetCurrentTime();

	CString strFilePath = L"";
	if (m_struct_CurrentProgram.strDataSavePath == L"")
	{
		CString strFileName = L"";
		strFileName.Format(L"%s%s.xls", m_Struct_BurningTest_Param.strTubeSn , Curtime.Format(_T("%Y%m%d_%H%M%S")));

		CFileDialog fileDlg(FALSE, _T("xls"), strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("(*.xls)|*.xls||"), this);
		
		int nRet = fileDlg.DoModal();
		if (nRet == IDOK)
		{
			strFilePath = fileDlg.GetPathName();
			strFileName = fileDlg.GetFileName();
		}
		else
		{
			return;
		}
		m_struct_CurrentProgram.strDataSavePath = strFilePath.Left(strFilePath.GetLength() - strFileName.GetLength());
		for (int i = 0; i < m_Vect_Program.size(); i++)
		{
			if (m_Vect_Program.at(i).strProgramName == m_struct_CurrentProgram.strProgramName)
			{
				m_Vect_Program.at(i).strDataSavePath = m_struct_CurrentProgram.strDataSavePath;
				break;
			}
		}

	}
	else
	{
		if (!PathFileExists(m_struct_CurrentProgram.strDataSavePath))
		{
			SHCreateDirectory(NULL, m_struct_CurrentProgram.strDataSavePath);
		}
		strFilePath = m_struct_CurrentProgram.strDataSavePath;
		if (strFilePath.Right(1) != L"\\")
		{
			strFilePath += L"\\";
		}
		CString strFileName = L"";
		strFileName.Format(L"%s%s.xls", m_Struct_BurningTest_Param.strTubeSn , Curtime.Format(_T("%Y%m%d_%H%M%S")));
		strFilePath += strFileName;

	}
	CApplication ExcelApp;

	CWorkbooks books;

	CWorkbook book;

	CWorksheets sheets;

	CWorksheet sheet;

	CRange range;
	//Range range;
	LPDISPATCH lpDisp = NULL;

	HRESULT r1 = OleInitialize(NULL);
	std::vector<double> vectRet;
	vectRet.clear();

	//创建Excel 服务器(启动Excel)
	if (!ExcelApp.CreateDispatch(_T("Excel.Application"), NULL))

	{

		AfxMessageBox(_T("启动Excel服务器失败!"));

		return;

	}


	/*判断当前Excel的版本*/

	CString strExcelVersion = ExcelApp.get_Version();

	int iStart = 0;

	strExcelVersion = strExcelVersion.Tokenize(_T("."), iStart);

	if (_T("11") == strExcelVersion)

	{

		//		AfxMessageBox(_T("当前Excel的版本是2003。"));

	}

	else if (_T("12") == strExcelVersion)

	{

		//		AfxMessageBox(_T("当前Excel的版本是2007。"));

	}
	else if (_T("15") == strExcelVersion)

	{

		//		AfxMessageBox(_T("当前Excel的版本是2013。"));

	}
	else

	{

		//AfxMessageBox(_T("当前Excel的版本是其他版本。"));

	}



	ExcelApp.put_Visible(FALSE);

	ExcelApp.put_UserControl(FALSE);

	ExcelApp.put_DisplayAlerts(FALSE);// .SetDisplayAlerts(false);    // 不弹出对话框询问是否保存 

	/*得到工作簿容器*/

	books.AttachDispatch(ExcelApp.get_Workbooks());


	/*打开一个工作簿，如不存在，则新增一个工作簿*/

	CString strBookPath = strFilePath;
	try

	{

		/*打开一个工作簿*/

		lpDisp = books.Open(strBookPath,

			vtMissing, vtMissing, vtMissing, vtMissing, vtMissing,

			vtMissing, vtMissing, vtMissing, vtMissing, vtMissing,

			vtMissing, vtMissing, vtMissing, vtMissing);

		book.AttachDispatch(lpDisp);

	}

	catch (CException* e)

	{

		/*增加一个新的工作簿*/

		lpDisp = books.Add(vtMissing);
		book.AttachDispatch(lpDisp);
		e->Delete();

	}





	/*得到工作簿中的Sheet的容器*/

	sheets.AttachDispatch(book.get_Sheets());



	CString strSheetName = _T("1");

	try
	{
		/*打开一个已有的Sheet*/
		lpDisp = sheets.get_Item(_variant_t(LONG(1)));
		sheet.AttachDispatch(lpDisp);
	}
	catch (CException* e)
	{
		AfxMessageBox(_T("打开excel Sheet失败!"));
		e->Delete();
		return;
	}
	//STRUCT_REPORTBASICINFO structtemp;

	int nRowNum = 7+ m_Vect_FireRecord.size(),nColumnNum = 9;
	CString strCellEnd = L"";
	strCellEnd.Format(L"I%d", nRowNum);

	lpDisp = sheet.get_Range(_variant_t("A1"), _variant_t(strCellEnd));
	range.AttachDispatch(lpDisp);
	range.put_NumberFormatLocal(COleVariant(L"@"));
	
	//设置网格
	for (int i = 'A'; i <= 'I'; i++)
	{
		for (int j = 1; j <= nRowNum; j++)
		{
			CString strtemp = L"";
			strtemp.Format(L"%c%d", i, j);
			CRange rangegrid = sheet.get_Range(_variant_t(strtemp), _variant_t(strtemp));
			rangegrid.BorderAround(_variant_t((long)1), (long)2, (long)1, vtMissing); // 设置水平内部边框为连续线
			rangegrid.ReleaseDispatch();
		}
	}

	//合并单元格

	for (int i = 1; i < 7; i++)
	{
		if (i == 1)
		{
			CRange rangetemp = sheet.get_Range(_variant_t("A1"), _variant_t("I1"));
			rangetemp.Merge(_variant_t((long)0));
			CFont0 excelFont;
			excelFont.AttachDispatch(rangetemp.get_Font()) ;
			excelFont.put_Name(_variant_t("楷体"));
			excelFont.put_Size(_variant_t((long)22));
			excelFont.put_Bold(_variant_t(VARIANT_TRUE));
			excelFont.ReleaseDispatch();
			rangetemp.ReleaseDispatch();
		}
		else if (i >= 2 && i <= 4)
		{
			CString startCell, endCell;
			startCell.Format(L"B%d", i);
			endCell.Format(L"D%d", i);
			CRange rangetemp = sheet.get_Range(_variant_t(startCell), _variant_t(endCell));
			rangetemp.Merge(_variant_t((long)0));
			rangetemp.ReleaseDispatch();

			startCell.Format(L"F%d", i);
			endCell.Format(L"I%d", i);
			rangetemp = sheet.get_Range(_variant_t(startCell), _variant_t(endCell));
			rangetemp.Merge(_variant_t((long)0));
			rangetemp.ReleaseDispatch();

		}
		else
		{
			CString startCell, endCell;
			startCell.Format(L"A%d", i);
			endCell.Format(L"i%d", i);
			CRange rangetemp = sheet.get_Range(_variant_t(startCell), _variant_t(endCell));
			rangetemp.Merge(_variant_t((long)0));
			rangetemp.ReleaseDispatch();
		}
	}
	//设置字体
	CRange rangeFont = sheet.get_Range(_variant_t("A2"), _variant_t("I6"));
	CFont0 excelFont;
	excelFont.AttachDispatch(rangeFont.get_Font());
	excelFont.put_Name(_variant_t("宋体"));
	excelFont.put_Size(_variant_t((long)11));
	excelFont.put_Bold(_variant_t(VARIANT_TRUE));
	excelFont.ReleaseDispatch();
	rangeFont.ReleaseDispatch();

	rangeFont = sheet.get_Range(_variant_t("A7"), _variant_t("I7"));
	excelFont.AttachDispatch(rangeFont.get_Font());
	excelFont.put_Name(_variant_t("宋体"));
	excelFont.put_Size(_variant_t((long)12));
	excelFont.put_Bold(_variant_t(VARIANT_TRUE));
	excelFont.ReleaseDispatch();
	rangeFont.ReleaseDispatch();


	//文本居中
	range.put_HorizontalAlignment(_variant_t((long)-4108));
	range.put_VerticalAlignment(_variant_t((long)-4108));


	SAFEARRAY* psa = GetSafeArrayForExport((void*)&sheet);

	if (psa != NULL)
	{
		VARIANT var;
		var.vt = VT_ARRAY | VT_VARIANT; /*vt必须和psa的数据类型保持一致*/
		var.parray = psa;
		range.put_Value2(var);
		SafeArrayDestroy(psa);
		psa = NULL;
	}



	CString strSaveAsName = strFilePath;

	//CString strSuffix = strSaveAsName.Mid(strSaveAsName.ReverseFind(_T('.')));
	CRange RangeColums;
	RangeColums.AttachDispatch(sheet.get_Columns());
	RangeColums.AutoFit();
	RangeColums.ReleaseDispatch();
	book.SaveAs(_variant_t(strSaveAsName), vtMissing, vtMissing, vtMissing, vtMissing,

		vtMissing, 0, vtMissing, vtMissing, vtMissing,

		vtMissing, vtMissing);
	range.ReleaseDispatch();

	sheet.ReleaseDispatch();

	sheets.ReleaseDispatch();

	book.ReleaseDispatch();

	books.ReleaseDispatch();

	ExcelApp.Quit();
	ExcelApp.ReleaseDispatch();
	AfxMessageBox(L"报告生成成功");
}