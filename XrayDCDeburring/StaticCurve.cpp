// StaticCurve.cpp : 实现文件
//

#include "pch.h"
#include "StaticCurve.h"
#include "MainFrm.h"
#include "resource.h"

// CStaticCurve

IMPLEMENT_DYNAMIC(CStaticCurve, CStatic)

CStaticCurve::CStaticCurve()
{
	m_fPixelValueUp = 1.0;
	m_fPixelValueDn = 1.0;
	m_fPixelValueHor = 1.0;
	m_nStartPointIndex = -1;
	m_nEndPointIndex = -1;

	m_MoveLinePos.x = 0.0;
	m_MoveLinePos.y = 0.0;
	m_MoveLineValue = _T("");
	m_MoveLineText = _T("");
	m_vect_pixelvalue.clear();
	m_bSelectAreaMode = FALSE;
	m_nSelectAreaStep = 0;
	m_nMoveSelectBox = 0;
	m_fMaxVoltage = 120;
	m_fTextFont.CreateFont(12,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial"));
}

CStaticCurve::~CStaticCurve()
{
	m_fTextFont.DeleteObject();
}


BEGIN_MESSAGE_MAP(CStaticCurve, CStatic)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CStaticCurve 消息处理程序




void CStaticCurve::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CPoint cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(&cursorPos);
	CRect clientRect;
	GetClientRect(&clientRect);
	if (clientRect.PtInRect(cursorPos))
	{
		InvalidateRect(clientRect, FALSE);
	}
	if (!m_bSelectAreaMode)
	{
		CStatic::OnMouseMove(nFlags, point);
		return;
	}
	int KeyState = GetKeyState(VK_LBUTTON);
	if ((KeyState & 0x80) != 0x80)
	{//按键未被按下
		CPoint CursorPos;
		GetCursorPos(&CursorPos);
		ScreenToClient(&CursorPos);
		if (m_nSelectAreaStep == 2)
		{
			if (CursorPos.x >= m_rectSelectArea.left && CursorPos.x <= m_rectSelectArea.right)
			{
				if (abs(CursorPos.y - m_rectSelectArea.top) < 3)
				{
					m_nMoveSelectBox = 3;
					HCURSOR hCursor = LoadCursor(NULL, IDC_SIZENS);

				}
				else if (abs(CursorPos.y - m_rectSelectArea.bottom) < 3)
				{
					m_nMoveSelectBox = 5;
					HCURSOR hCursor = LoadCursor(NULL, IDC_SIZENS);
				}
				else
				{
					m_nMoveSelectBox = 0;
				}
			}
			else if (CursorPos.y >= m_rectSelectArea.top && CursorPos.y <= m_rectSelectArea.bottom)
			{
				if (abs(CursorPos.x - m_rectSelectArea.left) < 3)
				{
					m_nMoveSelectBox = 2;
					HCURSOR hCursor = LoadCursor(NULL, IDC_SIZEWE);
				}
				else if (abs(CursorPos.x - m_rectSelectArea.right) < 3)
				{
					m_nMoveSelectBox = 4;
					HCURSOR hCursor = LoadCursor(NULL, IDC_SIZEWE);
				}
				else
				{
					m_nMoveSelectBox = 0;
				}
			}
		}
		Invalidate(FALSE);
		::ReleaseCapture();
	}
	else
	{//按键已被按下
		SetCapture();
		CPoint CursorPos;
		GetCursorPos(&CursorPos);
		ScreenToClient(&CursorPos);
		if (m_nSelectAreaStep == 1)
		{

			m_rectSelectArea.right = CursorPos.x;
			m_rectSelectArea.bottom = CursorPos.y;

			Invalidate(FALSE);
		}
		else if (m_nSelectAreaStep == 2)
		{
			if (m_nMoveSelectBox == 1)
			{
				m_rectSelectArea.left = m_rectOldSelectArea.left + (CursorPos.x - m_PointStart.x);
				m_rectSelectArea.top = m_rectOldSelectArea.top + (CursorPos.y - m_PointStart.y);
				m_rectSelectArea.right = m_rectOldSelectArea.right + (CursorPos.x - m_PointStart.x);
				m_rectSelectArea.bottom = m_rectOldSelectArea.bottom + (CursorPos.y - m_PointStart.y);
				HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
				Invalidate(FALSE);
			}
			else if (m_nMoveSelectBox == 2)
			{
				m_rectSelectArea.left = m_rectOldSelectArea.left + (CursorPos.x - m_PointStart.x);
				HCURSOR hCursor = LoadCursor(NULL, IDC_SIZEWE);
				Invalidate(FALSE);
			}
			else if (m_nMoveSelectBox == 3)
			{
				m_rectSelectArea.top = m_rectOldSelectArea.top + (CursorPos.y - m_PointStart.y);
				HCURSOR hCursor = LoadCursor(NULL, IDC_SIZENS);
				Invalidate(FALSE);
			}
			else if (m_nMoveSelectBox == 4)
			{
				m_rectSelectArea.right = m_rectOldSelectArea.right + (CursorPos.x - m_PointStart.x);
				HCURSOR hCursor = LoadCursor(NULL, IDC_SIZEWE);
				Invalidate(FALSE);
			}
			else if (m_nMoveSelectBox == 5)
			{
				m_rectSelectArea.bottom = m_rectOldSelectArea.bottom + (CursorPos.y - m_PointStart.y);
				HCURSOR hCursor = LoadCursor(NULL, IDC_SIZENS);
				Invalidate(FALSE);
			}

		}
	}
	CStatic::OnMouseMove(nFlags, point);
}


