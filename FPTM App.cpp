
// BACnet Test Module.cpp : Defines the class behaviors for the application.
//

// MAG 241028 Changelog for CalNEXT updates
// 241028 Add variable error bounds to FPvar class. 
//		Updated config file read and write. 
//		Updated RunTests()
//		TODO Enable multiple test lists (TRlist)
// 2411XX Added support for multiple sets of test lists (blocks) (XX == over appx 15 days)
// 241120 Add eventTime options
// <eventTime type="offset">xxx</>  xxx is time since start of script
// <eventTime type="interval">xxx</>  xxx is time since start previous event
// Notes and TODOs
// FPTM formulas - break formulas down into multiple steps where each step is a simple subset of the formula that can be easily parsed.

#include "stdafx.h"
#include <direct.h>	// for _getcwd
//#include <strsafe.h> // for error message code related to loading dll MAG NOTE 250416 this was causing errors after upgrade ot VS2022
#include "FPTM App.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// BACnet DLL functions, declare in namespace so it's easier to reference and track use
// copy here for reference
//#ifndef BACOFFLINE
//namespace BACdll{
//	// MAG 260227 add Device functions
//	extern "C" __declspec(dllimport) int ReadPVPropertyByDeviceSLI(char* devid, char* obj, char* inst, char* bv, char* fault);
//	extern "C" __declspec(dllimport) int ReadObjPropertyByDeviceSLI(char* devid, char* cobj, char* cinst, char* cprop, char* cbv, char* cfault);
//	extern "C" __declspec(dllimport) int WritePVPropertyByDeviceSLI(char* devid, char* cobj, char* cinst, char* cbv, char* cfault);
//	extern "C" __declspec(dllimport) int WriteObjPropertyByDeviceSLI(char* devid, char* cobj, char* cinst, char* cprop, char* cbv, char* cfault);
//	extern "C" __declspec(dllimport) int ReadObjPropertyByDevice(int count, int* devid, int* obj, int* inst, int* prop, int* index, char** bv, int* fault);
//	extern "C" __declspec(dllimport) int ReadPVPropertyByDevice(int count, int* devid, int* obj, int* inst, char** bv, int* fault);
//	extern "C" __declspec(dllimport) int GetDeviceListID(int devid);
//	extern "C" __declspec(dllimport) int GetDeviceIndexFromIdentifier(int objectID);
//
//	extern "C" __declspec(dllimport) int __stdcall WPCInit(void);
//	extern "C" __declspec(dllimport) void WPCClose(void); 
//	extern "C" __declspec(dllimport) void sendWhoIsUDP(void);	// for UDPIP
//	extern "C" __declspec(dllimport) int sendWhoIsB(void);		// for ethernet
//	extern "C" __declspec(dllexport) int GetDeviceListCount();
//	extern "C" __declspec(dllexport) int PrintDeviceListInfo(int id, char *mac, char *ip, char *ipport, char *net, char *router, char *routerip, char *routerport);
//	extern "C" __declspec(dllexport) int PrintDeviceListInfo2(int id, char *devid, char* mac, char* ip, char* ipport, char* net, char* router, char* routerip, char* routerport);
//	extern "C" __declspec(dllexport) int readPVProperty(int count, char **mac, int *net, char **router, int *obj, int *inst, char **bv, int *fault);
//	extern "C" __declspec(dllexport) int readPVPropertyIP(int count, char **mac, unsigned char **ip, int *ipport, int *net, char **router, unsigned char **routerip, int *routerport, int *obj, int *inst, char **bv, int *fault);
//	extern "C" __declspec(dllexport) int readObjProperty(int count, char **mac, int *net, char **router, int *obj, int *inst, int *prop, int *index, char **bv, int *fault);
//	extern "C" __declspec(dllexport) int readObjPropertyUDPIP(int count, char **mac, unsigned char **ip, int *ipport, int *net, char **router, unsigned char **routerip, int *routerport, int *obj, int *inst, int *prop, int *index, char **bv, int *fault);
//	extern "C" __declspec(dllexport) int writePVProperty(int count, char **mac, int *net, char **router, int *obj, int *inst, int *prty, char **bv, int *fault);
//	extern "C" __declspec(dllexport) int WriteObjProperty(int count, char **mac, int *net, char **router, int *obj, int *inst, int *prop, int *index, int *prty, char **bv, int *fault);
//	extern "C" __declspec(dllexport) int readObjPropertyIPch(char *mac, char *ip, char *ipport, char *net, char *router, char *routerip, char *routerport, char *obj, char *inst, char *prop, char *bv, char *fault);
//	extern "C" __declspec(dllexport) int writePVPropertyIP(int count, char **mac, unsigned char **ip, int *ipport, int *net, char **router, unsigned char **routerip, int *routerport, int *obj, int *inst, int *prty, char **bv, int *fault);
//	extern "C" __declspec(dllexport) int writeObjPropertyIPch(int count, char **mac, unsigned char **ip, int *port, int *net, char **router, unsigned char **routerip, int *routerport, int *obj, int *inst, int *prop, int *index, int *prty, char **bv, int *fault);
//	extern "C" __declspec(dllexport) int GetAdapterIPSLI(int i, char* label);
//	extern "C" __declspec(dllexport) int GetDefaultAdapter(void);
//}
//#endif	// BACOFFLINE

// CBACnetTestModuleApp construction


CFPTMApp::CFPTMApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	// for debugging
	m_BACdlg = NULL;

	return;
}


CFPTMApp::~CFPTMApp()
{
	
}


// The one and only CBACnetTestModuleApp object
CFPTMApp theApp;


BOOL 
CFPTMApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	setlocale(LC_ALL, "");

	CFPTMDlg dlg;
	m_pMainWnd = &dlg;
	m_BACdlg = &dlg;	// MAG added

