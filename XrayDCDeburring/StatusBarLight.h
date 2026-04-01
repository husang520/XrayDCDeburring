#pragma once


// CStatusBarLight

class CStatusBarLight : public CStatic
{
	DECLARE_DYNAMIC(CStatusBarLight)

public:
	CStatusBarLight();
	virtual ~CStatusBarLight();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
public:
	int m_nStatus;
	BOOL m_bFlashFlag;
	void SetStatus(int nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);
};


