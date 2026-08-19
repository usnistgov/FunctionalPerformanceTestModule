#pragma once
//#include "BTMline.h"

// CMDateTimeCtrl

class CMDateTimeCtrl : public CDateTimeCtrl
{
	DECLARE_DYNAMIC(CMDateTimeCtrl)

public:
	CMDateTimeCtrl();
	CMDateTimeCtrl(int lid);
	virtual ~CMDateTimeCtrl();
	void OnRButtonDown(UINT nFlags, CPoint point);
protected:
	DECLARE_MESSAGE_MAP()
	int lastinc;	// tracks right-button click state- 1 for incremented last time, 0 for didn't increment.  Init to 1 so first RC is not incrementing
public:
	afx_msg void OnDtnDatetimechange(NMHDR *pNMHDR, LRESULT *pResult);
	int lineID;
};


