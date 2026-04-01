// StatusBarLight.cpp : 实现文件
//

#include "pch.h"
#include "XrayDCDeburring.h"
#include "StatusBarLight.h"


// CStatusBarLight

IMPLEMENT_DYNAMIC(CStatusBarLight, CStatic)

CStatusBarLight::CStatusBarLight()
{
	m_nStatus = LIGHT_STATUS_NG;
	m_bFlashFlag = FALSE;


}

CStatusBarLight::~CStatusBarLight()
{
}


BEGIN_MESSAGE_MAP(CStatusBarLight, CStatic)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CStatusBarLight 消息处理程序




void CStatusBarLight::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CStatic::OnPaint()
	CRect ClientRect;
	GetClientRect(&ClientRect);
	if (m_bFlashFlag)
	{ 
		BYTE nRed, nGreen, nBlue;
		CString strText = _T("");
		int nLevel = 3;
		if (m_nStatus == LIGHT_STATUS_NG)
		{
			nRed = 0xF0;
			nGreen = 0x00;
			nBlue = 0x00;
			//strText = _T("NG");
		}
		else if(m_nStatus == LIGHT_STATUS_OK)
		{
			nRed = 0x00;
			nGreen = 0xF0;
			nBlue = 0x00;
			//strText = "OK";
		}
		else if (m_nStatus == LIGHT_STATUS_BAD)
		{
			nRed = 0xF0;
			nGreen = 0x00;
			nBlue = 0x00;
		}
		else if (m_nStatus == LIGHT_STATUS_GOOD)
		{
			nRed = 0x80;
			nGreen = 0x80;
			nBlue = 0x80;
		}

		//TRIVERTEX vert[5];
		//_GRADIENT_TRIANGLE gRect[4];
		//CRect clientRect;
		//GetClientRect(&clientRect);
		//vert[0].x = clientRect.left;
		//vert[0].y = clientRect.top;
		//vert[0].Red = nRed / nLevel * 0x101;
		//vert[0].Green = nGreen / nLevel * 0x101;
		//vert[0].Blue = nBlue * 0x101;
		//vert[0].Alpha = 0x0000;
		//vert[1].x = clientRect.right / 2;
		//vert[1].y = clientRect.bottom / 2;
		//vert[1].Red = nRed * 0x101;
		//vert[1].Green = nGreen * 0x101;
		//vert[1].Blue = nBlue * 0x101;
		//vert[1].Alpha = 0x0000;
		//vert[2].x = clientRect.right;
		//vert[2].y = clientRect.top;
		//vert[2].Red = nRed / nLevel * 0x101;
		//vert[2].Green = nGreen / nLevel * 0x101;
		//vert[2].Blue = nBlue * 0x101;
		//vert[2].Alpha = 0x0000;
		//vert[3].x = clientRect.right;
		//vert[3].y = clientRect.bottom;
		//vert[3].Red = nRed / nLevel * 0x101;
		//vert[3].Green = nGreen / nLevel * 0x101;
		//vert[3].Blue = nBlue * 0x101;
		//vert[3].Alpha = 0xFFFF;
		//vert[4].x = clientRect.left;
		//vert[4].y = clientRect.bottom;
		//vert[4].Red = nRed / nLevel * 0x101;
		//vert[4].Green = nGreen / nLevel * 0x101;
		//vert[4].Blue = nBlue * 0x101;
		//vert[4].Alpha = 0xFFFF;
		//gRect[0].Vertex1 = 0;
		//gRect[0].Vertex2 = 1;
		//gRect[0].Vertex3 = 2;
		//gRect[1].Vertex1 = 2;
		//gRect[1].Vertex2 = 3;
		//gRect[1].Vertex3 = 1;
		//gRect[2].Vertex1 = 3;
		//gRect[2].Vertex2 = 4;
		//gRect[2].Vertex3 = 1;
		//gRect[3].Vertex1 = 4;
		//gRect[3].Vertex2 = 0;
		//gRect[3].Vertex3 = 1;
		//dc.GradientFill(vert, 5, gRect, 4, GRADIENT_FILL_TRIANGLE);
		dc.FillSolidRect(ClientRect, RGB(nRed, nGreen, nBlue));
	}
	else
	{
		BYTE nRed, nGreen, nBlue;
		if (m_nStatus == LIGHT_STATUS_NG)
		{
			nRed = 0xF0;
			nGreen = 0x00;
			nBlue = 0x00;
			//strText = _T("NG");
		}
		else if (m_nStatus == LIGHT_STATUS_OK)
		{
			nRed = 0x00;
			nGreen = 0xF0;
			nBlue = 0x00;
			//strText = "OK";
		}
		else if (m_nStatus == LIGHT_STATUS_BAD)
		{
			nRed = 0xF0;
			nGreen = 0x00;
			nBlue = 0x00;
		}
		else if (m_nStatus == LIGHT_STATUS_GOOD)
		{
			nRed = 0x80;
			nGreen = 0x80;
			nBlue = 0x80;
		}
		dc.FillSolidRect(ClientRect, RGB(nRed, nGreen, nBlue));

		
	}

}


void CStatusBarLight::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		m_bFlashFlag = !m_bFlashFlag;
		Invalidate(FALSE);
	}

	CStatic::OnTimer(nIDEvent);
}


BOOL CStatusBarLight::Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO:  在此添加专用代码和/或调用基类
	
	return CStatic::Create(lpszText, dwStyle, rect, pParentWnd, nID);
}

void CStatusBarLight::SetStatus(int nStatus)
{
	m_nStatus = nStatus;
}