// MAG TODO NOTE FOR MONDAY- the BTM works on XP, the problem must be with the AFDD dll

// BACnet Test ModuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FPTM App.h"
#include "FPTM Dlg.h"
#include "afxdialogex.h"
#include "resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int  CaptureScreenImage(HWND hWnd, CString fname);  // predeclaration, function is at end of file


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};


CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}


void 
CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


CFPTMDlg::CFPTMDlg(CWnd* pParent)
	: CDialog(CFPTMDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void 
CFPTMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATALIST, m_datalist);
	DDX_Control(pDX, IDC_STATUS, m_status);
	DDX_Control(pDX, IDC_START, m_start);
	DDX_Control(pDX, IDC_DATETIME, m_datetime_display);
	DDX_Control(pDX, IDC_ADDEVENT, m_AddEvent);
	DDX_Control(pDX, IDC_RESETTIMES, m_ResetTimes);
	DDX_Control(pDX, IDC_LINESPIN, m_linespin);
	DDX_Control(pDX, IDC_SETSAVEFILE, m_setsavefile);
	DDX_Control(pDX, IDC_SETSAVEFILE2, m_netscan);
	DDX_Control(pDX, IDC_COMPILE_TIMESTAMP, m_compile_time);
	DDX_Control(pDX, IDC_DEBUG_TEST_DATA, m_debug_test_data);
	DDX_Control(pDX, IDC_SEND_DATA, m_send_data);
	DDX_Control(pDX, IDC_ADAPTER_SELECT, m_adapter_select);
	DDX_Control(pDX, IDC_COMPILE_TIMESTAMP2, m_select_network_adapter_label);
	DDX_Control(pDX, IDC_BLOCKSPIN, m_blockspin);
	DDX_Control(pDX, IDC_EVENT_LIST_ID, m_event_list_id);
	DDX_Control(pDX, IDC_EVENT_COUNT, m_event_count);
	DDX_Control(pDX, IDC_LAST_FILE, m_last_file);
	DDX_Control(pDX, IDC_STEP_JUMPER, m_step_jumper);
}


BEGIN_MESSAGE_MAP(CFPTMDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CFPTMDlg::OnBnClickedStart)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RESETTIMES, &CFPTMDlg::OnBnClickedResettimes)
	ON_BN_CLICKED(IDC_ADDEVENT, &CFPTMDlg::OnBnClickedAddevent)
	ON_NOTIFY(UDN_DELTAPOS, IDC_LINESPIN, &CFPTMDlg::OnDeltaposLinespin)
	ON_COMMAND(ID_FILE_SAVETOFILE, &CFPTMDlg::OnFileSavetofile)
	ON_COMMAND(ID_FILE_READFROMFILE, &CFPTMDlg::OnFileReadfromfile)
	ON_BN_CLICKED(IDC_SETSAVEFILE, &CFPTMDlg::OnBnClickedSetsavefile)
	ON_BN_CLICKED(IDOK, &CFPTMDlg::OnBnClickedOk)
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_SETSAVEFILE2, &CFPTMDlg::OnBnClickedSetsavefile2)
	ON_COMMAND(ID_FILE_RESETVALUES, &CFPTMDlg::OnFileResetvalues)
	ON_COMMAND(ID_RUNTIMEOPTIONS_SENDOUTOFSERVICE, &CFPTMDlg::OnRuntimeoptionsSendoutofservice)
	ON_COMMAND(ID_PRIORITYLEVEL_1, &CFPTMDlg::OnPrioritylevel1)
	ON_COMMAND(ID_PRIORITYLEVEL_2, &CFPTMDlg::OnPrioritylevel2)
	ON_COMMAND(ID_PRIORITYLEVEL_3, &CFPTMDlg::OnPrioritylevel3)
	ON_COMMAND(ID_PRIORITYLEVEL_4, &CFPTMDlg::OnPrioritylevel4)
	ON_COMMAND(ID_PRIORITYLEVEL_5, &CFPTMDlg::OnPrioritylevel5)
	ON_COMMAND(ID_PRIORITYLEVEL_6, &CFPTMDlg::OnPrioritylevel6)
	ON_COMMAND(ID_PRIORITYLEVEL_7, &CFPTMDlg::OnPrioritylevel7)
	ON_COMMAND(ID_PRIORITYLEVEL_8, &CFPTMDlg::OnPrioritylevel8)
	ON_COMMAND(ID_PRIORITYLEVEL_9, &CFPTMDlg::OnPrioritylevel9)
	ON_COMMAND(ID_PRIORITYLEVEL_10, &CFPTMDlg::OnPrioritylevel10)
	ON_COMMAND(ID_PRIORITYLEVEL_11, &CFPTMDlg::OnPrioritylevel11)
	ON_COMMAND(ID_PRIORITYLEVEL_12, &CFPTMDlg::OnPrioritylevel12)
	ON_COMMAND(ID_PRIORITYLEVEL_13, &CFPTMDlg::OnPrioritylevel13)
	ON_COMMAND(ID_PRIORITYLEVEL_14, &CFPTMDlg::OnPrioritylevel14)
	ON_COMMAND(ID_PRIORITYLEVEL_15, &CFPTMDlg::OnPrioritylevel15)
	ON_COMMAND(ID_PRIORITYLEVEL_16, &CFPTMDlg::OnPrioritylevel16)
	ON_COMMAND(ID_SENDOUTOFSERVICE_AI, &CFPTMDlg::OnSendoutofserviceAi)
	ON_COMMAND(ID_SENDOUTOFSERVICE_AO, &CFPTMDlg::OnSendoutofserviceAo)
	ON_COMMAND(ID_SENDOUTOFSERVICE_AV, &CFPTMDlg::OnSendoutofserviceAv)
	ON_COMMAND(ID_SENDOUTOFSERVICE_BI, &CFPTMDlg::OnSendoutofserviceBi)
	ON_COMMAND(ID_SENDOUTOFSERVICE_BO, &CFPTMDlg::OnSendoutofserviceBo)
	ON_COMMAND(ID_SENDOUTOFSERVICE_BV, &CFPTMDlg::OnSendoutofserviceBv)
	ON_COMMAND(ID_SENDOUTOFSERVICE_MSI, &CFPTMDlg::OnSendoutofserviceMsi)
	ON_COMMAND(ID_SENDOUTOFSERVICE_MSO, &CFPTMDlg::OnSendoutofserviceMso)
	ON_COMMAND(ID_SENDOUTOFSERVICE_MSV, &CFPTMDlg::OnSendoutofserviceMsv)
	ON_COMMAND(ID_RUNTIMEOPTIONS_REPEATEVENTLIST, &CFPTMDlg::OnRuntimeoptionsRepeateventlist)
	ON_COMMAND(ID_FILE_ABOUT, &CFPTMDlg::OnFileAbout)
	ON_WM_DROPFILES()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_DEBUG_TEST_DATA, &CFPTMDlg::OnBnClickedDebugTestData)
	ON_BN_CLICKED(IDC_SEND_DATA, &CFPTMDlg::OnBnClickedSendData)
	ON_COMMAND(ID_RUNTIMEOPTIONS_DISPLAYVARIABLES, &CFPTMDlg::OnRuntimeoptionsDisplayvariables)
	ON_COMMAND(ID_FILE_SAVESTATUSWINDOW, &CFPTMDlg::OnFileSavestatuswindow)
	ON_COMMAND(ID_RUNTIMEOPTIONS_CHECKOOS, &CFPTMDlg::OnRuntimeoptionsCheckoos)
	ON_NOTIFY(UDN_DELTAPOS, IDC_BLOCKSPIN, &CFPTMDlg::OnDeltaposBlockspin)
	ON_COMMAND(ID_CONFIGURATIONINFORMATION_VARIABLES, &CFPTMDlg::OnConfigurationinformationVariables)
	ON_COMMAND(ID_CONFIGURATIONINFORMATION_NAMEDPOINTS, &CFPTMDlg::OnConfigurationinformationNamedpoints)
	ON_COMMAND(ID_CONFIGURATIONINFORMATION_TESTS, &CFPTMDlg::OnConfigurationinformationTests)
	ON_COMMAND(ID_CONFIGURATIONINFORMATION_SPARSEEVENTS, &CFPTMDlg::OnConfigurationinformationSparseevents)
	ON_COMMAND(ID_CONFIGURATIONINFORMATION_SCREENCAPTURE, &CFPTMDlg::OnConfigurationinformationScreencapture)
	ON_COMMAND(ID_RUNTIMEOPTIONS_RUNTESTSNOW, &CFPTMDlg::OnRuntimeoptionsRuntestsnow)
	ON_COMMAND(ID_CONFIGURATIONINFORMATION_KNOWNDEVICES, &CFPTMDlg::OnConfigurationinformationKnowndevices)
	ON_CBN_SELCHANGE(IDC_STEP_JUMPER, &CFPTMDlg::OnCbnSelchangeStepJumper)
	ON_COMMAND(ID_CONFIGURATIONINFORMATION_EXPORTEXPECTEDVALUES, &CFPTMDlg::OnConfigurationinformationExportexpectedvalues)
	ON_CBN_SELCHANGE(IDC_ADAPTER_SELECT, &CFPTMDlg::OnCbnSelchangeAdapterSelect)
END_MESSAGE_MAP()


// CFPTMDlg message handlers

BOOL 
CFPTMDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	int			i = 0;
	int			nac = 0;		// network adapter count
	int			colcount = 21;  // MAG not 21 cols, some are double or quad width
	char		adapterIP[20] = {};
	double		colw = 0.0;
	CRect		rc;
	CRect		rc2;
	CString		cs1;
	CString		cs2;
	CTime		tnow = CTime::GetCurrentTime();
	CTimeSpan	ts(0,0,0, tnow.GetSecond());

	// vars for adding devices to dialog
	char			mac[20] = {};
	char			router[20] = {};
	char			devlabel[100] = {};  // device text label, if present
	unsigned char	routerip[20] = {};
	unsigned char	devip[20] = {};
	CString			ds2;

	showDeviceID = 0;

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL){
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	tnow -= ts;  // cut off seconds

	theApp.myFPTM.InitIPH();

	DisplayLineStart = 0;

	this->GetWindowRect(&rc);
	colw = (rc.Width() - 14) / colcount;

	// set spinner
	rc2 = CRect(0, 335, (int)colw, 361);
	m_linespin.MoveWindow(&rc2, 1);
	m_linespin.ShowWindow(0);
	
	// set rc2 for text location
	rc2 = CRect((int)colw, 335, (int)colw + 100, 361);
	m_event_count.MoveWindow((int)colw + 5, 335, 200, 30);

	mybtm.reserve(100);

	theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].reserve(100);  // 100 is just a guess, could be more or less actually used

	theApp.myFPTM.InitBACnetAdapters();

	// NOTE- there are always DISPLAY_LINES xx10xx display lines, they are not always visible depending on the number of tasks
	theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].push_back(TimedRequest(-1, tnow, 0, 0, 0, -999));
	for (int i = 0; i < DISPLAY_LINES; i++){
		mybtm.push_back(BTMline(i, rc.Width() - 14));
		for (unsigned int j = 0; j < theApp.myFPTM.BACnetList.size(); j++){
			theApp.myFPTM.BACnetList[j].GetIP(&devip[0]);
			theApp.myFPTM.BACnetList[j].GetRouterIP(&routerip[0]);
			theApp.myFPTM.BACnetList[j].GetMAC(mac);
			theApp.myFPTM.BACnetList[j].GetRouterMAC(router);
			if (devip[0] > 0){  // if the device has an IP address, so does the router
				if (router[0] > 0){
					ds2.Format(L"%03d.%03d.%03d.%03d / %03d.%03d.%03d.%03d", devip[0], devip[1], devip[2], devip[3], routerip[0], routerip[1], routerip[2], routerip[3]);
				} else {  // no router
					ds2.Format(L"%03d.%03d.%03d.%03d", devip[0], devip[1], devip[2], devip[3]);
				}
			} else if (routerip[0] > 0){  // router IP, device MAC
				ds2 = mac;
				ds2.AppendFormat(L"/ %03d.%03d.%03d.%03d" , routerip[0], routerip[1], routerip[2], routerip[3]);
			} else {	// device and router use MAC
				ds2 = mac;
				if (strlen(router) > 0){
					ds2 += " / ";
					ds2 += router;
				} //else {???
			}
			mybtm[i].AddDevice(ds2);
		}
	}
	mybtm[0].SetVisible(1);
	for (int i = 1; i < DISPLAY_LINES; i++) mybtm[i].SetVisible(0);

	// MAG 241106 position left/right arrows to change event blocks view
	//m_blockspin.SetWindowPos(&wndTop, 4 * colw, 35, colw, 12, SWP_SHOWWINDOW);

	m_datalist.SetExtendedStyle(LVS_EX_GRIDLINES || LVS_EX_FULLROWSELECT);
	m_datalist.SetWindowPos(&wndTop, 0, 50, rc.Width() - 14, 24, SWP_SHOWWINDOW);
	m_datalist.InsertColumn(0, L"Line", LVCFMT_LEFT, (int)colw);
	m_datalist.InsertColumn(1, L"Date", LVCFMT_LEFT, (int)(2*colw));
	m_datalist.InsertColumn(2, L"Time", LVCFMT_LEFT, (int)(2*colw));
	m_datalist.InsertColumn(3, L"Action", LVCFMT_LEFT, (int)colw * 2);
	m_datalist.InsertColumn(4, L"Device (address/router)", LVCFMT_LEFT, (int)colw * 4);
	m_datalist.InsertColumn(5, L"BACnet Object", LVCFMT_LEFT, (int)colw * 2);
	m_datalist.InsertColumn(6, L"Instance", LVCFMT_LEFT, (int)colw * 2);
	m_datalist.InsertColumn(7, L"Value", LVCFMT_LEFT, (int)colw * 2);
	m_datalist.InsertColumn(8, L"Comment", LVCFMT_LEFT, (int)colw * 4);