void CStaticCurve::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CStatic::OnPaint()
	CRect clientRect;
	GetClientRect(&clientRect);
	if (clientRect.Height() <= 0)
	{
		return;
	}
	CMemDC memDC(dc, clientRect);
	DrawAllElement(&memDC.GetDC(), clientRect);
	DrawSelectBox(&memDC.GetDC(), clientRect);


}

void CStaticCurve::DrawSelectBox(CDC* pDC, CRect cRect)
{
	//画选择框
	if (m_bSelectAreaMode)
	{
		CPen penSelect(PS_SOLID, 1, RGB(0, 255, 0));
		CPen* pOldPen = pDC->SelectObject(&penSelect);
		CBrush* boxBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));;
		CBrush* poldBursh = pDC->SelectObject(boxBrush);

		pDC->Rectangle(m_rectSelectArea);

		pDC->SelectObject(poldBursh);
		pDC->SelectObject(pOldPen);
		penSelect.DeleteObject();
	}
}
void CStaticCurve::DrawGraph(CDC * pDC, CRect cRect)
{
	CFont *oldFont = pDC->SelectObject(&m_fTextFont);
	CRect graphBkRect = cRect;
	DrawGraphBackground(pDC, graphBkRect);
	CRect m_rcXgraph = graphBkRect;
	//画坐标
	double fOnePixcelValue = 0;

	int nWidth = cRect.Width()-50;
	int nHeight = cRect.Height();
	
	int nBottom = nHeight - 40;

	CPen BlackPen;
	BlackPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* oldPen = pDC->SelectObject(&BlackPen);
	pDC->MoveTo(40,20);
	pDC->LineTo(40, nBottom);
	pDC->MoveTo(40, nBottom);
	pDC->LineTo(cRect.Width() - 10, nBottom);
	pDC->SetBkMode(TRANSPARENT);
	double maxValue = 0,minValue = 65535;
	//for (int i = 0; i < m_vect_pixelvalue.size(); i++)
	//{
	//	if (m_vect_pixelvalue.at(i) > maxValue)
	//	{
	//		maxValue = m_vect_pixelvalue.at(i);
	//	}
	//	if (m_vect_pixelvalue.at(i) < minValue)
	//	{
	//		minValue = m_vect_pixelvalue.at(i);
	//	}
	//}
	maxValue = ceil(m_fMaxVoltage/100.0)*100;
	minValue = 0;
	maxValue = maxValue;
	minValue = minValue;
	int nMinvalue = floor(minValue+0.5);
	int nmaxValue = floor(maxValue+0.5);
	double nSpanValue = nmaxValue - nMinvalue;
	double fonePixelValueY = nSpanValue / ((nBottom- 20)*1.0);
	double fSectionValue = (nBottom-20) / 5.0;
	for (int i = 0; i < 6; i++)
	{
		pDC->MoveTo(35, nBottom - fSectionValue *i);
		pDC->LineTo(40, nBottom - fSectionValue * i);
		CRect  textRect(0, nBottom - fSectionValue * i - 10, 37, nBottom - fSectionValue * i + 10);
		CString strText = L"";
		strText.Format(L"%d", nMinvalue+ nmaxValue/5*i);

		pDC->DrawText(strText, textRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
        
	}

	//pDC->MoveTo(35, 300 - 255);
	//pDC->LineTo(40, 300-255);
	//CRect  textRect(0, 300 - 255 - 10, 37, 300 -255 + 10);
	//CString strText = L"";
	//strText.Format(L"%d", 255);
	//pDC->DrawText(strText, textRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	int nSection = (nWidth / 10.0);
	for (int i = 0; i < 10; i++)
	{
		pDC->MoveTo(40 + nSection*i, nBottom);
		pDC->LineTo(40 + nSection * i, nBottom+5);
	}
	pDC->MoveTo(40 + nWidth, nBottom);
	pDC->LineTo(40 + nWidth, nBottom+5);


	int nValueSize = 0;
	if (m_nEndPointIndex != -1 && m_nStartPointIndex != -1)
	{
		nValueSize = m_nEndPointIndex - m_nStartPointIndex;
	}
	else
	{
		nValueSize = m_vect_pixelvalue.size();
	}
	int nMaxValueSize = ceil(nValueSize / 100.0) * 100;
	if (nValueSize == 0)
	{
		nMaxValueSize = 10;
	}
	for (int i = 0; i < 11; i++)
	{
		CString strText = L"";
		strText.Format(L"%d", 0 + nMaxValueSize / 10 * i);
		CRect textRect(40 + nSection * i - 10, nBottom + 7, 40 + nSection * i + 10, nBottom + 22);
		pDC->DrawText(strText, textRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}

	if (nValueSize <= 0)
	{
		pDC->SelectObject(oldFont);
		pDC->SetBkMode(OPAQUE);
		pDC->SelectObject(oldPen);
		BlackPen.DeleteObject();
		return;
	}

	fOnePixcelValue = (nWidth) / (nMaxValueSize * 1.0);

	CString strText = L"";
	strText.Format(L"%d", nValueSize);
	CRect  textRectver(nWidth-100, nBottom+3, nWidth+100, nBottom+3 + 10);
	pDC->DrawText(strText, textRectver, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	////画最大值和最小值
	//strText.Format(L"Min = %.2f                Max = %.2f", minValue, maxValue);
	//textRectver.left = 0;
	//textRectver.right = cRect.Width();
	//textRectver.top = 305;
	//textRectver.bottom = 350;
	//pDC->DrawText(strText, textRectver, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	pDC->SetBkMode(OPAQUE);
	pDC->SelectObject(oldPen);
	BlackPen.DeleteObject();


	std::vector<CPoint> vect_pts;
	vect_pts.clear();
	int nPointcount = 0;
	CPoint PointTemp;
	if (m_nEndPointIndex != -1 && m_nStartPointIndex != -1)
	{
		for (int i = m_nStartPointIndex; i < m_nEndPointIndex; i++)
		{
			if (m_vect_pixelvalue.size() > i)
			{
				PointTemp.x = 40 + (i - m_nStartPointIndex) * fOnePixcelValue;
				PointTemp.y = nBottom - (m_vect_pixelvalue.at(i) - nMinvalue) / fonePixelValueY;
				vect_pts.push_back(PointTemp);
			}

		}
	}
	else
	{
		for (int i = 0; i < m_vect_pixelvalue.size(); i++)
		{
			PointTemp.x = 40 + i * fOnePixcelValue;
			PointTemp.y = nBottom - (m_vect_pixelvalue.at(i) - nMinvalue) / fonePixelValueY;
			vect_pts.push_back(PointTemp);
		}
	}


	CPen LinePen;
	LinePen.CreatePen(PS_SOLID, 1, RGB(0, 80, 0));
	oldPen = pDC->SelectObject(&LinePen);
	
	CPoint *PointArray = new CPoint[vect_pts.size()];


		for (int i = 0; i < vect_pts.size(); i++)
		{
			PointArray[i] = vect_pts.at(i);
		}
	

	pDC->Polyline(PointArray, vect_pts.size());
	pDC->SelectObject(oldPen);
	LinePen.DeleteObject();
	if (PointArray != NULL)
	{
		delete[] PointArray;
		PointArray = NULL;
	}



	////画移动线
	//LinePen.CreatePen(PS_SOLID, 1, RGB(180, 5, 150));
	//oldPen = pDC->SelectObject(&LinePen);
	//CMainFrame *pMainframe = (CMainFrame *)AfxGetMainWnd();
	////CFriedSPCsysDoc *pDoc = (CFriedSPCsysDoc *)pMainframe->GetActiveDocument();
	//int nIndex = 0;
	//for (int i = 0; i < vect_pts.size(); i++)
	//{
	//	if (m_MoveLinePos.x == -1 && m_MoveLinePos.y == -1)
	//	{
	//		m_MoveLinePos.x = vect_pts.at(i).x;
	//		m_MoveLinePos.y = vect_pts.at(i).y;
	//		m_MoveLineValue = _T("");
	//	}
	//	CRect recttemp = graphBkRect;
	//	recttemp.left = vect_pts.at(i).pPt.x - m_fPixelValueHor / 2;
	//	recttemp.right = vect_pts.at(i).pPt.x + m_fPixelValueHor / 2;

	//	CPoint cursorPos;
	//	GetCursorPos(&cursorPos);
	//	ScreenToClient(&cursorPos);

	//	if (recttemp.PtInRect(cursorPos))
	//	{
	//		nIndex = i;
	//		m_MoveLinePos.x = vect_pts.at(i).pPt.x;
	//		m_MoveLinePos.y = vect_pts.at(i).pPt.y;
	//		//m_MoveLineValue.Format(_T("%.*f"), pMainframe->m_nDotBitNum, m_Vect_AlarmData.at(i).fValue);
	//		//if (m_structCurveData.vect_value_event.at(i).nEventID <= 0)
	//		//{
	//		//	m_MoveLineValue.Format(_T("%.*f"), pMainframe->m_nDotBitNum, m_structCurveData.vect_value_event.at(i).fValue);
	//		//	m_MoveLineText = _T();
	//		//}
	//		//else
	//		//{
	//		//	for (int q = 0; q < pMainframe->m_vect_events_Alarm.size(); q++)
	//		//	{
	//		//		if (m_structCurveData.vect_value_event.at(i).nEventID == pMainframe->m_vect_events_Alarm.at(q).nID)
	//		//		{
	//		//			m_MoveLineValue.Format(_T("%.*f"), pMainframe->m_nDotBitNum, m_structCurveData.vect_value_event.at(i).fValue);
	//		//			m_MoveLineText = pMainframe->m_vect_events_Alarm.at(q).strTitleName;
	//		//			break;
	//		//		}
	//		//	}
	//		//}
	//		break;
	//	}
	//}


	////if (m_structCurveData.vect_value_event.size()>0 && (m_MoveLinePos.x != -1 || m_MoveLinePos.y != -1))
	//if ((m_MoveLinePos.x != -1 || m_MoveLinePos.y != -1))
	//{
	//	pDC->MoveTo(m_MoveLinePos.x, graphBkRect.top + 10);
	//	pDC->LineTo(m_MoveLinePos.x, graphBkRect.bottom - 5);
	//	CRect TextRect;
	//	CSize sz = pDC->GetTextExtent(m_MoveLineValue);
	//	TextRect.left = m_MoveLinePos.x - sz.cx / 2;
	//	TextRect.right = m_MoveLinePos.x + sz.cx / 2;
	//	TextRect.top = graphBkRect.top + 1;
	//	TextRect.bottom = graphBkRect.top + 10;
	//	pDC->SetBkMode(TRANSPARENT);
	//	COLORREF oldColor = pDC->SetTextColor(RGB(180, 5, 150));
	//	pDC->DrawText(m_MoveLineValue, TextRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	//	pDC->SetTextColor(oldColor);


	//	TextRect.left = m_MoveLinePos.x - 30;
	//	TextRect.right = m_MoveLinePos.x + 30;
	//	TextRect.top = TextRect.bottom + 2;
	//	TextRect.bottom = TextRect.top + 10;

	//	oldColor = pDC->SetTextColor(RGB(180, 150, 15));
	//	pDC->DrawText(m_MoveLineValue, TextRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	//	pDC->SetTextColor(oldColor);

	//	pDC->SetBkMode(OPAQUE);


	//}
	//pDC->SelectObject(oldPen);
	//LinePen.DeleteObject();

	pDC->SelectObject(oldFont);
}
void CStaticCurve::DrawAllElement(CDC* pDC, CRect cRect)
{
	//画背景
	CBrush BkBrush;
	CPen   borderPen;
	borderPen.CreatePen(PS_SOLID, 1, RGB(127, 127, 127));
	BkBrush.CreateSolidBrush(0xCCCCCC);
	CBrush* oldBrush = pDC->SelectObject(&BkBrush);
	CPen* oldPen = pDC->SelectObject(&borderPen);
	pDC->Rectangle(cRect);
	pDC->SelectObject(oldBrush);
	pDC->SelectObject(oldPen);

	////画值图--控件绘图区域
	CRect XRect = cRect;
	XRect.top += 1;
	XRect.bottom -= 1;
	XRect.left += 1;
	XRect.right -= 1;
	DrawGraph(pDC, XRect);

	BkBrush.DeleteObject();
	borderPen.DeleteObject();
}
void CStaticCurve::DrawGraphBackground(CDC * pDC, CRect cRect)
{
	TRIVERTEX vert[4];
	_GRADIENT_TRIANGLE gRect[2];

	vert[0].x = cRect.left;
	vert[0].y = cRect.top;
	vert[0].Red = 230 * 0x101;
	vert[0].Green = 0xFF * 0x101;
	vert[0].Blue = 230 * 0x101;
	vert[0].Alpha = 0x0000;
	vert[1].x = cRect.left;
	vert[1].y = cRect.bottom;
	vert[1].Red = 230 * 0x101;
	vert[1].Green = 0xFF * 0x101;
	vert[1].Blue = 230 * 0x101;
	vert[1].Alpha = 0x0000;
	vert[2].x = cRect.right;
	vert[2].y = cRect.bottom;
	vert[2].Red = 230 * 0x101;
	vert[2].Green = 0xFF * 0x101;
	vert[2].Blue = 230 * 0x101;
	vert[2].Alpha = 0x0000;
	vert[3].x = cRect.right;
	vert[3].y = cRect.top;
	vert[3].Red = 200 * 0x101;
	vert[3].Green = 235 * 0x101;
	vert[3].Blue = 200 * 0x101;
	vert[3].Alpha = 0x0000;

	gRect[0].Vertex1 = 1;
	gRect[0].Vertex2 = 0;
	gRect[0].Vertex3 = 3;
	gRect[1].Vertex1 = 2;
	gRect[1].Vertex2 = 1;
	gRect[1].Vertex3 = 3;
	pDC->GradientFill(vert, 4, gRect, 2, GRADIENT_FILL_TRIANGLE);
	CPen borderPen;
	borderPen.CreatePen(PS_SOLID, 0, RGB(127, 127, 127));
	CPen *oldPen = pDC->SelectObject(&borderPen);
	CGdiObject *oldObj = pDC->SelectStockObject(NULL_BRUSH);
	pDC->Rectangle(cRect);
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldObj);
	borderPen.DeleteObject();
}





void CStaticCurve::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

		if (m_bSelectAreaMode)
		{
			if (m_nSelectAreaStep == 1)
			{
				CPoint CursorPos;
				GetCursorPos(&CursorPos);
				ScreenToClient(&CursorPos);
				m_rectSelectArea.right = CursorPos.x;
				m_rectSelectArea.bottom = CursorPos.y;
				m_nSelectAreaStep = 2;
			}
		}
	
	CStatic::OnLButtonUp(nFlags, point);
}


void CStaticCurve::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bSelectAreaMode)
	{
		CPoint CursorPos;
		GetCursorPos(&CursorPos);
		ScreenToClient(&CursorPos);
		CRect clientRect;
		GetClientRect(&clientRect);
		if (CursorPos.y <= clientRect.Height())
		{
			if (m_nSelectAreaStep == 0)
			{
				m_nSelectAreaStep = 1;
				m_rectSelectArea.left = CursorPos.x;
				m_rectSelectArea.right = CursorPos.x;
				m_rectSelectArea.top = CursorPos.y;
				m_rectSelectArea.bottom = CursorPos.y;
			}
			else if (m_nSelectAreaStep == 2)
			{
				if (m_rectSelectArea.PtInRect(CursorPos) && m_nMoveSelectBox == 0)
				{
					m_nMoveSelectBox = 1;
					HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
				}

				if (m_nMoveSelectBox == 1)
				{
					HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
				}
				else if (m_nMoveSelectBox == 2)
				{
					HCURSOR hCursor = LoadCursor(NULL, IDC_SIZEWE);
				}
				else if (m_nMoveSelectBox == 3)
				{
					HCURSOR hCursor = LoadCursor(NULL, IDC_SIZENS);
				}
				else if (m_nMoveSelectBox == 4)
				{
					HCURSOR hCursor = LoadCursor(NULL, IDC_SIZEWE);
				}
				else if (m_nMoveSelectBox == 5)
				{
					HCURSOR hCursor = LoadCursor(NULL, IDC_SIZENS);
				}

				m_PointStart = CursorPos;
				m_rectOldSelectArea = m_rectSelectArea;
			}
		}
	}
	CStatic::OnLButtonDown(nFlags, point);
}


void CStaticCurve::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CStatic::OnMButtonDblClk(nFlags, point);
}


void CStaticCurve::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect clientRect;
	GetClientRect(clientRect);
	m_bSelectAreaMode = FALSE;
	m_nStartPointIndex = (m_rectSelectArea.left - 40) / ((clientRect.Width() - 50) * 1.0) * m_vect_pixelvalue.size();
	m_nEndPointIndex = (m_rectSelectArea.right - 40) / ((clientRect.Width() - 50) * 1.0) * m_vect_pixelvalue.size();
	Invalidate(FALSE);
	CStatic::OnLButtonDblClk(nFlags, point);
}
