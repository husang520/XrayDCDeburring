#pragma once


// CFireRecordDlg 对话框

class CFireRecordDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFireRecordDlg)

public:
	CFireRecordDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFireRecordDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FIRERECORD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	CDateTimeCtrl m_DataTime_Start;
	CDateTimeCtrl m_DataTime_End;
	CDateTimeCtrl m_DataTime_Start;
	CListCtrl m_List_FireRecord;
	afx_msg void OnBnClickedButtonQuery();
	virtual BOOL OnInitDialog();
	void UpdateListFire(CString strSql=L"");
	CString m_Edit_TubSn;
	CButton m_Check_TIME;
	CButton m_Check_TubeSn;
};
