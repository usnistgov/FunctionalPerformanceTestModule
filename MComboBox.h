#pragma once


// CMComboBox

class CMComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CMComboBox)

public:
	CMComboBox();
	virtual ~CMComboBox();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchange();
	// Constructor with line ID
	CMComboBox(int lid, int type);
	int lineID;
	int typeID;
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	void SetObjectSel(int obj);	// only used for objects, maps object ID to dropdown index
	//	typeID:	affects:
	//		0	*mode;		// read or write - read=0, write=1
	//		1	*device;	// mac / router
	//		2	*object;	// BACnet object

};