#ifdef BACOFFLINE
	m_status.AddString(L"Functional Performance Test Module v1.5 (OFFLINE)");		// NOTE version
#else
	m_status.AddString(L"NIST Engineering Laboratory - Functional Performance Test Module BETA v1.6");		// NOTE version- also change in About dialog
	m_status.AddString(L"Note: This program is in active development.");
#endif
	m_status.AddString(L"By: Mike Galler (mikeg@nist.gov)");
	m_status.AddString(L"Startup complete.");
#ifdef USEAFDD
	cs1.Format(L"AFDD Mode- Use 'File->Open Event File' file to start data collection and transfer.");
#else
	cs1.Format(L"Click the 'Net Scan' button to initiate a network scan.");
#endif

	m_status.AddString(cs1);

	nac = theApp.myFPTM.GetNetworkAdapterCount();
	if (nac > 1) {
		m_status.AddString(L"NOTE: Select correct Network Adapter by IP from dropdown menu at top of FPTM before starting script.");
		for (i = 0; i < nac; i++){
			if (theApp.myFPTM.GetNetworkAdapterIP(i, adapterIP) == 1) {
				cs1 = adapterIP;
				m_adapter_select.AddString(cs1);
			} else {
				m_adapter_select.AddString(L"NA");
			}
		}
		// MAG TODO? add code to show default adapter (GetDefaultAdapter(void);)

	} else {
		m_adapter_select.ShowWindow(0);
		m_select_network_adapter_label.ShowWindow(0);
	}
#ifdef USEAFDD
	// hide command buttons not used in this mode
	m_start.ShowWindow(0);
	m_AddEvent.ShowWindow(0);
	m_ResetTimes.ShowWindow(0);
	m_setsavefile.ShowWindow(0);
	m_netscan.ShowWindow(0);

	CMenu* mmenu = GetMenu();
	CMenu* submenu = mmenu->GetSubMenu(0);
	submenu->EnableMenuItem(ID_FILE_SAVETOFILE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
#else	// only show list of found devices for HVAC-Cx mode
	for (unsigned int i = 0; i < theApp.myFPTM.BACnetList.size(); i++){
		theApp.myFPTM.BACnetList[i].GetIP(&devip[0]);
		theApp.myFPTM.BACnetList[i].GetRouterIP(&routerip[0]);
		theApp.myFPTM.BACnetList[i].GetMAC(mac);
		theApp.myFPTM.BACnetList[i].GetRouterMAC(router);

		if (devip[0] > 0)  // if the device has an IP address, so does the router
		{
			cs1.Format(L"%d IP: %03d.%03d.%03d.%03d RTR: %03d.%03d.%03d.%03d", i, devip[0], devip[1], devip[2], devip[3], routerip[0], routerip[1], routerip[2], routerip[3]);
		} else if (routerip[0] > 0){  // router IP, device MAC
			cs1.Format(L"%d MAC: ", i);
			cs1 += mac;
			cs2.Format(L" RTR: %03d.%03d.%03d.%03d",routerip[0], routerip[1], routerip[2], routerip[3]);
			cs1 += cs2;
		} else if (strlen(router) > 0){	// device and router use MAC
			cs1.Format(L"%d  MAC: ", i);
			cs1 += mac;
			cs2.Format(L" RTR: ");
			cs1 += cs2;
			cs1 += router;
		} else {  // just a lonely MAC address
			cs1.Format(L"%d MAC: ", i);
			cs1 += mac;
		}
		theApp.myFPTM.BACnetList[i].GetDeviceLabel(devlabel);
		if (strlen(devlabel) > 0){
			cs1 += "(";
			cs1 += devlabel;
			cs1 += ")";
		}
		m_status.AddString(cs1);
	}
#endif

#ifndef _DEBUG
	m_debug_test_data.ShowWindow(0);
	m_send_data.ShowWindow(0);
#endif

	m_last_file.SetWindowText(L"");//"Nothing to see here...");
	m_event_count.SetWindowText(L"");

	// MAG 160712 set up compile time display
	sprintf_s(devlabel, 100, "Version: %s %s", __DATE__, __TIME__);
	cs1 = devlabel;
	m_compile_time.SetWindowText(cs1);

	SetTimer(1, 1000, NULL);	// Timer that does screen updates

#ifdef BACOFFLINE
	SetWindowText(L"Functional Performance Test Module (OFFLINE)");
#endif

	return TRUE;  // return TRUE  unless you set the focus to a control
} // end CFPTMDlg::OnInitDialog


void 
CFPTMDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX){
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	} else {
		CDialog::OnSysCommand(nID, lParam);// MAG 161006 changed from CDialogEX
	}

	return;
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void
CFPTMDlg::OnPaint()
{
	if (IsIconic()){
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialog::OnPaint();  // MAG 161006 changed from CDialogEX
	}

	return;
}


// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR 
CFPTMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// Starts and stops tests from running, rather important!
void 
CFPTMDlg::OnBnClickedStart()
{
	// TODO: Add your control notification handler code here
	int		i = 0;
	int		mc = 0;
	CString cs1;

	if (theApp.myFPTM.TRlist.size() == 0) {
		m_status.AddString(L"ERROR: There is no active event list. Add an event list and events to start event timer.");
		return;
	}
	if (theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size() == 0) {
		m_status.AddString(L"ERROR: There are no configured events. Add events to start event timer.");
		return;
	}

	tnow = CTime::GetCurrentTime();  // this is used for multiple events

	// get size of current list, if less than 10 (# of display lines) use lesser number to adjust displays
	mc = theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size();
	if (mc > DISPLAY_LINES) mc = DISPLAY_LINES;

	// starting
	if (theApp.myFPTM.LaunchState == 0){
		i = theApp.myFPTM.SetStartState();

		if (i == 0) { // uh oh- error messages should be in SetStartState
			return;
		}

		for (i = 0; i < mc; i++){
			mybtm[i].SetStart();
		}

		m_start.SetWindowText(L"Stop");

		SetTimer(4, (60 - tnow.GetSecond()) * 1000, NULL);	// set timer to start at next minute
		m_status.AddString(L"Timed job started.");
		m_AddEvent.EnableWindow(0);
		m_ResetTimes.EnableWindow(0);
	} else {	// stopping
		for (i = 0; i < mc; i++){
			mybtm[i].SetStop();
		}
		m_start.SetWindowText(L"Start");
		theApp.myFPTM.LaunchState = 0;
		KillTimer(2);
		KillTimer(4);
		theApp.myFPTM.PrintVariables();
		m_AddEvent.EnableWindow(1);
		m_ResetTimes.EnableWindow(1);
		theApp.myFPTM.CloseLogFiles();
		m_status.AddString(L"Timed job stopped.");
		m_status.AddString(L"If tests remain you may run them now by selecting Runtime Options->Run Tests Now");
	}

	return;
}  // end OnBnClickedStart


