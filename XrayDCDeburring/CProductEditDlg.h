#pragma once


// CProductEditDlg 对话框

class CProductEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProductEditDlg)

public:
	CProductEditDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CProductEditDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PRODUCT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	CString m_Edit_Product;
//	int m_Edit_LowPower;
	VECT_PROGRAM m_Vect_Program;
	CString m_Edit_Product;
	double m_Edit_HightPower_Limit;
	double m_Edit_LowPower;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	BOOL m_bModify;
	int m_nModifyIndex;
	int m_Edit_LowPowerCurrent;
	int m_Edit_LowPower_ValtageLimit;
	CMFCEditBrowseCtrl m_BrowseCtrl_Report;
};
