#pragma once


// CCommSetDlg 对话框

class CCommSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCommSetDlg)

public:
	CCommSetDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCommSetDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_COMMSET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_ComboBox_HightPower_Com;
	CComboBox m_ComboBox_LowPower_Com;
	CComboBox m_ComboBox_MCU_Com;
	CString m_Edit_Shiboqi_IP;
	BOOL GetSystemSerialComport(CArray<CString, CString>& comarray);
	afx_msg void OnBnClickedOk();
	afx_msg void OnMenuCreateprogram();
	afx_msg void OnUpdateMenuCreateprogram(CCmdUI* pCmdUI);
	CComboBox m_ComboBox_HiPower_Baudrate;
	CComboBox m_ComboBox_Lowpower_Baudrate;
	CComboBox m_ComboBox_Mcu_Baudrate;
	CComboBox m_ComboBox_Shiboqi_Baudrate;
	CComboBox m_ComboBox_Shiboqi_Com;
};