// MAG OnTimer- This is where events, step tests, blocks, etc. are controlled.
// Timer 1:  1 s: Display update, including clocks/timers
// Timer 2: 10 s: Main event/etc. control loop. Checks when events are run, test cycles finished, etc.......
// Timer 3: XX s: Alignment timer for Timer 5. When called determines time until Timer 5 should be called and sets timer.
// Timer 4: XX s: Alignment timer for Timer 2. When called determines time until Timer 2 should be called and sets timer.
// Timer 5: 10 s: Run 3 s after Timer 2. Screenshots, Reads controller data, writes periodic values, saves T3 log
// Timer RAMPTIMER: ?? x: If ramps are in current test step, will calculate and send ramp values
// Timer NEXTLIST: XX s: Performs actions needed to move to next test step or block, or end run. Called as needed.
// Gemchecked
void 
CFPTMDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	tnow = CTime::GetCurrentTime();  // this is used for multiple events
	unsigned int	i = 0;
	int				iTR = 0;	// local copy of TRactive
	int				iTS = 0;	// local copy of TSactive
	int				iid = 0;	// interpID- use to store ID of enterpolation when checked
	int				tpast = 0;	// timers past count
	int				runResult = 0;
	int				ti = 0;	// test array iterator
	int				rti = 0; // ramp timer loop iterator
	int				triggerEvent = 0;
	int				varIndex1 = 0;
	int				varIndex2 = 0;
	int				varOperator1 = 0;
	int				rampID = -1;
	int				testStepRun = 0; // bool so only one teststep is run per call to funciton
	int				endTestStep = 0;
	int				testCol = 0;	// holds column ID for current test step (iTS)
	char			dtts[50] = {};
	int				exitFlag = 0;
	int				t3logSaved = 0;	// MAG 251218 flag indicates if T3 log has been saved this cycle, used in case timer 2
	int				readThisCycle = 0; // MAG 260319 test if T3 log is saved without reads done in current cycle
	double			varValue1 = 0;
	double			varValue2 = 0;
	double			modValue1 = 0;
	double			dv = 0.0;
	RampValue		lrv;
	CTimeSpan		cts; // used to calculate ramp write values
	CString			fname;
	CString			dts;
	CString			cs1;
	CString			predts;		// pre and post time stamp strings
	CString			postdts;
	CString			testStepLabel; // holds.... Test Step Label! i.e. 2a, 4b. A is for write step (top part of col) b is for reads/tests
	CTime			tevent;		// event time
	CTimeSpan		tspan;		// event time span
	CTime			tnext;		// time of next event
	std::vector<int> evRunThisCycle; // events run this cycle, used to check for associated tests
	std::vector<int> testsToRunThisCycle; // Tests to run this cycle
	std::vector<int> rampDel;  // list of ramp array members to delete
	std::vector<int> failList; // list of test IDs that fail- to rerun?
	std::stringstream sst; // MAG 250618 use for output to reportT2
	//std::vector<std::string>	testType = { "LT","LTE", "EQ", "GTE", "GT","NE", "MA", "ANY" };  // for reference

	iTR = theApp.myFPTM.TRactive;
	iTS = theApp.myFPTM.TSactive;

	if ((theApp.myFPTM.LaunchState != 0)&&(iTS >= (int)theApp.myFPTM.testSteps[iTR].size())) {
		if (nIDEvent == 2) {  // this is the only event where this matters, this shouldn't happen
			cs1.Format(L"ERROR: Test step incremented beyond list size! Call on timer %d", nIDEvent);  // line for breakpoint
			m_status.AddString(cs1);
			m_status.AddString(L"### AUTOSAVE ###");
			OnFileSavestatuswindowAuto();
			KillTimer(2);
			theApp.myFPTM.LaunchState = 0;
			return;
		}
		if (nIDEvent < NEXTLIST) return; // return without crying about it, allow NEXTLIST to run
	}

	dts = theApp.GetDTS(tnow);
	theApp.GetDTSC(dtts, tnow);
	strcat_s(dtts, 40, "\t");

	// find current column for use in print statements below (do not use value of iTS directly for print statements)
	try {
		if ((iTR >= 0) && (iTS >= 0) && (iTS < (int)theApp.myFPTM.testSteps[iTR].size())) {  // catch nextlist calls, iTS has been ++ to end of loop
			testCol = theApp.myFPTM.testSteps[iTR][iTS].GetListMinEvent(); // get ID of first event (holds column info)
			if ((testCol < 0) || (testCol >= (int)theApp.myFPTM.TRlist[iTR].size())) {	// re-use testCol here
				testCol = 0;
				testStepLabel = "ERR";
			} else {
				testCol = theApp.myFPTM.TRlist[iTR][testCol].GetSourceColumn();
				testStepLabel.Format(L"%d", testCol);
			}
			if (iTS & 1) {
				testStepLabel += "b";  // odd iTS for write steps
			} else {
				testStepLabel += "a"; // even iTS for read/test steps
			}
		} else {
			testStepLabel = "NA";
		}
	}
	catch (const std::exception& e) {
		std::stringstream ss1;
		m_status.AddString(L"ERROR: Caught exception in OnTimer!");
		ss1 << "ERROR: " << e.what();
		cs1 = ss1.str().c_str();
		m_status.AddString(cs1);
	}
	catch (...) {
		cs1.Format(L"ERROR- Catch in OnTimer. iTR=%d iTS=%d Event=%d", iTR, iTS, nIDEvent);
		m_status.AddString(cs1);
	}

	switch(nIDEvent){
		case 1: // timer update
			timestring = tnow.Format(_T("%m/%d/%Y, %H:%M:%S"));
			m_datetime_display.SetWindowText(timestring);
			break;
		case 4: // start timer 2 on the minute- this timer is only to align timer 2
			cs1.Format(L"%s Enter Timer 4: Alignment Timer 1 Block %c Step %s.", dts.GetString(), 'A' + iTR, testStepLabel.GetString());
			m_status.AddString(cs1);
			KillTimer(4);
			// align with minute
			if (tnow.GetSecond()%10 != 0){		// didn't quite get it, will try again
				i = 10 - tnow.GetSecond()%10;
				SetTimer(4, i * 1000,NULL);
				cs1.Format(L"%s Timer 4: Misalignment! Reset Timer 4 for %d seconds.", dts.GetString(), i);
				m_status.AddString(cs1);
				cs1.Format(L"%s Exit Timer 4 on break.", dts.GetString());
				break;
			} else {	// second%10 == 0, so start of 10 second window
				// MAG 250424 change timer 2 from 60 seconds to 10 seconds for CalNEXT
				SetTimer(2, 10000,NULL);		// this should be called on the second pass through timer 4 (or first if after re-align from timer 2
				cs1.Format(L"%s Timer 4: Start Timer 2 for 10 seconds.", dts.GetString());
				m_status.AddString(cs1);

				SetTimer(3, 3000, NULL);		// this sets up the call to the Timer 5 alignment (3)
				cs1.Format(L"%s Timer 4: Start Timer 3 (Alignment Timer 2) for 3 seconds (one time).", dts.GetString());
				m_status.AddString(cs1);

				// MAG NOTE timer 4 is called by OnBnClickedStart, called in NEXTLIST timer
				if (theApp.myFPTM.rampList[theApp.myFPTM.TRactive].size() > 0) {
					SetTimer(RAMPTIMER, theApp.myFPTM.rampList[theApp.myFPTM.TRactive][0].GetUpdateSpan() * 1000, NULL);  // set ramp timer for every (update span) seconds
					sst << "Call Set RampTimer at " << dtts << std::endl;
					theApp.myFPTM.reportT2 << sst.str();
					sst.clear();
					sst.str("");
				} else {
					KillTimer(RAMPTIMER);
				}
			}
			cs1.Format(L"%s Exit Timer 4, continue to Timer 2:", dts.GetString());
			m_status.AddString(cs1);
			// allow to fall through to case 2
		case 2:	// MAIN EVENT TIMER
			try {
				cs1.Format(L"%s Enter Timer 2: Ten Second Timer Block %c Step %s (%d).", dts.GetString(), 'A' + iTR, testStepLabel.GetString(),iTS);
				m_status.AddString(cs1);

				// MAG note this for debug only
				//for (size_t iiTS = 0;iiTS < theApp.myFPTM.testSteps[iTR].size(); iiTS++) {
				//	predts = theApp.GetDTS(theApp.myFPTM.testSteps[iTR][iiTS].GetTime());
				//	cs1.Format(L"%s TestStep %02d event time %s.", dts.GetString(), iiTS, predts.GetString());
				//	m_status.AddString(cs1);
				//}

				// MAG 250424 modify if by adding "% 10" to work with 10 second timer mod
				if ((tnow.GetSecond() % 10) != 0) {  // try to re-align with minute, collect data now anyway
					KillTimer(2);
					SetTimer(4, 1000 * (10 - (tnow.GetSecond()%10)), NULL); // MAG 250425 modify to align with next 10 second window
					cs1.Format(L"%s Exit Timer 2: Reset timer by %d seconds.", dts.GetString(), (10 - (tnow.GetSecond() % 10)));
					m_status.AddString(cs1);
					// MAG note- not aligned with 10s time step, not saving T3 log
					break;	// MAG 250828 add break here.
				}
				//m_status.AddString(L"Checking timers..."); // enable for debugging as needed

				// Main loop to execute events- NOTE each loop only run once, when timer indicates
				tpast = 0;
				tevent = theApp.myFPTM.testSteps[iTR][iTS].GetTime();
				tspan = theApp.myFPTM.testSteps[iTR][iTS].GetTimeInterval();

				// run when time is at or greater than TS start time + interval
				if (tnow < tevent) {
					cs1.Format(L"%s Exit Timer 2: Test step %s start time (%s) not reached.", dts.GetString(), testStepLabel.GetString(), theApp.GetDTS(tevent).GetString());
					m_status.AddString(cs1);

					theApp.myFPTM.SaveLogT3();  // MAG 251209 Move save T3 log from Timer 5 (add at all exit points)
					t3logSaved++;

					break;
				}

				theApp.myFPTM.RunPeriodic();  // MAG 251203 move this here
				if (theApp.myFPTM.testSteps[iTR][ti].piList.size() == 0) {
					cs1.Format(L"%s Note: no Periodic Event for %c Step %s.", dts.GetString(), 'A' + iTR, testStepLabel.GetString());
					m_status.AddString(cs1);
				}

				if (theApp.myFPTM.testSteps[iTR].size() > 0) {  // using testSteps to control TRlist (CalNEXT)
					cs1.Format(L"%s Timer 2: Start test step evaluation step %s. Event start time %s. Event span %d seconds until %s.",
						dts.GetString(), testStepLabel.GetString(),theApp.GetDTS(tevent).GetString(),(int)tspan.GetTotalSeconds(),theApp.GetDTS(tevent + tspan));
					m_status.AddString(cs1);

					// if in interval between step start and step end, run tests
					// MAG 250618 Check step ending "threshold" tests- (copied from timer 5)
					if (tnow < tevent + tspan) {
						rti = theApp.myFPTM.CheckTestStepTest();  // THRESHOLD TEST
						if (rti == 1) {
							// Need to run end-of-test-step actions
							// same strategy as after end of test, remove the gap between now and time of test
							predts = theApp.GetDTS(tevent);
							// use interval between when test was supposed to end (tevent + interval) and 'now' to determine how much time to subract from future events
							if (tnow >= tevent + tspan) {
								cts = 0;  // it's too late to decrement!
							} else {
								if (iTS < (int)theApp.myFPTM.testSteps[iTR].size() - 1) {
									tnext = theApp.myFPTM.testSteps[iTR][iTS + 1].GetTime();
									cts =  tnext - tnow;
									if(cts.GetTotalSeconds()  > 20) cts -= 10;
								}
								cts = (cts.GetTotalSeconds() < 10) ? 10 : cts;  // can't be below xxxzeroxxx ten! check just in case?
								if ((tevent + tspan) - tnow <= 10) cts = 0; // edge case where it's only 10 seconds until (tevent + tspan)
							}
							theApp.myFPTM.testSteps[iTR][iTS].SetTimeInterval(0);//cts); // this should cause end of cycle code to run below

							cs1.Format(L"%s Timer 2: Test Step %d Evaluation passes, ending test step early. Decrement %d seconds. Modify from event time %s.", 
								dts.GetString(), testCol, (int)cts.GetTotalSeconds(), predts.GetString());
							m_status.AddString(cs1);

							// 
							// MAG 250917 TODO: Add report to T1 log when threshold evaluation passes
							cs1.Format(L"%s Test Step %d Threshold test pass, test step ends %d seconds early.", dts.GetString(), testCol, (int)cts.GetTotalSeconds());
							// Decrement start time of subsequent test steps and events
							for (size_t iiTS = iTS + 1;iiTS < theApp.myFPTM.testSteps[iTR].size(); iiTS++) {
								predts = theApp.GetDTS(theApp.myFPTM.testSteps[iTR][iiTS].GetTime());
								theApp.myFPTM.testSteps[iTR][iiTS].AdjustTime(cts, 0); // 0 to decrement time
								postdts = theApp.GetDTS(theApp.myFPTM.testSteps[iTR][iiTS].GetTime());
								cs1.Format(L"%s Decrement TestStep %d by %d seconds from %s to %s. Decrementing associated events...", dts.GetString(), iiTS,(int) cts.GetTotalSeconds(),predts.GetString(), postdts.GetString());
								m_status.AddString(cs1);
								//cs1.Format(L"%s Decrementing Events...", dts);
								// parse through testStep.trList to get event ids, then decrement event start times by cts
								for (size_t ievent = 0; ievent < theApp.myFPTM.testSteps[iTR][iiTS].GetListSize(); ievent++){
									ti = theApp.myFPTM.testSteps[iTR][iiTS].GetListEventID(ievent);
									theApp.myFPTM.TRlist[iTR][ti].AdjustEventTime(cts, 0); // decrement time
									//cs1.Format(L"%s Decrement Event %d by %d seconds.", dts, ti, cts.GetSeconds());
									//m_status.AddString(cs1);
								}
							}
						} else {
							switch (rti) {
								case 0: cs1.Format(L"%s Timer 2: Test Step %s Evaluation fails (cause %d Parameters indicate failure), continuing test step.", dts.GetString(), testStepLabel.GetString(), rti); break;
								case -1: cs1.Format(L"%s Timer 2: Test Step %s Evaluation fails (cause %d No test steps found), continuing test step.", dts.GetString(), testStepLabel.GetString(), rti); break;
								case -2: cs1.Format(L"%s Timer 2: Test Step %s Evaluation fails (cause %d Past test steps), continuing test step.", dts.GetString(), testStepLabel.GetString(), rti); break;
								case -3: cs1.Format(L"%s Timer 2: Test Step %s Evaluation fails (cause %d Test time has not started yet), continuing test step.", dts.GetString(), testStepLabel.GetString(), rti); break;
								case -4: cs1.Format(L"%s Timer 2: Test Step %s Evaluation fails (cause %d No evaluation for this test step), continuing test step.", dts.GetString(), testStepLabel.GetString(), rti); break;
								default: cs1.Format(L"%s Timer 2: Test Step %s Evaluation fails (cause %d Error in evaluation configuration), continuing test step.", dts.GetString(), testStepLabel.GetString(), rti); break;
							}
							m_status.AddString(cs1);
							//break; // test step evaluation has failed, nothing further to check.
							// MAG note break here exits Timer 2
						}
					} else {  // MAG 251203 note read steps (iTS is even ##) seem to be getting sent here? 
						// TODO: After end of test interval! Run tests now. Remove the gap between now and time of test
						// set interval so it ends now
						cts = tnow - tevent;  // get interval since test start
						if (cts != tspan) {
							theApp.myFPTM.testSteps[iTR][iTS].SetTimeInterval(cts); // this should cause end of cycle code to run below
						}
						if (tnow > tevent + tspan) {
							cs1.Format(L"%s Timer 2: Test step evaluation (%s) time exceeded (%s). Modify step time by %d to end cycle and run tests.", dts.GetString(), testStepLabel.GetString(), theApp.myFPTM.testSteps[iTR][iTS].GetTimeString().GetString(),(int)(tspan.GetTotalSeconds() - cts.GetTotalSeconds()));
						} else {
							cs1.Format(L"%s Timer 2: Test step evaluation (%s) time met. End cycle and run tests.", dts.GetString(), testStepLabel.GetString());
						}
						m_status.AddString(cs1);
					}
					
					tevent = theApp.myFPTM.testSteps[iTR][iTS].GetTime();
					tspan = theApp.myFPTM.testSteps[iTR][iTS].GetTimeInterval();

					// scan through current test step
					// MAG will loop through correct iTS below //for(i=0; i < theApp.myFPTM.testSteps[iTR][iTS].GetListSize();i++)
					//for (i = 0; (i < theApp.myFPTM.testSteps[theApp.myFPTM.TRactive].size())&&(testStepRun == 0); i++) 
					{
						cs1.Format(L"%s Timer 2: Run Test Step %c : %s. Scheduled start time %s.", 
							dts.GetString(), 'A' + iTR, testStepLabel.GetString(), theApp.myFPTM.testSteps[iTR][iTS].GetTimeString().GetString());
						m_status.AddString(cs1);

						// check for overrun past end of event
						cts = tnow - (tevent + tspan);

						if (theApp.myFPTM.testSteps[iTR][iTS].GetStatus() == 1) {  // check if it's run, then skip if it has
							cs1.Format(L"%s Exit Timer 2: Test has been run previously.", dts.GetString());
							m_status.AddString(cs1);
							theApp.myFPTM.SaveLogT3();  // MAG 251209 Move save T3 log from Timer 5 (add at all exit points)
							t3logSaved++;
							break;
						}
						cts = theApp.myFPTM.testSteps[iTR][iTS].GetTime() - tnow;  // negative before time to run test

						// check if it's time to run the tests
						if (tnow >= tevent + tspan) {  // found current testStep list
							fname.Format(L"%S Start test step %s with %d events.", dtts, testStepLabel.GetString(), theApp.myFPTM.testSteps[iTR][iTS].GetListSize());
							m_status.AddString(fname);
							if (theApp.myFPTM.reportT2) {
								sst << CT2A(fname) << std::endl;
								theApp.myFPTM.reportT2 << sst.str();
								sst.clear();
								sst.str("");
							}
							ti = theApp.myFPTM.testSteps[iTR][iTS].GetListMinEvent(); // index of first event in test step
							theApp.myFPTM.SetCurrentTestStep( theApp.myFPTM.TRlist[iTR][ti].GetTestStep() );  // used in T3 log
							theApp.myFPTM.SetActiveColumn(theApp.myFPTM.TRlist[iTR][ti].GetSourceColumn());  // used in T3 log
							DisplayLineStart = ti;//theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][ti].
							UpdateEvents();

							// < run mathvars, events, tests, etc... > 
							// Run MathVars
							for (size_t pi = 0; pi < theApp.myFPTM.testSteps[iTR][iTS].mvList.size(); pi++) {
								varIndex1 = theApp.myFPTM.testSteps[iTR][iTS].mvList[pi].GetVar1Index();
								if ((varIndex1 < 0) || (varIndex1 >= (int)theApp.myFPTM.VARlist.size())) break; // MAG this should just exit for loop
								varValue1 = theApp.myFPTM.VARlist[varIndex1].GetValue();

								varIndex2 = theApp.myFPTM.testSteps[iTR][iTS].mvList[pi].GetVar2Index();
								if ((varIndex2 < 0) || (varIndex2 >= (int)theApp.myFPTM.VARlist.size())) {  // 2nd is a value not var
									varValue2 = theApp.myFPTM.testSteps[iTR][iTS].mvList[pi].GetVar2Value();
								} else {	// retrieve VAR value
									varValue2 = theApp.myFPTM.VARlist[varIndex2].GetValue();
								}

								// get operator and perform calculation
								varOperator1 = theApp.myFPTM.testSteps[iTR][iTS].mvList[pi].GetVarOperator();
								switch (varOperator1) {
									case 0: break;  // pass thru, no change
									case 1: varValue1 += varValue2; break;
									case 2: varValue1 -= varValue2; break;
									case 3: varValue1 *= varValue2; break;
									case 4: varValue1 /= varValue2; break;
									case 5:
										if (varValue1 == 0) break;
										varValue1 = varValue2 / varValue1; break;
									default:  // also no change to varValue1
										break;
								}
								// write value to destination VAR
								varIndex1 = theApp.myFPTM.testSteps[iTR][iTS].mvList[pi].GetVarOutIndex();
								if ((varIndex1 < 0) || (varIndex1 >= (int)theApp.myFPTM.VARlist.size())) break;
								theApp.myFPTM.VARlist[varIndex1].SetValue(varValue1);
							}  // finish MathVars

							failList.clear();
							// Run Events
							for (int j = 0; j < (int)theApp.myFPTM.testSteps[iTR][iTS].GetListSize(); j++) {
								triggerEvent = theApp.myFPTM.testSteps[iTR][iTS].GetListEventID(j);
								if (triggerEvent == -1) {
									cs1.Format(L"%s ERROR: Invalid triggerEvent in OnTimer. Index %d Block %d Step %d",dts.GetString(),j, iTR, iTS);
									m_status.AddString(cs1);
									continue;
								}
								runResult = theApp.myFPTM.RunRequest(triggerEvent);
								if (runResult == 0) {
									failList.push_back(triggerEvent);
									cs1.Format(L"%s Note: RunRequest with triggerEvent %d fail with result 0. Block %d Step %d", dts.GetString(), triggerEvent, iTR, iTS);
								}
								tpast++;
								evRunThisCycle.push_back(triggerEvent);
								//add test id to testsRun....
								if (theApp.myFPTM.TRlist[iTR][triggerEvent].GetRunTest() >= 0) {
									testsToRunThisCycle.push_back(theApp.myFPTM.TRlist[iTR][triggerEvent].GetRunTest());
								}
								// MAG 191002 add these lines to update the display position
								//DisplayLineStart = i;
								UpdateEvents();
							}  // finish events
							if (failList.size() > 0) {
								cs1.Format ( L"%s Rerun %d failed requests:", dts.GetString(), failList.size());
								m_status.AddString(cs1);
								for (size_t j = 0; j < failList.size(); j++)
								{
									runResult = theApp.myFPTM.RunRequest(failList[j]);
									cs1.Format(L"%s Rerun failed request %d. Status %d", dts.GetString(), failList[j], runResult);
									m_status.AddString(cs1);
								}
							}

							theApp.myFPTM.testSteps[iTR][iTS].SetStatus(1);
							testStepRun = 1;

							endTestStep = 1; // signal to end current test step, run tests, increment TSactive
							cs1.Format(L"%s Set signal to end test step. Test step events have been processed.", theApp.GetDTS(NULL).GetString());// dts.GetString());
							m_status.AddString(cs1);
						} else {  // There is no test step evaluation, and the time is during the span of the test step
							cs1.Format(L"%s Timer 2: Step end time %s not met. Flag set to Exit Timer 2.", theApp.GetDTS(NULL).GetString(),theApp.GetDTS(tevent+tspan).GetString());
							m_status.AddString(cs1);
							exitFlag = 1; // delay exit until after periodic events!
							//break;
						}// endif time >= test step start time

						if (exitFlag == 1) {
							cs1.Format(L"%s Timer 2: Exit on flag.", dts.GetString());
							m_status.AddString(cs1);
							theApp.myFPTM.SaveLogT3();  // MAG 251209 Move save T3 log from Timer 5 (add at all exit points)
							t3logSaved++;
							break;
						}

					}  // end xxx loop xxx through testSteps
				} else {  // using TRlist directly
					cs1.Format(L"%s Timer 2: No test step found, processing Event list.", dts.GetString());
					m_status.AddString(cs1);
					for (i = 0; i < theApp.myFPTM.TRlist[iTR].size(); i++) {
						if (tnow == theApp.myFPTM.TRlist[iTR][i].GetEventTime()) {
							timestring.Format(L"Start event %d", i);
							m_status.AddString(timestring);
							runResult = theApp.myFPTM.RunRequest(i);
							tpast++;
							evRunThisCycle.push_back(i);
							// MAG 191002 add these lines to update the display position
							UpdateEvents();
						}
						if (tnow > theApp.myFPTM.TRlist[iTR][i].GetEventTime()) tpast++;
					}
				}

				// NOTE- if test step evaluation has failed, and tnow < event time + tspan, we should have exited (break) already

				// Now run tests based on testsToRunThisCycle
				cs1.Format(L"%s Review step tests: find %d tests to run.", theApp.GetDTS(NULL).GetString(), testsToRunThisCycle.size());
				m_status.AddString(cs1);
				for (i = 0; i < testsToRunThisCycle.size(); i++) {
					theApp.myFPTM.RunTest(testsToRunThisCycle[i]);
				}
				testsToRunThisCycle.clear();
#ifdef USEAFDD
				if (tpast == theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size()) {
					if ((theApp.myFPTM.TRlistRepeat > 0) && (theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][0].GetEventTime() < tnow)) {  // whoops, went past list w/o resetting
						theApp.myFPTM.ResetDelay(theApp.myFPTM.TRlistRepeat);
					}
				}
#else
				if (t3logSaved == 0) {  // MAG add this save point as else step count is incremented before log is saved!
					theApp.myFPTM.SaveLogT3();
					t3logSaved++;
				}

				// This moves to the next test step (i.e. A.1 -> A.2)
				if (endTestStep) {
					theApp.myFPTM.FlushLogFiles();
					cs1.Format(L"%s Incrementing Test Step %s.",dts.GetString(), testStepLabel.GetString());

					// MAG 251218 add bump LAST variable values
					for (i = 0; i < theApp.myFPTM.VARlist.size(); i++) theApp.myFPTM.VARlist[i].BumpPrevious();

					// Increment test step
					theApp.myFPTM.TSactive++;  // NOTE variable iTS not used after this step, do not need to also increment
					
					// Check for end of test block
					if (theApp.myFPTM.TSactive >= (int)theApp.myFPTM.testSteps[iTR].size()) {
						endTestStep = 2; // flag for ending test sequence
						cs1.Format(L"%s Signal to end test block!", dts.GetString());
						m_status.AddString(cs1);
					}
				}

				// This moves to next test list (i.e. Section A -> B) by incrementing TRlist or ending
				if(endTestStep == 2){
					// stop timers
					// done below if necessary SetTimer(NEXTLIST, 5, NULL); // set timer to start next cycle
					KillTimer(2);
					KillTimer(4);
					KillTimer(5);
					KillTimer(RAMPTIMER);
					fname.Format(L"%S Timers stopped- past all test steps for Test Block %c.", dtts,'A' + theApp.myFPTM.TRactive);
					m_status.AddString(fname);
				
					if (theApp.myFPTM.TRlistRepeat == 1) {
						theApp.myFPTM.RunTests();
						OnBnClickedStart();  // stop stuff happening
						theApp.myFPTM.ResetDelay(theApp.myFPTM.TRlistRepeat);
						OnBnClickedStart();	// restart stuff happening...
					} else {  // if multiple lists loaded move to next one
						theApp.myFPTM.RunTests();  // print summary of test results
						SetTimer(NEXTLIST, 5, NULL); // set timer to start next cycle, increments TRactive etc. OR the finish up reporting for last cycle
						if (iTR < (int)theApp.myFPTM.TRlist.size() - 1) {  // not at end of list yet
							fname.Format(L"%S Set timer to start test block %c...", dtts, 'A' + iTR + 1);
							sst << CT2A(fname) << std::endl;
							theApp.myFPTM.reportT2 << sst.str();
							m_status.AddString(fname);
							sst.clear();
							sst.str("");
						} else { // last list
							OnBnClickedStart();  // stop button, reset logs, etc
						}
					}
					// NOTE check for end of all tests is in next section
				}  // end if endTestStep
#endif			
				m_status.SetCurSel(m_status.GetCount() - 1);
			}
			catch (...) {
				m_status.AddString(L"ERROR: Caught exception in OnTimer::Case 2!");
			}
			m_status.SetCurSel(m_status.GetCount() - 1);
			CDialog::OnTimer(nIDEvent); // MAG 161006 changed from CDialogEX

			// MAG 251209 Move T3 log save here from Timer 5
			if(t3logSaved == 0) theApp.myFPTM.SaveLogT3();
			
			tnow = CTime::GetCurrentTime();  // shows how long step took
			dts.Format(L"%02d/%02d/%02d %02d:%02d:%02d\t", tnow.GetMonth(), tnow.GetDay(), tnow.GetYear() - 2000, tnow.GetHour(), tnow.GetMinute(), tnow.GetSecond());
			cs1.Format(L"%s Exit Timer 2.", dts.GetString());
			theApp.myFPTM.reportT2 << CT2A(cs1) << std::endl;

			break; // end case 2
			
		case 3:		// case 3 reserved as alignment timer for case 5. Reads should occur on 10 second intervals but offset 5 seconds (i.e. 5 sec, 15 sec, ...)
			cs1.Format(L"%s Enter Timer 3: Enter Alignment Timer 2 Block %c Step %d. Start Timer 5. Exit Timer 3. Pass through to Timer 5:", dts.GetString(), 'A' + iTR, iTS);
			m_status.AddString(cs1);

			SetTimer(5, 10000,NULL);
			KillTimer(3);
			// fall through to case 5
			// end case 3

		case 5: // MAG Screen capture timer (10s) AND reporting log 3 timer AND Periodic value processing AND check for event tests
			if (theApp.myFPTM.LaunchState == 0) break;

			if ((theApp.myFPTM.LaunchState == 1) && (theApp.myFPTM.setScreenCapture == 1)) {
				m_status.SetTopIndex(m_status.GetCount() - 1);
				fname.Format(L"FPTM screen %02d%02d%02d-%02d%02d%02d.bmp", tnow.GetMonth(), tnow.GetDay(), tnow.GetYear() - 2000, tnow.GetHour(), tnow.GetMinute(), tnow.GetSecond());
				CaptureScreenImage(this->GetSafeHwnd(),fname);
			}

			// MAG 250509 run READ events in current test step- T3 log is not updated properly without interval reads
			ti = iTS;
			cs1.Format(L"%s Enter Timer 5: Timed Reads TestStep=%d (not end of cycle! Test Step writes skipped.)",dts.GetString(),iTS);
			m_status.AddString(cs1);
