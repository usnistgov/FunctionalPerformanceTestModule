#pragma once


// CEditDevice

class CEditDevice : public CEdit
{
	DECLARE_DYNAMIC(CEditDevice)

public:
	CEditDevice();
	CEditDevice(int nid, int ntype);
	virtual ~CEditDevice();
	int lineID;	// indicates which line it's part of
	int typeID;	// indicates which type of data it holds

protected:
	DECLARE_MESSAGE_MAP()
	/*	typeID	from BTMline.h:
		1		CEditDevice		*line;
		2		CEditDevice		*instance;
		3		CEditDevice		*value;
		4		CEditDevice		*comment;
*/
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


