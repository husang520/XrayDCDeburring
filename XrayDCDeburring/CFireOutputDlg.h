#pragma once


// CFireOutputDlg 对话框

class CFireOutputDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFireOutputDlg)

public:
	CFireOutputDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFireOutputDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FIRE_OUTPUT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ListCtrl_FireOutput;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT OnUpdateFireRecord(WPARAM wParam, LPARAM lParam);
	CButton m_Button_Report;
	afx_msg void OnBnClickedButtonReport();
};