#ifdef _DEBUG
			theApp.m_BACdlg->m_status.AddString(L"NOTE FPTM RUN IN DEBUG MODE NO PACKETS WERE SENT OR RECEIVED!");
#endif
			failList.clear();
			for (size_t j = 0; j < theApp.myFPTM.testSteps[iTR][ti].GetListSize(); j++) {
				triggerEvent = theApp.myFPTM.testSteps[iTR][ti].GetListEventID(j);
				if (theApp.myFPTM.TRlist[iTR][triggerEvent].GetRW() != 0) continue;  // only doing READS
				runResult = theApp.myFPTM.RunRequest(triggerEvent);
				if (runResult == 0) failList.push_back(triggerEvent);

			}
			if (failList.size() > 0) {
				cs1.Format(L"%s T5 Rerun %d failed requests:", dts.GetString(), failList.size());
				m_status.AddString(cs1);
				for (size_t j = 0; j < failList.size(); j++){
					runResult = theApp.myFPTM.RunRequest(failList[j]);
					cs1.Format(L"%s Rerun failed request %d. Status %d", dts.GetString(), failList[j], runResult);
					m_status.AddString(cs1);
				}
			}

			m_status.SetCurSel(m_status.GetCount() - 1);
			dts = theApp.GetDTS();
			cs1.Format(L"%s Exit Timer 5.", dts.GetString());
			m_status.AddString(cs1);
			break;
		case RAMPTIMER:
			try {
				for (rti = 0;rti < (int)theApp.myFPTM.rampList[iTR].size();rti++) {
					lrv = theApp.myFPTM.rampList[iTR][rti];
					tevent = theApp.myFPTM.TRlist[iTR][lrv.GetEventID()].GetEventTime();
					if (tnow <= tevent) continue; // it's not time yet

					if (tnow < tevent + lrv.GetDuration()) {
						// it's time! 
						cts = tnow - theApp.myFPTM.TRlist[iTR][lrv.GetEventID()].GetEventTime(); // get time since ramp start
						// check for var values in ramp start/end
						varIndex1 = lrv.GetVarStart();
						if (varIndex1 >= 0)
							lrv.SetStartValue(theApp.myFPTM.VARlist[varIndex1].GetValue());
						varIndex1 = lrv.GetVarEnd();
						if (varIndex1 >= 0)
							lrv.SetEndValue(theApp.myFPTM.VARlist[varIndex1].GetValue());

						// get value at time
						dv = lrv.GetValueAtTime((int)(cts.GetTimeSpan()));				// get ramp variable value
						theApp.myFPTM.TRlist[iTR][lrv.GetEventID()].SetValue(dv);		// set value to event
						// set value to VAR
						varIndex1 = theApp.myFPTM.TRlist[iTR][lrv.GetEventID()].GetFPV();
						if (varIndex1 >= 0) {
							theApp.myFPTM.VARlist[varIndex1].SetValue(dv);
						}
						theApp.myFPTM.RunRequest(lrv.GetEventID());								// run the event
						sst << "Ramp " << rti << " time " << dtts << " send value " << dv << std::endl;
						theApp.myFPTM.reportT2 << sst.str();
						sst.clear();
						sst.str("");
					} else { // it's past time, delete this ramp and timer if it's the last ramp
						rampDel.push_back(rti);
					}
				}
				if (rampDel.size() > 0) {
					while (rampDel.size() > 0) {
						theApp.myFPTM.rampList[iTR].erase(theApp.myFPTM.rampList[iTR].begin() + rampDel[rampDel.size() - 1]);
						rampDel.pop_back();
					}
					if (theApp.myFPTM.rampList.size() == 0) KillTimer(RAMPTIMER);
				}
			}
			catch (...) {
				m_status.AddString(L"ERROR: Caught exception in OnTime::RampTimer!");
			}
			m_status.SetCurSel(m_status.GetCount() - 1);
			CDialog::OnTimer(nIDEvent); // MAG 161006 changed from CDialogEX
			break;
		case NEXTLIST:  // move to next TRlist
			cs1.Format(L"%s Enter NEXTLIST timer.", dts.GetString());
			m_status.AddString(cs1);
			KillTimer(NEXTLIST);

			DisplayLineStart = 0;

			if(iTR < (int)theApp.myFPTM.TRlist.size() - 1){	// more test sections remain to test
				fname.Format(L"%S Block %c completed. Start Block %c.", dtts, 'A' + iTR, 'A' + iTR + 1);
				m_status.AddString(fname);

				theApp.myFPTM.ChangeActiveTRlist(1);	// bump up TRactive
				OnBnClickedResettimes();  
				theApp.myFPTM.SetStartState();
				SetTimer(4, (60 - tnow.GetSecond()) * 1000, NULL);	// set timer to start at next minute
			} else { 
				fname.Format(L"%S Test Block %c completed. This was the final Test Block.", dtts,'A' + iTR);
				m_status.AddString(fname);
				theApp.myFPTM.LaunchState = 0;
				cs1.Format(L"%s All Test Blocks have been processed. The test is over.",dts.GetString());
				m_status.AddString(cs1);
				KillTimer(2);
				KillTimer(4);
				KillTimer(5);
				KillTimer(RAMPTIMER);
			}
			m_status.SetCurSel(m_status.GetCount() - 1);

			theApp.myFPTM.FlushLogFiles();
