// CCommSetDlg.cpp: 实现文件
//

#include "pch.h"
#include "XrayDCDeburring.h"
#include "CCommSetDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"

// CCommSetDlg 对话框

IMPLEMENT_DYNAMIC(CCommSetDlg, CDialogEx)

CCommSetDlg::CCommSetDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_COMMSET, pParent)
	, m_Edit_Shiboqi_IP(_T(""))
{

}

CCommSetDlg::~CCommSetDlg()
{
}

void CCommSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_HIGHTPOWER_COM, m_ComboBox_HightPower_Com);
	DDX_Control(pDX, IDC_COMBO_LOWPOWER_COM, m_ComboBox_LowPower_Com);
	DDX_Control(pDX, IDC_COMBO_MCU_COM, m_ComboBox_MCU_Com);
	DDX_Text(pDX, IDC_EDIT_SHIBOQI, m_Edit_Shiboqi_IP);
	DDX_Control(pDX, IDC_COMBO_HIGHTPOWER_BAUDRATE, m_ComboBox_HiPower_Baudrate);
	DDX_Control(pDX, IDC_COMBO_LOWPOWER_BAUDRATE, m_ComboBox_Lowpower_Baudrate);
	DDX_Control(pDX, IDC_COMBO_MCU_BAUDRATE, m_ComboBox_Mcu_Baudrate);
	DDX_Control(pDX, IDC_COMBO_SHIBOQI_BAUDRATE, m_ComboBox_Shiboqi_Baudrate);
	DDX_Control(pDX, IDC_COMBO_SHIBOQI_COM, m_ComboBox_Shiboqi_Com);
}


