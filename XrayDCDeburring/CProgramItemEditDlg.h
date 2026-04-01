#pragma once


// CProgramItemEditDlg 对话框

class CProgramItemEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProgramItemEditDlg)

public:
	CProgramItemEditDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CProgramItemEditDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROGITEM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int m_Edit_KeepTime;
//	int m_nStep;
	double m_Edit_Valtage;
	int m_Edit_Step;
	int m_nModifyIndex;
	BOOL m_bModify;
	VECT_PROGRAMITEM m_VectProgItem;
	CSpinButtonCtrl m_SpinButtonCtrl_Step;
	afx_msg void OnBnClickedOk();
};