#ifdef _DEBUG
			cs1.Format(L"%s Exit NEXTLIST timer.", dts.GetString());
			m_status.AddString(cs1);
#endif
			OnFileSavestatuswindowAuto();  // save contents of status window
			break;
		default:
			break;
	} // end timer switch

	return;
} // end CFPTMDlg::OnTimer


// MAG Function is called when start button is pressed and between test script blocks
// Function sets start time for each event and test script step, and sets Model Start Time.
// Gemchecked
void 
CFPTMDlg::OnBnClickedResettimes()
{
	// TODO: Add your control notification handler code here
	CTime			tnow = CTime::GetCurrentTime();
	CTime			tstart = {};	// original start time
	CTimeSpan		ts(0,0,0,0);  // set to 0
	CTimeSpan		oneMinute(0, 0, 1, 0);  // set to 1 minute
	CString			cs1;
	CString			dts;
	CString			dtsPre;
	CString			dtsPost;
	unsigned int	i = 0;
	int				j = 0;
	int				iTR = theApp.myFPTM.TRactive;  // Note bounds checked outside of subroutine

	dts = theApp.GetDTS();
	if (theApp.myFPTM.TRlist.size() == 0) {
		m_status.AddString(L"ERROR: There is no active event list. Add events to start event timer.");
		return;
	}

	if (((int)theApp.myFPTM.TRlist.size() < iTR)||(iTR < 0)) {	// list length problem
		m_status.AddString(L"ERROR: There is a logic problem. Please restart the FPTM.");
		return;
	}

	if (theApp.myFPTM.TRlist[iTR].size() == 0) {	// there is no active test list
		m_status.AddString(L"ERROR: There is no active event list. Add events to start event timer.");
		return;
	}

	// round to next minute
	ts = (CTimeSpan)tnow.GetSecond();
	tnow -= ts;

	if (theApp.myFPTM.TRlist[iTR].size() == 1){		// there's only one event in the current list
		theApp.myFPTM.TRlist[iTR][0].SetEventTime(tnow+oneMinute);
		UpdateEvents();
		dtsPost = theApp.GetDTS(theApp.myFPTM.TRlist[iTR][0].GetEventTime());
		cs1.Format(L"%s: Change event 1 time to %s.", dts.GetString(), dtsPost.GetString());
		return;
	}

	tstart = theApp.myFPTM.TRlist[iTR][0].GetEventTime();
	theApp.myFPTM.TRlist[iTR][0].SetEventTime(tnow + oneMinute);
	theApp.myFPTM.SetModelStartTime(tnow + oneMinute);
	dtsPre = theApp.GetDTS(tnow + oneMinute);
	mybtm[0].SetActivationTime(tnow + oneMinute);
	m_status.SetRedraw(FALSE);
	for (i = 1; i < theApp.myFPTM.TRlist[iTR].size(); i++){		// update the event list
		dtsPre = theApp.GetDTS(theApp.myFPTM.TRlist[iTR][i].GetEventTime());
		ts = theApp.myFPTM.TRlist[iTR][i].GetEventTime() - tstart;
		theApp.myFPTM.TRlist[iTR][i].SetEventTime(tnow + ts + oneMinute);
		dtsPost = theApp.GetDTS(theApp.myFPTM.TRlist[iTR][i].GetEventTime());
		cs1.Format(L"%s: Change event %d time from %s to %s.",dts.GetString(), i, dtsPre.GetString(), dtsPost.GetString());
		m_status.AddString(cs1);
	}
	for (i = 0; i < theApp.myFPTM.testSteps[iTR].size(); i++) {	// update the test step list
		j = theApp.myFPTM.testSteps[iTR][i].GetListEventID(0);
		if (j >= 0) {
			dtsPre = theApp.GetDTS( theApp.myFPTM.testSteps[iTR][i].GetTime());
			tstart = theApp.myFPTM.TRlist[iTR][j].GetEventTime();  // reuse tstart
			theApp.myFPTM.testSteps[iTR][i].SetTime(tstart);
			dtsPost = theApp.GetDTS(tstart);
			cs1.Format(L"%s: Change test step %d time from %s to %s.", dts.GetString(), i, dtsPre.GetString(), dtsPost.GetString());
			m_status.AddString(cs1);
		}
	}
	m_status.SetRedraw(TRUE);
	m_status.Invalidate();
	m_status.SetCurSel(m_status.GetCount());

	UpdateEvents();

	return;
} // end OnBnClickedResettimes


// Adds an event to the current event list. Note there is mo maximum length for this list.
// Gemchecked
void 
CFPTMDlg::OnBnClickedAddevent()
{
	// TODO: Add your control notification handler code here
	int		initSize = 0;
	CString cs;
	CTime	evtime = CTime::GetCurrentTime();

	if (theApp.myFPTM.TRlist.size() == 0) {
		theApp.myFPTM.AddEventList();  // add an event list
	}

	// check that TRactive doesn't exceed the TRlist array size
	if (theApp.myFPTM.TRactive >= (int)theApp.myFPTM.TRlist.size()) {
		m_status.AddString(L"ERROR: There is a logic error. Please restart the FPTM.");
		return;
	}

	initSize = theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size();

	if (theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size() > 0) 
		evtime = theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size() - 1].GetEventTime();

	theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].push_back(TimedRequest(-1, evtime, 0, 0, 0, -999));
	if (initSize < (int)theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size()){
		cs.Format(L"A new event has been added to the event list.  There are now %d events.", theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size());
	} else {
		cs.Format(L"There was a problem adding a new event to the event list.  There remains %d events.", theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size());
	}
	m_status.AddString(cs);
	DisplayLineStart = theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size() - DISPLAY_LINES;
	if (DisplayLineStart < 0) DisplayLineStart = 0;
	UpdateEvents();

	return;
}


// Updates the display of BTMlines
int
CFPTMDlg::UpdateEvents()
{
	int		i = 0;
	int		j = 0;
	CString csi;

	if (theApp.myFPTM.TRlist.size() > 0) {
		j = theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size();
		if (j > DISPLAY_LINES) m_linespin.ShowWindow(1);
		else m_linespin.ShowWindow(0);

		if (j > DISPLAY_LINES) j = DISPLAY_LINES;

		for (i = 0; i < j; i++) {
			mybtm[i].DisplayLine(i + DisplayLineStart);
		}
	}

	for ( i = j; i < DISPLAY_LINES; i++){
		mybtm[i].SetVisible(0);
	}

	if (theApp.myFPTM.TRlist.size() > 0) {
		i = theApp.myFPTM.ChangeActiveTRlist(0); // 0 returns active list ID (note print value is this +1)
		csi.Format(L"%d/%d %s", i + 1, theApp.myFPTM.TRlist.size(),theApp.myFPTM.TRdata[i].GetLabel().GetString());
		m_event_list_id.SetWindowText(csi);
		if (theApp.myFPTM.TRlist[i].size() > DISPLAY_LINES) {
			csi.Format(L"%d events in list.", theApp.myFPTM.TRlist[i].size());
			m_event_count.SetWindowText(csi);
		} else {
			m_event_count.SetWindowText(L"");
		}
	} else {
		m_event_list_id.SetWindowText(L"0");
		m_event_count.SetWindowText(L"");
	}

	return 0;
}


// Updates the display
void 
CFPTMDlg::UpdateDevices()
{
	// vars for adding devices to dialog
	char			mac[20] = {};
	char			router[20] = {};
	char			devlabel[260] = {};
	int				snet = 0;
	int				i = 0;
	int				sdev = 0;  // starting device
	unsigned char	routerip[20] = {};
	unsigned char	devip[20] = {};
	CString			ds2;

	sdev = mybtm[0].GetDeviceCount();
	for (unsigned int j = sdev; j < theApp.myFPTM.BACnetList.size(); j++){
		theApp.myFPTM.BACnetList[j].GetIP(&devip[0]);
		theApp.myFPTM.BACnetList[j].GetRouterIP(&routerip[0]);
		theApp.myFPTM.BACnetList[j].GetMAC(mac);
		theApp.myFPTM.BACnetList[j].GetRouterMAC(router);
		snet = theApp.myFPTM.BACnetList[j].GetNet();

		ds2.Format(L"DevID %d", theApp.myFPTM.BACnetList[j].GetDevObjInst());
		// MAG 260326 Modify display to only show Device Object ID (line above)
		/*if (devip[0] > 0) {  // if the device has an IP address, so does the router
			if (router[0] > 0){
				ds2.Format(L"%03d.%03d.%03d.%03d / net %d / %03d.%03d.%03d.%03d", devip[0], devip[1], devip[2], devip[3], snet, routerip[0], routerip[1], routerip[2], routerip[3]);
			} else {  // no router
				ds2.Format(L"%03d.%03d.%03d.%03d", devip[0], devip[1], devip[2], devip[3]);
			}
		} else if (routerip[0] > 0){  // router IP, device MAC
			ds2.Format(L"%S / net %d / %03d.%03d.%03d.%03d", mac, snet, routerip[0], routerip[1], routerip[2], routerip[3]);
		} else {	// device and router use MAC
			ds2.Format(L"%S / net %d / %S", mac, snet, router);
		}*/

		theApp.myFPTM.BACnetList[j].GetDeviceLabel(devlabel);
		if (strlen(devlabel) > 0){
			ds2 += " (";
			ds2 += devlabel;
			ds2 += ")";
		}

		// add to all device combo boxes
		for (i = 0; i < DISPLAY_LINES; i++){
			mybtm[i].AddDevice(ds2);
		}
	}

	return;
}


