// ComboBoxDevice.cpp : implementation file
//

#include "stdafx.h"
#include "FPTM App.h"
#include "ComboBoxDevice.h"


// CComboBoxDevice

IMPLEMENT_DYNAMIC(CComboBoxDevice, CComboBox)

CComboBoxDevice::CComboBoxDevice()
{

}

CComboBoxDevice::~CComboBoxDevice()
{
}

//http://stackoverflow.com/questions/698910/catching-when-user-selects-an-item-from-a-ccombobox
BEGIN_MESSAGE_MAP(CComboBoxDevice, CComboBox)
//	ON_CBN_SELENDOK(IDC_COMBO1, &MyDialog::OnComboChanged)
END_MESSAGE_MAP()



// CComboBoxDevice message handlers


