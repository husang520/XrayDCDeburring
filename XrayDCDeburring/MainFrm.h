
// MainFrm.h: CMainFrame 类的接口
//

#pragma once
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "include/SerialMFC.h"
#include "include/sqlite3.h"
#include "DatabaseControl.h"



class CMainFrame : public CFrameWndEx
{
	
protected: // 仅从序列化创建
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// 特性
public:
	CSerialMFC m_Serial_MCU;
	CSerialMFC m_Serial_HightPower;
	CSerialMFC m_Serial_LowPower;
	CSerialMFC m_Serial_Shiboqi;
	CString m_strHightPower_Com;
	CString m_strLowPower_Com;
	CString m_strMcu_Com;
	CString m_strShiboqi_Com;
	CString m_strShiboqi_IP;
	int m_nBaudRate_MCU;
	int m_nBaudRate_HightPower;
	int m_nBaudRate_LowPower;
	int m_nBaudRate_Shiboqi;
	VECT_PROGRAM m_Vect_Program;
	CDatabaseControl m_dbControl;
	STRUCT_PROGRAM m_struct_CurrentProgram;
// 操作
public:
	void ReadIniConfig();
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
public:
	COutputWnd        m_wndOutput;


// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	//afx_msg void OnToolbarCommset();
	//afx_msg void OnUpdateToolbarCommset(CCmdUI* pCmdUI);
	//afx_msg void OnToolbarTestplan();
	//afx_msg void OnUpdateToolbarTestplan(CCmdUI* pCmdUI);
	//afx_msg void OnToolbarDeviceLink();
	//afx_msg void OnUpdateToolbarDeviceLink(CCmdUI* pCmdUI);

	BOOL InitializeHightPower();
	BOOL InitializeLowPower();
	BOOL InitializeMcu();
	BOOL InitializeShiboqi();
	BOOL CreateCommThreads();
	BOOL StopCommThreads();
	BOOL StartTestThread();
	BOOL StopTestThread();
	BOOL InitializeTest();
	BOOL SetLowpowerLimit(int nLowpowerLimit);
	BOOL SetLowpowerCurrent(int nLowpowerCurrent);
	BOOL SetLowpowerVoltage(int nLowpowerVoltage);
	BOOL SetHipowerVoltage(int nHipowerVoltage);
	BOOL StopHipowerOut();
	BOOL StopLowpowerOut();
	BOOL StartLowpowerOut();
	BOOL ResetMcu();
	unsigned short CRC16(const unsigned char* puchMsg, unsigned char usDataLen);
	uint8_t get_check_sum(uint8_t* buf, uint8_t len);
	CWinThread* m_pThread_HightPower;
	CWinThread* m_pThread_LowPower;
	CWinThread* m_pThread_Mcu;
	CWinThread* m_pThread_Shiboqi;
	CWinThread* m_pThread_TestThread;
	CPropertiesWnd    m_wndProperties;
	BOOL m_bStopHightPowerThread;
	BOOL m_bStopLowPowerThread;
	BOOL m_bStopMcuThread;
	BOOL m_bStopShiboqiThread;
	BOOL m_bStopTestThread;
	BOOL m_bBeginGetValue;
	CTime m_tBeginGetValue;
	CString m_strUser;
	STRUCT_COMMRET m_Struct_CommRet;
	STRUCT_COMMRET m_Struct_CommRetOld;
	BOOL m_bFireFlag;
	STRUCT_COMMSTATUS m_Struct_CommStatus;
	STRUCT_BURNINGTEST_PARAM m_Struct_BurningTest_Param;
	VECT_CURVEVALUE m_Vect_CurveValue;
	VECT_FIRERECORD m_Vect_FireRecord;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnToolbarCommset();
	afx_msg void OnUpdateToolbarCommset(CCmdUI* pCmdUI);
	afx_msg void OnToolbarTestplan();
	afx_msg void OnUpdateToolbarTestplan(CCmdUI* pCmdUI);
	afx_msg void OnToolbarDeviceLink();
	afx_msg void OnUpdateToolbarDeviceLink(CCmdUI* pCmdUI);
	LRESULT OnExecuteCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnToolbarFirerecord();
	afx_msg void OnUpdateToolbarFirerecord(CCmdUI* pCmdUI);
	void SaveTestRecode();
	SAFEARRAY* GetSafeArrayForExport(void* workshet);
};