void
CFPTMDlg::OnDeltaposLinespin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	// pNMUpDown->iDelta == -1 for click up, 1 for click down
	int trs = 0;
	
	*pResult = 0;

	if (theApp.myFPTM.TRlist.size() == 0) return;

	trs = theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size();
	if (pNMUpDown->iDelta == -1){  // show higher numbers
		if (DisplayLineStart < trs - DISPLAY_LINES) DisplayLineStart++;
	} else {
		if (DisplayLineStart > 0) DisplayLineStart--;
	}
	UpdateEvents();

	return;
}


void
CFPTMDlg::OnFileSavetofile()
{
	// TODO: Add your command handler code here
	CWaitCursor		cwait;
	char			fileName2[MAX_PATH] = {};
	CString			pathName = L"FPTM Save File.xml";
	std::wofstream	ofop;
	CFileDialog		fileDlg(FALSE, L"xml", pathName, OFN_OVERWRITEPROMPT, L"XML Files (*.xml)|*.xml||");

	try{
		if (fileDlg.DoModal() == IDOK) {
			pathName = fileDlg.GetPathName();  // note we are not opening the file here, just getting the name
			CT2A path_str(pathName);

			if (theApp.myFPTM.SaveToFile(path_str.m_psz) != EXIT_SUCCESS) {
				m_status.AddString(L"ERROR: could not save config file.");
			}
		}
	}
	catch (const std::exception& e) {
		// Log the error message
		CString err;
		m_status.AddString(L"ERROR: Caught exception in OnFileSavetofile!");
		err.Format(L"ERROR: %S", e.what());
		m_status.AddString(err);
	}
	catch (...){
		m_status.AddString(L"Problem opening save file dialog- try again.");
	}

	return;
}


// File menu selection to read config from file
void 
CFPTMDlg::OnFileReadfromfile()
{
	// TODO: Add your command handler code here
	CString			pathName;
	std::wifstream	ofop;

	CFileDialog fileDlg(FALSE, L"xml", pathName, OFN_FILEMUSTEXIST || OFN_READONLY, L"XML Files (*.xml)|*.xml||");

	try{
		if (fileDlg.DoModal() == IDOK) {
			pathName = fileDlg.GetPathName();
			CT2A path_str(pathName);

			if (theApp.myFPTM.ReadFromFile(path_str.m_psz) == 1){
				m_status.AddString(L"Config read from file (no error):");
			} else {
				m_status.AddString(L"Error reading config file.");
			}
		}
	} 
	catch (const std::exception& e) {
		// Log the error message
		CString err;
		m_status.AddString(L"ERROR: Caught exception in OnFileReadfromfile!");
		err.Format(L"ERROR: %S", e.what());
		m_status.AddString(err);
	}
	catch (...){
		m_status.AddString(L"Problem opening read file dialog- try again.");
	}

	return;
}


void 
CFPTMDlg::OnBnClickedSetsavefile()
{
	// TODO: Add your control notification handler code here
	CString			pathName;
	std::wifstream	ofop;

	CFileDialog fileDlg(FALSE, L"txt", pathName, OFN_OVERWRITEPROMPT, L"TXT Files (*.txt)|*.txt||");

	try {
		if (fileDlg.DoModal() == IDOK) {
			pathName = fileDlg.GetPathName();
			CT2A path_str(pathName);
			theApp.myFPTM.SetEventSavefile(path_str);
		}
	}
	catch (const std::exception& e) {
		// Log the error message
		CString err;
		m_status.AddString(L"ERROR: Caught exception in OnBnClickedSetsavefile!");
		err.Format(L"ERROR: %S", e.what());
		m_status.AddString(err);
	}
	catch (...) {
		m_status.AddString(L"ERROR: Exception in OnBnClickedSetsavefile. Problem opening read file dialog- try again.");
	}

	return;
}


void 
CFPTMDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	// NOTE parallel code in OnClose
	CString cs1("Do you want to exit the FPTM?");
	int i = AfxMessageBox(cs1, MB_YESNO);
	
	if (i != 6) return;
	CDialog::OnOK();

	return;
}


int 
CFPTMDlg::PreTranslateMessage(MSG *pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE){
		OnBnClickedOk();
		return 0;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


// Net Scan
void
CFPTMDlg::OnBnClickedSetsavefile2()
{
	// TODO: Add your control notification handler code here
	theApp.myFPTM.UpdateBACnetDevices();
	UpdateDevices();

	return;
}


// MAG 160705 add this function
// Purpose: to send a reset to all overwritten values
// Note: sends to read and write labelled values, in case one was written then changed to write
// Note: individual points are also reset when manually deleted.
void 
CFPTMDlg::OnFileResetvalues()
{
	// TODO: Add your command handler code here
	if (theApp.myFPTM.TRlist.size() == 0) return;

	m_status.AddString(L"Resetting all values in event list to controller default.");
	for (size_t i = 0; i < theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size(); i++){
		theApp.myFPTM.RunReset(i);
	}

	return;
}


void 
CFPTMDlg::OnRuntimeoptionsSendoutofservice()
{
	// TODO: Add your command handler code here
	// NOTE function deprecated
	//CMenu* mmenu = GetMenu();
	//CMenu* submenu = mmenu->GetSubMenu(1); 
	//UINT state = submenu->GetMenuState(ID_RUNTIMEOPTIONS_SENDOUTOFSERVICE, MF_BYCOMMAND);
	//if (state & MF_CHECKED)
	//	submenu->CheckMenuItem(ID_RUNTIMEOPTIONS_SENDOUTOFSERVICE, MF_UNCHECKED | MF_BYCOMMAND);
	//else
	//	submenu->CheckMenuItem(ID_RUNTIMEOPTIONS_SENDOUTOFSERVICE, MF_CHECKED | MF_BYCOMMAND);
	//theApp.myFPTM.SetSendOutOfService();

	return;
}


void 
CFPTMDlg::SetPriorityLevelMenu(int newLevel)
{
	CStringW	cs1;
	CMenu		*tlm = GetMenu();									// gets the menu bar
	CMenu		*settings = tlm->GetSubMenu	(1);					// settings pulldown
	CMenu		*setpl = settings->GetSubMenu(1);					// priority level line
	int			prLevel = theApp.myFPTM.GetWritePriorityLevel();	// previous priority level

	// range check on level
	if (newLevel < 1) return;
	if (newLevel > 16) return;
	
	setpl->CheckMenuItem(ID_PRIORITYLEVEL_1+(prLevel-1), MF_UNCHECKED | MF_BYCOMMAND);
	setpl->CheckMenuItem(ID_PRIORITYLEVEL_1 + (newLevel - 1), MF_CHECKED | MF_BYCOMMAND);
	theApp.myFPTM.SetWritePriorityLevel(newLevel);

	cs1.Format(L"Settings change: Set write priority level from %d to %d.", prLevel, newLevel);
	m_status.AddString(cs1);

	return;
}


void 
CFPTMDlg::OnPrioritylevel1()
{
	// TODO: Add your command handler code here
	SetPriorityLevelMenu(1);

	return;
}


void 
CFPTMDlg::OnPrioritylevel2()
{
	// TODO: Add your command handler code here
	SetPriorityLevelMenu(2);

	return;
}


void 
CFPTMDlg::OnPrioritylevel3()
{
	// TODO: Add your command handler code here
	SetPriorityLevelMenu(3);

	return;
}


void 
CFPTMDlg::OnPrioritylevel4()
{
	// TODO: Add your command handler code here
	SetPriorityLevelMenu(4);

	return;
}


void 
CFPTMDlg::OnPrioritylevel5()
{
	// TODO: Add your command handler code here
	SetPriorityLevelMenu(5);

	return;
}


void 
CFPTMDlg::OnPrioritylevel6()
{
	// TODO: Add your command handler code here
	SetPriorityLevelMenu(6);

	return;
}


void 
CFPTMDlg::OnPrioritylevel7()
{
	// TODO: Add your command handler code here
	SetPriorityLevelMenu(7);

	return;
}


void 
CFPTMDlg::OnPrioritylevel8()
{
	// TODO: Add your command handler code here
	SetPriorityLevelMenu(8);

	return;
}


void
CFPTMDlg::OnPrioritylevel9()
{
	// TODO: Add your command handler code here
	SetPriorityLevelMenu(9);

	return;
}


void 
CFPTMDlg::OnPrioritylevel10()
{
	// TODO: Add your command handler code here
	SetPriorityLevelMenu(10);

	return;
}


void 
CFPTMDlg::OnPrioritylevel11()
{
	// TODO: Add your command handler code here
	SetPriorityLevelMenu(11);

	return;
}


void
CFPTMDlg::OnPrioritylevel12()
{
	// TODO: Add your command handler code here
	SetPriorityLevelMenu(12);

	return;
}


void
CFPTMDlg::OnPrioritylevel13()
{
	// TODO: Add your command handler code here
	SetPriorityLevelMenu(13);

	return;
}


void
CFPTMDlg::OnPrioritylevel14()
{
	// TODO: Add your command handler code here
	SetPriorityLevelMenu(14);

	return;
}


void
CFPTMDlg::OnPrioritylevel15()
{
	// TODO: Add your command handler code here
	SetPriorityLevelMenu(15);

	return;
}


void
CFPTMDlg::OnPrioritylevel16()
{
	// TODO: Add your command handler code here
	SetPriorityLevelMenu(16);

	return;
}


// MAG 161014 add this function to support correct menu appearance when reading settings files
// repeat event value is read from app, not from input
void
CFPTMDlg::SetRepeatEventListMenu()
{
	CMenu*	mmenu = GetMenu();
	CMenu*	submenu = mmenu->GetSubMenu(1);	// Runtime Options
	CString cs1;
	int		curval = 0;

	// get current oos status
	curval = theApp.myFPTM.GetListRepeat();

	if (curval > 0){
		submenu->CheckMenuItem(ID_RUNTIMEOPTIONS_REPEATEVENTLIST, MF_CHECKED | MF_BYCOMMAND);
	} else {
		submenu->CheckMenuItem(ID_RUNTIMEOPTIONS_REPEATEVENTLIST, MF_UNCHECKED | MF_BYCOMMAND);
	}

	return;
}


void
CFPTMDlg::SetSendOutOfServiceMenu(int objid)
{
	CMenu* mmenu = GetMenu();
	CMenu* submenu = mmenu->GetSubMenu(1);	// settings
	CMenu *setoos = submenu->GetSubMenu(0);	// send out of service line
	int curval = 0;
	int guiid = 0;
	CString cs1;
	char objtype[15] = {};
	
	// get current oos status
	curval = theApp.myFPTM.GetSendOutOfService(objid);

	//write new oos status
	theApp.myFPTM.SetSendOutOfService(objid);
	
	// map to GUI ID
	switch (objid){
		case 0: guiid = ID_SENDOUTOFSERVICE_AI; strcpy_s(objtype, 5,"AI"); break;
		case 1: guiid = ID_SENDOUTOFSERVICE_AO; strcpy_s(objtype, 5,"AO"); break;
		case 2: guiid = ID_SENDOUTOFSERVICE_AV; strcpy_s(objtype, 5,"AV"); break;
		case 3: guiid = ID_SENDOUTOFSERVICE_BI; strcpy_s(objtype, 5,"BI"); break;
		case 4: guiid = ID_SENDOUTOFSERVICE_BO; strcpy_s(objtype, 5,"BO"); break;
		case 5: guiid = ID_SENDOUTOFSERVICE_BV; strcpy_s(objtype, 5,"BV"); break;
		case 13: guiid = ID_SENDOUTOFSERVICE_MSI; strcpy_s(objtype, 5, "MI"); break;
		case 14: guiid = ID_SENDOUTOFSERVICE_MSO; strcpy_s(objtype, 5, "MO"); break;
		case 19: guiid = ID_SENDOUTOFSERVICE_MSV; strcpy_s(objtype, 5, "MV"); break;

		default: return;  // not sure what happened here
		break;
	}
	// update check marks on gui
	if (curval == 1){ // set check mark off
		setoos->CheckMenuItem(guiid, MF_UNCHECKED | MF_BYCOMMAND);
		cs1.Format(L"Settings change: Out-of-service NOT sent before write command for object %S.", objtype);
	} else {
		setoos->CheckMenuItem(guiid, MF_CHECKED | MF_BYCOMMAND);
		cs1.Format(L"Settings change: Out-of-service IS sent before write command for object %S.", objtype);
	}
	m_status.AddString(cs1);

	return;
}


void 
CFPTMDlg::OnSendoutofserviceAi()
{
	// TODO: Add your command handler code here
	SetSendOutOfServiceMenu(0);

	return;
}


void 
CFPTMDlg::OnSendoutofserviceAo()
{
	// TODO: Add your command handler code here
	SetSendOutOfServiceMenu(1);

	return;
}


void 
CFPTMDlg::OnSendoutofserviceAv()
{
	// TODO: Add your command handler code here
	SetSendOutOfServiceMenu(2);

	return;
}


void
CFPTMDlg::OnSendoutofserviceBi()
{
	// TODO: Add your command handler code here
	SetSendOutOfServiceMenu(3);

	return;
}


void
CFPTMDlg::OnSendoutofserviceBo()
{
	// TODO: Add your command handler code here
	SetSendOutOfServiceMenu(4);

	return;
}


void
CFPTMDlg::OnSendoutofserviceBv()
{
	// TODO: Add your command handler code here
	SetSendOutOfServiceMenu(5);

	return;
}


void 
CFPTMDlg::OnSendoutofserviceMsi()
{
	// TODO: Add your command handler code here
	SetSendOutOfServiceMenu(13);

	return;
}


void
CFPTMDlg::OnSendoutofserviceMso()
{
	// TODO: Add your command handler code here
	SetSendOutOfServiceMenu(14);

	return;
}


void
CFPTMDlg::OnSendoutofserviceMsv()
{
	// TODO: Add your command handler code here
	SetSendOutOfServiceMenu(19);

	return;
}


void
CFPTMDlg::OnRuntimeoptionsRepeateventlist()
{
	// TODO: Add your command handler code here
	CMenu* mmenu = GetMenu();
	CMenu* submenu = mmenu->GetSubMenu(1);	// settings
	CString cs1;

	UINT state = submenu->GetMenuState(ID_RUNTIMEOPTIONS_REPEATEVENTLIST, MF_BYCOMMAND);
	if (state & MF_CHECKED){
		submenu->CheckMenuItem(ID_RUNTIMEOPTIONS_REPEATEVENTLIST, MF_UNCHECKED | MF_BYCOMMAND);
		theApp.myFPTM.SetListRepeat(0);
		cs1.Format(L"Settings change: Event loop WILL NOT loop.");
	} else {
		submenu->CheckMenuItem(ID_RUNTIMEOPTIONS_REPEATEVENTLIST, MF_CHECKED | MF_BYCOMMAND);
		theApp.myFPTM.SetListRepeat(1);
		cs1.Format(L"Settings change: Event loop WILL loop.");
	}
	m_status.AddString(cs1);

	return;
}


void
CFPTMDlg::OnFileAbout()
{
	// TODO: Add your command handler code here
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();

	return;
}


void
CFPTMDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	// NOTE parallel code in OnBnClickedOk
	if (AfxMessageBox(L"Do you want to exit the FPTM?", MB_YESNO) == IDYES){
		CDialog::OnClose(); // MAG 161006 changed from CDialogEX
	} else { 
		if (theApp.myFPTM.TRlist.size() > 0) {
			for (size_t i = 0; i < theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size(); i++) {
				theApp.myFPTM.RunReset(i);
			}
		}
	}

	return;
}


