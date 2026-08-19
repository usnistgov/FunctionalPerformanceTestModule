
// FPTM Dlg.h : header file
//

#pragma once
#include "BTMline.h"
#include <vector>

#define DISPLAY_LINES 10 // Number of device display lines in the GUI

// CFPTMDlg dialog
class CFPTMDlg : public CDialog
{

// Construction
public:
	CFPTMDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_BACNETTESTMODULE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL			OnInitDialog();
	afx_msg void			OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void			OnPaint();
	afx_msg HCURSOR			OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	CTime					tnow;		// Used in OnTimer
	CString					timestring; // Used in OnTimer
	int						showDeviceID; // MAG 260305 Device ID shows address/router (0, original) or device object ID (1)
	void					FillStepJumper(); // MAG 260324

public:
	std::vector<BTMline>	mybtm; // struct for display lines
	CListCtrl				m_datalist;
	CListBox				m_status;
	CButton					m_start;
	CEdit					m_datetime_display;
	
	//afx_msg void			OnBnClickedLaunch();
	afx_msg void			OnBnClickedStart();
	afx_msg void			OnTimer(UINT_PTR nIDEvent);
	afx_msg	void 			OnBnClickedResettimes();
	afx_msg	void 			OnBnClickedAddevent();
	void					UpdateDevices();
	void					OnFileSavestatuswindowAuto();
	// Updates the display of BTMlines
	int						UpdateEvents();

	// index of TRlist to start display at
	int						DisplayLineStart;
	CButton					m_AddEvent;
	CButton					m_ResetTimes;
	CSpinButtonCtrl			m_linespin;
	CButton					m_setsavefile;
	CButton					m_netscan;
	CStatic					m_compile_time;
	CString					GetDTS(CTime ctin = NULL);

	afx_msg void			OnDeltaposLinespin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void			OnFileSavetofile();
	afx_msg void			OnFileReadfromfile();
	afx_msg void			OnBnClickedSetsavefile();
	afx_msg void			OnBnClickedOk();
	int						PreTranslateMessage(MSG *pMsg);
	afx_msg void			OnBnClickedSetsavefile2();
	afx_msg void			OnFileResetvalues();
	afx_msg void			OnRuntimeoptionsSendoutofservice();
	void					SetPriorityLevelMenu(int i);
	void					SetSendOutOfServiceMenu(int i);
	void					SetRepeatEventListMenu();	// menu setting read from app
	afx_msg void			OnPrioritylevel1();
	afx_msg void			OnPrioritylevel2();
	afx_msg void			OnPrioritylevel3();
	afx_msg void			OnPrioritylevel4();
	afx_msg void			OnPrioritylevel5();
	afx_msg void			OnPrioritylevel6();
	afx_msg void			OnPrioritylevel7();
	afx_msg void			OnPrioritylevel8();
	afx_msg void			OnPrioritylevel9();
	afx_msg void			OnPrioritylevel10();
	afx_msg void			OnPrioritylevel11();
	afx_msg void			OnPrioritylevel12();
	afx_msg void			OnPrioritylevel13();
	afx_msg void			OnPrioritylevel14();
	afx_msg void			OnPrioritylevel15();
	afx_msg void			OnPrioritylevel16();
	afx_msg void			OnSendoutofserviceAi();
	afx_msg void			OnSendoutofserviceAo();
	afx_msg void			OnSendoutofserviceAv();
	afx_msg void			OnSendoutofserviceBi();
	afx_msg void			OnSendoutofserviceBo();
	afx_msg void			OnSendoutofserviceBv();
	afx_msg void			OnSendoutofserviceMsi();
	afx_msg void			OnSendoutofserviceMso();
	afx_msg void			OnSendoutofserviceMsv();
	afx_msg void			OnRuntimeoptionsRepeateventlist();
	afx_msg void			OnFileAbout();
	afx_msg void			OnClose();
	afx_msg void			OnDropFiles(HDROP hDropInfo);

	CButton					m_debug_test_data;
	afx_msg void			OnFileSavestatuswindow();
	afx_msg void			OnBnClickedDebugTestData();
	afx_msg void			OnBnClickedSendData();
	afx_msg void			OnRuntimeoptionsDisplayvariables();

	CStatic					m_nistel_logo;
	CButton					m_send_data;
	CComboBox				m_adapter_select;
	CStatic					m_select_network_adapter_label;
	CSpinButtonCtrl			m_blockspin;
	// Shows ID of currently displayed event list.
	CStatic					m_event_list_id;
	CStatic					m_event_count;

	afx_msg void			OnRuntimeoptionsCheckoos();
	afx_msg void			OnDeltaposBlockspin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void			OnConfigurationinformationVariables();
	afx_msg void			OnConfigurationinformationNamedpoints();
	afx_msg void			OnConfigurationinformationTests();
	afx_msg void			OnConfigurationinformationSparseevents();
	afx_msg void			OnConfigurationinformationScreencapture();
	afx_msg void			OnRuntimeoptionsRuntestsnow();

	// Control for last read file display
	CStatic m_last_file;
	afx_msg void OnConfigurationinformationKnowndevices();
	afx_msg void OnCbnSelchangeStepJumper();
	CComboBox m_step_jumper;
	afx_msg void OnConfigurationinformationExportexpectedvalues();
	afx_msg void OnCbnSelchangeAdapterSelect();
};
