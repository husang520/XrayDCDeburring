#pragma once

//#include "StaticLogo.h"
// CDetectInfoInputDlg 对话框
#include"StaticCurveVoltageSecond.h"
class CDetectInfoInputDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDetectInfoInputDlg)

public:
	CDetectInfoInputDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDetectInfoInputDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DETECTINFO_INPUT };
#endif
protected:
	CBrush m_BkBrush;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

    
	void SetWorkshift(CString strWorkshift);
	void SetMachineNum(CString strMachineNum);
	void SetMeasureType(CString strMeasureType);
	//CStaticLogo m_Static_DetectMode;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void InitProgramSel();
	void UpdateProgramItem();
	void InitDetectTypeInfo();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonStart();
//	CComboBox m_ComboBox_Factor;
	CComboBox m_ComboBox_ProgSel;
	int m_Edit_Goback_Steps;
//	int m_Edit_IncreaseRate;
	int m_nRepeatTimes;
	CListCtrl m_ListCtrl_ProgramItems;
	CSpinButtonCtrl m_Spin_GobackSteps;
//	CSpinButtonCtrl m_Spin_IncreaseRate;
	CSpinButtonCtrl m_SpinRepeatTimes;
	CStatic m_Static_CurrentStep;
	CStaticCurveVoltageSecond m_Static_VoltageCurve;
	CStatic m_Static_FinishTimes;
	CStatic m_Static_HightVoltage;
	CStatic m_Static_KeepTime_Real;
	CStatic m_Static_LowVoltage;
	afx_msg void OnCbnSelchangeComboProgsel();
	CComboBox m_ComboBox_IncreaseSpeed;
	CString m_Edit_TubeSN;
//	afx_msg void OnTimeChange();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonContinue();
	afx_msg void OnBnClickedButtonStop();
	int m_Edit_StartStep;
	CSpinButtonCtrl m_SpinButtonCtrl_StartStep;
};