// A file has been dropped on the FPTM GUI, deal with it
void
CFPTMDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	int filecount = 0;
	int i = 0;
	int j = 0;
	TCHAR filename[MAX_PATH];
	char filename2[MAX_PATH];
	CString fb;

	filecount = DragQueryFile(hDropInfo, 0xFFFFFFFF, filename, MAX_PATH);
	
	for (i = 0; i < filecount; i++){
		j = DragQueryFile(hDropInfo, i, filename, MAX_PATH);

		WideCharToMultiByte(CP_ACP, 0, filename, MAX_PATH, filename2, MAX_PATH,NULL,NULL);
		j = strlen(filename2);
		if (j <= 0) return;
		fb = filename2;
		if (fb.Right(3).MakeUpper().Compare(L"XML") != 0) {
			m_status.AddString(L"ERROR: Invalid file type entered.");
			return;
		}
		j = theApp.myFPTM.ReadFromFile(filename2);
		if (j == 1) {
			fb.Format(L"Last settings file: %S", filename2);
			m_last_file.SetWindowText(fb);
		}
	}
	m_status.SetCurSel(m_status.GetCount() - 1);
	FillStepJumper();
	UpdateEvents();

	CDialog::OnDropFiles(hDropInfo);

	return;
}


// MAG 170519 add function
//  Purpose: to fill TRList with data to help debug AFDD
void 
CFPTMDlg::OnBnClickedDebugTestData()
{
	// TODO: Add your control notification handler code here
	static int cycles=0;

	if (theApp.myFPTM.TRlist.size() == 0)  return;

	while(theApp.myFPTM.TRlist[theApp.myFPTM.TRactive].size() < 11){
		OnBnClickedAddevent();
	}
	// MAG currently set for Iowa terminal box C129
	theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][0].SetValue(2.5); // Pressure
	theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][1].SetValue(80 + 3*cos(cycles*5 / 6.28)); // DA Temp
	theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][2].SetValue(82); // DAT SP
	theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][3].SetValue(78 + 3 * cos(cycles * 5 / 6.28)); // Zone Temp
	theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][4].SetValue(70); // Htg SP
	theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][5].SetValue(82); // Clg SP
	theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][6].SetValue(75); // Box htg
	theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][7].SetValue(65); // Damper
	theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][8].SetValue(1.5); // sup flow
	theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][9].SetValue(1.6); // sup flow SP
	theApp.myFPTM.TRlist[theApp.myFPTM.TRactive][10].SetValue(1); // occ
	UpdateEvents();
	cycles++;

	return;
}


void 
CFPTMDlg::OnBnClickedSendData()
{
	// TODO: Add your control notification handler code here
#ifdef USEAFDD
	theApp.myFPTM.SendDataAFDD();
#endif
	//CaptureScreenImage(this->GetSafeHwnd());
	OnBnClickedDebugTestData();  // updates changed values

	return;
}


void 
CFPTMDlg::OnRuntimeoptionsDisplayvariables()
{
	// TODO: Add your command handler code here
	CString cs1;
	std::vector<FPvar>::iterator it;
	char lab1[250] = {};

	if (theApp.myFPTM.VARlist.size() > 0) {
		for (it = theApp.myFPTM.VARlist.begin(); it != theApp.myFPTM.VARlist.end(); it++){
			it->GetLabel(lab1);
			cs1.Format(L"Variable name: '%S' : Value: %f", lab1, it->GetValue());
			m_status.AddString(cs1);
		}
	}

	return;
}


// MAG 250821
// Parameter autoname: 0 to open file selection window, 1 to use default generated file name
void 
CFPTMDlg::OnFileSavestatuswindow()
{
	// TODO: Add your command handler code here
	int		i = 0;
	char	baseFilech[256] = {};
	CTime	tnow = CTime::GetCurrentTime();
	std::wofstream ofop;
	CString pathName;
	CString cs1;

	// set preferred file name
	if (theApp.myFPTM.reportdts.size() > 1) {
		pathName.Format(L"FPTM-STATUS-log-%S.txt", theApp.myFPTM.reportdts.c_str());
	} else {
		pathName.Format(L"FPTM-STATUS-log-%02d%02d%02d-%02d%02d%02d.txt", tnow.GetMonth(), tnow.GetDay(), tnow.GetYear() - 2000, tnow.GetHour(), tnow.GetMinute(), tnow.GetSecond());
	}

	try {
		CFileDialog fileDlg(FALSE, L"txt", pathName, OFN_OVERWRITEPROMPT, L"TXT Files (*.txt)|*.txt||");
		if (fileDlg.DoModal() == IDOK) {
			ofop.open(pathName, std::ios::out);
		}
		if (!ofop.is_open()) {
			theApp.m_BACdlg->m_status.AddString(L"ERROR could not open file. Status window contents were not saved.");
			return;
		}
		
		if (ofop.is_open()) {
			for (i = 0;i < m_status.GetCount(); i++) {
				m_status.GetText(i, cs1);
				ofop << cs1.GetString() << std::endl;
			}
			ofop.close();
			m_status.AddString(L"Status file has been saved.");
		} else {
			m_status.AddString(L"Error saving status file.");
		}
	}
	catch (const std::exception& e) {
		std::stringstream ss1;
		m_status.AddString(L"ERROR: Caught exception in OnFileSavestatuswindow!");
		ss1 << "ERROR: " << e.what();
		cs1 = ss1.str().c_str();
		m_status.AddString(cs1);
	}
	catch (...) {
		m_status.AddString(L"Problem opening read file dialog- try again.");
	}
	m_status.SetCurSel(m_status.GetCount() - 1);

	return;
} // end OnFileSavestatuswindow


// MAG 250821
// Copied shamelessly from OnFileSavestatuswindow
// Gemchecked
void
CFPTMDlg::OnFileSavestatuswindowAuto()
{
	// TODO: Add your command handler code here
	CWaitCursor		cwait;
	CTime			tnow = CTime::GetCurrentTime();
	CString			pathName;
	CString			cs1;
	std::wofstream	ofop;
	int				statCount = m_status.GetCount();

	// set preferred file name NOTE current working directory is set outside of subroutine
	if (theApp.myFPTM.reportdts.size() > 1) {
		pathName.Format(L"FPTM-STATUS-log-%S.txt", theApp.myFPTM.reportdts.c_str());
	} else {
		pathName.Format(L"FPTM-STATUS-log-%02d%02d%02d-%02d%02d%02d.txt", tnow.GetMonth(), tnow.GetDay(), tnow.GetYear() - 2000, tnow.GetHour(), tnow.GetMinute(), tnow.GetSecond());
	}

	try {
		ofop.open(pathName, std::ios::out);
		if(ofop.is_open()){
			for (int i = 0;i < statCount; i++) {
				m_status.GetText(i, cs1);
				ofop << cs1.GetString() << L"\n";
			}
			ofop.close();
			cwait.Restore();
			m_status.AddString(L"Status window file has been autosaved.");
		} else {
			m_status.AddString(L"Error autosaving status window file.");
		}
	}
	catch (const std::exception& e) {
		// Log the error message
		CString err;
		m_status.AddString(L"ERROR: Caught exception autosaving status window file!");
		err.Format(L"ERROR: %S", e.what());
		m_status.AddString(err);
	}
	catch (...) {
		m_status.AddString(L"ERROR: Problem autosaving status window file.");
	}
	m_status.SetCurSel(m_status.GetCount() - 1);

	return;
}


void 
CFPTMDlg::OnRuntimeoptionsCheckoos()
{
	// TODO: Add your command handler code here
	CMenu*	mmenu = GetMenu();
	CMenu*	submenu = mmenu->GetSubMenu(1);	// settings
	CString cs1;
	UINT	state = submenu->GetMenuState(ID_RUNTIMEOPTIONS_CHECKOOS, MF_BYCOMMAND);

	if (state & MF_CHECKED) {
		submenu->CheckMenuItem(ID_RUNTIMEOPTIONS_CHECKOOS, MF_UNCHECKED | MF_BYCOMMAND);
		theApp.myFPTM.SetListRepeat(0);
		cs1.Format(L"Settings change: Event loop WILL NOT loop.");
	} else {
		submenu->CheckMenuItem(ID_RUNTIMEOPTIONS_CHECKOOS, MF_CHECKED | MF_BYCOMMAND);
		theApp.myFPTM.SetListRepeat(1);
		cs1.Format(L"Settings change: Event loop WILL loop.");
	}
	m_status.AddString(cs1);

	return;
}


// MAG
// Purpose: Called when blockspin is selected to change Test Block (i.e. A -> B, aka TRlist)
// Calls other subroutines to update and refresh display
void 
CFPTMDlg::OnDeltaposBlockspin(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	// TODO: Add your control notification handler code here
	theApp.myFPTM.ChangeActiveTRlist((pNMUpDown->iDelta == -1?1:-1));

	FillStepJumper();

	DisplayLineStart = 0;  // reset events display

	UpdateEvents();

	*pResult = 0;
}


