// CProgramItemEditDlg.cpp: 实现文件
//

#include "pch.h"
#include "XrayDCDeburring.h"
#include "CProgramItemEditDlg.h"
#include "afxdialogex.h"


// CProgramItemEditDlg 对话框

IMPLEMENT_DYNAMIC(CProgramItemEditDlg, CDialogEx)

CProgramItemEditDlg::CProgramItemEditDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PROGITEM, pParent)
	, m_Edit_KeepTime(0)
	, m_Edit_Valtage(0)
	, m_Edit_Step(1)
{
	m_nModifyIndex = -1;
	m_bModify = FALSE;
	m_VectProgItem.clear();
}

CProgramItemEditDlg::~CProgramItemEditDlg()
{
}

void CProgramItemEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_KEEPTIME, m_Edit_KeepTime);
	//  DDX_Text(pDX, IDC_EDIT_STEPS, m_nStep);
	DDX_Text(pDX, IDC_EDIT_VALTAGE, m_Edit_Valtage);
	DDX_Text(pDX, IDC_EDIT_STEPS, m_Edit_Step);
	DDX_Control(pDX, IDC_SPIN_STEPS, m_SpinButtonCtrl_Step);
}


BEGIN_MESSAGE_MAP(CProgramItemEditDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CProgramItemEditDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CProgramItemEditDlg 消息处理程序


BOOL CProgramItemEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (m_bModify)
	{
		if (m_VectProgItem.size() > m_nModifyIndex)
		{
			m_Edit_Step = m_VectProgItem.at(m_nModifyIndex).nStep;
			m_Edit_Valtage = m_VectProgItem.at(m_nModifyIndex).nHightVotage;
			m_Edit_KeepTime = m_VectProgItem.at(m_nModifyIndex).nKeepTime;
		}
	}
	else
	{
		if (m_VectProgItem.size() > 0)
		{
			m_Edit_Step = m_VectProgItem.at(m_VectProgItem.size() - 1).nStep + 1;
		}
	}
	m_SpinButtonCtrl_Step.SetRange(1, 1000);
	UDACCEL Accel;
	Accel.nSec = 0;  // 立即响应
	Accel.nInc = 1; // 每次增加10
	m_SpinButtonCtrl_Step.SetAccel(1, &Accel);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CProgramItemEditDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!UpdateData(TRUE))
	{
		return;
	}
	if (m_bModify)
	{
		if (m_VectProgItem.size() > m_nModifyIndex)
		{
			m_VectProgItem.at(m_nModifyIndex).nStep = m_Edit_Step;
			m_VectProgItem.at(m_nModifyIndex).nHightVotage = m_Edit_Valtage;
			m_VectProgItem.at(m_nModifyIndex).nKeepTime = m_Edit_KeepTime;
		}
	}
	else
	{
		STRUCT_PROGRAMITEM structtemp;
		structtemp.nStep = m_Edit_Step;
		structtemp.nHightVotage = m_Edit_Valtage;
		structtemp.nKeepTime = m_Edit_KeepTime;
		m_VectProgItem.push_back(structtemp);
	}

	CDialogEx::OnOK();
}
