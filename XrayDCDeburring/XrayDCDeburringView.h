
// XrayDCDeburringView.h: CXrayDCDeburringView 类的接口
//

#pragma once
#include "StaticCurve.h"
#include "StatusBarLight.h"
#include "XrayDCDeburringDoc.h"
class CXrayDCDeburringView : public CFormView
{
protected: // 仅从序列化创建
	CXrayDCDeburringView() noexcept;
	DECLARE_DYNCREATE(CXrayDCDeburringView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_XRAYDCDEBURRING_FORM };
#endif
	CFont *m_Font;
// 特性
public:
	CXrayDCDeburringDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnInitialUpdate(); // 构造后第一次调用
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CXrayDCDeburringView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
//	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	CStatic m_Static_WorkStatus;
	CStatusBarLight m_Static_Shiboqi_Satus;
	CStatusBarLight m_Static_Mcu_Status;
	CStatusBarLight m_Static_Breakdown_Status;
	CStatusBarLight m_Static_Lowpower_Status;
	CStatusBarLight m_Static_HiPowerStatus;
	CStaticCurve m_Static_Curve;
	CStatic m_Static_LouCurrent_Ch1;
	CStatic m_Static_LouCurrent_Ch2;
	CStatic m_Static_LouCurrent_Ch3;
	CStatic m_Static_LouCurrent_Ch4;
	CStatic m_Static_Jichuan_Current;
	CStatic m_Static_Jichuan_CH;
	CStatic m_Static_LowPower_Current;
	CStatic m_Static_LowPower_Voltage;
	CStatic m_Static_SplitLine;
	STRUCT_COMMRET m_StructCommRet;
	STRUCT_BURNINGTEST_PARAM m_Struct_BurningTest_Param;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	CStatic m_Static_Hight_Voltage;
};

#ifndef _DEBUG  // XrayDCDeburringView.cpp 中的调试版本
inline CXrayDCDeburringDoc* CXrayDCDeburringView::GetDocument() const
   { return reinterpret_cast<CXrayDCDeburringDoc*>(m_pDocument); }
#endif