void
CFPTMDlg::FillStepJumper()
{
	int eventCount = 0;
	int i = 0;
	int curlen = 0;
	CString cs1;

	// TODO: Add your control notification handler code here
	eventCount = theApp.myFPTM.testSteps[theApp.myFPTM.TRactive].size();
	curlen = m_step_jumper.GetCount();  //MAG TODO NOTE- TS index does not correlate to step- list W and R steps with sazme ##
	if (curlen < eventCount) {
		for (i = curlen; i < eventCount; i++) {
			if (i % 2 == 0) {
				cs1.Format(L"%dW", 1 + (int)i / 2);// should be set to W or R, for Top/Write or Bottom/Read
			} else {
				cs1.Format(L"%dR", 1 + (int)i / 2);
			}
			m_step_jumper.AddString(cs1);
		}
	} else {
		while (m_step_jumper.GetCount() > eventCount) m_step_jumper.DeleteString(m_step_jumper.GetCount() - 1);
	}
	m_step_jumper.SetCurSel(0);
}


void 
CFPTMDlg::OnConfigurationinformationVariables()
{
	// TODO: Add your command handler code here
	CString cs1;
	int		i = 1;

	if (theApp.myFPTM.VARlist.size() == 0) {
		m_status.AddString(L"There are no variables defined.");
		return;
	}
	cs1.Format(L"There are %d variables:", theApp.myFPTM.VARlist.size());
	m_status.AddString(cs1);
	for (std::vector<FPvar>::iterator fpi = theApp.myFPTM.VARlist.begin(); fpi < theApp.myFPTM.VARlist.end(); fpi++){
		cs1.Format(L"%02d Label: \"%S\"  Value %5.2f", i, fpi->GetLabel().c_str(), fpi->GetValue());
		m_status.AddString(cs1);
		i++;
	}
	m_status.AddString(L"--END LIST---");
}


void 
CFPTMDlg::OnConfigurationinformationNamedpoints()
{
	// TODO: Add your command handler code here
	CString cs1;
	int		i = 1;

	if (theApp.myFPTM.namedPointList.size() == 0) {
		m_status.AddString(L"There are no named points.");
		return;
	}

	cs1.Format(L"There are %d named points:", theApp.myFPTM.namedPointList.size());
	m_status.AddString(cs1);
	for (std::vector<NamedPoint>::iterator fpi = theApp.myFPTM.namedPointList.begin(); fpi < theApp.myFPTM.namedPointList.end(); fpi++){
		cs1.Format(L"%02d Dev: %02d Obj: %02d Inst: %02d Label: \"%S\"  Alias: \"%S\"", i, fpi->GetDevice(),fpi->GetBACnetObject(), fpi->GetInstance(),
			fpi->GetLabel().c_str(), fpi->GetDescription().c_str());
		m_status.AddString(cs1);
		i++;
	}
	m_status.AddString(L"--END LIST---");
}


void 
CFPTMDlg::OnConfigurationinformationTests()
{
	// TODO: Add your command handler code here
	CString cs1;
	int		i = 1;

	if (theApp.myFPTM.TESTlist.size() == 0) {
		m_status.AddString(L"There are no tests defined.");
		return;
	}
	cs1.Format(L"There are %d tests:", theApp.myFPTM.TESTlist.size());
	m_status.AddString(cs1);
	for (std::vector<DualVarTestList>::iterator fpi = theApp.myFPTM.TESTlist[theApp.myFPTM.TRactive].begin(); fpi < theApp.myFPTM.TESTlist[theApp.myFPTM.TRactive].end(); fpi++){
		cs1.Format(L"%02d Var: %2d Value: %5.2f Trigger: %2d Relate: %2d Result: %2d", i, fpi->GetVarIndex(), fpi->GetValue(), fpi->GetTriggerEvent(),
			fpi->GetRelate(), fpi->GetResult());
		m_status.AddString(cs1);
		i++;
	}
	m_status.AddString(L"--END LIST---");
}


// MAG 241223
// When sparseEvents is set to zero all input/output cells read from Taylor/CalNEXT input scripts create an event.
// When it is set to 1 only cells with a value different from the previous column create an event.
void 
CFPTMDlg::OnConfigurationinformationSparseevents()
{
	// TODO: Add your command handler code here
	CMenu* tlm = GetMenu();		// gets the menu bar
	CMenu* settings = tlm->GetSubMenu(2);	// config info pulldown
	CMenu* setpl = settings->GetSubMenu(3);	// sparse events line
	int spLevel = 0;

	theApp.myFPTM.sparseEvents = (theApp.myFPTM.sparseEvents ? 0 : 1);  // flip from 0 to 1 or the opposite
	if (theApp.myFPTM.sparseEvents == 1) {
		settings->CheckMenuItem(ID_CONFIGURATIONINFORMATION_SPARSEEVENTS, MF_CHECKED | MF_BYCOMMAND);
		m_status.AddString(L"Sparse input event generation enabled. Note this will not affect events already loaded.");
	} else {
		settings->CheckMenuItem(ID_CONFIGURATIONINFORMATION_SPARSEEVENTS, MF_UNCHECKED | MF_BYCOMMAND);
		m_status.AddString(L"Sparse input event generation disabled.");
	}

	return;
}


void 
CFPTMDlg::OnConfigurationinformationScreencapture()
{
	// TODO: Add your command handler code here
	CMenu* tlm = GetMenu();		// gets the menu bar
	CMenu* settings = tlm->GetSubMenu(2);	// config info pulldown
	CMenu* setpl = settings->GetSubMenu(4);	// set screencapture line

	theApp.myFPTM.setScreenCapture = (theApp.myFPTM.setScreenCapture ? 0 : 1);  // flip from 0 to 1 or the opposite
	if (theApp.myFPTM.setScreenCapture == 1) {
		settings->CheckMenuItem(ID_CONFIGURATIONINFORMATION_SCREENCAPTURE, MF_CHECKED | MF_BYCOMMAND);
		m_status.AddString(L"Screen capture 10s interval enabled.");
	} else {
		settings->CheckMenuItem(ID_CONFIGURATIONINFORMATION_SCREENCAPTURE, MF_UNCHECKED | MF_BYCOMMAND);
		m_status.AddString(L"Screen capture disabled.");
	}

	return;
}


// Based on MS instructions
int  
CaptureScreenImage(HWND hWnd, CString fname)
{
	HDC hdcScreen;
	HDC hdcWindow;
	HDC hdcMemDC = nullptr;
	HBITMAP hbmScreen = nullptr;
	BITMAP bmpScreen;
	DWORD dwBytesWritten = 0;
	DWORD dwSizeofDIB = 0;
	HANDLE hFile = nullptr;
	char* lpbitmap = nullptr;
	HANDLE hDIB = nullptr;
	DWORD dwBmpSize = 0;
	static int picCounter = 0;
	RECT myRect = { 0,0,0,0 };
	LPRECT	mylpr = &myRect;
	int mycx = 0;
	int mycy = 0;

	CTime	tnow = CTime::GetCurrentTime();

	// Retrieve the handle to a display device context for the client area of the window. 
	hdcScreen = GetDC(NULL);
	hdcWindow = GetDC(hWnd);

	// Create a compatible DC, which is used in a BitBlt from the window DC.
	hdcMemDC = CreateCompatibleDC(hdcWindow);

	if (!hdcMemDC){
		MessageBox(hWnd, L"CreateCompatibleDC has failed", L"Failed", MB_OK);
		goto done;
	}

	// Get the client area for size calculation.
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);

	GetClientRect(hWnd, &myRect);
	mycx = myRect.right - myRect.left;
	mycy = myRect.bottom - myRect.top;

	// This is the best stretch mode.
	SetStretchBltMode(hdcWindow, HALFTONE);

	// Create a compatible bitmap from the Window DC.
	hbmScreen = CreateCompatibleBitmap(hdcWindow, mycx, mycy);

	if (!hbmScreen){
		MessageBox(hWnd, L"CreateCompatibleBitmap Failed", L"Failed", MB_OK);
		goto done;
	}

	// Select the compatible bitmap into the compatible memory DC.
	SelectObject(hdcMemDC, hbmScreen);

	// Bit block transfer into our compatible memory DC.
	if (!BitBlt(hdcMemDC,
		0, 0,
		mycx, mycy,
		hdcWindow,
		0, 0,
		SRCCOPY))
	{
		MessageBox(hWnd, L"BitBlt has failed", L"Failed", MB_OK);
		goto done;
	}

	// Get the BITMAP from the HBITMAP.
	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = mycx;
	bi.biHeight = mycy;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

	hDIB = GlobalAlloc(GHND, dwBmpSize);
	if (hDIB != 0) {
		lpbitmap = (char*)GlobalLock(hDIB);
	}

	// Gets the "bits" from the bitmap, and copies them into a buffer that's pointed to by lpbitmap.
	GetDIBits(hdcWindow, hbmScreen, 0,
		(UINT)bmpScreen.bmHeight,
		lpbitmap,
		(BITMAPINFO*)&bi, DIB_RGB_COLORS);

	fname.Format(L"FPTM screen %02d%02d%02d-%02d%02d%02d.bmp", tnow.GetMonth(), tnow.GetDay(), tnow.GetYear() - 2000, tnow.GetHour(), tnow.GetMinute(), tnow.GetSecond());

	// A file is created, this is where we will save the screen capture.
	hFile = CreateFile(fname, 
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	// Add the size of the headers to the size of the bitmap to get the total file size.
	dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// Offset to where the actual bitmap bits start.
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	// Size of the file.
	bmfHeader.bfSize = dwSizeofDIB;

	// bfType must always be BM for Bitmaps.
	bmfHeader.bfType = 0x4D42; // BM.

	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	// Unlock and Free the DIB from the heap.
	if (hDIB != 0) {
		GlobalUnlock(hDIB);
		GlobalFree(hDIB);
	}

	// Close the handle for the file that was created.
	CloseHandle(hFile);

	// Clean up.
done:
	if(hbmScreen != 0) DeleteObject(hbmScreen);
	if (hdcMemDC != 0) DeleteObject(hdcMemDC);
	ReleaseDC(NULL, hdcScreen);
	ReleaseDC(hWnd, hdcWindow);

	return 0;
} // end CaptureScreenImage


void 
CFPTMDlg::OnRuntimeoptionsRuntestsnow()
{
	// TODO: Add your command handler code here
	theApp.myFPTM.RunTestsOnDemand();
}


// MAG 260311 add function
// Purpose: Retrieves information about devices in the DLL device list so the info can be displayed in the status window.
void CFPTMDlg::OnConfigurationinformationKnowndevices()
{
	// TODO: Add your command handler code here
	int i = 0;
	int devc = 0;
	int rv = 0;
	char mac[40] = {};
	char ip[40] = {};
	char ipport[40] = {};
	char net[40] = {};
	char router[40] = {};
	char routerip[40] = {};
	char routerport[40] = {};
	CString cs1;
	CString cs2;
	
	// Get device count
	devc = BACdll::GetDeviceInfoSLI(-1, mac, ip, ipport, net, router, routerip, routerport);
	if (devc == 0) {
		m_status.AddString(L"List of network devices the FPTM has information about:");
		m_status.AddString(L"There are no known devices.");
		return;
	}

	cs1.Format(L"List of network devices (%d) the FPTM has information about:",devc);
	m_status.AddString(cs1);

	for (i = 0; i < devc; i++){
		rv = BACdll::GetDeviceInfoSLI(i, mac, ip, ipport, net, router, routerip, routerport);
		if (rv == 1) {  // no errors, print it
			cs1.Format(L"%02d: DevObj: %04d Net: %S ", i, BACdll::GetDeviceListID(i), net);
			if (strlen(ip) > 0) {  // print IP address
				cs2.Format(L"IP: %S", ip);
			} else {
				cs2.Format(L"MAC: %S", mac);
			}
			cs1 += cs2;

			if (strlen(routerip) > 0) {
				cs2.Format(L" RouterIP: %S ", routerip);
				cs1 += cs2;
			}

			m_status.AddString(cs1);
		}

	}
}  // end OnConfigurationinformationKnowndevices


// MAG 260323
// Purpose: Adjust the display to the first event of the selected test step
void 
CFPTMDlg::OnCbnSelchangeStepJumper()
{
	// TODO: Add your control notification handler code here
	int newStep = m_step_jumper.GetCurSel();
	int eventIndex = 0;

	int iTR = theApp.myFPTM.TRactive;
	int iTS = theApp.myFPTM.TSactive;


	if (newStep < 0) {  // this should never happen, but maybe an error condition?
		m_step_jumper.SetCurSel(0);
		return;
	}

	eventIndex = theApp.myFPTM.testSteps[iTR][newStep].GetListMinEvent();  // retrieve the index of the first event in the test step

	DisplayLineStart = eventIndex;

	UpdateEvents();
}

void CFPTMDlg::OnConfigurationinformationExportexpectedvalues()
{
	// TODO: Add your command handler code here
	theApp.myFPTM.SaveExpectedValues();
}

void CFPTMDlg::OnCbnSelchangeAdapterSelect()
{
	// TODO: Add your control notification handler code here
	CString cs1;
	CString cs2;
	int i = m_adapter_select.GetCurSel();

	BACdll::SetDefaultAdapter(i);

	m_adapter_select.GetLBText(i, cs2);
	cs1.Format(L"Change default adapter to #%d:%s", i, cs2);
	m_status.AddString(cs1);
}
