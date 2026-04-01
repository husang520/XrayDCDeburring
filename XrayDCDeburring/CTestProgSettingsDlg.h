#pragma once


// CTestProgSettingsDlg 对话框

class CTestProgSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTestProgSettingsDlg)

public:
	CTestProgSettingsDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTestProgSettingsDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TESTPROG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_Edit_HightPower_Limit;
	CString m_Edit_LowpowerVoltage;
	CListCtrl m_ListCtrl_ProgramItem;
	CTreeCtrl m_TreeCtrl_Product; 
	CImageList m_Imagelist;
	VECT_PROGRAM m_Vect_Program;
	CString m_strCurrentProgram;
	void InitTreeCtrl();
	void RefreshTreeListProgsel();

	void RefreshDetectInfoShow(int nProgIndex,int nProgitemIndex = -1);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnNMRClickTreeProduct(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuCreateprogram();
	afx_msg void OnUpdateMenuCreateprogram(CCmdUI* pCmdUI);
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnMenuModifyprogram();
	afx_msg void OnUpdateMenuModifyprogram(CCmdUI* pCmdUI);
	afx_msg void OnTvnSelchangedTreeProduct(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnMenuDeletprogram();
	afx_msg void OnUpdateMenuDeletprogram(CCmdUI* pCmdUI);
};
