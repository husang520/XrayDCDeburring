#pragma once
#include "pch.h"

// CStaticCurveVoltageSecond

class CStaticCurveVoltageSecond : public CStatic
{
	DECLARE_DYNAMIC(CStaticCurveVoltageSecond)

public:
	CStaticCurveVoltageSecond();
	virtual ~CStaticCurveVoltageSecond();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();

	CFont m_fTextFont;
	//纵向
	double m_fPixelValueUp;
	double m_fPixelValueDn;
	//横向
	double m_fPixelValueHor;
	//八大判异原则
	BOOL m_bAlarm;
	//鼠标所在线位置
	CPoint m_MoveLinePos;
	//鼠标线对应显示的报警事件
	CString m_MoveLineValue;
	CString m_MoveLineText;
	CRect m_rectSelectArea;
	BOOL m_bSelectAreaMode;
	int m_nSelectAreaStep;
	int m_nMoveSelectBox;
	CPoint m_PointStart;
	CPoint m_PointEnd;
	CRect m_rectOldSelectArea;
	int m_nStartPointIndex;
	int m_nEndPointIndex;
	int m_nMaxVoltage;
	int m_nStep;
	STRUCT_PROGRAM m_struct_Program;
	//当前程序被刷新时 数据结构体
	//VECT_CURVEVALUE m_vect_pixelvalue;
	void DrawAllElement(CDC* pDC, CRect cRect);
	void DrawGraph(CDC * pDC, CRect cRect);
	void DrawGraphBackground(CDC * pDC, CRect cRect);
	void DrawSelectBox(CDC* pDC, CRect cRect);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