BEGIN_MESSAGE_MAP(CCommSetDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCommSetDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCommSetDlg 消息处理程序


BOOL CCommSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CMainFrame* pMainframe = (CMainFrame*)AfxGetMainWnd();
	CArray<CString, CString> comarray;
	m_ComboBox_HightPower_Com.ResetContent();
	m_ComboBox_LowPower_Com.ResetContent();
	m_ComboBox_MCU_Com.ResetContent();
	int nIndexHightPower = -1, nIndexLowPower = -1, nIndexMcu = -1, nIndexShiboqi = -1;
	if (GetSystemSerialComport(comarray))
	{
		for (int i = 0; i < comarray.GetSize(); i++)
		{
			CString strTemp = comarray.GetAt(i);
			m_ComboBox_HightPower_Com.AddString(strTemp);
			m_ComboBox_LowPower_Com.AddString(strTemp);
			m_ComboBox_MCU_Com.AddString(strTemp);
			m_ComboBox_Shiboqi_Com.AddString(strTemp);
			if (pMainframe->m_strHightPower_Com == strTemp)
			{
				nIndexHightPower = i;
			}
			if (pMainframe->m_strLowPower_Com == strTemp)
			{
				nIndexLowPower = i;
			}
			if (pMainframe->m_strMcu_Com == strTemp)
			{
				nIndexMcu = i;
			}
		}

		if (nIndexHightPower != -1)
		{
			m_ComboBox_HightPower_Com.SetCurSel(nIndexHightPower);
		}
		else
		{
			m_ComboBox_HightPower_Com.SetCurSel(0);
		}

		if (nIndexLowPower != -1)
		{
			m_ComboBox_LowPower_Com.SetCurSel(nIndexLowPower);
		}
		else
		{
			m_ComboBox_LowPower_Com.SetCurSel(0);
		}

		if (nIndexMcu != -1)
		{
			m_ComboBox_MCU_Com.SetCurSel(nIndexMcu);
		}
		else
		{
			m_ComboBox_MCU_Com.SetCurSel(0);
		}
		m_Edit_Shiboqi_IP = pMainframe->m_strShiboqi_IP;
		UpdateData(FALSE);
	}
	nIndexHightPower = -1; 
	nIndexLowPower = -1; 
	nIndexMcu = -1;
	nIndexShiboqi = -1;
	CString strBauteRate[] = { L"9600",L"19200",L"38400",L"57600",L"115200" };
	for (int i = 0; i < sizeof(strBauteRate) / sizeof(CString); i++)
	{
		m_ComboBox_HiPower_Baudrate.AddString(strBauteRate[i]);
		m_ComboBox_Lowpower_Baudrate.AddString(strBauteRate[i]);
		m_ComboBox_Mcu_Baudrate.AddString(strBauteRate[i]);
		m_ComboBox_Shiboqi_Baudrate.AddString(strBauteRate[i]);
		if (pMainframe->m_nBaudRate_HightPower == _ttoi(strBauteRate[i]))
		{
			nIndexHightPower = i;
		}
		if (pMainframe->m_nBaudRate_LowPower == _ttoi(strBauteRate[i]))
		{
			nIndexLowPower = i;
		}
		if (pMainframe->m_nBaudRate_MCU == _ttoi(strBauteRate[i]))
		{
			nIndexMcu = i;
		}
		if (pMainframe->m_nBaudRate_Shiboqi == _ttoi(strBauteRate[i]))
		{
			nIndexShiboqi = i;
		}


	}
	if (nIndexHightPower != -1)
	{
		m_ComboBox_HiPower_Baudrate.SetCurSel(nIndexHightPower);
	}
	else
	{
		m_ComboBox_HiPower_Baudrate.SetCurSel(0);
	}

	if (nIndexLowPower != -1)
	{
		m_ComboBox_Lowpower_Baudrate.SetCurSel(nIndexLowPower);
	}
	else
	{
		m_ComboBox_Lowpower_Baudrate.SetCurSel(0);
	}

	if (nIndexMcu != -1)
	{
		m_ComboBox_Mcu_Baudrate.SetCurSel(nIndexMcu);
	}
	else
	{
		m_ComboBox_Mcu_Baudrate.SetCurSel(0);
	}

	if (nIndexShiboqi != -1)
	{
		m_ComboBox_Shiboqi_Baudrate.SetCurSel(nIndexShiboqi);
	}
	else
	{
		m_ComboBox_Shiboqi_Baudrate.SetCurSel(0);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
BOOL CCommSetDlg::GetSystemSerialComport(CArray<CString, CString>& comarray)
{
	HKEY hKey = NULL;

	comarray.RemoveAll();

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	CString valuename, databuffer;
	DWORD valuenamebufferlength = 200, valuetype, databuddersize = 200;

	int i = 0;
	while (RegEnumValue(hKey, i++, valuename.GetBuffer(200), &valuenamebufferlength, NULL, &valuetype, (BYTE*)databuffer.GetBuffer(200), &databuddersize) != ERROR_NO_MORE_ITEMS)
	{
		comarray.Add(CString(databuffer));

		databuddersize = 200;
		valuenamebufferlength = 200;
	}

	RegCloseKey(hKey);

	return TRUE;
}

void CCommSetDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!UpdateData())
	{
		return;
	}
	CMainFrame* pMainframe = (CMainFrame*)AfxGetMainWnd();
	CString strIniPath = theApp.m_strRootPath + _T("SystemConfig.ini");
	CString strtemp = L"";
	m_ComboBox_HightPower_Com.GetLBText(m_ComboBox_HightPower_Com.GetCurSel(),strtemp);
	pMainframe->m_strHightPower_Com = strtemp;
	::WritePrivateProfileString(_T("SystemSettings"), _T("HIGHT_POWER_COM"), strtemp, strIniPath);
	m_ComboBox_LowPower_Com.GetLBText(m_ComboBox_LowPower_Com.GetCurSel(), strtemp);
	pMainframe->m_strLowPower_Com = strtemp;
	::WritePrivateProfileString(_T("SystemSettings"), _T("LOW_POWER_COM"), strtemp, strIniPath);

	m_ComboBox_MCU_Com.GetLBText(m_ComboBox_MCU_Com.GetCurSel(), strtemp);
	pMainframe->m_strMcu_Com = strtemp;
	::WritePrivateProfileString(_T("SystemSettings"), _T("MCU_COM"), strtemp, strIniPath);

	::WritePrivateProfileString(_T("SystemSettings"), _T("SHIBOQI_IP"), m_Edit_Shiboqi_IP, strIniPath);
	pMainframe->m_strShiboqi_IP = m_Edit_Shiboqi_IP;

	m_ComboBox_HiPower_Baudrate.GetLBText(m_ComboBox_HiPower_Baudrate.GetCurSel(), strtemp);
	pMainframe->m_nBaudRate_HightPower = _ttoi(strtemp);
	::WritePrivateProfileString(_T("SystemSettings"), _T("HIGHT_POWER_BAUDRATE"), strtemp, strIniPath);

	m_ComboBox_Lowpower_Baudrate.GetLBText(m_ComboBox_Lowpower_Baudrate.GetCurSel(), strtemp);
	pMainframe->m_nBaudRate_LowPower = _ttoi(strtemp);
	::WritePrivateProfileString(_T("SystemSettings"), _T("LOW_POWER_BAUDRATE"), strtemp, strIniPath);

	m_ComboBox_Mcu_Baudrate.GetLBText(m_ComboBox_Mcu_Baudrate.GetCurSel(), strtemp);
	pMainframe->m_nBaudRate_MCU = _ttoi(strtemp);
	::WritePrivateProfileString(_T("SystemSettings"), _T("MCU_BAUDRATE"), strtemp, strIniPath);

	m_ComboBox_Shiboqi_Baudrate.GetLBText(m_ComboBox_Shiboqi_Baudrate.GetCurSel(), strtemp);
	pMainframe->m_nBaudRate_Shiboqi = _ttoi(strtemp);
	::WritePrivateProfileString(_T("SystemSettings"), _T("SHIBOQI_BAUDRATE"), strtemp, strIniPath);

	CDialogEx::OnOK();
}