#ifdef USEAFDD // MAG NOTE- AFDD code is deprecated, remains as reference
	
	hLinkDLL = LoadLibrary(L"EAFDD-VAV.dll");	// adds ".dll" bydefault
	if (hLinkDLL == NULL){
		char mypath[MAX_PATH];
		dlg.m_status.AddString(L"ERROR- Could not load EAFDD-VAV.DLL, please put this file in the application directory and restart.");
		_getcwd(mypath, MAX_PATH);

		LPVOID lpMsgBuf;
		LPVOID lpDisplayBuf;
		DWORD dw = GetLastError();

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);

		// Display the error message and exit the process
		LPTSTR lpszFunction = L"TEST";
		lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
			(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
		StringCchPrintf((LPTSTR)lpDisplayBuf,
			LocalSize(lpDisplayBuf) / sizeof(TCHAR),
			TEXT("%s failed with error %d: %s"),
			lpszFunction, dw, lpMsgBuf);
		MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

		LocalFree(lpMsgBuf);
		LocalFree(lpDisplayBuf);

	} else {
		try{
				//extern "C" __declspec(dllexport) int AFDD(time_t dataTime, float *input);
				//extern "C" __declspec(dllexport) int InitAFDD(void);
				//extern "C" __declspec(dllexport) int AFDDGetStates(int *vals);
				//extern "C" __declspec(dllexport) int StartTimer(void);
				//extern "C" __declspec(dllimport) int OpenAFDDWindow(void);

				AFDD = (f_IntTimeDouble)GetProcAddress(hLinkDLL, "AFDD");
				InitAFDD = (f_IntVoid)GetProcAddress(hLinkDLL, "InitAFDD");
				AFDDGetStates = (f_IntInt)GetProcAddress(hLinkDLL, "AFDDGetStates");
				StartTimer = (f_IntVoid)GetProcAddress(hLinkDLL, "StartTimer");
				OpenAFDDWindow = (f_IntVoid)GetProcAddress(hLinkDLL, "OpenAFDDWindow");

				if ((AFDD == NULL) || (InitAFDD == NULL) || (AFDDGetStates == NULL) || (StartTimer == NULL) || (OpenAFDDWindow == NULL)){
					dlg.m_status.AddString(L"Error linking to subroutine in DLL.  Please check that correct version is available.");
					return FALSE;
				}

				InitAFDD();
				StartTimer();
				//std::async(InitAFDD);

				// MAG 171205 change to load dynamically (above)
				//AFDDdll::InitAFDD();
				//AFDDdll::StartTimer();

				//AFDDdll::OpenAFDDWindow();
		}
		catch (...){
		}
	}
#endif

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK){
		// TODO: Place code here to handle when the dialog is
		// dismissed with OK
	} else if (nResponse == IDCANCEL){
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	} else if (nResponse == -1){
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL){
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

	return FALSE;
}	// end InitInstance


BEGIN_MESSAGE_MAP(CFPTMApp, CWinApp)
	ON_COMMAND(ID_RUNTIMEOPTIONS_NEWEVENTLIST, &CFPTMApp::OnRuntimeoptionsNewEventList)
	ON_COMMAND(ID_RUNTIMEOPTIONS_DELETEEVENTLIST, &CFPTMApp::OnRuntimeoptionsDeleteEventList)
END_MESSAGE_MAP()


// MAG 241108
// Called from menu item, adds new empty event list
void 
CFPTMApp::OnRuntimeoptionsNewEventList()
{
	// TODO: Add your command handler code here
	myFPTM.OnRuntimeoptionsNewEventList();

	return;
}


void
CFPTMApp::OnRuntimeoptionsDeleteEventList()
{
	// TODO: Add your command handler code here
	myFPTM.OnRuntimeoptionsDeleteEventList();

	return;
}


// MAG 250904 Get Date Time String
// Add to simplify getting date strings
// NOTE can call with no parameter to get current time.
CString
CFPTMApp::GetDTS(CTime ctin)
{
	CString cs;
	CTime tnow;

	if (ctin == NULL) {
		tnow = CTime::GetCurrentTime();
	} else {
		tnow = ctin;
	}
	cs.Format(L"%02d/%02d/%02d %02d:%02d:%02d ", tnow.GetMonth(), tnow.GetDay(), tnow.GetYear() - 2000, tnow.GetHour(), tnow.GetMinute(), tnow.GetSecond());

	return cs;
}


// MAG Character version of DTS
int
CFPTMApp::GetDTSC(char *dtss, CTime ctin, int useComma)  // useComma defaults to 0
{
	CString cs;
	CTime tnow;
	char usec[3] = {}; // comma, space, endl OR space, endl

	if (useComma == 0) {
		usec[0] = ' ';
		usec[1] = 0;
	} else {
		usec[0] = ',';
		usec[1] = ' ';
	}
	// note usec[2] == 0

	if (dtss == NULL) return 0;

	if (ctin == NULL) {
		tnow = CTime::GetCurrentTime();
	} else {
		tnow = ctin;
	}
	
	try {
		sprintf_s(dtss,40,"%02d/%02d/%02d%s %02d:%02d:%02d%s", tnow.GetMonth(), tnow.GetDay(), tnow.GetYear() - 2000, usec, tnow.GetHour(), tnow.GetMinute(), tnow.GetSecond(),usec);
	}
	catch (const std::exception& e) {
		CString err;
		err.Format(L"ERROR in GetDTSC: %S", e.what());
		this->m_BACdlg->m_status.AddString(err);
		return 0;
	}
	catch (...) {
		CString err;
		err.Format(L"ERROR in GetDTSC: Unknown");
		this->m_BACdlg->m_status.AddString(err);
		return 0;
	}

	return 1;
}
