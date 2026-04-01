// CProductEditDlg.cpp: 实现文件
//

#include "pch.h"
#include "XrayDCDeburring.h"
#include "CProductEditDlg.h"
#include "afxdialogex.h"


// CProductEditDlg 对话框

IMPLEMENT_DYNAMIC(CProductEditDlg, CDialogEx)

CProductEditDlg::CProductEditDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PRODUCT, pParent)
	, m_Edit_Product(_T(""))
	, m_Edit_LowPower(0)
	, m_Edit_HightPower_Limit(0)
	, m_Edit_LowPowerCurrent(0)
	, m_Edit_LowPower_ValtageLimit(0)
{
	m_Vect_Program.clear();
	m_bModify = FALSE;
	m_nModifyIndex = -1;
}

CProductEditDlg::~CProductEditDlg()
{
}

void CProductEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Text(pDX, IDC_EDIT_HIGHTPOWER_LIMIT, m_Edit_Product);
	//  DDX_Text(pDX, IDC_EDIT_LOW_POWER, m_Edit_LowPower);
	DDX_Text(pDX, IDC_EDIT_PRODUCT, m_Edit_Product);
	DDX_Text(pDX, IDC_EDIT_HIGHTPOWER_LIMIT, m_Edit_HightPower_Limit);
	DDX_Text(pDX, IDC_EDIT_LOW_POWER, m_Edit_LowPower);
	DDX_Text(pDX, IDC_EDIT_LOW_POWER_CURRENT, m_Edit_LowPowerCurrent);
	DDX_Text(pDX, IDC_EDIT_LOW_POWERLIMIT, m_Edit_LowPower_ValtageLimit);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_REPORT, m_BrowseCtrl_Report);
}


BEGIN_MESSAGE_MAP(CProductEditDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CProductEditDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CProductEditDlg 消息处理程序


BOOL CProductEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (m_bModify)
	{
		if (m_nModifyIndex >= 0 && m_Vect_Program.size() > m_nModifyIndex)
		{
			m_Edit_Product = m_Vect_Program.at(m_nModifyIndex).strProgramName;
			m_Edit_HightPower_Limit = m_Vect_Program.at(m_nModifyIndex).fHightVoltage_Limit;
			m_Edit_LowPower = m_Vect_Program.at(m_nModifyIndex).nLowVoltage;
			m_Edit_LowPower_ValtageLimit = m_Vect_Program.at(m_nModifyIndex).nLowValtageLimit;
			m_Edit_LowPowerCurrent = m_Vect_Program.at(m_nModifyIndex).nLowCurrent;
			m_BrowseCtrl_Report.SetWindowText(m_Vect_Program.at(m_nModifyIndex).strDataSavePath);
		}
	}
	else
	{

	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CProductEditDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!UpdateData())
	{
		return;
	}
	
	if (m_bModify)
	{
		BOOL bExist = FALSE;
		for (int i = 0; i < m_Vect_Program.size(); i++)
		{
			if (i != m_nModifyIndex)
			{
				if (m_Vect_Program.at(i).strProgramName == m_Edit_Product)
				{
					AfxMessageBox(L"程序已经存在!");
					return;
				}
			}
		}
	    m_Vect_Program.at(m_nModifyIndex).fHightVoltage_Limit = m_Edit_HightPower_Limit;
		m_Vect_Program.at(m_nModifyIndex).nLowVoltage = m_Edit_LowPower;
		m_Vect_Program.at(m_nModifyIndex).nLowCurrent = m_Edit_LowPowerCurrent;
		m_Vect_Program.at(m_nModifyIndex).nLowValtageLimit = m_Edit_LowPower_ValtageLimit;
		m_Vect_Program.at(m_nModifyIndex).strProgramName = m_Edit_Product;
		m_BrowseCtrl_Report.GetWindowText(m_Vect_Program.at(m_nModifyIndex).strDataSavePath);
	}
	else
	{
		STRUCT_PROGRAM structtemp;
		structtemp.nProgramID = 1;
		if (m_Vect_Program.size() > 0)
		{
			structtemp.nProgramID = m_Vect_Program.at(m_Vect_Program.size() - 1).nProgramID + 1;
		}
		structtemp.fHightVoltage_Limit = m_Edit_HightPower_Limit;
		structtemp.nLowVoltage = m_Edit_LowPower;
		structtemp.nLowCurrent = m_Edit_LowPowerCurrent;
		structtemp.nLowValtageLimit = m_Edit_LowPower_ValtageLimit;
		structtemp.strProgramName = m_Edit_Product;
		m_BrowseCtrl_Report.GetWindowText(structtemp.strDataSavePath);
		m_Vect_Program.push_back(structtemp);
	}
	CDialogEx::OnOK();
}
