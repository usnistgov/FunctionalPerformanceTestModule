#include "FPTMbase.h"
#include "FPTM App.h"

extern CFPTMApp theApp;

// Constructor
FPTMbase::FPTMbase()
{
	std::vector<TimedRequest>		trInit;
	TimedRequestData				trdInit; // let it go.
	std::vector<RampValue>			rampInit;
	std::vector<DualVarTestList>	tlInit;
	std::vector<DualVarTestList>	dvtlInit;
	std::vector<TestStep>			tsInit;

	TRlist.push_back(trInit);
	TRdata.push_back(trdInit);
	TESTlist.push_back(tlInit);
	rampList.push_back(rampInit);
	eventTimerTestList.push_back(dvtlInit);
	testSteps.push_back(tsInit);

	BACnetList.clear();
	BACnetReadCount = 0;
	checkOOS = 1;
	writePriorityLevel = 7;
	adapterCount = 0;
	defaultAdapter = 0;
	TRactive = 0;
	TSactive = -1;
	evOut = NULL;
	evOutLabel[0] = 0;
	LaunchState = 0;
	TRlistRepeat = 0;
	fault = NULL;
	runtimeVar = 0;
	sparseEvents = 0;
	setScreenCapture = 0;
	memset(sendOutOfService, 0, 20 * sizeof(int));
	currentTestStep = 0;
	activeColumn = 0;
	sleepTimer = 5;

	// from init
	LaunchState = 0;
	writePriorityLevel = WRITE_LEVEL;

	// NOTE- if changing out-of-service defaults, be sure to also modify the default checkboxes in the GUI (MENU1)
	for (int i = 0; i < 20; i++) sendOutOfService[i] = 0;
	sendOutOfService[AI] = 1;
	sendOutOfService[BI] = 1;
	sendOutOfService[MSI] = 1;

	bacnetObjectTypes.push_back("AI");	// 0
	bacnetObjectTypes.push_back("AO");	// 1
	bacnetObjectTypes.push_back("AV");	// 2
	bacnetObjectTypes.push_back("BI");	// 3
	bacnetObjectTypes.push_back("BO");	// 4
	bacnetObjectTypes.push_back("BV");	// 5
	bacnetObjectTypes.push_back("BV");	// 5
	bacnetObjectTypes.push_back("CA");	// 6
	bacnetObjectTypes.push_back("CO");	// 7
	bacnetObjectTypes.push_back("DV");	// 8
	bacnetObjectTypes.push_back("EE");	// 9
	bacnetObjectTypes.push_back("FL");	// 10
	bacnetObjectTypes.push_back("GR");	// 11
	bacnetObjectTypes.push_back("LP");	// 12
	bacnetObjectTypes.push_back("MI");	// 13
	bacnetObjectTypes.push_back("MO");	// 14
	bacnetObjectTypes.push_back("NC");	// 15
	bacnetObjectTypes.push_back("PR");	// 16
	bacnetObjectTypes.push_back("SC");	// 17
	bacnetObjectTypes.push_back("AG");	// 18
	bacnetObjectTypes.push_back("MV");	// 19
	bacnetObjectTypes.push_back("TL");	// 20

// MAG NOTE 260312 can probably remove all EAFDD related code
#ifdef USEAFDD

	hLinkDLL = LoadLibrary(L"EAFDD-VAV.dll");	// adds ".dll" bydefault
	if (hLinkDLL == NULL) {
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
		try {
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

			if ((AFDD == NULL) || (InitAFDD == NULL) || (AFDDGetStates == NULL) || (StartTimer == NULL) || (OpenAFDDWindow == NULL)) {
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
		catch (...) {
		}
	}
#endif

	return;
}  // end class init


FPTMbase::~FPTMbase()
{

}


// MAG 221114
// Add subroutine to explicitely init BACnet adapters
// Returns: Number of adapters, or -1 on error
int
FPTMbase::InitBACnetAdapters()
{
	unsigned char locip[4] = {};
	
	adapterCount = BACdll::WPCInit();
	BACdll::GetAdapterIP(-1, locip);  // get default IP

	// get info for device id, send i-am
	localDeviceID = locip[2] * 1000 + locip[3] + 100;
	BACdll::SendIAM(localDeviceID, 10, 0, 0);

	return adapterCount;
}


// Sets to start state
// Returns: 1 on success, 0 on error
int
FPTMbase::SetStartState()
{
	size_t	i = 0;
	int		tt = 0;
	CString cs1;
	CTime	tnow;

	// check that TRlist is valid
	for (i = 0; i < theApp.myFPTM.TRlist[TRactive].size(); i++) {
		if (TRlist[TRactive][i].CheckValid() == 0) {
			cs1.Format(L"ERROR: The event list element %d is not configured. The test has not started.", i);
			theApp.m_BACdlg->m_status.AddString(cs1);
			return 0;
		}
	}

	InitLogFiles();

	currentTestStep = 0;
	activeColumn = 1;

	// reset testSteps status to not run
	for (i = 0; i < testSteps[TRactive].size(); i++) {
		testSteps[TRactive][i].SetStatus(0);
	}

	LaunchState = 1;

	TSactive = 0;

	return 1;
}


// MAG 250620
// This should just decrement the time in remaining test steps such that the current test step is ended now
//TODO
// Returns: Is void
void
FPTMbase::EndTestStep()
{
	CTime		tnow = CTime::GetCurrentTime();
	CTime		testTime;
	CTimeSpan	cts;
	int			tstep = 0;
	int			mintest = 0;

	mintest = testSteps[TRactive][TSactive].GetListMinEvent();
	testTime = TRlist[TRactive][mintest].GetEventTime();
	cts = testTime - tnow;

	if (cts.GetSeconds() <= 0) {  // test if we're already at or past step time
		return;
	}

	tstep = TSactive;

	// step through following test steps
	for (size_t i = tstep; i < testSteps[TRactive].size(); i++) {
		TRlist[TRactive][i].AdjustEventTime(cts);
	}

	for (size_t i = tstep; i < testSteps[TRactive].size(); i++){
		testSteps[TRactive][i].AdjustTime(cts);
	}
}  // end EndTestStep


// MAG 250130
// Purpose: Inits log files T1, T2, T3
// Tier 1 pass fail at each test step, values not needed, block pass/fail summary (no HVAC values)
// Tier 2 what happened at each step, for ramp etc, any time values are sent.Values sent, values read if changed at this step.Only changed data!
// Tier 3 CSV values, all the data 10 second interval
// Returns: 1 if no erros, 0 if any errors (check status window for error info).
int
FPTMbase::InitLogFiles()			// MAG 250130 add to support Taylor/CalNEXT data/log files
{
	CTime tnow = CTime::GetCurrentTime();
	CString cs1;
	char dts[40] = {};
	int rv = 1; // return value
	char computerName[MAX_COMPUTERNAME_LENGTH + 1] = {};
	DWORD cnsize = sizeof(computerName);

	if (!GetComputerNameA(computerName, &cnsize)) {
		//std::cerr << "GetComputerName failed: " << GetLastError() << "\n";
		strncpy_s(computerName, 8, "UNKNOWN",cnsize);
	}
	if (reportT1.is_open()) return 1;

	// For file name, do not add tab or : here!
	sprintf_s(dts, "%02d%02d%02d-%02d%02d%02d", tnow.GetMonth(), tnow.GetDay(), tnow.GetYear() - 2000, tnow.GetHour(), tnow.GetMinute(), tnow.GetSecond());
	reportdts = dts;

	theApp.m_BACdlg->m_status.AddString(L"Open log files:");

	cs1.Format(L"FPTM-T1-log-%S.txt", dts);
	reportT1.open(cs1);
	theApp.m_BACdlg->m_status.AddString(cs1);

	cs1.Format(L"FPTM-T2-log-%S.txt", dts);
	reportT2.open(cs1);
	theApp.m_BACdlg->m_status.AddString(cs1);

	cs1.Format(L"FPTM-T3-log-%S.csv", dts);
	reportT3.open(cs1);
	theApp.m_BACdlg->m_status.AddString(cs1);

	if (reportT1.good() != 1){ theApp.m_BACdlg->m_status.AddString(L"ERROR: T1 Log file did not open."); rv = 0;}
	if (reportT2.good() != 1){ theApp.m_BACdlg->m_status.AddString(L"ERROR: T2 Log file did not open."); rv = 0;}
	if (reportT3.good() != 1){ theApp.m_BACdlg->m_status.AddString(L"ERROR: T3 Log file did not open."); rv = 0;}
	
	// MAG format this for display
	sprintf_s(dts, "%02d/%02d/%02d %02d:%02d:%02d\t", tnow.GetMonth(), tnow.GetDay(), tnow.GetYear() - 2000, tnow.GetHour(), tnow.GetMinute(), tnow.GetSecond());

	if (reportT1.good()) {
		reportT1 << "FPTM Build " << __DATE__ << " " << __TIME__ << std::endl;
		reportT1 << dts << "Start T1 log file on host " << computerName << "." << std::endl;
		if (FilesRead.size() > 0) {
			reportT1 << dts << "Files entered:" << std::endl;
			for (size_t i = 0; i < FilesRead.size(); i++) {
				reportT1 << dts << FilesRead[i] << std::endl;
			}
		} else {
			reportT1 << dts << "Note: no config files were entered." << std::endl;
		}
		reportT1.flush();
	}

	if (reportT2.good()) {
		reportT2 << dts << "Start T2 log file on host " << computerName << "." << std::endl;
		reportT2 << dts << "Test Line do not read." << std::endl;
	}

	if (reportT3.good()) {
		// MAG 250416 NOTE offset is time since test started
		reportT3 << "Date, Time, Offset, Block, Step, ";
		if (VARlist.size() > 0) {
			for (size_t i = 0; i < VARlist.size() - 1; i++) {
				reportT3 << VARlist[i].GetLabel() << ", ";
			}
			reportT3 << VARlist.back().GetLabel() << std::endl;
		}
	}

	return rv;
}	// end InitLogFiles


// MAG 250130 add to support Taylor/CalNEXT data/log files
void
FPTMbase::CloseLogFiles()			
{
	if (reportT1log.size() > 0) {
		for (const std::string& str : reportT1log) {
			reportT1 << str << std::endl;	// write the log to the file
		}
		reportT1log.clear();
	}

	reportT1.close();
	reportT2.close();
	reportT3.close();
}


// MAG 250912 add to support Taylor/CalNEXT data/log files
void
FPTMbase::FlushLogFiles()			
{
	if (reportT1log.size() > 0) {
		for (const std::string& str : reportT1log) {
			reportT1 << str << std::endl;	// write the log to the file
		}
		reportT1log.clear();
	}

	reportT1.flush();
	reportT2.flush();
	reportT3.flush();
}


// MAG 250912 add to support Taylor/CalNEXT data/log files
void
FPTMbase::SaveLogT3()
{
	CTime		tnow = CTime::GetCurrentTime();
	CTimeSpan	tspan = tnow - modelStartTime;
	char		dts[40] = {};
	//int			sourceCol = -1;
	int			i = 0;
	CString		cs1;
	std::stringstream	sts1;
	std::string			ss1;

	if (!reportT3.good()) {
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Could not save T3 log.");
		return;
	}
	if (TRactive < 0) {
		theApp.m_BACdlg->m_status.AddString(L"ERROR: T3 log save called without active test.");
		return;  // there is no test or data to save- why was this called?
	}
	if (TRactive < 26) {	// MAG 260331 add code to allow high numbered blocks >26
		cs1.Format(L"%c",'A' + (char)TRactive);
	} else { // if more than 26 blocks then use format like AA, AB, AC...ZZ  this is higher than block limit
		cs1.Format(L"%d%c",'A' + (char)(TRactive/26), 'A' + (char)TRactive%26);  
	}
	try {
		// date, time
		i = tnow.GetSecond() % 10;  // seconds to subtract
		tnow -= CTimeSpan(0, 0, 0, i);
		//tnow = tnow - CTimeSpan(tnow.GetSecond() % 10);
		tspan = tnow - modelStartTime;
		theApp.GetDTSC(dts, tnow, 1); // 1 for commaGetDTSC

		// block, step
		reportT3 << dts << tspan.GetTotalSeconds() << ", " << CT2A(cs1) << ", " << activeColumn << ", ";  // MAG note comma included in dts

		if (VARlist.size() > 0) {
			for (size_t i = 0; i < VARlist.size() - 1; i++) {
				reportT3 << VARlist[i].GetValue() << ", ";
			}
			reportT3 << VARlist.back().GetValue() << std::endl; 
		}
	}
	catch (const std::exception& e) {
		// Log the error message
		CString err;
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Caught exception in SaveLogT3!");
		err.Format(L"ERROR: %S", e.what());
		theApp.m_BACdlg->m_status.AddString(err);
	}
	catch(...) {
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Caught exception in SaveLogT3!");
	}
	return;
}


// MAG 250912 add to support Taylor/CalNEXT data/log files
// Gemchecked
void
FPTMbase::SaveExpectedValues()
{
	CWaitCursor		cwait;
	CTime			tnow = CTime::GetCurrentTime();
	CTimeSpan		tspan = tnow - modelStartTime;
	char			blockChar = 'A';
	int				ievsum = 0;
	int				maxloop = 0;
	int				stepInc = 1;
	CString			cs1;
	CString			pathName;
	std::wofstream	ofop;

	// set preferred file name
	if (theApp.myFPTM.reportdts.size() > 1) {
		pathName.Format(L"FPTM-EVENT-VALUE-log-%S.csv", theApp.myFPTM.reportdts.c_str());
	} else {
		pathName.Format(L"FPTM-EVENT-VALUE-log-%02d%02d%02d-%02d%02d%02d.csv", tnow.GetMonth(), tnow.GetDay(), tnow.GetYear() - 2000, tnow.GetHour(), tnow.GetMinute(), tnow.GetSecond());
	}

	try {
		CFileDialog fileDlg(FALSE, L"csv", pathName, OFN_OVERWRITEPROMPT, L"CSV Files (*.csv)|*.csv||");
		if(fileDlg.DoModal() == IDOK){
			ofop.open(fileDlg.GetPathName(), std::ios::out);
		}
		if (!ofop.is_open()) {
			theApp.m_BACdlg->m_status.AddString(L"ERROR could not open file. Expected values were not saved.");
			return;
		}
		
		pathName = fileDlg.GetPathName();

		// write to file below	
		// print headers
		while ((maxloop < (int)TRlist[0].size()) && (TRlist[0][maxloop].GetRW() == 1)) maxloop++;  // count Write events
		while ((maxloop < (int)TRlist[0].size()) && (TRlist[0][maxloop].GetRW() == 0)) maxloop++;  // count Read events

		ofop << "Block/Step, ";
		for (size_t iHead = 0; iHead < (size_t)maxloop; iHead++){  // printing headers only
			ofop << VARlist[iHead].GetLabel().c_str();
			if (iHead < (size_t)maxloop-1) {
				ofop << ", ";
			}
		}
		ofop << std::endl;

		// now print expected values
		for (size_t iBlock = 0; iBlock < TRlist.size(); iBlock++){
			blockChar = 'A' + (char)iBlock; // init label
			ofop << blockChar << stepInc << ", ";  // print label for new block
			for (size_t iStep = 0; iStep < TRlist[iBlock].size(); iStep++) {
				ofop << TRlist[iBlock][iStep].GetValue();
				if (ievsum >= maxloop-1) {
					ievsum = 0;
					stepInc++;
					if (iStep < TRlist[iBlock].size() - 1) {  // will not start new block print label
						ofop << std::endl;  // return for next line (printed below for last line in block
						ofop << blockChar << stepInc << ", ";  // print label for next line
					}
				} else {
					ofop << ", ";
					ievsum++;
				}
			}
			ofop << std::endl;
			ievsum = 0;
			stepInc = 1;
		}

		// close file here
		ofop.close();

		// print success message
		cs1.Format(L"Expected Values saved to file: '%s'",(LPCTSTR) pathName);
		theApp.m_BACdlg->m_status.AddString(cs1);
		theApp.m_BACdlg->m_status.AddString(L"Note that current values are used for calculations, runtime changes can not be accounted for.");
	}
	catch (const std::exception& e) {
		// Log the error message
		CString err;
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Caught exception in SaveExpectedValues!");
		err.Format(L"ERROR: %S", e.what());
		theApp.m_BACdlg->m_status.AddString(err);
	}
	return;
}


// MAG add function
// Purpose: Fills out BACnetList by sending out who-is requests and parsing responses
// Returns: 1 on success, 0 on error or no devices found
int
FPTMbase::AddBACnetDevices()
{
#ifdef BACOFFLINE
	return  1;
#else
	char macch[20] = {};
	char ipch[20] = {};
	char ipportch[20] = {};
	char netch[20] = {};
	char routerch[20] = {};
	char routeripch[20] = {};
	char routerportch[20] = {};
	char devidch[20] = {};
	int i = 0;
	int status = 0;
	CString cs1;

	if (theApp.m_BACdlg != NULL) theApp.m_BACdlg->m_status.AddString(L"Sending who-is ETH");
	i = BACdll::sendWhoIsB();

	if ((i == -1) && (theApp.m_BACdlg != NULL)) {
		theApp.m_BACdlg->m_status.AddString(L"There was an error sending the who-is over Ethernet.");
	}

	if (theApp.m_BACdlg != NULL) theApp.m_BACdlg->m_status.AddString(L"Sending who-is UDP");
	BACdll::sendWhoIsUDP();
	Sleep(50);

	BACnetReadCount = BACdll::GetDeviceListCount();

	if (BACnetReadCount == 0) {
		if (theApp.m_BACdlg != NULL) {  // put this check in to eliminate warning about m_BACdlg being NULL.
			theApp.m_BACdlg->m_status.AddString(L"ERROR: Network scan found no devices.");
		}
		return 0;
	}

	cs1.Format(L"Find %d devices", BACnetReadCount);
	if (theApp.m_BACdlg != NULL) theApp.m_BACdlg->m_status.AddString(cs1);

	BACnetList.resize(BACnetReadCount);

	// initialize BACnetList and TRlist
	for (i = 0; i < BACnetReadCount; i++) {
		status = BACdll::PrintDeviceListInfo2(i, devidch, macch, ipch, ipportch, netch, routerch, routeripch, routerportch);
		if (status != 1) break;

		BACnetList[i].SetMAC(macch);
		BACnetList[i].SetIP(ipch);
		BACnetList[i].SetIPPort(ipportch);
		BACnetList[i].SetNet(netch);
		BACnetList[i].SetDevObjInst(devidch);
		if ((strlen(routerch) > 0) && (strncmp(routerch, "000000", 6) != 0)) {// has a valid router
			//if (theApp.op != NULL){ fprintf(theApp.op, "      Set Router MAC\n"); fflush(theApp.op);}
			BACnetList[i].SetRouterMAC(routerch);
			//if (theApp.op != NULL){ fprintf(theApp.op, "      Set Router IP\n"); fflush(theApp.op);}
			BACnetList[i].SetRouterIP(routeripch);
			//if (theApp.op != NULL){ fprintf(theApp.op, "      Set Router Port\n"); fflush(theApp.op);}
			BACnetList[i].SetRouterPort(routerportch);
			// MAG 260312 NOTE config id is deprecated when using ObjectID for read/write
			// set configID
			if (strcmp(routeripch, "0.0.0.0") == 0) {  // No IP on router, dev is MAC only too
				BACnetList[i].SetConfigID(2);
			} else {
				if (strlen(ipch) > 0) BACnetList[i].SetConfigID(0);  // dev and rtr have IP address
				else BACnetList[i].SetConfigID(1);					// dev has MAC, rtr has IP
			}
		} else {  // configID is 3 or 4
			if (strcmp(ipch, "0.0.0.0") != 0) BACnetList[i].SetConfigID(3);
			else BACnetList[i].SetConfigID(4);
		}
		// configID- 0:IP/rIP, 1: MAC/rIP, 2: MAC/rMAC, 3: IP only, 4: MAC only
	}

	return(1);
#endif // BACOFFLINE
} // end FPTMbase::AddBACnetDevices


 // Returns: 0 for error, or 1 for success with IP in parameter iptext
int
FPTMbase::GetNetworkAdapterIP(int id, char* iptext)
{
	if (id < 0) return 0;
	if (id >= adapterCount) return 0;
	if (iptext == NULL) return 0;

	return BACdll::GetAdapterIPSLI(id, iptext);
}


// called from the Net Scan button
void
FPTMbase::UpdateBACnetDevices()
{
#ifdef BACOFFLINE
	return 1;
#else
	char devidch[20] = {};
	char macch[20] = {};
	char ipch[20] = {};
	char ipportch[20] = {};
	char netch[20] = {};
	char routerch[20] = {};
	char routeripch[20] = {};
	char routerportch[20] = {};

	int i = 0;
	int nc = 0;		// new count
	int oc = 0;		// old count 
	CString cs1;
	int status = 0;
	BACdev lbd;

	if (theApp.m_BACdlg != NULL) theApp.m_BACdlg->m_status.AddString(L"Sending who-is UDP");
	BACdll::sendWhoIsUDP();

	if (theApp.m_BACdlg != NULL) theApp.m_BACdlg->m_status.AddString(L"Sending who-is ETH");
	i = BACdll::sendWhoIsB();
	if ((i == -1) && (theApp.m_BACdlg != NULL)) {
		theApp.m_BACdlg->m_status.AddString(L"There was an error sending the who-is over Ethernet.");
	}

	nc = BACdll::GetDeviceListCount();
	if (BACnetReadCount == nc) return ;

	BACnetReadCount = nc;  // need to update so we don't loop through this every time

	cs1.Format(L"Find %d devices", BACnetReadCount);
	if (theApp.m_BACdlg != NULL) theApp.m_BACdlg->m_status.AddString(cs1);

	// initialize BACnetList and TRlist
	for (i = 0; i < nc; i++) {  // was from oc to nc, but need to check for repeats between scanned lists and read lists
		//if (theApp.op != NULL){ fprintf(theApp.op, "   Loop iter %d/%d step 1\n",i,BACnetListCount); fflush(theApp.op);}
		status = BACdll::PrintDeviceListInfo2(i, devidch, macch, ipch, ipportch, netch, routerch, routeripch, routerportch);
		if (status != 1) break;
		//cs1.Format(L"%dc: %x:%x:%x:%x:%x:%x", i, umac[0][0], umac[0][1], umac[0][2], umac[0][3], umac[0][4], umac[0][5] );

		lbd.SetMAC(macch);
		lbd.SetIP(ipch);
		lbd.SetIPPort(ipportch);
		lbd.SetNet(netch);
		lbd.SetDevObjInst(devidch);

		if ((strlen(routerch) > 0) && (strncmp(routerch, "000000", 6) != 0)) {// has a valid router
			lbd.SetRouterMAC(routerch);
			lbd.SetRouterIP(routeripch);
			lbd.SetRouterPort(routerportch);
			if (strcmp(routeripch, "0.0.0.0") == 0) {  // No IP on router, dev is MAC only too
				lbd.SetConfigID(2);
			} else {
				if (strlen(ipch) > 0) lbd.SetConfigID(0);  // dev and rtr have IP address
				else lbd.SetConfigID(1);					// dev has MAC, rtr has IP
			}
		} else {  // configID is 3 or 4
			if (strcmp(ipch, "0.0.0.0") != 0) lbd.SetConfigID(3);
			else lbd.SetConfigID(4);
		}

		status = 0;  // assume not repeated
		for (oc = 0; oc < (int)BACnetList.size(); oc++) {
			if (BACnetList[oc].Compare(&lbd) == 1) {  // they're the same!
				status = 1;
				break;
			}
		}

		if (status == 0) {
			if (theApp.m_BACdlg != nullptr) {
				theApp.m_BACdlg->m_status.AddString(L"Found a new BACnet device on network- adding to list.");
				cs1.Format(L"There are already %d devices.", BACnetList.size());
				theApp.m_BACdlg->m_status.AddString(cs1);
			}
			lbd.SetIndex(BACnetList.size());
			BACnetList.push_back(lbd);
		}
	}

	return;
#endif // BACOFFLINE
} // end FPTMbase::UpdateBACnetDevices


void
FPTMbase::InitIPH(void)
{
	// MAG 160711 set up iph
	iph.mac = (char**)malloc(sizeof(char*) * IPHMAX);
	iph.bv = (char**)malloc(sizeof(char*) * IPHMAX);
	iph.router = (char**)malloc(sizeof(char*) * IPHMAX);
	iph.ip = (unsigned char**)malloc(sizeof(unsigned char*) * IPHMAX);
	iph.routerip = (unsigned char**)malloc(sizeof(unsigned char*) * IPHMAX);
	iph.index = (int*)malloc(sizeof(int) * IPHMAX);
	iph.inst = (int*)malloc(sizeof(int) * IPHMAX);
	iph.ipport = (int*)malloc(sizeof(int) * IPHMAX);
	iph.routerport = (int*)malloc(sizeof(int) * IPHMAX);
	iph.net = (int*)malloc(sizeof(int) * IPHMAX);
	iph.obj = (int*)malloc(sizeof(int) * IPHMAX);
	iph.prop = (int*)malloc(sizeof(int) * IPHMAX);
	iph.prty = (int*)malloc(sizeof(int) * IPHMAX);
	iph.devObjInst = (int*)malloc(sizeof(int) * IPHMAX);

	memset(iph.index, 0, sizeof(int) * IPHMAX);
	memset(iph.inst, 0, sizeof(int) * IPHMAX);
	memset(iph.ipport, 0, sizeof(int) * IPHMAX);
	memset(iph.routerport, 0, sizeof(int) * IPHMAX);
	memset(iph.net, 0, sizeof(int) * IPHMAX);
	memset(iph.obj, 0, sizeof(int) * IPHMAX);
	memset(iph.prop , 0, sizeof(int) * IPHMAX);
	memset(iph.prty , 0, sizeof(int) * IPHMAX);
	memset(iph.devObjInst, 0, sizeof(int) * IPHMAX);

	for (int i = 0; i < IPHMAX; i++) {
		iph.router[i]	= (char*)malloc(IPHLEN);
		iph.bv[i]		= (char*)malloc(IPHLEN);
		iph.mac[i]		= (char*)malloc(IPHLEN);
		iph.ip[i]		= (unsigned char*)malloc(4);
		iph.routerip[i] = (unsigned char*)malloc(4);

		if(iph.router[i] != 0)	memset(iph.router[i], 0, IPHLEN * sizeof(char));
		if(iph.bv[i] != 0)		memset(iph.bv[i]	, 0, IPHLEN * sizeof(char));
		if(iph.mac[i] != 0)		memset(iph.mac[i]	, 0, IPHLEN * sizeof(char));
		if(iph.ip[i] != 0)		memset(iph.ip[i]	, 0, 4 * sizeof(unsigned char));
		if(iph.routerip[i] != 0) memset(iph.routerip[i], 0, 4 * sizeof(unsigned char));
	}
	fault = (int*)malloc(sizeof(int) * IPHMAX);

	return;
}


// Process requested event (# rid)
// Returns: 1 on success, 0 on error
int
FPTMbase::RunRequest(int rid)
{
	BACdev* mybd = NULL;
	int		i = 0;
	int		idev = 0;
	int		devObjInst = 0;
	int		failedReads = 0;
	int		ivar = -1;
	int		resetVal = -1;
	int		rwReturn = 0;	// holds the return value of read/write requests, to be used in results printout
	double	nval = 0;
	CString status;
	CString cs1;
	CString cs2;
	CString csp(" ");
	CString errRep;  // for error reports
	CStringA csa;
	CTime 	tnow = CTime::GetCurrentTime();  // this is used for multiple events
	char	dts[40] = {};

#ifdef BACOFFLINE
	return 1;
#endif
	if ((TRactive < 0)||(TRlist[TRactive].size() <= 0)) {
		theApp.m_BACdlg->m_status.AddString(L"ERROR: RunRequest called without active test.");
		return 0;
	}
	theApp.GetDTSC(dts, tnow);
	strcat_s(dts,40, "\t");

	if (reportT2.good()) reportT2 << dts << "Run Event " << rid << std::endl;

	// Check range on rid
	if (rid >= (int)TRlist[TRactive].size()) return 0;
	if (rid < 0) return 0;
	idev = TRlist[TRactive][rid].GetDevice();
	if (idev < 0) return 0;	// device not set
	if (idev >= (int)BACnetList.size()) return 0;
	
	cs1.Format(L"Run request %d, idev %d", rid, idev);
	theApp.m_BACdlg->m_status.AddString(cs1);

	// MAG 260227 using object instance to run requests now
	if (iph.devObjInst == NULL) return 0;
	iph.devObjInst[0] = BACnetList[idev].GetDevObjInst();

	mybd = &BACnetList[idev];
	iph.obj[0] = TRlist[TRactive][rid].GetBACnetObject();
	iph.inst[0] = TRlist[TRactive][rid].GetInstance();
	iph.prty[0] = writePriorityLevel;	// NOTE- priority in RunReset must match this value
	fault[0] = 0;

	if (sleepTimer < 5) sleepTimer = 5;  //error check to keep in range
	if (sleepTimer > 50) sleepTimer = 50;  //error check to keep in range

	// Check if it's read or write
	switch (TRlist[TRactive][rid].GetRW()) {
		case 0:   // READING VALUE
			status.Format(L"%S Reading value %02d:%02d from device ObjID %d", dts,iph.obj[0], iph.inst[0], iph.devObjInst[0]);
			if (reportT2.good()) {
				reportT2 << CT2A(status) << std::endl;
			}
			// MAG 260227 object id based read, configId is irrelevant
#ifndef BACOFFLINE
			rwReturn = BACdll::ReadPVPropertyByDevice(1, iph.devObjInst, iph.obj, iph.inst, iph.bv, fault);
#else
			rwReturn = 1;  // fake it 'till you... can send it?
#endif
			// return value: 1=no errors, 0=some error
			if ((rwReturn == 0) || (strncmp(iph.bv[0], "ERR:", 4) == 0)) {// looking for "ERR: Timeout on request" or similar
				errRep.Format(L"%S FAIL Read value %S:%02d from device ObjID %d",dts, bacnetObjectTypes[iph.obj[0]].c_str(), iph.inst[0], iph.devObjInst[0]);
				theApp.m_BACdlg->m_status.AddString(errRep);
				failedReads++;

				Sleep(sleepTimer);	// take a short break and send it again.

#ifndef BACOFFLINE
				rwReturn = BACdll::ReadPVPropertyByDevice(1,iph.devObjInst, iph.obj, iph.inst, iph.bv, fault);
#else
				rwReturn = 1; // when in debug mode
#endif
				if (rwReturn == 0) {// still screwed up =[
					TRlist[TRactive][rid].IncStale();
					errRep.Format(L"%S FAIL2 Reading value %02d:%02d from device ObjID %d", dts, iph.obj[0], iph.inst[0], iph.devObjInst[0]);
					theApp.m_BACdlg->m_status.AddString(errRep);
					status.Format(L"%S EVENT %d: Timeout on request (s=%d).", dts, rid, TRlist[TRactive][rid].GetStale());
					theApp.m_BACdlg->m_status.AddString(status);
					if (sleepTimer < 50) sleepTimer++;

					return 0;
				}
			}  // end ERR on read loop
	//#ifndef _DEBUG
			nval = atof(iph.bv[0]);		// NOTE NOT in debug mode
			TRlist[TRactive][rid].SetValue(nval);
			cs1.Format(L" Value: %5.2f", nval);
			status += cs1;
	//#else 
	//		nval = TRlist[TRactive][rid].GetValue();	// when in debug mode
	//		// MAG NOTE TRlist value does not need to be written back
	//#endif

			i = TRlist[TRactive][rid].GetFPV();
			if ((i >= 0) && ((int)VARlist.size() > i)) VARlist[i].SetValue(nval);

			theApp.m_BACdlg->UpdateEvents();

			theApp.m_BACdlg->m_status.AddString(status);
			if((failedReads == 0)&&(sleepTimer > 5)) sleepTimer--;

			break;
		case 1: // WRITING VALUE 
			// MAG 170327 add out of service check, called immediately before writes
			// NOTE- may have to have this run sooner if timing is an issue, could have request run 5s earlier
			// check out of service
			status.Format(L"%S Writing value %f (%02d:%02d) from device ObjID %d", dts,TRlist[TRactive][rid].GetValue(), iph.obj[0], iph.inst[0], iph.devObjInst[0]);
			theApp.m_BACdlg->m_status.AddString(status);
			if (reportT2.good()) {
				reportT2 << CT2A(status) << std::endl;
			}
			if (TRlist[TRactive][rid].GetInitOOS() == -1) {
				iph.prop[0] = 81;  // out of service  
				iph.index[0] = -1;	// no index

				iph.prty[0] = writePriorityLevel;	// NOTE- priority in RunReset must match this value

				nval = TRlist[TRactive][rid].GetValue();
				iph.bv[0][0] = '1';
				iph.bv[0][1] = 0;
				
				rwReturn = 1;  //MAG set default to success, useful when BACOFFLINE

#ifndef BACOFFLINE
				if (sendOutOfService[iph.obj[0]] == 1) { // MAG NOTE outofservice is reset in RunReset
					rwReturn = BACdll::WriteObjPropertyByDevice(1, iph.devObjInst, iph.obj, iph.inst, iph.prop, iph.index, iph.prty, iph.bv, fault);
				}
				nval = TRlist[TRactive][rid].GetValue();
				if (nval == -999.0) {
					cs1.Format(L"%S ERROR caught writing value -999! DevObjId %d Obj %d Inst %d", dts,iph.devObjInst[0], iph.obj[0],iph.inst[0]);
					theApp.m_BACdlg->m_status.AddString(cs1);
				}
				csa.Format("%f", nval);	// move formating etc from char/sprintf to CString
				if (csa.GetLength() > 20) cs1.SetAt(19, 0);  // make sure it's not too long for bv
				strcpy_s(iph.bv[0], 20, (LPCSTR)csa);
				//sprintf_s(iph.bv[0], 20, "%f", nval); remove line after verifying csa method is working
				iph.prop[0] = 85; // present value

				rwReturn = BACdll::WritePVPropertyByDevice(1, iph.devObjInst, iph.obj, iph.inst, iph.bv, fault);
#endif
				// check if value needs to be saved
				ivar = TRlist[TRactive][rid].GetFPV();
				if ((TRlist[TRactive][rid].GetRampID() >= 0) && (ivar >= 0)) {
					VARlist[ivar].SetValue(nval); // updated stored value when changed by RAMP 
				} else {	// check for useValue==1, save value to VAR (i.e. writing normal value)
					if (TRlist[TRactive][rid].GetUseValue() == 1) {
						if (ivar >= 0) {
							VARlist[ivar].SetValue(nval);
						}
					}
				}
			}  // end else WRITING VALUE

			nval = atof(iph.bv[0]);
			TRlist[TRactive][rid].SetSentValue(nval);

			// MAG TODO fix this and uncomment!
			//if (rwReturn != 1) {
			//	status.Format(L"%S EVENT %d: Write Value ERROR: %d.", dts, rid, fault[0]);
			//} else {
			//	status.Format(L"%S EVENT %d: Write Value: ", dts, rid);
			//	status += iph.bv[0];
			//}
			theApp.m_BACdlg->m_status.AddString(status);

			break;
		case 2: // RESET VALUE
			resetVal = RunReset(rid);
			if (resetVal == 1) {
				cs1.Format(L"EVENT %d: Reset has been sent.", rid);
			} else {
				cs1.Format(L"EVENT %d: ERR on reset.", rid);
			}
			theApp.m_BACdlg->m_status.AddString(cs1);

			break;
		default: // uh oh...
			cs1.Format(L"ERROR: Found unknown request type %d (not read/write/reset).", TRlist[TRactive][rid].GetRW());
			theApp.m_BACdlg->m_status.AddString(cs1);
			return 0;
	} // end switch (TRlist[TRactive][rid].GetRW())

	if (evOut != NULL) {
#ifdef BACOFFLINE
		fprintf(evOut, "NOTE FPTM RUN IN BACOFFLINE MODE NO PACKETS WERE SENT OR RECEIVED!\n");
#endif
		cs1 = TRlist[TRactive][rid].GetEventTime().Format("%c");
		fprintf(evOut, "EVENT REPORT: Event ID %d | Time %ls | Send OutOfService %d | Write Priority Level %d\n", rid, cs1.GetBuffer(), sendOutOfService[TRlist[TRactive][rid].GetBACnetObject()], writePriorityLevel);
		switch (TRlist[TRactive][rid].GetRW()) {
			case 0:
				fprintf(evOut, "Read value: %s\n", iph.bv[0]);
				fprintf(evOut, "Read status: %d\n", rwReturn);
				break;
			case 1:
				fprintf(evOut, "Write value: %s\n", iph.bv[0]);
				fprintf(evOut, "Write status: %d\n", rwReturn);
				break;
			case 2:
				fprintf(evOut, "Reset event return value: %d", resetVal);
				break;
		}
		fprintf(evOut, "END EVENT REPORT\n* * * * * * *\n");
		fflush(evOut);
	}

#ifdef USEAFDD
	if (rid == TRlist[TRactive].size() - 1) {
		SendDataAFDD();	// MAG send data before resetting time
		if (TRlistRepeat > 0)
			ResetDelay(TRlistRepeat);
	}
#endif

	theApp.m_BACdlg->m_status.SetCurSel(theApp.m_BACdlg->m_status.GetCount() - 1);

	return 1;
}  // end RunRequest


// MAG 210607 prints values of all variables to status
void
FPTMbase::PrintVariables()
{
	CString cs1;
	char vlabel[100] = {};

	theApp.m_BACdlg->m_status.AddString(L"(Variable name) : (value)");
	for (std::vector<FPvar>::iterator i = VARlist.begin(); i != VARlist.end(); i++){
		i->GetLabel(vlabel);
		cs1.Format(L"%S: %5.2f", vlabel, i->GetValue());
		theApp.m_BACdlg->m_status.AddString(cs1);
	}

	return;
} // end PrintVariables


void
FPTMbase::RunTestsOnDemand()
{
	CTime tnow = CTime::GetCurrentTime();
	char dts[40] = {};
	std::stringstream st1;

	theApp.GetDTSC(dts, tnow);
	strcat_s(dts, 40, "\t");

	st1 << dts << "NOTE Tests run on demand. Results are not definitive." << std::endl;
	reportT1log.push_back(st1.str());
	RunTests();

	reportT1 << dts << "NOTE End tests run on demand." << std::endl;
	st1 << dts << "NOTE End tests run on demand." << std::endl;
	reportT1log.push_back(st1.str());

	return;
}


// MAG this is called to run all remaining (might be all) tests when final event timer has passed
void
FPTMbase::RunTests()
{
	int			passCount = 0;
	int			failCount = 0;
	int			manualCount = 0;
	int			testStatus = -1; // 0=fail 1=pass -1=not tested
	int			i = 0;
	int			tc = 1;	// test count, print value (1-indexed)
	int			ltr = -1; // local test result
	int			tstep = 1; // test step currently being analyzed, corresponding to a column
	int			testid = 1; // test id, number of test in current test step, corresponding to row of column
	int			prevTriggerEvent = 0;
	double		errB = 0.0; // error bound from variable
	char		dts[80] = {};
	char		dtsTest[80] = {};
	char		testBlockChar = 'A';
	CString		cs1;
	CString		cs2;
	CStringA	csa;
	CTime		tnow = CTime::GetCurrentTime();
	CTime		ttest;
	CTimeSpan	startSpan;	// time since test started
	std::vector<std::string>	eventReport;
	std::vector<std::string>	testType = { "LT","LTE", "EQ", "GTE", "GT","NE", "MA", "ANY"};
	std::stringstream			stst;

	theApp.GetDTSC(dts, tnow);
	strcat_s(dts, 40, "\t");
	strcpy_s(dtsTest, 40, dts);

	try {
		testBlockChar += TRactive; // will remain as 'A' or increment to 'B' etc. as appropriate
		theApp.m_BACdlg->m_status.AddString(L"Block Summary of Evaluation Results:");
		cs1.Format(L"%S Begin Test Block %c Summary Report. Block Description: \"%s\" Block \"%s\" containing %d tests.",
			dts, testBlockChar, TRdata[TRactive].GetLabel(), TRdata[TRactive].GetTestIdentifier(), TESTlist[TRactive].size());
		eventReport.push_back(std::string(CT2A(cs1)));
		stst << dts << "Note: RV = Read Value from controller : EV = Expected Value from test setup";
		eventReport.push_back(stst.str());

		if (TESTlist[TRactive].size() == 0) {
			theApp.m_BACdlg->m_status.AddString(L"There are NO tests entered.");
			eventReport.push_back("There are NO tests entered.");
			// run events instead (hack? for running resets)
			if (TRlist[TRactive].size() == 0) return;
			theApp.m_BACdlg->m_status.AddString(L"Sending test list...");
			for (i = 0; i < (int)TRlist[TRactive].size(); i++) {
				RunRequest(i);
			}
			return;
		}
		else {
			cs1.Format(L"There are %d tests entered.", TESTlist[TRactive].size());
			theApp.m_BACdlg->m_status.AddString(cs1);
			//cs1.Format(L"%S: There are %d tests entered.", dts, TESTlist[TRactive].size());
			//eventReport.push_back(std::string(CT2A(cs1)));
		}

		prevTriggerEvent = TESTlist[TRactive][0].GetTriggerEvent();
		for (size_t ii = 0; ii < TESTlist[TRactive].size(); ii++) {
			RunTest(ii);
			ltr = TESTlist[TRactive][ii].GetResult();
			if (ltr == 1) passCount++;
			if (ltr == 0) failCount++;
			// MAG 250306 implement reduced T1 report size- 1-2 lines?

			ttest = TESTlist[TRactive][ii].GetRunTime();
			startSpan = ttest - TESTlist[TRactive][0].GetRunTime(); // MAG 250415 save time in seconds between this evaluation and test start

			if (ttest > 0) {
				csa.Format("%02d/%02d/%02d\t%02d:%02d:%02d\t%03d", ttest.GetMonth(), ttest.GetDay(), ttest.GetYear() - 2000, ttest.GetHour(), ttest.GetMinute(), ttest.GetSecond(), (int)startSpan.GetTotalSeconds());
				strcpy_s(dtsTest, 80, (LPCSTR)csa);
				//sprintf_s(dtsTest, "%02d/%02d/%02d\t%02d:%02d:%02d\t%03d", ttest.GetMonth(), ttest.GetDay(), ttest.GetYear() - 2000, ttest.GetHour(), ttest.GetMinute(), ttest.GetSecond(),(int)startSpan.GetTotalSeconds());
			}

			if (prevTriggerEvent != TESTlist[TRactive][ii].GetTriggerEvent()) { // moving to next test step
				testid = 1;
				tstep++;
				prevTriggerEvent = TESTlist[TRactive][ii].GetTriggerEvent();
			}
			cs1.Format(L"%S\t%S-%02d-%02d\t%s\t%s\tTest:\t(RV)\t%5.2f\t%S\t%5.2f\t(EV)",
				dtsTest, TRdata[TRactive].GetTestIdentifier().GetString(), tstep, testid, (ltr == 1) ? L"PASS :" : L"FAIL *",
				TESTlist[TRactive][ii].Label.GetString(), TESTlist[TRactive][ii].GetEvalV1(), testType[TESTlist[TRactive][ii].GetRelate() + 2].c_str(), TESTlist[TRactive][ii].GetEvalV2());
			theApp.m_BACdlg->m_status.AddString(cs1);

			eventReport.push_back(std::string(CT2A(cs1)));
			//eventReport.push_back("* * * * * * *");

			testid++;
		}
		
	}
	catch (const std::exception& e) {
		std::stringstream ss1;
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Caught exception in RunTests!");
		ss1 << "ERROR: " << e.what();
		cs1 = ss1.str().c_str();
		theApp.m_BACdlg->m_status.AddString(cs1);
	}
	catch (...) {
		std::stringstream ss1;
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Caught unknown exception in RunTests!");
	}

	cs1.Format(L"Test block %c PASS: %d/%d (%5.2f %%).", testBlockChar, passCount, TESTlist[TRactive].size(),100.0*passCount/TESTlist[TRactive].size());
	theApp.m_BACdlg->m_status.AddString(cs1);
	eventReport.push_back(std::string(CT2A(cs1)));

	cs1.Format(L"Test block %c FAIL: %d/%d (%5.2f %%).", testBlockChar, failCount, TESTlist[TRactive].size(), 100.0 * failCount / TESTlist[TRactive].size());
	theApp.m_BACdlg->m_status.AddString(cs1);
	eventReport.push_back(std::string(CT2A(cs1)));

	cs1.Format(L"Test block %c PASS %d FAIL: %d.", testBlockChar, passCount, failCount);
	theApp.m_BACdlg->m_status.AddString(cs1);
	reportT1log.push_back(std::string(CT2A(cs1)));

	if (failCount == 0) {
		theApp.m_BACdlg->m_status.AddString(L"The test block has PASSED.");
		eventReport.push_back(std::string("The test block has PASSED."));
	} else {
		theApp.m_BACdlg->m_status.AddString(L"The test block has FAILED.");
		eventReport.push_back(std::string("The test block has FAILED."));
	}

	if (manualCount > 0) {
		cs1.Format(L"NOTE- there are %d manual test(s) to review.", manualCount);
		theApp.m_BACdlg->m_status.AddString(cs1);
		eventReport.push_back(std::string(CT2A(cs1)));
	} else {
		cs1.Format(L"NOTE- there are no manual test(s) to review.");
		theApp.m_BACdlg->m_status.AddString(cs1);
		eventReport.push_back(std::string(CT2A(cs1)));
	}

	if (evOut != NULL) {
		for (tc = 0; tc < (int)eventReport.size(); tc++) {
			fprintf(evOut, "%s\n", eventReport[tc].c_str());
		}
		fprintf(evOut, "END TEST REPORT\n* * * * * * *\n");
		fflush(evOut);
	}

	for (tc = 0; tc < (int)eventReport.size(); tc++) {
		reportT1log.push_back(eventReport[tc]);
	}

	reportT1log.push_back("END TEST REPORT");

	return;
} // end RunTests


// MAG 221110 shamelessly copied from RunTests, modified to just run one
void
FPTMbase::RunTest(int tid)
{
	CString		cs1;
	CString		cs2;
	CString		lvLabel;
	CString		rvLabel;
	std::stringstream stst;
	std::string ss1;
	CTime		tnow = CTime::GetCurrentTime();
	CTimeSpan	tspan;
	TestList*	it = NULL;
	int			ltr = -1; // local test result
	int			dbgval = 0;
	int			itval = 0;
	int			i = 0;
	int			testID = 0;
	int			sCol = 0;	// source column for test
	int			sRow = 0;	// source row for test
	double		lval = 0;	// Value held in VARlist for current row of test script.
	double		rval = 0;	// Value in cell of test script, or value of referenced VAR, or value of math function in cell
	char		dts[40] = {};
	char		TRchar = 'A' + TRactive;
	double		tol = 0.5; // tolerance value
	std::vector<std::string>  testType = { "LT","LTE", "EQ", "GTE", "GT","NE", "MA", "ANY", "UNKOWN"};

	sprintf_s(dts, "%02d/%02d/%02d-%02d:%02d:%02d\t", tnow.GetMonth(), tnow.GetDay(), tnow.GetYear() - 2000, tnow.GetHour(), tnow.GetMinute(), tnow.GetSecond());

	// No such test
	if ((tid < 0) || (tid >= (int)TESTlist[TRactive].size())) {
		cs1.Format(L"ERROR: Request to evaluate test (%d): out of range (0-%d)", tid, TESTlist[TRactive].size());
		theApp.m_BACdlg->m_status.AddString(cs1);
		ss1 = CT2CA(cs1);
		reportT1log.push_back(ss1);

		return;
	}

	sRow = TESTlist[TRactive][tid].GetSourceRow();
	sCol = TESTlist[TRactive][tid].GetSourceColumn();

	if (reportT1.good()) {
		stst << dts << "Evaluate Test # " << TRchar << ":" << sCol << ":" << sRow << " "; // add stst to reportT1log at end
	}

	if (reportT2.good()) { // this line identical to T1 entry
		reportT2 << dts << "Evaluate Test # " << TRchar << ":" << sCol << ":" << sRow << " "; // add stst to reportT1log at end
	}

	// Put label on output notes
	if (TESTlist[TRactive][tid].Label.GetLength() > 0) {
		cs1.Format(L"%S Test %c:%d:%d %s", dts, TRchar, sCol, sRow, TESTlist[TRactive][tid].Label.GetString());
		TESTlist[TRactive][tid].testNotes.push_back(std::string(CT2A(cs1)));
		if (reportT2.good()) {
			reportT2 << CT2A(cs1) << std::endl;
		}
	} else {
		cs1.Format(L"%S Test %c:%d:%d No Label", dts, TRchar, sCol, sRow);
		TESTlist[TRactive][tid].testNotes.push_back(std::string(CT2A(cs1)));
		if (reportT2.good()) {
			reportT2 << CT2A(cs1) << std::endl;
		}
	}

	// it's a shortcut to the test
	it = &TESTlist[TRactive][tid];

	// MAG might need more feedback on previous pass/fail
	i = it->GetResult();
	// tests that have not run return i == -1 here
	if (i == 1) {
		cs1.Format(L"Test previously reported as PASSED.");
		if (reportT2.good()) {
			reportT2 << dts << CT2A(cs1);// << std::endl;
		}
		ss1 = CT2A(cs1);
		stst << ss1;
		reportT1log.push_back(stst.str()); // NOTE for reportT1log end line is printed when saved to T1 log file.
		return;
	} else if (i == 0) {
		cs1.Format(L"Test previously reported as FAILED.");
		if (reportT2.good()) {
			reportT2 << dts << CT2A(cs1);// << std::endl;
		}
		ss1 = CT2A(cs1);
		stst << ss1;
		reportT1log.push_back(stst.str());
		return;
	}

	dbgval = TESTlist[TRactive][tid].GetRelate();
	TESTlist[TRactive][tid].testNotes.push_back(std::string(CT2A(cs1)));
	reportT2 << CT2A(cs1) << std::endl;

	i = TESTlist[TRactive][tid].GetVarIndex();

	// Set lval. This is it for lval.
	lval = VARlist[i].GetValue();
	tol = VARlist[i].GetErrBound();
	lvLabel = VARlist[i].GetLabel().c_str();

	// MAG 250430 TODO add code to check for and run interpolate values for RVAL
	// Set rval.
	if (TESTlist[TRactive][tid].GetUsePrevV1()) { // test for use LAST
		rval = VARlist[i].GetPrevValue();
		rvLabel = VARlist[i].GetLabel().c_str();
	} else if (TESTlist[TRactive][tid].myInterpolate.GetMode() >= 0) {
		itval = TESTlist[TRactive][tid].myInterpolate.GetVarIndexX();
		tspan = tnow - TESTlist[TRactive][tid].GetTriggerTime();  //MAG check GetInterpolateValue second parameter
		if (itval >= 0) {
			rval = GetInterpolatedValue(tid, VARlist[itval].GetValue());
			rvLabel = VARlist[itval].GetLabel().c_str();
		} else { 
			rval = GetInterpolatedValue(tid, TESTlist[TRactive][tid].myInterpolate.GetX()); 
			rvLabel = TESTlist[TRactive][tid].Label;
		}

		// set value to VAR or test for reporting
		itval = TESTlist[TRactive][tid].GetVarIndex();
		if (itval >= 0) VARlist[itval].SetValue(rval);
		else TESTlist[TRactive][tid].SetValue(rval);
	} else {
		i = TESTlist[TRactive][tid].GetVarIndex2();  // check for second var instead of just value
		if (i > -1) {
			rval = VARlist[i].GetValue();
			rvLabel = VARlist[i].GetLabel().c_str();
		} else {
			rval = TESTlist[TRactive][tid].GetValue();
			rvLabel = "Value";// TESTlist[TRactive][tid].Label;
		}
	}

	it->SetRunTime(tnow);
	
	testID = TESTlist[TRactive][tid].GetRelate();

	switch (TESTlist[TRactive][tid].GetRelate()) {
		case -2: // LT
			//if (VARlist[TESTlist[TRactive][tid].GetVarIndex()].GetValue() < TESTlist[TRactive][tid].GetValue()) 
			if ((lval - tol) < rval) {
				ltr = 1;
				//cs1.Format(L"LT Test %d passed.", tid);
			} else {
				ltr = 0;
				//cs1.Format(L"LT Test %d *failed*.", tid);
			}
			//cs1 += cs2;
			break;
		case -1: // LTE
			//if (VARlist[TESTlist[TRactive][tid].GetVarIndex()].GetValue() <= TESTlist[TRactive][tid].GetValue()) {
			if ((lval - tol) <= rval) {
				ltr = 1;
				//cs1.Format(L"LTE Test %d passed.", tid);
			} else {
				ltr = 0;
				//cs1.Format(L"LTE Test %d *failed*.", tid);
			}
			//cs1 += cs2;
			break;
		case 0: // EQ
			//if (abs(VARlist[TESTlist[TRactive][tid].GetVarIndex()].GetValue() - TESTlist[TRactive][tid].GetValue()) < 0.01) {
			if (abs(lval - rval) <= tol) {
				ltr = 1;
				//cs1.Format(L"EQ Test %d passed.", tid);
			} else {
				ltr = 0;
				//cs1.Format(L"EQ Test %d *failed*.", tid);
			}
			//cs1 += cs2;
			break;
		case 1: // GTE
			//if (VARlist[TESTlist[TRactive][tid].GetVarIndex()].GetValue() >= TESTlist[TRactive][tid].GetValue()) {
			if ((lval + tol) >= rval) {
				ltr = 1;
				//cs1.Format(L"GTE Test %d passed.", tid);
			} else {
				ltr = 0;
				//cs1.Format(L"GTE Test %d *failed*.", tid);
			}
			//cs1 += cs2;
			break;
		case 2: // GT
			//if (VARlist[TESTlist[TRactive][tid].GetVarIndex()].GetValue() > TESTlist[TRactive][tid].GetValue()) {
			if ((lval + tol) > rval) {
				ltr = 1;
				//cs1.Format(L"GT Test %d passed.", tid);
			} else {
				ltr = 0;
				//cs1.Format(L"GT Test %d *failed*.", tid);
			}
			//cs1 += cs2;
			break;
		case 3: // NE
			//if (VARlist[TESTlist[TRactive][tid].GetVarIndex()].GetValue() - TESTlist[TRactive][tid].GetValue() > 0.01) {
			if (abs(lval - rval) > tol) {
				ltr = 1;
				//cs1.Format(L"NE Test %d passed.", tid);
			} else {
				ltr = 0;
				//cs1.Format(L"NE Test %d *failed*.", tid);
			}
			//cs1 += cs2;
			break;
		case 4: // MA MANUAL REVIEW- can not fail here.
			ltr = 1;
			//cs1.Format(L"Test %d: Manual check VAR value is %5.2f", tid, lval);//VARlist[TESTlist[TRactive][tid].GetVarIndex()].GetValue());
			//reportT2 << dts << CT2A(cs1) << std::endl;
			//cs1.Format(L"Test %d: Manual check REL value is %5.2f", tid, rval);//TESTlist[TRactive][tid].GetValue());
			//reportT2 << dts << CT2A(cs1) << std::endl;
			cs1.Format(L"Result: Manual check (VAL1 %5.2f) vs (VAL2 %5.2f)", lval, rval);//TESTlist[TRactive][tid].GetValue());

			//manualCount++;
			break;
		case 5: // ANY- can not fail here.
			ltr = 1;
			cs1.Format(L"Result: PASS (ANY) ");
			//cs1.Format(L"Test %d: ANY value no test.", tid);
			break;
		default:
			ltr = 0;
			//cs1.Format(L"Test %d: Test type not known. No test was run. LVAL = %5.2f RVAL = %5.2f", tid, lval, rval);
			cs1.Format(L"Result: Test type %d not known. No test was run. LVAL = %5.2f RVAL = %5.2f", testID, lval, rval);
			testID = 6;  // to use testType array
			break;
	}	// end switch

	if (testID < 4) {
		if (ltr == 1) {
			cs1.Format(L"Result: PASS ");
		} else {
			cs1.Format(L"Result: *FAIL* ");
		}
		cs2.Format(L"(%s: %5.2f) %S (%s: %5.2f) (tol: +/- %5.2f)", lvLabel.GetString(), lval, testType[testID + 2].c_str(), rvLabel.GetString(), rval, tol);
		cs1 += cs2;
	}  // for other cases cs1 is already filled

	ss1 = CT2A(cs1);
	stst << ss1;// << std::endl;
	reportT1log.push_back(stst.str());
	theApp.m_BACdlg->m_status.AddString(cs1);

	reportT2 << stst.str() << std::endl;
	TESTlist[TRactive][tid].testNotes.push_back(stst.str());//std::string(CT2A(cs2)));
	TESTlist[TRactive][tid].SetResult(ltr, lval, rval);

	return;
}  // end RunTest


// MAG 
int
FPTMbase::ResetDelay(int interval)
{
	// TODO: Add your control notification handler code here
	CTime			tnow = CTime::GetCurrentTime();
	CTime			tnext = {};
	CTime			tstart = {};	// original start time
	CTimeSpan		ts(0, 0, 1, 0);  // set to 1 minute
	unsigned int	i = 0;

	// Set tnow to next minute , i.e. HH:(MM+1):00
	tnow += ts;
	ts = tnow.GetSecond();
	tnow -= ts;

	ts = CTimeSpan(0, 0, interval - 1, 0);// NOTE- 1 minute is added to interval by code above, requiring the '-1' here
	tnext = tnow + ts;

	// edge case- just one element in TRlist block (this is unlikely!!!)
	if (TRlist[TRactive].size() == 1) {
		TRlist[TRactive][0].SetEventTime(tnext);
		theApp.m_BACdlg->UpdateEvents();
		return 1;
	}

	tstart = TRlist[TRactive][0].GetEventTime();
	TRlist[TRactive][0].SetEventTime(tnext);
	theApp.m_BACdlg->mybtm[0].SetActivationTime(tnext);
	for (i = 1; i < TRlist[TRactive].size(); i++) {
		ts = TRlist[TRactive][i].GetEventTime() - tstart;
		TRlist[TRactive][i].SetEventTime(tnext + ts);
	}
	theApp.m_BACdlg->UpdateEvents();

	return 1;
} // end ResetDelay


// MAG 160705 todo: clean up the heinous memory leaks in this subroutine
// MAG resets value of indicated BACnet object on controller if it's been overwritten.
int
FPTMbase::RunReset(int rid)
{
#ifndef BACOFFLINE  // run if BACOFFLINE is NOT set
	BACdev* mybd = NULL;
	int i = 0;
	int idev = 0;
	int devobj = 0;
	CString status;
	CString cs1;
	CString csp(" ");

	if (rid >= (int)TRlist[TRactive].size()) return 0;
	if (rid < 0) return 0;

	idev = TRlist[TRactive][rid].GetDevice();
	if (idev < 0) return 0;	// device not set
	if (idev >= (int)BACnetList.size()) return 0;

	mybd = &BACnetList[idev];
	iph.devObjInst[0] = mybd->GetDevObjInst();

	mybd->GetMAC(iph.mac[0]);
	mybd->GetRouterMAC(iph.router[0]);
	iph.net[0] = mybd->GetNet();
	iph.obj[0] = TRlist[TRactive][rid].GetBACnetObject();
	iph.inst[0] = TRlist[TRactive][rid].GetInstance();
	mybd->GetIP(iph.ip[0]);
	mybd->GetRouterIP(iph.routerip[0]);
	iph.ipport[0] = mybd->GetIPPort();
	iph.routerport[0] = mybd->GetRouterPort();
	i = 0;
	iph.prop[0] = 81;  // out of service  
	iph.index[0] = -1;	// no index

	if ((checkOOS == 0) || (TRlist[TRactive][rid].GetInitOOS() != 1)) {	// MAG 170327 add check for initially not set OOS
		iph.prty[0] = writePriorityLevel;
		sprintf_s(iph.bv[0], 20, "0");

		status.Format(L"Writing RESET value to device %d MAC ", TRlist[TRactive][rid].GetDevice());
		status += iph.mac[0];
		status += " net = ";
		cs1.Format(L"%d", iph.net[0]);
		status += cs1;
		status += " router = ";
		status += iph.router[0];

		status += " obj = ";
		cs1.Format(L"%d", iph.obj[0]);
		status += cs1;
		status += " isnt = ";
		cs1.Format(L"%d", iph.inst[0]);
		status += cs1;
		status += " val = NULL";

		if ((mybd->GetConfigID() == 4) || (mybd->GetConfigID() == 2)) {
			theApp.m_BACdlg->m_status.AddString(cs1);
			// always send reset to outofservice
			i = BACdll::WriteObjPropertyByDevice(1, iph.devObjInst, iph.obj, iph.inst, iph.prop, iph.index, iph.prty, iph.bv, fault);
			//i = BACdll::WriteObjProperty(1, iph.mac, iph.net, iph.router, iph.obj, iph.inst, iph.prop, iph.index, iph.prty, iph.bv, fault);

			sprintf_s(iph.bv[0], 20, "NULL");
			iph.prop[0] = 85;
			i = BACdll::WriteObjPropertyByDevice(1, iph.devObjInst, iph.obj, iph.inst, iph.prop, iph.index, iph.prty, iph.bv, fault);
			//i = BACdll::writePVProperty(1, iph.mac, iph.net, iph.router, iph.obj, iph.inst, iph.prty, iph.bv, fault);
		} else {
			status += " IP = ";
			status += iph.ip[0][0];
			status == ".";
			status += iph.ip[0][1];
			status == ".";
			status += iph.ip[0][2];
			status == ".";
			status += iph.ip[0][3];
			// always send reset to outofservice

			i = BACdll::WriteObjPropertyByDevice(1, iph.devObjInst, iph.obj, iph.inst, iph.prop, iph.index, iph.prty, iph.bv, fault);
			//i = BACdll::writeObjPropertyIPch(1, iph.mac, iph.ip, iph.ipport, iph.net, iph.router, iph.routerip, iph.routerport, iph.obj, iph.inst, iph.prop, iph.index, iph.prty, iph.bv, fault);

			sprintf_s(iph.bv[0], 20, "NULL");
			iph.prop[0] = 85;

			i = BACdll::WriteObjPropertyByDevice(1, iph.devObjInst, iph.obj, iph.inst, iph.prop, iph.index, iph.prty, iph.bv, fault);
			//i = BACdll::writePVPropertyIP(1, iph.mac, iph.ip, iph.ipport, iph.net, iph.router, iph.routerip, iph.routerport, iph.obj, iph.inst, iph.prty, iph.bv, fault);
		}
	} else {
		status.Format(L"NOT writing RESET value to device %d MAC, leaving in original state out-of-service == 1 (checkOOS == 0)", TRlist[TRactive][rid].GetDevice());
		theApp.m_BACdlg->m_status.AddString(status);
	}

#endif // BACOFFLINE
	return 1;
}  // end RunReset


// MAG 251120 consolidate this function from variation in OnTimer
// Returns: 1: no faults found
//			0: some fault found
int
FPTMbase::RunPeriodic()
{
	int		iTR = TRactive;
	int		iTS = TSactive;
	int		varIndex1 = 0;
	int		varIndex2 = 0;
	int		expIndex = 0;	// expression index
	int		varOperator1 = 0;
	int		retVal = 1;  // return no error by default
	int		testID = 0;
	int		colRef = 0;		// reference to column in TestStep::TRlist[0]
	double	varValue1 = 0;
	double	modValue1 = 0;
	double	varValue2 = 0;
	CString cs1;
	CString dts;
	CTime	tnow;

	tnow = CTime::GetCurrentTime();  // this is used for multiple events
	dts = theApp.GetDTS(tnow);

	if (testSteps[iTR][iTS].piList.size() == 0) {
		cs1.Format(L"%s Enter RunPeriodic- no periodic variables in this step.", dts.GetString());
		theApp.m_BACdlg->m_status.AddString(cs1);
		return 1;
	}

	retVal = testSteps[iTR][iTS].GetListMinEvent();
	colRef = TRlist[iTR][retVal].GetSourceColumn();
	cs1.Format(L"%s Enter RunPeriodic: %d periodic variables in step %c:%d.", dts.GetString(), testSteps[iTR][iTS].piList.size(),'A' + iTR,colRef);
	theApp.m_BACdlg->m_status.AddString(cs1);
	reportT2 << CT2A(cs1) << std::endl;

	for (size_t pi = 0; pi < testSteps[iTR][iTS].piList.size(); pi++) {
		// MAG 251217 we only need to read from VARlist

		varIndex1 = testSteps[iTR][iTS].piList[pi].GetExpressionIndex();
		if ((varIndex1 < 0) || (varIndex1 >= (int)theApp.myFPTM.VARlist.size())) {
			retVal = 0;
			break;
		}
		reportT2 << CT2A(dts) << " varIndex1 = " << varIndex1;

		varValue1 = VARlist[varIndex1].GetValue();
		varOperator1 = testSteps[iTR][iTS].piList[pi].GetModType();
		reportT2 << " varValue1 = " << varValue1 << " varOperator1 = " << varOperator1;
		
		if (varOperator1 >= 0) {
			modValue1 = testSteps[iTR][iTS].piList[pi].GetModValue();
			reportT2 << " modValue1 = " << modValue1;
		}
		switch (varOperator1) {
			case 0: varValue1 += modValue1; break;
			case 1: varValue1 -= modValue1; break;
			case 2: varValue1 *= modValue1; break;
			case 3: varValue1 /= modValue1; break;
			case 4:
				if (varValue1 == 0) break;
				varValue1 = modValue1 / varValue1; break;
			default:  // no change to varValue1
				break;
		}
		varIndex1 = testSteps[iTR][iTS].piList[pi].GetValueIndex();
		if ((varIndex1 < 0) || (varIndex1 >= (int)VARlist.size())) {
			retVal = 0;
			break;
		}
		VARlist[varIndex1].SetValue(varValue1);
		cs1.Format(L"%s RunPeriodic var %d variable %S (%d) value %f.", dts.GetString(),pi,VARlist[varIndex1].GetLabel().c_str(),varIndex1, varValue1);
		reportT2 << " varValue1 Final = " << varValue1 << std::endl;

		theApp.m_BACdlg->m_status.AddString(cs1);

		// Send out the periodic request value (write)
		varIndex1 = testSteps[iTR][iTS].piList[0].GetTRSource();
		varIndex2 =  RunRequest(varIndex1);
		if(varIndex2 == 1){ // Success!
			reportT2 << CT2A(dts) << " Periodic value sent successfully. Var " << varIndex1 << " Block: " << iTR << " Step: " << iTS << std::endl;
		} else {
			reportT2 << CT2A(dts) << " Periodic value send fail. Var " << varIndex1 << " Block: " << iTR << " Step: " << iTS << std::endl;
		}
	} // end for loop through pi list

	return retVal;
}  // end RunPeriodic


// Gemchecked
int
FPTMbase::DeleteTask(int rid)
{
	CString cs1;

	if (rid < 0) return 0;
	if (TRlist.size() == 0) return 0;
	if (rid >= (int)TRlist[TRactive].size()) return 0;
	
	// check not deleting last item!!!
	TRlist[TRactive].erase(TRlist[TRactive].begin() + rid);
	if (theApp.m_BACdlg->DisplayLineStart > (int)TRlist[TRactive].size() - 10){
		theApp.m_BACdlg->DisplayLineStart--;
	}
	if (theApp.m_BACdlg->DisplayLineStart < 0) theApp.m_BACdlg->DisplayLineStart = 0;

	theApp.m_BACdlg->UpdateEvents();
	cs1.Format(L"Deleted task %d at user command.", rid);
	theApp.m_BACdlg->m_status.AddString(cs1);

	return 1;
}


int
FPTMbase::AddTask()
{
	return 1;
}

// MAG save test script file in XML format
// MAG 260326 todo: remove legacy sprintf_s
int
FPTMbase::SaveToFile(char* opname)
{
	int				i = 0;
	int				j = 0;
	int				fpr = 0;
	int				fpv = 0;
	CString			cs2;
	CString			cs3;
	std::string		ss1;
	std::string		ss2;
	std::stringstream stst1;
	char			nodedata[260] = {};
	unsigned char	ip[4] = {};
	xml_element*	x1= nullptr;		// devices or events
	xml_element*	BTMroot= nullptr;
	xml_element*	curnode= nullptr;	// current device or event
	xml_element*	subnode= nullptr;	// subnode of device or event
	FILE*			lopf = nullptr;
	double			varVal = 0.0;
	std::string		scriptName("scriptLabel");
	CTime			tFirst;
	CTime			tNode; // time at current node
	CTimeSpan		tSpan;
	std::vector<int>::iterator	vint;
	std::vector<int>			vintList;

	BTMroot = filex.addElement(filex.getRoot(), "FPTM");
	if (BTMroot == NULL) {
		BTMroot = filex.addElement(filex.getRoot(), "BTM");
	}
	if (BTMroot == NULL) {
		theApp.m_BACdlg->m_status.AddString(L"Error saving file- no root node.");
		return EXIT_FAILURE;
	}

	while (BTMroot->subelement != NULL) {
		filex.deleteElement(BTMroot->subelement);
	}

	if (scriptLabel.size() > 0) {
		filex.addprop(BTMroot, &scriptName, &scriptLabel);
	}

	//	DEVICES
	if (BACnetList.size() > 0) {
		x1 = filex.addElement(BTMroot, "devices");
		if (x1->subelement == NULL) {	// might already be formed
			filex.addprop(x1, "delete", "1");

			filex.addprop(x1, "size", BACnetList.size());
			for (i = 0; i < (int)BACnetList.size(); i++) {
				stst1.clear();
				curnode = filex.createElement(x1, "device");  // curnode is the device node
				filex.addprop(curnode, "id", BACnetList[i].GetIndex());	// save the index

				BACnetList[i].GetMAC(nodedata);
				filex.addprop(curnode, "mac", nodedata);

				BACnetList[i].GetIP(ip);
				if (ip[0] > 0) {
					//sprintf_s(nodedata, 50, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
					cs2.Format(L"%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
					ss1 = CStringA(cs2);	// MAG 260326 update to use string
					filex.addprop(curnode, "ip", &ss1);
					filex.addprop(curnode, "ipport", BACnetList[i].GetIPPort());
				}

				filex.addprop(curnode, "net", BACnetList[i].GetNet());

				BACnetList[i].GetRouterMAC(nodedata);
				if (strlen(nodedata) > 0)
					filex.addprop(curnode, "routermac", nodedata);

				BACnetList[i].GetRouterIP(ip);
				if (ip[0] > 0) {
					//sprintf_s(nodedata, 50, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
					cs2.Format(L"%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
					ss1 = CStringA(cs2);	// MAG 260326 update to use string
					filex.addprop(curnode, "routerip", &ss1);

					filex.addprop(curnode, "routeripport", BACnetList[i].GetRouterPort());
				}

				// write device object instance ##
				fpr = BACnetList[i].GetDevObjInst();
				if (fpr >= 0) {
					//sprintf_s(nodedata, "%d", fpr);
					filex.addprop(curnode, "devinst", fpr);
				} // else will not have instance

				filex.addprop(curnode, "configID", BACnetList[i].GetConfigID());

				BACnetList[i].GetDeviceLabel(nodedata);
				if (strlen(nodedata) > 0) {
					filex.addprop(curnode, "label", nodedata);
				}

				// MAG 250617 save named points as <point />
				// scan through namedpoints, check if ID == i, then save if it matches
				for (j = 0;j < (int)namedPointList.size();j++) {
					if (namedPointList[j].GetDevice() == i) {
						subnode = filex.createElement(curnode, "point");  // subnode is point

						switch (namedPointList[j].GetBACnetObject()) {
							case 0:	 ss1 = "AI"; break;
							case 1:	 ss1 = "AO"; break;
							case 2:	 ss1 = "AV"; break;
							case 3:	 ss1 = "BI"; break;
							case 4:	 ss1 = "BO"; break;
							case 5:	 ss1 = "BV"; break;
							case 12: ss1 = "LP"; break; // MAG 250709 add loop object here
							case 13: ss1 = "MI"; break;
							case 14: ss1 = "MO"; break;
							case 19: ss1 = "MV"; break;
							default: ss1 = "DV";//namedPointList[j].GetInstance(); MAG 260227 change to device
						}
						filex.addprop(subnode, "type", &ss1);

						filex.addprop(subnode, "id", namedPointList[j].GetInstance());
						ss1 = namedPointList[j].GetLabel();
						filex.addprop(subnode, "label", &ss1);
						ss1 = namedPointList[j].GetDescription();
						filex.addprop(subnode, "description", &ss1);
					}
				}
			}
		}
	}	// end DEVICES

	// MAG add VARIABLES section
	// VARIABLES
	if (VARlist.size() > 0) {
		x1 = filex.createElement(BTMroot, "variables");
		filex.addprop(x1, "delete", "1");

		for (i = 0; i < (int)VARlist.size(); i++) {
			subnode = filex.createElement(x1, "var");
			VARlist[i].GetLabel(nodedata);
			curnode = filex.createElement(subnode, "label", nodedata);
			varVal = VARlist[i].GetValue();
			curnode = filex.createElement(subnode, "value", varVal);
			varVal = VARlist[i].GetErrBound();
			curnode = filex.createElement(subnode, "errBound", varVal);
		}
	}

	// set tstart from first node
	if (TRlist.size() > 0) {
		if (TRlist[0].size() > 0) {
			tFirst = TRlist[0][0].GetEventTime();
		}
	}

	// EVENTS add events in TRlist
	for (size_t evLoop = 0; evLoop < TRlist.size(); evLoop++) {
		if (TRlist[evLoop].size() > 0) {
			x1 = filex.createElement(BTMroot, "events");
			filex.addprop(x1, "size", TRlist[evLoop].size());
			filex.addprop(x1, "delete", "1");

			cs2 = TRdata[evLoop].GetLabel();
			if (cs2.GetLength() > 0) {
				cs3 = "label";
				filex.addprop(x1, cs3, cs2);
			}
			cs2 = TRdata[evLoop].GetTestIdentifier();
			if (cs2.GetLength() > 0) {
				cs3 = "testIdentifier";
				filex.addprop(x1, cs3, cs2);
			}

			if (TRlistRepeat == 1) filex.addprop(x1, "repeat", "1");

			for (i = 0; i < (int)TRlist[evLoop].size(); i++) {
				subnode = filex.createElement(x1, "event");
				filex.addprop(subnode, "id", i);

				ss1 = "bacdevice";
				cs2.Format(L"%d", TRlist[evLoop][i].GetDevice());
				ss2 = CStringA(cs2);
				curnode = filex.createElement(subnode, &ss1, &ss2);

				// 2015-04-12T18:51:19+00:00 ISO 8601 format
				// use full time for first node
				if (i == 0) {
					cs2 = TRlist[evLoop][i].GetEventTime().Format("%Y-%m-%dT%H:%M:%S");
					ss2 = CStringA(cs2);
					ss1 = "eventTime";
					curnode = filex.createElement(subnode, &ss1, &ss2);
				}
				else {	// type="offset"
					tNode = TRlist[evLoop][i].GetEventTime();
					tSpan = tNode - tFirst;
					cs2.Format(L"%d", (int)tSpan.GetTimeSpan());
					ss1 = "eventTime";
					ss2 = CStringA(cs2);
					curnode = filex.createElement(subnode, &ss1, &ss2);
					filex.addprop(curnode, "type", "offset");
				}
				ss1 = "object";
				j = TRlist[evLoop][i].GetBACnetObject();
				switch (j) {
					case 0: cs2 = "AI"; break;
					case 1: cs2 = "AO"; break;
					case 2: cs2 = "AV"; break;
					case 3: cs2 = "BI"; break;
					case 4: cs2 = "BO"; break;
					case 5: cs2 = "BV"; break;
					case 13: cs2 = "MI"; break;
					case 14: cs2 = "MO"; break;
					case 19: cs2 = "MV"; break;
					default: cs2 = "UNK";
				}

				//cs2.Format(L"%d", TRlist[TRactive][i].GetBACnetObject());
				ss2 = CStringA(cs2);
				curnode = filex.createElement(subnode, &ss1, &ss2);

				ss1 = "instance";
				cs2.Format(L"%d", TRlist[evLoop][i].GetInstance());
				ss2 = CStringA(cs2);
				curnode = filex.createElement(subnode, &ss1, &ss2);

				ss1 = "readwrite";
				cs2.Format(L"%d", TRlist[evLoop][i].GetRW());
				ss2 = CStringA(cs2);
				curnode = filex.createElement(subnode, &ss1, &ss2);

				ss1 = "value";
				cs2.Format(L"%f", TRlist[evLoop][i].GetValue());
				ss2 = CStringA(cs2);
				curnode = filex.createElement(subnode, &ss1, &ss2);

				if (TRlist[evLoop][i].GetUseValue() == 1){
					ss1 = "useValue";
					ss2 = "1";
					curnode = filex.createElement(subnode, &ss1, &ss2);
				}

				fpv = TRlist[evLoop][i].GetFPV();
				if (fpv > -1) {
					VARlist[fpv].GetLabel(nodedata);
					curnode = filex.createElement(subnode, "var", nodedata);

					//save fprelate and fpvalue or fpmodvar if used
					fpr = TRlist[evLoop][i].GetFPVRelate();
					if (fpr > 0) {  // NOTE- these are children of subnode, not curnode
						filex.createElement(subnode, "operator", fpr);
						if (TRlist[evLoop][i].GetFPVModvar() == -1) {
							filex.createElement(subnode, "modvalue", TRlist[evLoop][i].GetFPVValue());
						} else {
							j = TRlist[evLoop][i].GetFPVModvar();
							VARlist[j].GetLabel(nodedata);
							curnode = filex.createElement(subnode, "modvar", nodedata);
						}
					}

					// save process var index ("procvar")
					fpr = TRlist[evLoop][i].GetPVI();
					if (fpr >= 0) {
						filex.createElement(subnode, "procvar", fpr);
					}
				}

				// save event source column- offset into testStep
				stst1.str("");		// empty stst1
				ss1 = "eventCol";
				stst1 << TRlist[evLoop][i].GetSourceColumn();
				ss2 = stst1.str();
				curnode = filex.createElement(subnode, &ss1, &ss2);
				stst1.clear();

				ss1 = "comment";
				cs2 = TRlist[evLoop][i].GetComment();
				ss2 = CStringA(cs2);
				curnode = filex.createElement(subnode, &ss1, &ss2);
			}
		}	// end this EVENTS

		// add save Test Step in EVENTS loop
		for (i = 0; i < (int)testSteps[evLoop].size(); i++) {
			subnode = filex.createElement(x1, "step");
			filex.addprop(subnode, "id", i);

			ss1 = "eventList";	// aka trList
			stst1.str("");		// empty stst1
			// put all elements of trList in CSV string (no spaces)
			vintList = testSteps[evLoop][i].GetList(); // copy trList
			for (j = 0; j < (int)vintList.size(); j++) {
				stst1 << (int)vintList[j];
				stst1 << ",";
			}
			ss2 = stst1.str();
			ss2.erase(ss2.size() - 1, 1); // erase trailing comma
			curnode = filex.createElement(subnode, &ss1, &ss2);
			stst1.clear();
			stst1.str("");

			ss1 = "label";
			ss2 = testSteps[evLoop][i].GetLabel();
			curnode = filex.createElement(subnode, &ss1, &ss2);

			j = testSteps[evLoop][i].GetTest();
			if (j > 0) {
				ss1 = "test";
				stst1 << j;
				ss2 = stst1.str();
				curnode = filex.createElement(subnode, &ss1, &ss2);
				stst1.clear();
			}

			// save tsInterval
			ss1 = "interval";
			stst1 << testSteps[evLoop][i].GetTimeInterval().GetTotalSeconds();
			ss2 = stst1.str();
			curnode = filex.createElement(subnode, &ss1, &ss2);
			stst1.clear();

			// MAG 250424 TODO add save mathvar and periodic inputs
		}

	}  // end EVENTS loop

	// write TESTS
	for (size_t tLoop = 0; tLoop < TRlist.size(); tLoop++) {
		x1 = filex.createElement(BTMroot, "tests");
		filex.addprop(x1, "delete", "1");

		cs2 = TRdata[tLoop].GetTestIdentifier(); // MAG 250304 add this to help ensure alignment of test list and event list
		if (cs2.GetLength() > 0) {
			cs3 = "testIdentifier";
			filex.addprop(x1, cs3, cs2);
		}

		for (i = 0; i < (int)TESTlist[tLoop].size(); i++) {
			curnode = filex.createElement(x1, "test");
			filex.addprop(curnode, "id", i);

			VARlist[TESTlist[tLoop][i].GetVarIndex()].GetLabel(nodedata);
			subnode = filex.createElement(curnode, "var", nodedata);

			//sprintf_s(nodedata, "%f", TESTlist[tLoop][i].GetValue());
			//subnode = filex.createElement(curnode, "value", nodedata);  MAG convert to use native float version
			subnode = filex.createElement(curnode, "value", TESTlist[tLoop][i].GetValue());

			subnode = filex.createElement(curnode, "relate", TESTlist[tLoop][i].GetRelate());

			subnode = filex.createElement(curnode, "level", TESTlist[tLoop][i].GetLevel());

			subnode = filex.createElement(curnode, "triggerEvent", TESTlist[tLoop][i].GetTriggerEvent());

			j = TESTlist[tLoop][i].GetResult();
			if (j > -1) subnode = filex.createElement(curnode, "result", j);

			if (TESTlist[tLoop][i].Label.GetLength() > 0) {
				sprintf_s(nodedata, "%S", TESTlist[tLoop][i].Label.GetString());
				cs2 = TESTlist[tLoop][i].Label;
				
				j = cs2.GetLength();// MAG update to use std::string
				ss1 = CStringA(cs2);
				subnode = filex.createElement(curnode, "label", &ss1);
			}

			for (j = 0; j < (int)TESTlist[tLoop][i].testNotes.size(); j++) {
				sprintf_s(nodedata, "%hS", TESTlist[tLoop][i].testNotes[j].c_str());
				ss1 = TESTlist[tLoop][i].testNotes[j];
				subnode = filex.createElement(curnode, "testNote", &ss1);
			}
		}
	}

	// write PRIORITY
	x1 = filex.createElement(BTMroot, "priorityLevel", writePriorityLevel);

	// write SEND OUT-OF-SERVICE
	x1 = filex.createElement(BTMroot, "OutOfService");
	if (x1 != NULL) {
		curnode = filex.createElement(x1, "AI", sendOutOfService[AI]);
		curnode = filex.createElement(x1, "AO", sendOutOfService[AO]);
		curnode = filex.createElement(x1, "AV", sendOutOfService[AV]);
		curnode = filex.createElement(x1, "BI", sendOutOfService[BI]);
		curnode = filex.createElement(x1, "BO", sendOutOfService[BO]);
		curnode = filex.createElement(x1, "BV", sendOutOfService[BV]);
		curnode = filex.createElement(x1, "MI", sendOutOfService[MSI]);
		curnode = filex.createElement(x1, "MO", sendOutOfService[MSO]);
		curnode = filex.createElement(x1, "MV", sendOutOfService[MSV]);
	}

	// write SAVE FILE if used
	if (evOut != NULL) {
		x1 = filex.createElement(BTMroot, "LogFile", evOutLabel);
	}
	
	cs2 = opname;
	cs2 = cs2.Right(3);
	cs2 = cs2.MakeLower();
	if (cs2.Compare(L"xml") != 0) strcat_s(opname,256, ".xml");

	fopen_s(&lopf, opname, "w");
	if (lopf != NULL) {
		filex.print_token(BTMroot, lopf);
		fclose(lopf);
	} else {
		return EXIT_FAILURE;
	}

	theApp.m_BACdlg->m_status.AddString(L"Event file has been saved.");

	return EXIT_SUCCESS;
} // end FPTMbase::SaveToFile


// Enter devices, variables, and events list from a XML config file
int
FPTMbase::ReadFromFile(char* ipname)
{
	xml_element*					x1= nullptr;		// devices or events
	xml_element*					FPTMroot= nullptr;
	xml_element*					subnode= nullptr;	// subnode of device or event
	xml_element*					subnode2= nullptr;	// subnode of device or event
	xml_element*					evnode= nullptr;	// event/device node
	xml_props*						myprop= nullptr;
	CString							mac;
	CString							rmac;
	CString							cs1;
	CStringArray					errorList;
	char							ch1[MAX_PATH] = {};		// used for misc and for save file
	int								deviceID = 0;	// used as array index, for device currently being read
	int								bool1 = 0;
	int								i = 0;
	int								evLoop = 0;
	int								devObjInst = 0;		// MAG 260316 stores device object id
	int								errorsFound = 0;	// MAG 180703 used to indicate if errors were found, used at end of sub for status
	int								devCount = 0;			// number of devices indicated in file
	int								devRead = 0;			// number of devices actually read so far
	int								varCount = 0;			// number of variables actually read so far
	int								hasValueNode = 0;		// used for error checking when reading event variables
	int								hasOperatorNode = 0;	// used for error checking when reading event variables
	double							di = 0.0;
	unsigned char					anip[4] = {};		// an IP, used to hold an IP address
	BACdev							bvar;	// used to add BACnet devices
	FPvar							mvar("var1");
	std::locale						loc;
	std::string						str1;
	std::string::iterator			strit;
	std::stringstream				convert;	// used to convert strings to ints
	std::vector<TimedRequest>		localTR;
	std::vector<RampValue>			rampInit;
	std::vector<DualVarTestList>	tlInit;
	std::vector<DualVarTestList>	dvtlInit;
	std::vector<TestStep>			tsInit;
	std::vector<FPvar>::iterator	it;
	DualVarTestList					tl1;
	NamedPoint						mynp;
	TimedRequest					mtr;  // MAG add 180628- modify events so that this is added, instead of resizing, etc.
	TimedRequestData				mtrd; // MAG 241028 add metadata functionality
	TimedRequestData				trdInit; // let it go.
	TestStep						myts;	// fill then add to testSteps
	std::time_t						tstart = 0;// std::time(0);
	std::time_t						prevNodeTime = 0;
	std::time_t						ttemp = 0;
	struct tm						pntm;	// to hold previous node time
	int								initBACnetCount = 0;
	std::wifstream					ifop; // MAG 260330 handle for input file


	if(ipname != NULL){
		filex.deleteElement(filex.getRoot());
		//filex.read_xml_file(lopf);
		filex.read_xml_file(ipname);
		//fclose(lopf);
	} else {
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Could not open input file.");
		return 0;
	}

	initBACnetCount = BACnetList.size();

	// MAG 2506011 Change working directory to one from last file dropped.
	strncpy(ch1, ipname, MAX_PATH);
	if (ch1 == NULL) {
		return 0; // MAG add some debug info
	}
	ch1[MAX_PATH - 1] = 0;  // ensure that ch1 is zero-terminated
	i = strlen(ch1) - 1;

	while ((ch1[i] != '\\')&&(i >= 0)) i--;
	if (i > 0) {
		ch1[i + 1] = 0;
		cs1.Format(L"Changing working directory to '%S'", ch1);
		theApp.m_BACdlg->m_status.AddString(cs1);
		if (_chdir(ch1) != 0) {
			theApp.m_BACdlg->m_status.AddString(L"ERROR: could not change working directory.");
		}
	}

	FilesRead.push_back(ipname);

	mac.Format(L"Reading Input file '%S'", ipname);
	theApp.m_BACdlg->m_status.AddString(mac);

	// MAG 241028 add check for Taylor/CalNEXT-format XML spreadsheet
	FPTMroot = filex.element_by_name(filex.getRoot(), "?mso-application");
	if (FPTMroot != NULL) {
		ReadCalNEXTScript();  // note- filex is member of app so visible in called subroutine
		return 0;
	}

	FPTMroot = filex.element_by_name(filex.getRoot(), "FPTM");
	if (FPTMroot == NULL) FPTMroot = filex.element_by_name(filex.getRoot(), "BTM");	// keep for backwards compatibility
	if (FPTMroot == NULL) {
		// check if it's a BDS config file, used to fill the device list
		FPTMroot = filex.element_by_name(filex.getRoot(), "bdsDatapointList");
		if (FPTMroot != NULL) {
			theApp.m_BACdlg->m_status.AddString(L"Reading BDS mapping file...");
			i = ReadBDSSettingsFile(FPTMroot);
			devCount = BACnetList.size();
			theApp.m_BACdlg->UpdateDevices();
			if (i == 0) {
				theApp.m_BACdlg->m_status.AddString(L"ERROR- input file is not valid.");
				return 0;
			}
			//theApp.m_BACdlg->m_status.AddString(L"BDS Device file has been read."); NOTE explanatory string is in ReadBDSSettingsFile
			return 1;
		}
		theApp.m_BACdlg->m_status.AddString(L"ERROR- input file is not valid.");
		return 0;
	}

	myprop = filex.getprop(FPTMroot, "scriptLabel");
	if (myprop != NULL) {
		scriptLabel = myprop->data;
		cs1.Format(L"Read script with name '%S'", scriptLabel.c_str());
	}
	else {
		scriptLabel.clear();
	}

	// READ IN DEVICES
	try {
		x1 = filex.element_by_name(FPTMroot, "devices");
		if (x1 == NULL) 		x1 = filex.element_by_name(FPTMroot, "Devices");
		if (x1 == NULL) 		x1 = filex.element_by_name(FPTMroot, "DEVICES");
		// MAG NOTE- getprop returns NULL if x1 is NULL
		myprop = filex.getprop(x1, "delete");	// MAG 160705 changed from "reset"
		if (myprop != NULL) {
			if (myprop->data.compare("1") == 0)
				BACnetList.clear();
		}
		// MAG using a while loop so we can gracefully exit this part if needed (due to an error) and continue to events.
		while (x1 != NULL) {
			subnode = filex.element_by_name(x1, "device");
			if (subnode == NULL) subnode = filex.element_by_name(x1, "Device");
			if (subnode == NULL) subnode = filex.element_by_name(x1, "DEVICE");
			while (subnode != NULL) {
				// read device object instance ##
				myprop = filex.getprop(subnode, "devinst");  // 
				if (myprop == NULL) myprop = filex.getprop(subnode, "devInst");
				if (myprop != NULL) {
					sprintf_s(ch1, "%s", myprop->data.c_str()); // check if this needs %s or %S
					devObjInst = atoi(ch1);
					bvar.SetDevObjInst(devObjInst);
				} // else will remain at default -1

				myprop = filex.getprop(subnode, "id");	// read the index from file
				if (myprop == NULL) myprop = filex.getprop(subnode, "ID");
				if (myprop == NULL) {
					theApp.m_BACdlg->m_status.AddString(L"ERROR reading id: node not found");
					return(0);
				}
				i = atoi(myprop->data.c_str());
				bvar.SetIndex(i);

				myprop = filex.getprop(subnode, "mac");
				if (myprop == NULL) myprop = filex.getprop(subnode, "MAC");
				if (myprop == NULL) {
					theApp.m_BACdlg->m_status.AddString(L"ERROR reading input file: MAC address not found");
					return(0);
				}
				sprintf_s(ch1, 50, "%s", myprop->data.c_str());
				bvar.SetMAC(ch1);
				BACdll::AddDevListInfo(devObjInst, ch1, NULL, 0, 0, NULL, NULL, 0);

				myprop = filex.getprop(subnode, "ip");
				if (myprop == NULL) myprop = filex.getprop(subnode, "IP");
				if (myprop != NULL) {	// MAG 160318 add check for NULL
					sprintf_s(ch1, 50, "%s", myprop->data.c_str());
					bvar.SetIP(ch1);
					bvar.GetIP(anip);  // get IP in unsigned-int
					BACdll::AddDevListInfo(devObjInst, NULL, anip, 0, 0, NULL, NULL, 0);
					
					myprop = filex.getprop(subnode, "ipport");
					if (myprop == NULL) myprop = filex.getprop(subnode, "IPPORT");
					if (myprop == NULL) myprop = filex.getprop(subnode, "IPport");
					if (myprop != NULL) {
						sprintf_s(ch1, 50, "%s", myprop->data.c_str());
						i = atoi(ch1);
						bvar.SetIPPort(i);
						BACdll::AddDevListInfo(devObjInst, NULL, NULL, i, 0, NULL, NULL, 0);
					}
					else bvar.SetIPPort("0xBAC0");
				} else {
					bvar.SetIP("");
					bvar.SetIPPort(0);
				}

				myprop = filex.getprop(subnode, "net");
				if (myprop == NULL) myprop = filex.getprop(subnode, "NET");
				if (myprop == NULL) myprop = filex.getprop(subnode, "Net");
				if (myprop == NULL) {
					strcpy_s(ch1, "0");
				} else {
					sprintf_s(ch1, 50, "%s", myprop->data.c_str());
				}
				i = atoi(ch1);
				bvar.SetNet(i);
				BACdll::AddDevListInfo(devObjInst, NULL, NULL, 0, i, NULL, NULL, 0);

				myprop = filex.getprop(subnode, "routermac");
				if (myprop == NULL) myprop = filex.getprop(subnode, "routerMAC");
				if (myprop == NULL) myprop = filex.getprop(subnode, "RouterMAC");
				if (myprop == NULL) myprop = filex.getprop(subnode, "ROUTERMAC");
				if (myprop != NULL) {
					sprintf_s(ch1, 50, "%s", myprop->data.c_str());
					bvar.SetRouterMAC(ch1);
					BACdll::AddDevListInfo(devObjInst, NULL, NULL, 0, 0, ch1, NULL, 0);
				} else {
					bvar.SetRouterMAC("");
				}

				myprop = filex.getprop(subnode, "routerip");
				if (myprop == NULL) myprop = filex.getprop(subnode, "routerIP");
				if (myprop == NULL) myprop = filex.getprop(subnode, "RouterIP");
				if (myprop == NULL) myprop = filex.getprop(subnode, "ROUTERIP");
				if (myprop != NULL) {
					sprintf_s(ch1, 50, "%s", myprop->data.c_str());
					bvar.SetRouterIP(ch1);
					bvar.GetRouterIP(anip);
					BACdll::AddDevListInfo(devObjInst, NULL, NULL, 0, 0, NULL, anip, 0);
				}

				myprop = filex.getprop(subnode, "routeripport");
				if (myprop == NULL) myprop = filex.getprop(subnode, "routerIPport");
				if (myprop == NULL) myprop = filex.getprop(subnode, "RouterIPport");
				if (myprop == NULL) myprop = filex.getprop(subnode, "ROUTERIPPORT");
				if (myprop != NULL) {
					sprintf_s(ch1, 50, "%s", myprop->data.c_str());
					bvar.SetRouterPort(ch1);
					BACdll::AddDevListInfo(devObjInst, NULL, NULL, 0, 0, NULL, NULL, atoi(ch1));
				}

				myprop = filex.getprop(subnode, "configID");
				if (myprop == NULL) myprop = filex.getprop(subnode, "configid");
				if (myprop == NULL) myprop = filex.getprop(subnode, "configID");
				if (myprop == NULL) myprop = filex.getprop(subnode, "ConfigID");
				if (myprop == NULL) myprop = filex.getprop(subnode, "CONFIGID");
				if (myprop != NULL) {
					sprintf_s(ch1, 50, "%s", myprop->data.c_str());
					i = strtol(ch1, NULL, 10);
					bvar.SetConfigID(i);
				} else {
					i = bvar.AutoConfigID();
				}

				myprop = filex.getprop(subnode, "label");
				if (myprop == NULL) myprop = filex.getprop(subnode, "Label");
				if (myprop == NULL) myprop = filex.getprop(subnode, "LABEL");
				if (myprop != NULL) {
					sprintf_s(ch1, 50, "%s", myprop->data.c_str());
					bvar.SetDeviceLabel(ch1);
				} else {
					sprintf_s(ch1, 50, "Device%02d", bvar.GetIndex());
					bvar.SetDeviceLabel(ch1);
				}

				BACnetList.push_back(bvar);

				// MAG 241127 add check for <point> data as part of Taylor/CalNEXT upgrade
				/* NamedPoint has:
					std::string	label;	// as read from device property
					std::string alias;	// if used, alias label from config file or set by user
					int		object;		// BACnet object type (i.e. AI=0, BV=5, etc.)
					int		instance;	// instance of BACnet object
					int		bacdevice;	// identifier for device, index into BACnetList
				*/
				subnode2 = filex.element_by_name(subnode, "point");
				while (subnode2 != NULL) { // found a point
					if (subnode2->name.compare(0, 3, "!--") == 0) {
						subnode2 = subnode2->next;
						continue;
					}

					myprop = filex.getprop(subnode2, "type");	// object
					if (myprop != NULL) {
						i = FindBACnetObjectType(myprop->data);
						mynp.SetBACnetObject(i);
					}
					else mynp.SetBACnetObject(0);

					myprop = filex.getprop(subnode2, "id");		// instance
					if (myprop != NULL) {
						mynp.SetInstance(stoi(myprop->data));
					}
					else mynp.SetInstance(0);

					myprop = filex.getprop(subnode2, "label");	// label
					if (myprop != NULL) {
						mynp.SetLabel(myprop->data);
					}
					else mynp.SetLabel("NA");

					myprop = filex.getprop(subnode2, "description");	// description
					if (myprop != NULL) {
						mynp.SetDescription(myprop->data);
					}
					else mynp.SetDescription("NA");

					myprop = filex.getprop(subnode2, "alias");	// alias
					if (myprop != NULL) {
						mynp.SetAlias(myprop->data);
					}
					//else mynp.SetAlias("NA"); // leave it blank

					mynp.SetDevice(deviceID);					// bacdevice

					namedPointList.push_back(mynp);

					subnode2 = subnode2->next; // look for next point
				} // end of entering named point list

				subnode = subnode->next;
				deviceID++;
			} // end while subnode != NULL
			x1 = NULL;
		}
	}
	catch (const std::exception& e) {
		std::stringstream ss1;
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Caught exception in ReadFromFile (1)!");
		ss1 << "ERROR: " << e.what();
		cs1 = ss1.str().c_str();
		theApp.m_BACdlg->m_status.AddString(cs1);
		errorsFound++;
	}
	catch (...) {
		theApp.m_BACdlg->m_status.AddString(L"Error Reading Devices!  Please review the devices in the input file for format.");
		errorList.Add(L"Error Reading Devices!  Please review the devices in the input file for format.");
		errorsFound++;
	}
	devCount = BACnetList.size();

	//time conversion from 
	//http://arsenmk.blogspot.com/2014/07/converting-string-to-datetime-and-vice.html
	static const std::string dateTimeFormat("%Y-%m-%dT%H:%M:%S");
	std::stringstream ss;// dateTime is a std::wstring { dateTime };
	std::tm dt = {};
	std::tm* pdt;

	// READ IN VARIABLES MAG 180702
	try {
		x1 = filex.element_by_name(FPTMroot, "variables");

		if (x1 != NULL) {
			myprop = filex.getprop(x1, "delete");
			if (myprop != NULL) {
				if (myprop->data.compare("1") == 0)
					VARlist.clear();
			}
			varCount = VARlist.size();

			subnode = filex.element_by_name(x1, "var");
			while (subnode != NULL) {
				// read label
				evnode = filex.element_by_name(subnode, "label");
				if (evnode != NULL) {
					sprintf_s(ch1, 50, "%s", evnode->data.c_str());
					mvar.SetLabel(ch1);
				} else {
					sprintf_s(ch1, 50, "var%02d", varCount);
					mvar.SetLabel(ch1);
					mac.Format(L"NOTE- Variable was declared with no name.  Using default name '%S'", ch1);
					theApp.m_BACdlg->m_status.AddString(mac);
				}

				// read value if present
				evnode = filex.element_by_name(subnode, "value");
				if (evnode != NULL) {
					sprintf_s(ch1, 50, "%s", evnode->data.c_str());
					mvar.SetValue(ch1);
				} else {
					mvar.SetValue((double)0);  // if value has been set by previous iteraton
				}

				// read errBound if present
				evnode = filex.element_by_name(subnode, "errBound");
				if (evnode == NULL) evnode = filex.element_by_name(subnode, "errbound");
				if (evnode != NULL) {
					sprintf_s(ch1, 50, "%s", evnode->data.c_str());
					mvar.SetErrBound(ch1);
				} else {
					mvar.SetErrBound((double)0);  // default value 
				}

				subnode = subnode->next;
				VARlist.push_back(mvar);
			}
		} // end check for NULL
	}	// end try
	catch (const std::exception& e) {
		std::stringstream ss1;
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Caught exception in ReadFromFile! (2)");
		ss1 << "ERROR: " << e.what();
		cs1 = ss1.str().c_str();
		theApp.m_BACdlg->m_status.AddString(cs1);
	}
	catch (...) {
		theApp.m_BACdlg->m_status.AddString(L"Error Reading Variables!  Please review the variables in the input file for format.");
		errorList.Add(L"Error Reading Variables!  Please review the variables in the input file for format.");
		errorsFound++;
	}

	// READ IN EVENTS
	deviceID = 0;
	try {
		x1 = filex.element_by_name(FPTMroot, "events");

		while ((x1 != NULL) && (x1->name.compare("events") == 0)) {
			TRlist.push_back(localTR); // this is accessed by TRlist[evLoop]
			TRdata.push_back(trdInit);
			rampList.push_back(rampInit);
			TESTlist.push_back(tlInit);
			eventTimerTestList.push_back(dvtlInit);
			testSteps.push_back(tsInit);

			if (x1 != NULL) {
				myprop = filex.getprop(x1, "delete");	// MAG 160705 added to events node
				if (myprop != NULL) {
					if (myprop->data.compare("1") == 0)
						TRlist[evLoop].clear();
				}

				myprop = filex.getprop(x1, "reset");	// MAG reset was used originally, but repeat makes more sense.
				if (myprop == NULL) myprop = filex.getprop(x1, "repeat");
				if (myprop == NULL) {
					TRlistRepeat = 0;
					theApp.m_BACdlg->m_status.AddString(L"Note- Repeat not set.  Events in file will only run one time.");
				} else {
					TRlistRepeat = atoi(myprop->data.c_str());
					if (TRlistRepeat > 0) {
						mac.Format(L"Settings Change: Event loop WILL loop.");
						theApp.m_BACdlg->m_status.AddString(mac);
					}
				}
				theApp.m_BACdlg->SetRepeatEventListMenu();

				devCount = TRlist[evLoop].size();

				// MAG 241028 read metadata for event list
				evnode = filex.element_by_name(x1, "label");
				if (evnode != NULL) {
					sprintf_s(ch1, 100, "%s", evnode->data.c_str());
					cs1 = ch1;
					mtrd.SetLabel(cs1);
				}
				evnode = filex.element_by_name(x1, "testIdentifier");
				if (evnode != NULL) {
					sprintf_s(ch1, 50, "%s", evnode->data.c_str());
					cs1 = ch1;
					mtrd.SetTestIdentifier(cs1);
				}
				TRdata.push_back(mtrd);
				mtrd.Clear();

				// start out on the first event or step node
				evnode = filex.element_by_name(x1, "event");
				if(evnode == NULL) evnode = filex.element_by_name(x1, "step");
				while (evnode != NULL) {
					/*  // MAG 161013 remove use of device id parameter, now just uses as an index
					NOTE- id parameter is not even read by FPTM, it's just for user convenience when editing XML file
					}*/

					// Process event node
					if (evnode->name.compare("event") == 0) {

						subnode = filex.element_by_name(evnode, "eventTime");
						if (subnode == NULL) subnode = filex.element_by_name(evnode, "EVENTTIME");
						if (subnode == NULL) subnode = filex.element_by_name(evnode, "eventtime");
						if (subnode == NULL) subnode = filex.element_by_name(evnode, "Eventtime");
						if (subnode == NULL) subnode = filex.element_by_name(evnode, "EventTime");
						if (subnode != NULL) {
							ss.clear();
							myprop = filex.getprop(subnode, "type");
							if (myprop == NULL) myprop = filex.getprop(subnode, "Type");
							if (myprop != NULL) { // check if it's offset type
								if (tstart == 0) {
									tstart = std::time(0);
									tstart = tstart - tstart % 60;	// round down to nearest minute
									tstart += 60;					// bump to next minute
								}
								if (myprop->data.compare("offset") == 0) {	// found offset time (preferred moving forward)
									i = atoi(subnode->data.c_str());
									if (i < 0) {
										i = 0;
										errorList.Add(L"ERROR: Negative offset time in event node. Offset time set to zero.");
									}
									if (i > 86400) {
										i = 0;
										errorList.Add(L"ERROR: One day+ offset time in event node. Offset time set to zero.");
									}
									if ((subnode->data.find(':') != std::string::npos) || (subnode->data.find('-') != std::string::npos)) {
										i = 0;
										errorList.Add(L"ERROR: Offset time not formatted correctly (find : or -). Offset time set to zero.");
									}
									// set node time to start + offset
									ttemp = tstart + i;
									pdt = std::localtime(&ttemp);
									mtr.SetEventTime(pdt);
								}
								else if (myprop->data.compare("interval") == 0) { // found interval time
									i = atoi(subnode->data.c_str());
									if (i < 0) {
										i = 0;
										errorList.Add(L"ERROR: Negative interval time in event node. Interval time set to zero.");
									}
									if (i > 86400) {
										i = 0;
										errorList.Add(L"ERROR: One day+ interval time in event node. Interval time set to zero.");
									}
									if ((subnode->data.find(':') != std::string::npos) || (subnode->data.find('-') != std::string::npos)) {
										i = 0;
										errorList.Add(L"ERROR: Interval time not formatted correctly (find : or -). Interval time set to zero.");
									}
									// set node time to previous + offset
									if (prevNodeTime == 0) prevNodeTime = tstart;
									ttemp = prevNodeTime + i;
									pdt = std::localtime(&ttemp);
									mtr.SetEventTime(pdt);
								} else {	// node contains time/date string - DEFAULT TYPE (NOTE- exact copy of default type code below!)
									ss << subnode->data.c_str();
									ss >> std::get_time(&dt, dateTimeFormat.c_str());
									if (ss.fail()) {
										errorList.Add(L"ERROR: eventTime node found with unreadable date/time. Time set to start.");
										errorsFound++;
										pdt = std::localtime(&tstart);
										mtr.SetEventTime(pdt); // on error set to start time
									} else {	// else set time as read from file
										mtr.SetEventTime(&dt);
										tstart = mktime(&dt);
									}
								}  // end else node type parameter not known type
							} else {	// node contains time/date string - DEFAULT TYPE
								ss << subnode->data.c_str();
								ss >> std::get_time(&dt, dateTimeFormat.c_str());
								if (ss.fail()) {
									errorList.Add(L"ERROR: eventTime node found with unreadable date/time. Time set to start.");
									errorsFound++;
									pdt = std::localtime(&tstart);
									mtr.SetEventTime(pdt); // on error set to start time
								}
								else {	// else set time as read from file
									mtr.SetEventTime(&dt);
									tstart = mktime(&dt);
								}
							}
						} else {	// if there is no eventTime subnode
							//TRlist[evLoop][deviceID].SetEventTime(NULL);	// sets to now
							if (tstart == 0) {
								tstart = std::time(0);			// get current time
								tstart = tstart - tstart % 60;	// round down to nearest minute
								tstart += 60;					// bump to next minute
								mtr.SetEventTime(&dt);
							} else {
								mtr.SetEventTime(prevNodeTime);	// set to same time as previous event
							}
						}
						(mtr.GetEventTime()).GetLocalTm(&pntm);
						prevNodeTime = mktime(&pntm);

						// check for point, else check for object/instance/device
						subnode = filex.element_by_name(evnode, "point");
						if (subnode == NULL) subnode = filex.element_by_name(evnode, "Point");
						if (subnode != NULL) {
							NamedPoint mynp;
							myprop = filex.getprop(subnode, "alias");
							if (myprop == NULL) myprop = filex.getprop(subnode, "Alias");
							if (myprop == NULL) myprop = filex.getprop(subnode, "id");
							if (myprop == NULL) myprop = filex.getprop(subnode, "ID");
							if (myprop != NULL) {
								mynp = FindNamedPoint(myprop->data);
								if (mynp.GetBACnetObject() >= 0) {  // check that it's valid, object == -1 for invalid
									mtr.SetBACnetObject(mynp.GetBACnetObject());
									mtr.SetInstance(mynp.GetInstance());
									mtr.SetDevice(mynp.GetDevice());
								}
							}
						} else { // search for object/instance/device
							bool1 = 0;
							subnode = filex.element_by_name(evnode, "bacdevice");
							if (subnode == NULL) subnode = filex.element_by_name(evnode, "BACDEVICE");
							if (subnode == NULL) subnode = filex.element_by_name(evnode, "BACdevice");
							if (subnode != NULL) {
								// MAG this appears to not be setting the correct value
								i = atoi(subnode->data.c_str());	// read index value of bacdev, then find position in array
								i += initBACnetCount;  // offset for number of controllers already read in before file
								for (int ii = 0; ii < (int)BACnetList.size(); ii++) {
									if (BACnetList[ii].GetIndex() == i) {
										i = ii;
										bool1 = 1;  // found it!
										break;
									}
								}
								if (bool1 == 0) {
									theApp.m_BACdlg->m_status.AddString(L"Error- In device setting for event, device not found.");
									errorList.Add(L"Error- In device setting for event, device not found.");
									i = 0;
									errorsFound++;
								}
								mtr.SetDevice(i);
							} else {
								theApp.m_BACdlg->m_status.AddString(L"Error- Find event with no device setting, please review settings file for errors.  Setting to device 0.");
								errorList.Add(L"Error- Find event with no device setting, please review settings file for errors.  Setting to device 0.");
								mtr.SetDevice(0);
								errorsFound++;
							}

							subnode = filex.element_by_name(evnode, "object");
							if (subnode == NULL) subnode = filex.element_by_name(evnode, "OBJECT");
							if (subnode == NULL) subnode = filex.element_by_name(evnode, "Object");
							if (subnode != NULL) {
								i = FindBACnetObjectType(subnode->data);  // MAG 241127 add function to replace functionality moved to function
								mtr.SetBACnetObject(i);
							} else {
								theApp.m_BACdlg->m_status.AddString(L"Error- Find event with no BACnet object type, please review settings file for errors.  Setting to AI.");
								errorList.Add(L"Error- Find event with no BACnet object type, please review settings file for errors.  Setting to AI.");
								mtr.SetBACnetObject(0);
								errorsFound++;
							}

							subnode = filex.element_by_name(evnode, "instance");
							if (subnode == NULL) subnode = filex.element_by_name(evnode, "INSTANCE");
							if (subnode == NULL) subnode = filex.element_by_name(evnode, "Instance");
							if (subnode != NULL) {
								i = atoi(subnode->data.c_str());
								mtr.SetInstance(i);
							} else {
								theApp.m_BACdlg->m_status.AddString(L"Error- Find event with no object instance, please review settings file for errors.  Setting to 0.");
								errorList.Add(L"Error- Find event with no object instance, please review settings file for errors.  Setting to 0.");
								mtr.SetInstance(0);
								errorsFound++;
							}
						}  // end else device/object/instance

						subnode = filex.element_by_name(evnode, "readwrite");
						if (subnode == NULL) subnode = filex.element_by_name(evnode, "READWRITE");
						if (subnode == NULL) subnode = filex.element_by_name(evnode, "readWrite");
						if (subnode == NULL) subnode = filex.element_by_name(evnode, "ReadWrite");
						if (subnode != NULL) {
							// MAG 200506 add capability to use r or w, instead of 0 or 1
							switch (subnode->data.at(0)){
								case '0':	// read value
								case 'r':
								case 'R':
									mtr.SetRW(0);
									break;
								case '1':	// write value
								case 'w':
								case 'W':
									mtr.SetRW(1);
									break;
								case '2':	// reset value
								case 'x':
								case 'X':
									mtr.SetRW(2);
									break;
								default:
									theApp.m_BACdlg->m_status.AddString(L"Error- Find event with invalid read/write, please review settings file for errors.  Setting to 0.");
									errorList.Add(L"Error- Find event with invalid read/write, please review settings file for errors.  Setting to read.");
									mtr.SetRW(0);
									errorsFound++;
									break;
							}
						} else {
							mtr.SetRW(0);
						}

						subnode = filex.element_by_name(evnode, "value");
						if (subnode == NULL) subnode = filex.element_by_name(evnode, "VALUE");
						if (subnode == NULL) subnode = filex.element_by_name(evnode, "Value");
						if (subnode != NULL) {
							di = atof(subnode->data.c_str());
							mtr.SetValue(di);
						} else {
							mtr.SetValue(0);
						}

						subnode = filex.element_by_name(evnode, "scaleFactor");
						if (subnode == NULL) subnode = filex.element_by_name(evnode, "scalefactor");
						if (subnode == NULL) subnode = filex.element_by_name(evnode, "SCALEFACTOR");
						if (subnode != NULL) {
							di = atof(subnode->data.c_str());
							mtr.SetScaleFactor(di);
							mtr.SetScaleAO(1);
						} else {
							mtr.SetScaleFactor(1);
							mtr.SetScaleAO(0);
						}

						subnode = filex.element_by_name(evnode, "comment");
						if (subnode == NULL) subnode = filex.element_by_name(evnode, "COMMENT");
						if (subnode != NULL) {
							mac = subnode->data.c_str();  // reusing mac
							mtr.SetComment(mac);
						} else {
							mac = "";
							mtr.SetComment(mac);
						}

						subnode = filex.element_by_name(evnode, "var");
						if (subnode == NULL) subnode = filex.element_by_name(evnode, "VAR");
						if (subnode == NULL) subnode = filex.element_by_name(evnode, "Var");
						if (subnode != NULL) {
							// read label
							subnode2 = filex.element_by_name(subnode, "label");
							if (subnode2 == NULL) subnode2 = filex.element_by_name(subnode, "LABEL");
							// find matching VAR, set index
							if (subnode2 == NULL) {
								if (subnode->data.length() > 0)
									subnode2 = subnode; // MAG 190809 allow variable name to be saved in <var>, deprecate <label>
							}
							i = 0;
							if (subnode2 != NULL) {
								sprintf_s(ch1, 50, "%s", subnode2->data.c_str());
								std::vector<FPvar>::iterator it;
								for (it = VARlist.begin(); it != VARlist.end(); it++)
								{
									if (it->IsVar(ch1)) {
										mtr.SetFPV(i);	// save index to variable
										i = -1;
										break;
									}
									i++;
								}
							} else {
								theApp.m_BACdlg->m_status.AddString(L"Error- Find event with missing variable label, please review settings file for errors.");
								errorList.Add(L"Error- Find event with missing variable label, please review settings file for errors.");
								errorsFound++;
							}

							if (i != -1) {
								mac.Format(L"Error- Find event with invalid variable reference (variable '%S' not declared), please review settings file for errors.", ch1);
								errorList.Add(mac);
								theApp.m_BACdlg->m_status.AddString(mac);
								errorsFound++;
							}

							// read value if present
							subnode = filex.element_by_name(evnode, "modvalue");
							if (subnode == NULL) subnode = filex.element_by_name(evnode, "ModValue");
							if (subnode == NULL) subnode = filex.element_by_name(evnode, "value");
							if (subnode == NULL) subnode = filex.element_by_name(evnode, "Value");
							if (subnode == NULL) subnode = filex.element_by_name(evnode, "VALUE");
							if (subnode != NULL) {
								sprintf_s(ch1, 50, "%s", subnode->data.c_str());
								di = atof(ch1);
								mtr.SetFPVValue(di);
								hasValueNode = 1;  // for has value node
							} else {
								mtr.SetFPVValue((double)0);
								hasValueNode = 0;  // for has no value node
							}

							// read mod var if present
							subnode = filex.element_by_name(evnode, "modvar");
							if (subnode == NULL) subnode = filex.element_by_name(evnode, "ModVar");
							if (subnode == NULL) subnode = filex.element_by_name(evnode, "MODVAR");
							if (subnode != NULL) {
								sprintf_s(ch1, 50, "%s", subnode->data.c_str());
								i = 0;
								for (it = VARlist.begin(); it != VARlist.end(); it++) {
									if (it->IsVar(ch1)) {
										mtr.SetFPVModvar(i);	// save index to variable
										i = -1;
										break;
									}
									i++;
								}
								hasValueNode += 2;  // for has value node
							}

							// read source column if present
							subnode = filex.element_by_name(evnode, "sourceCol");
							if (subnode == NULL) subnode = filex.element_by_name(evnode, "SourceCol");
							if (subnode == NULL) subnode = filex.element_by_name(evnode, "sourcecol");
							if (subnode != NULL) {
								sprintf_s(ch1, 50, "%s", subnode->data.c_str());
								mtr.SetSourceColumn(atoi(ch1));
							}

							// MAG 230516 add procvar node
							// read process var if present
							subnode = filex.element_by_name(evnode, "procvar");
							if (subnode == NULL) subnode = filex.element_by_name(evnode, "PROCVAR");
							if (subnode != NULL) {
								sprintf_s(ch1, 50, "%s", subnode->data.c_str());
								i = 0;
								for (it = VARlist.begin(); it != VARlist.end(); it++) {
									if (it->IsVar(ch1)) {
										mtr.SetFPVProcvar(i);	// save index to variable
										i = -1;
										break;
									}
									i++;
								}
								hasValueNode += 2;  // for has value node
							}

							// read operator if present
							subnode = filex.element_by_name(evnode, "operator");
							if (subnode == NULL) subnode = filex.element_by_name(evnode, "OPERATOR");
							if (subnode == NULL) subnode = filex.element_by_name(evnode, "Operator");
							if (subnode != NULL) {
								// variable relation to value: 1:add 2:subtract 3:multiply 4:divide by 5:set to
								hasOperatorNode = 1;
								sprintf_s(ch1, 50, "%s", subnode->data.c_str());
								switch (ch1[0]) {
									case '1':
									case '+': mtr.SetFPVOperator(1); break;
									case '2':
									case '-': mtr.SetFPVOperator(2); break;	// var - modvalue
									case '3':
									case 'x':
									case '*': mtr.SetFPVOperator(3); break;
									case '4':
									case '/': mtr.SetFPVOperator(4); break;	// var / modvalue
									case '5': mtr.SetFPVOperator(5); break;	// this is subtraction with the order reversed, modvalue - var
									case '6': mtr.SetFPVOperator(6); break; // this is division with the order reversed, modvalue / var
									default:
										mac.Format(L"Error- Find event with invalid operator ('%S'), please review settings file for errors.", ch1);
										theApp.m_BACdlg->m_status.AddString(mac);
										errorList.Add(mac);
										errorsFound++;
										mtr.SetFPVOperator(0); 
										break;
								}
								if (mtr.GetRW() == 0) {
									mac.Format(L"Note- Find read event variable with operator node.  This may indicate a configuration error.");
									errorsFound++;  // just a note, but should be indicated
									errorList.Add(mac);
								}
							} else {
								hasOperatorNode = 0;
								mtr.SetFPVOperator(0);  // resets if value has been set by previous iteraton
							}

							if (hasOperatorNode && !hasValueNode) {  // has value node but no operator node
								mac.Format(L"Note- Find event variable with operator node but no value node.  This may indicate a configuration error.");
								errorsFound++;  // just a note, but should be indicated
								errorList.Add(mac);
							}
						} else {
							mtr.SetFPV(-1);// end if var node != NULL
							mtr.SetFPVOperator(0);
							mtr.SetFPVValue((double)0);
						}

						TRlist[evLoop].push_back(mtr);
						deviceID++;
					} else if (evnode->name.compare("step") == 0) {  // found a test step node!
						// MAG 250424 Check for step
						// create test step local struct
						// read eventList
						subnode = filex.element_by_name(evnode, "eventList");
						if (subnode == NULL)	subnode = filex.element_by_name(evnode, "eventlist");
						if (subnode == NULL)	subnode = filex.element_by_name(evnode, "Eventlist");
						if (subnode != NULL) {
							str1 = subnode->data;
							convert << str1;
							while (std::getline(convert, str1, ',')) {
								myts.AddToList(std::stoi(str1));
							}
						}

						// read label
						subnode = filex.element_by_name(evnode, "eventList");
						if (subnode != NULL) {
							myts.SetLabel(subnode->data);
						} else {
							myts.SetLabel("No Label");
						}

						// read interval
						subnode = filex.element_by_name(evnode, "interval");
						if (subnode != NULL) {
							myts.SetTimeInterval(std::stoi(subnode->data));
						}  // else leave at default 0

						// read test id
						subnode = filex.element_by_name(evnode, "test");
						if (subnode != NULL) {
							myts.SetTest(std::stoi(subnode->data));
						}  // else leave at default -1

						// set times from first event
						// MAG TODO this line below does not work
						//myts.SetTime(TRlist[TRactive][myts.GetListMinEvent()].GetEventTime());

						// set status to default (not run)
						myts.SetStatus(0);

						// MAG 250424 TODO: read mvList
						// MAG 250424 TODO: read piList

						// add to testSteps[current]
						testSteps[TRactive].push_back(myts);
						myts.Reset();
					}  // end process test step node
					
					evnode = evnode->next;
				}  // end while evnode != NULL
				
				// increment to next node, probably another events loop (not a test step node)
				x1 = x1->next;
			}  // end while x1 != NULL or "events"
			evLoop++;
		}  // end while events loop
	} // end try loop
	catch (const std::exception& e) {
		std::stringstream ss1;
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Caught exception in ReadFromFile! (3)");
		ss1 << "ERROR: " << e.what();
		cs1 = ss1.str().c_str();
		theApp.m_BACdlg->m_status.AddString(cs1);
		errorList.Add(cs1);
		errorsFound++;
	}
	catch (...) {
		theApp.m_BACdlg->m_status.AddString(L"Error Reading Events!  Please review the events in the input file for format.");
		errorList.Add(L"Error Reading Events!  Please review the events in the input file for format.");
		errorsFound++;
	}

	evLoop = 0; // used to read in tests

	// READ IN TESTS
	x1 = filex.element_by_name(FPTMroot, "tests");
	if (x1 == NULL) x1 = filex.element_by_name(evnode, "TESTS");
	while ((x1 != NULL) && (x1->name.compare("events") == 0)) {
		myprop = filex.getprop(x1, "delete");
		if (myprop != NULL) {
			if (myprop->data.compare("1") == 0)
				TESTlist[evLoop].clear();
		}

		evnode = filex.element_by_name(x1, "test");
		if (evnode == NULL) evnode = filex.element_by_name(evnode, "TEST");
		while (evnode != NULL) {
			// read var node
			subnode = filex.element_by_name(evnode, "var");
			if (subnode == NULL) subnode = filex.element_by_name(evnode, "VAR");
			if (subnode == NULL) {
				theApp.m_BACdlg->m_status.AddString(L"ERROR: Test node has no variable (<var />) node.");
				errorList.Add(L"ERROR: Test node has no variable node.");
				evnode = evnode->next;
				continue;
			}
			subnode2 = filex.element_by_name(subnode, "label"); // label is optional, deprecated?
			if (subnode2 == NULL) subnode2 = filex.element_by_name(subnode, "LABEL");
			if (subnode2 != NULL) subnode2->name = "varlabel"; // MAG 230510 rename this subnode if it exists as it causes conflict with test->label node
			if (subnode2 == NULL) subnode2 = subnode; // use subnode2 either way
			sprintf_s(ch1, 50, "%s", subnode2->data.c_str());
			i = 0;
			for (it = VARlist.begin(); it != VARlist.end(); it++) {
				if (it->IsVar(ch1)) {
					tl1.SetVarIndex(i);	// save index to variable
					i = -1;
					break;
				}
				i++;
			}
			if (i > 0) {
				cs1.Format(L"ERROR: Variable specified in node (%S) not found.", ch1);
				theApp.m_BACdlg->m_status.AddString(cs1);
				evnode = evnode->next;
				errorList.Add(cs1);
				continue;
			}

			// read value node
			subnode = filex.element_by_name(evnode, "value");
			if (subnode == NULL) subnode = filex.element_by_name(evnode, "VALUE");
			if (subnode == NULL) {
				theApp.m_BACdlg->m_status.AddString(L"ERROR: Test node has no value node.");
				errorList.Add(L"ERROR: Test node has no value node.");
				evnode = evnode->next;
				continue;
			}
			if (subnode->data.length() == 0) {
				theApp.m_BACdlg->m_status.AddString(L"ERROR: Test node value node has no content.");
				errorList.Add(L"ERROR: Test node value node is empty.");
				evnode = evnode->next;
				continue;
			}
			di = std::stof(subnode->data);
			tl1.SetValue(di);

			// read relate node
			subnode = filex.element_by_name(evnode, "relate");
			if (subnode == NULL) subnode = filex.element_by_name(evnode, "RELATE");
			if (subnode == NULL) {
				theApp.m_BACdlg->m_status.AddString(L"ERROR: Test node has no relate node.");
				errorList.Add(L"ERROR: Test node has no relate node.");
				evnode = evnode->next;
				continue;
			}
			//strit = *subnode->data.begin();
			if (std::isalpha(subnode->data[0], loc)) {
				i = -5;
				if (subnode->data.compare("LT") == 0) i = -2;
				if (subnode->data.compare("LTE") == 0) i = -1;
				if (subnode->data.compare("EQ") == 0) i = 0;
				if (subnode->data.compare("GTE") == 0) i = 1;
				if (subnode->data.compare("GT") == 0) i = 2;
				if (subnode->data.compare("NE") == 0) i = 3;
				if (subnode->data.compare("MA") == 0) i = 4; // manual
				if (subnode->data.compare("ANY") == 0) i = 5; // manual
				if (i == -5) {
					cs1.Format(L"ERROR: Relate node has value out of range (%S).", subnode->data.c_str());
					theApp.m_BACdlg->m_status.AddString(cs1);
					evnode = evnode->next;
					errorList.Add(cs1);
					continue;
				}
			}
			else { // read in a numerical setting
				i = std::stoi(subnode->data);
				if ((i < -2) || (i > 5)) {
					theApp.m_BACdlg->m_status.AddString(L"ERROR: Relate node has value out of range.");
					evnode = evnode->next;
					errorList.Add(L"ERROR: Relate node has value out of range.");
					continue;
				}
			}
			tl1.SetRelate(i);

			// read trigger event node
			subnode = filex.element_by_name(evnode, "trigger");
			if (subnode == NULL) {
				tl1.SetTriggerEvent(-1);
			}
			else {
				i = std::stoi(subnode->data);
				tl1.SetTriggerEvent(i);
			}

			// read level node
			subnode = filex.element_by_name(evnode, "level");
			if (subnode == NULL) {
				tl1.SetLevel(-1);
			}
			else {
				i = std::stoi(subnode->data);
				tl1.SetLevel(i);
			}

			// read result node
			subnode = filex.element_by_name(evnode, "result");
			if (subnode == NULL) {
				tl1.SetResult(-1);
			}
			else {
				i = std::stoi(subnode->data);
				tl1.SetResult(i);
			}

			// read (optional) label node
			subnode = filex.element_by_name(evnode, "label");
			if (subnode != NULL) {
				tl1.Label = subnode->data.c_str();
			}
			else {
				tl1.Label = "";
			}

			// read test notes lines
			subnode = filex.element_by_name(evnode, "testNotes");
			while (subnode != NULL) {
				tl1.testNotes.push_back(subnode->data);

				if (subnode->next == NULL) break;
				subnode = subnode->next;
				if (subnode->data.compare("testNotes") != 0) break;
			}

			TESTlist[evLoop].push_back(tl1);
			evnode = evnode->next;

			if (VARlist.size() > 0) {
				VARlist[tl1.GetVarIndex()].GetLabel(ch1);
			} else {
				errorList.Add(L"ERROR: Variable list has not been read. Review file format.");
			}
			cs1.Format(L"NOTE: Read test %d: compare variable %S to value %5.2f", TESTlist[evLoop].size(), ch1, tl1.GetValue());
			theApp.m_BACdlg->m_status.AddString(cs1);
		}  // end while reading test nodes

		evLoop++;
		x1 = x1->next;
	}  // end tests loop

	// READ IN PRIORITY LEVEL
	x1 = filex.element_by_name(FPTMroot, "priorityLevel");
	if (x1 != NULL) x1 = filex.element_by_name(FPTMroot, "prioritylevel");
	if (x1 != NULL) x1 = filex.element_by_name(FPTMroot, "PRIORITYLEVEL");
	if (x1 != NULL) {
		i = atoi(x1->data.c_str());
		if ((i < 0) || (i > 16)) {
			mac.Format(L"ERROR in event script file- Priority Level in file is %d - Priority Level not changed.", i);
			theApp.m_BACdlg->m_status.AddString(mac);
			errorList.Add(mac);
			errorsFound++;
		} else {	// it's ok!
			writePriorityLevel = i;
			mac.Format(L"Settings Change: Priority Level set to %d.", i);
			theApp.m_BACdlg->m_status.AddString(mac);
		}
	}

	// READ IN SEND OUT-OF-SERVICE
	x1 = filex.element_by_name(FPTMroot, "OutOfService");
	if (x1 == NULL) x1 = filex.element_by_name(FPTMroot, "outofservice");
	if (x1 == NULL) x1 = filex.element_by_name(FPTMroot, "OUTOFSERVICE");
	if (x1 == NULL) x1 = filex.element_by_name(FPTMroot, "outOfService"); // preferred, to be consistent
	if (x1 != NULL) {
		// AI
		evnode = filex.element_by_name(x1, "ai");
		if (evnode == NULL)evnode = filex.element_by_name(x1, "AI");
		if (evnode != NULL) {
			i = atoi(evnode->data.c_str());
			sendOutOfService[AI] = i;
			mac.Format(L"Settings Change: Send out-of-service for AI set to %d.", i);
			theApp.m_BACdlg->m_status.AddString(mac);
		}
		// AO
		evnode = filex.element_by_name(x1, "ao");
		if (evnode == NULL)evnode = filex.element_by_name(x1, "AO");
		if (evnode != NULL) {
			i = atoi(evnode->data.c_str());
			sendOutOfService[AO] = i;
			mac.Format(L"Settings Change: Send out-of-service for AO set to %d.", i);
			theApp.m_BACdlg->m_status.AddString(mac);
		}
		// AV
		evnode = filex.element_by_name(x1, "av");
		if (evnode == NULL)evnode = filex.element_by_name(x1, "AV");
		if (evnode != NULL) {
			i = atoi(evnode->data.c_str());
			sendOutOfService[AV] = i;
			mac.Format(L"Settings Change: Send out-of-service for AV set to %d.", i);
			theApp.m_BACdlg->m_status.AddString(mac);
		}

		// BI
		evnode = filex.element_by_name(x1, "bi");
		if (evnode == NULL)evnode = filex.element_by_name(x1, "BI");
		if (evnode != NULL) {
			i = atoi(evnode->data.c_str());
			sendOutOfService[BI] = i;
			mac.Format(L"Settings Change: Send out-of-service for BI set to %d.", i);
			theApp.m_BACdlg->m_status.AddString(mac);
		}
		// BO
		evnode = filex.element_by_name(x1, "bo");
		if (evnode == NULL)evnode = filex.element_by_name(x1, "BO");
		if (evnode != NULL) {
			i = atoi(evnode->data.c_str());
			sendOutOfService[BO] = i;
			mac.Format(L"Settings Change: Send out-of-service for BO set to %d.", i);
			theApp.m_BACdlg->m_status.AddString(mac);
		}
		// BV
		evnode = filex.element_by_name(x1, "bv");
		if (evnode == NULL)evnode = filex.element_by_name(x1, "BV");
		if (evnode != NULL) {
			i = atoi(evnode->data.c_str());
			sendOutOfService[BV] = i;
			mac.Format(L"Settings Change: Send out-of-service for BV set to %d.", i);
			theApp.m_BACdlg->m_status.AddString(mac);
		}

		// MI
		evnode = filex.element_by_name(x1, "mi");
		if (evnode == NULL)evnode = filex.element_by_name(x1, "MI");
		if (evnode != NULL) {
			i = atoi(evnode->data.c_str());
			sendOutOfService[MSI] = i;
			mac.Format(L"Settings Change: Send out-of-service for MI set to %d.", i);
			theApp.m_BACdlg->m_status.AddString(mac);
		}
		// MO
		evnode = filex.element_by_name(x1, "mo");
		if (evnode == NULL)evnode = filex.element_by_name(x1, "MO");
		if (evnode != NULL) {
			i = atoi(evnode->data.c_str());
			sendOutOfService[MSO] = i;
			mac.Format(L"Settings Change: Send out-of-service for MO set to %d.", i);
			theApp.m_BACdlg->m_status.AddString(mac);
		}
		// MV
		evnode = filex.element_by_name(x1, "mv");
		if (evnode == NULL)evnode = filex.element_by_name(x1, "MV");
		if (evnode != NULL) {
			i = atoi(evnode->data.c_str());
			sendOutOfService[MSV] = i;
			mac.Format(L"Settings Change: Send out-of-service for MV set to %d.", i);
			theApp.m_BACdlg->m_status.AddString(mac);
		}
	}

	x1 = filex.element_by_name(FPTMroot, "LogFile");
	if (evnode == NULL) evnode = filex.element_by_name(x1, "logfile");
	if (evnode == NULL) evnode = filex.element_by_name(x1, "logFile");
	if (x1 != NULL) {
		SetEventSavefile(x1->data);
	}

	// END reading in file
	TRactive = 0;  // MAG 260326 add line
	theApp.m_BACdlg->UpdateDevices();
	theApp.m_BACdlg->UpdateEvents();
	theApp.m_BACdlg->m_status.AddString(L"NOTE- Event script file has been imported.");

	if (errorList.GetSize() > 0) {
		theApp.m_BACdlg->m_status.AddString(L"NOTE- ERRORS WERE FOUND IN EVENT SCRIPT FILE!!!");
		for (i = 0; i < errorList.GetSize(); i++)
		{
			theApp.m_BACdlg->m_status.AddString(errorList[i]);
		}
		theApp.m_BACdlg->m_status.AddString(L"NOTE- REVIEW ERRORS BEFORE RUNNING EVENT SCRIPT FILE!!!");
	}
	theApp.m_BACdlg->m_status.AddString(L"Input summary:");
	mac.Format(L"Devices: %d", BACnetList.size());
	theApp.m_BACdlg->m_status.AddString(mac);
	mac.Format(L"Named Points: %d", namedPointList.size());
	theApp.m_BACdlg->m_status.AddString(mac);
	mac.Format(L"Variables: %d", VARlist.size());
	theApp.m_BACdlg->m_status.AddString(mac);
	mac.Format(L"Events: %d", TRlist[TRactive].size());
	theApp.m_BACdlg->m_status.AddString(mac);
	mac.Format(L"Tests: %d", TESTlist[TRactive].size());
	theApp.m_BACdlg->m_status.AddString(mac);
	mac.Format(L"Errors: %d", errorList.GetSize());
	theApp.m_BACdlg->m_status.AddString(mac);

	theApp.m_BACdlg->m_status.AddString(L"Import complete.");

	theApp.m_BACdlg->m_status.SetCurSel(theApp.m_BACdlg->m_status.GetCount() - 1);
#ifdef USEAFDD
	theApp.m_BACdlg->OnBnClickedResettimes();
	theApp.m_BACdlg->OnBnClickedStart();
#endif

	return 1;
} // end FPTMbase::ReadFromFile


// MAG 
// Purpose: read in a Taylor/CalNEXT script file. Files are in Excel/XML format, i.e.:
/*<?mso-application progid="Excel.Sheet"?>
  <Workbook xmlns="urn:schemas-microsoft-com:office:spreadsheet" ... />
*/
int
FPTMbase::ReadCalNEXTScript()
{
	xml_element*	x1 = NULL;		// devices or events
	xml_element*	FileRoot = NULL;
	xml_element*	worksheet = NULL;
	xml_element*	xmlrow = NULL;
	xml_element*	xmlcell = NULL;
	xml_element*	subnode = NULL;	// subnode of device or event
	xml_element*	subnode2 = NULL;	// subnode of device or event
	xml_element*	evnode = NULL;	// event/device node
	xml_props*		myprop = NULL;
	// The XML will be read in to myWorksheet as strings, from there values will be extracted.
	std::vector<std::vector<std::string>>	myWorksheet; // the worksheet as a grid of strings
	std::vector<std::string>				curRow;			// use to build up row to add to myWorksheet
	int wsx = 0;	// x coordinate in myWorksheet
	int wsy = 0;	// y coordinate in myWorksheet
	std::string blankString;
	std::vector<std::vector<char>>	myStyleListGrid; // 0 for not a percent data cell, 1 if it is
	std::vector<char>				mySLGhelper;

	// row indexes for sections, need to be made dynamic but static now for testing
	// MAG Note- updated for draft 14, values are Excel row - 1
	// MAG Note- values are checked when reading input, lines between input/threshold/output (gray lines) must start with #
	int testBlockIndex = 5;
	int testStepIndex = 6;
	int clockTimeIndex = 42;
	int varNameIndex = 43;
	int varValueIndex = 44;
	int inputRowIndex = 7;  // first index for input rows in the XML file
	int outputRowIndex = 46;  // first index for output rows in the XML file
	int inputRowCount = 34;
	int outputRowCount = 38;

	int iCol = 3;				// iterator for parsing columns, start at column D
	int newBlockCycle = 0;		// indicates if a new block was found on the current input column
	//int	testStepColStart = 3;	// indicates column current test block started on
	int noMapPoints = 0;		// indicates number of points read where no point map was found- print error at end of sub
	int blockColumnOffset = 3;	// Column starting current block (start at 3)

	int		deviceID = 0;	// used as array index, for device currently being read
	int		j = 0;
	int		k = 0;
	double	dbv = 0.0;
	char	ch1[MAX_PATH] = {};		// used for misc and for save file
	int		numSteps = 0;	// total number of 
	int		curCol = 0;
	int		outputStepTestID =-1; // inputs can't have step tests, this holds the ID for the Output step
	int		totalEventCount = 0;	// equivalent to TRlist index, will show index of event being read in.

	CString mac;
	CString rmac;
	CString	cs1;
	std::string					prevTestBlock = "A";  // holds character indicating previous column test block when reading test block and step
	std::vector<std::string>	fileReport; // holds info about file/parsing to be displayed at end of subroutine- errors mostly
	std::vector<xml_element*>::iterator xit;

	// local classes used to push to end of main arrays
	PointVarMap					myPvm;
	FPvar						myVar;
	std::vector<TimedRequest>	mytrList;	// base list to use at each timestep
	DualVarTestList				mytl;
	DualVarTestList				mytldv;
	std::vector<DualVarTestList> mytlvec;
	NamedPoint					mynp; // use to connect input data loop to object/instance
	PeriodicInput				pi1;
	InterpolateValue			myInterp;
	TestStep					myTestStep;
	TimedRequest				mytreq;
	RampValue					myRampVal;

	std::vector<std::string> styleList;	// holds ss:ID value of styles with a % in the ss:Format field, bounds values are x100 below when read in from outputs;
	std::string		cellData;
	int				curTRlist = 0;
	int				curTestStep = 0; // Test step from input file, row 7, starts at 1
	CTime			timeThisCycle = CTime::CTime(); // baseTime + stepTotal, note input=start of cycle output=end of cycle
	CTime			baseTime = CTime::CTime(2025, 1, 1, 0, 0, 0); // Jan 1 2025, start time for all event lists
	CTimeSpan		stepInterval = 0;	// interval until next step, in seconds, from Clock Time (row 41 Draft 8)
	CTimeSpan		stepTotal = 0;		// total interval since start- add to baseTime for step time, increment between input and output
	CTimeSpan		tenSeconds = CTimeSpan(0, 0, 0, 10);
	CString			wallTime = theApp.GetDTS();  // formatted time string
	std::string		parse_string;
	std::stringstream	sstr;

	// set time to next minute
	timeThisCycle = baseTime;
	stepInterval = 0;

	// clear out existing TR list
	while (TRlist.size() > 1) {
		TRlist.pop_back();
		rampList.pop_back();
		TESTlist.pop_back();
		TRdata.pop_back();
		testSteps.pop_back();
		eventTimerTestList.pop_back();
	}

	TRactive = 0;
	while (TRlist[0].size() > 0) TRlist[0].pop_back();

	// clear out existing VAR list
	VARlist.clear();

	FileRoot = filex.getRoot();
	theApp.m_BACdlg->m_status.AddString(L"Reading CalNEXT format input file...");

	// using xmlrow even though we're not at rows yet.
	// Find style labels for percent data, they will require adjustment 
	xmlrow = filex.element_by_name(FileRoot, "Styles");
	if (xmlrow != NULL) {
		xmlrow = xmlrow->subelement;
		while (xmlrow != NULL) {
			x1 = filex.element_by_name(xmlrow, "NumberFormat");
			if (x1 != NULL) {
				myprop = filex.getprop(x1, "ss:Format");
				if (myprop != NULL) {
					if (myprop->data.find('%') != std::string::npos) { // found a '%'
						myprop = filex.getprop(xmlrow, "ss:ID");
						if (myprop != NULL) styleList.push_back(myprop->data);
					}
				}
			}
			xmlrow = xmlrow->next;  // move to next style
		}
	}

	worksheet = filex.element_by_name(FileRoot, "Worksheet");
	if (worksheet == NULL) {
		cs1.Format(L"ERROR: Worksheet node not found. File not loaded.");
		theApp.m_BACdlg->m_status.AddString(cs1);
		return 0;
	}
	// V2 transfer XML to myWorksheet
	// V2 transfer XML to myWorksheet
	// V2 transfer XML to myWorksheet
	xmlrow = filex.element_by_name(worksheet, "Row");// row 1 test script equipment
	// transfer info from XML to array of strings
	while (xmlrow != NULL) {
		xmlcell = filex.element_by_name(xmlrow, "Cell");
		while (xmlcell != NULL) {
			/* need to search for cell ss:Index="XX" for skipped cells! if(index found) then push back empty cells until size==index
			nedd to do this before code below*/
			myprop = filex.getprop(xmlcell, "ss:Index");
			if (myprop != NULL) {
				j = stoi(myprop->data);
				while ((int)curRow.size() < j - 1) {
					curRow.push_back(blankString);
				}
			}
			// TODO: search for formatting for % cells
			myprop = filex.getprop(xmlcell, "ss:StyleID");
			if (myprop != NULL) {
				auto it = std::find(styleList.begin(), styleList.end(), myprop->data);
				if (it != styleList.end()) { // found one!
					mySLGhelper.push_back('1');
				} else { 
					mySLGhelper.push_back('0'); 
				}
			} else {
				mySLGhelper.push_back('0');  // not a percent cell
			}

			if (xmlcell->subelement != NULL) {
				curRow.push_back(xmlcell->subelement->data);
			} else {
				curRow.push_back(blankString);
			}
			xmlcell = xmlcell->next;
		}
		for (size_t i = 0; i < curRow.size(); i++) {
			// debug line!!!
			//if (curRow[i].find("LAST") != std::string::npos) {
			//	j = 0;
			//}
			// end debug line
			j = 0;
			if (curRow[i].find("&amp;") != std::string::npos) {
				curRow[i] = std::regex_replace(curRow[i], std::regex("&amp;"), "&");
				j++;
			}
			if (curRow[i].find("&gt;") != std::string::npos) {
				curRow[i] = std::regex_replace(curRow[i], std::regex("&gt;"), ">");
				j++;
			}
			if (curRow[i].find("&lt;") != std::string::npos) {
				curRow[i] = std::regex_replace(curRow[i], std::regex("&lt;"), "<");
				j++;
			}
			if (curRow[i].find("&#10;") != std::string::npos) {
				curRow[i] = std::regex_replace(curRow[i], std::regex("&#10;"), "");
				j++;
			}
			if ((j == 0) && (curRow[i].find("&") != std::string::npos) && (curRow[i].find(";") != std::string::npos)) {
				j = 0; // debug
			}
		}  // end transfer XML to worksheet

		myWorksheet.push_back(curRow);
		curRow.clear();
		myStyleListGrid.push_back(mySLGhelper);
		mySLGhelper.clear();
		xmlrow = xmlrow->next;
	}  // end reading XML

	// MAG 250424 find index and count for input, time, outputs, etc.
	j = 7; // using j as an index counter
	while ((j < (int)myWorksheet.size()) && (myWorksheet[j][0].substr(0, 1).compare("#") != 0)) {
		j++;  // bump up to #Conditions line
	}
	inputRowCount = j - 7;
	j++;
	clockTimeIndex = j;
	j++; // 
	varNameIndex = j;
	j++;
	varValueIndex = j;
	j++;  // on #BACnet Expected Outputs line
	j++;
	outputRowIndex = j;
	outputRowCount = myWorksheet.size() - j;
	if (outputRowCount < 0) {  // this indicates an error
		cs1.Format(L"ERROR: Could not parse input file. Please check for format error.");
		theApp.m_BACdlg->m_status.AddString(cs1);
		return 0;
	}

	// read input labels and names
	for (size_t si = 0; si < (size_t)inputRowCount; si++) {
		myPvm.SetLongName(myWorksheet[si + inputRowIndex][0]);
		myPvm.SetPointName(myWorksheet[si + inputRowIndex][1]);
		myVar.SetLabel(myWorksheet[si + inputRowIndex][1]);
		myPvm.SetVariableNameIndex(si + numSteps);
		pvmInput.push_back(myPvm);
		myVar.SetErrBound(0.0);
		myVar.SetValue(0.0); // can't assign value here as some references may not be read in yet. //inputRows[si]->subelement->data);
		//MAG TODO 241126 set values when first column is read later
		VARlist.push_back(myVar);
	}

	// read output labels, names, and toleraces
	for (size_t si = 0; si < (size_t)outputRowCount; si++) {
		myPvm.SetLongName(myWorksheet[si + outputRowIndex][0]);
		myPvm.SetPointName(myWorksheet[si + outputRowIndex][1]);
		myVar.SetLabel(myWorksheet[si + outputRowIndex][1]);
		myPvm.SetVariableNameIndex(si + numSteps);
		pvmOutput.push_back(myPvm);

		myVar.SetErrBound(myWorksheet[si + outputRowIndex][2]);
		// check for % in style, adjust var if necessary
		if (myStyleListGrid[si + outputRowIndex][2] == '1') {
			dbv = myVar.GetErrBound();
			myVar.SetErrBound(dbv * 100.0);
		}

		myVar.SetValue(0.0); // can't assign value here as some references may not be read in yet. //inputRows[si]->subelement->data);
		//MAG TODO 241126 set values when first column is read later
		VARlist.push_back(myVar);
	}

	cs1.Format(L"Input file parameters: inputs: %d  outputs: %d", inputRowCount, outputRowCount);
	theApp.m_BACdlg->m_status.AddString(cs1);

	// need to fill in as many parts of TimedRequest mytreq as possible
	mytreq.SetRW(1);  // we're writing input values
	TRdata[0].SetLabel(myWorksheet[0][3]);
	TRdata[0].SetTestIdentifier(myWorksheet[testBlockIndex][3]);

	// START EVENTS
	// START EVENTS
	// START EVENTS
	// iterate through columns, read data, create events and event lists
	// start at column D ([3])
	for (iCol = 3; iCol < (int)myWorksheet[inputRowIndex].size(); iCol++) {
		// check for new test block
		newBlockCycle = 0;
		if (myWorksheet[testBlockIndex][iCol].compare(prevTestBlock) != 0) {
			OnRuntimeoptionsNewEventList();
			prevTestBlock = myWorksheet[testBlockIndex][iCol];		// this stores the test block info (i.e. test block A, B, etc. not the xml node
			curTRlist = TRlist.size() - 1;
			TRdata[curTRlist].SetLabel(myWorksheet[0][iCol]);
			TRdata[curTRlist].SetTestIdentifier(myWorksheet[testBlockIndex][iCol]);
			newBlockCycle = 1;
			timeThisCycle = baseTime;
			stepInterval = 0; // default interval, may be updated to other value below
			//testStepColStart = iCol;
			blockColumnOffset = iCol;
			// needs to add event list mytrlist to event list
		}

		// MAG 250312 add tracking test step
		curTestStep = std::stoi(myWorksheet[testStepIndex][iCol]);
		mytreq.SetTestStep(curTestStep);
		mytreq.SetSourceColumn((iCol - blockColumnOffset) +1);  // iCol starts at 3, column id starts at 1
		parse_string = myWorksheet[0][iCol] + ": " + myWorksheet[5][iCol] + myWorksheet[6][iCol];
		myTestStep.SetLabel(parse_string);
		outputStepTestID = -1;	// MAG 250820 init to -1 each cycle

		mytreq.SetFPV(VARlist.size()); // sets to last member of fpvar, var will be added in after

		// check the time for this step, note that read and write happen at different times. Read is done after the interval time.
		// Time interval for write test step is zero, time interval for read step is indicated in test script file
		// Interval is time between start of test and when read/write is performed.
		if (myWorksheet[clockTimeIndex][iCol].size() > 0) {  // NOTE- excel format for 1 minute is "1899-12-31T00:01:00.000"
			sstr = std::stringstream(myWorksheet[clockTimeIndex][iCol]);
			std::getline(sstr, parse_string, ':');
			parse_string = parse_string.substr(parse_string.size() - 2, 2);
			stepInterval = (long int)(3600 * stol(parse_string));	// hours -> seconds
			std::getline(sstr, parse_string, ':');
			stepInterval += (long int)(60.0 * stol(parse_string));		// minutes -> seconds
			std::getline(sstr, parse_string, ':');
			stepInterval += stol(parse_string);			// seconds
		} else {
			stepInterval = 60;	// default interval 60 seconds for OUTPUTS if not specified
		}

		// MAG 250912 note on timing- Span is time given for write and read steps- Write starts at 0s, read starts at 10s and goes to span-10s

		myTestStep.SetTime(baseTime + stepTotal);
		// MAG set interval to 0 so write events happen at start of step
		myTestStep.SetTimeInterval(0);//stepInterval);  // inputs have interval as specified in test, outputs interval=10
		timeThisCycle = baseTime + stepTotal;  // NOTE stepTotal updated after inputs are read in

		// check if there is a value treshold check (line 44 and 45 v11) at this timestep
		mytldv.Reset();
		mytldv.SetLevel(1);
		mytldv.SetSourceColumn(iCol - 2);  // MAG note not 3 because source column is for print and is 1-based not  0-based
		if (myWorksheet[varNameIndex][iCol].size() > 0) {
			mytldv.SetTriggerTime(timeThisCycle);
			parse_string = myWorksheet[varNameIndex][iCol];
			j = FindVariablePos(parse_string);
			if (j >= 0) {		// found var name, proceed with var value
				mytldv.SetVarIndex(j); // Set Left side of equation
				parse_string = myWorksheet[varValueIndex][iCol];
				switch (parse_string[0]) {
				case '<': // < or <=
					if (parse_string[1] == '=') mytldv.SetRelate(-2); // LTE
					else mytldv.SetRelate(-1);						// LT
					break;
				case '=': mytldv.SetRelate(0);						// EQ
					break;
				case '>': // > or >=
					if (parse_string[1] == '=') mytldv.SetRelate(1);	// GTE
					else mytldv.SetRelate(2);							// GT
					break;
				case '!': // not equal
					mytldv.SetRelate(3);								// NE
					break;
				default: // variable label or number
					mytldv.SetRelate(0);								// EQ again
					break;
				}
				// find number value
				j = FindVariablePos(parse_string); // MAY be a variable
				if (j >= 0) {	// it's a variable all right...
					mytldv.SetVarIndex2(j);  // Set Right side of equation
				} else { // just a number
					switch (parse_string[0]) {
					case '<':
					case '=':
					case '>':
						parse_string.erase(0, 1);
					} // default not needed, this switch is complete					
					if (parse_string[0] == '=') parse_string.erase(0, 1);
					dbv = stod(parse_string);
					mytldv.SetValue(dbv);  // Again for right side of equation
				}
				eventTimerTestList[TRactive].push_back(mytldv);
				outputStepTestID = eventTimerTestList[TRactive].size() - 1;
			} else {
				cs1.Format(L"ERROR: Found evaluation condition that could not parse. Variable Name unknown.");
				theApp.m_BACdlg->m_status.AddString(cs1);
				fileReport.push_back("ERROR: Found evaluation condition that could not parse. Variable Name unknown.");
			}
		} // end conditions for evaluation of test step

		// read input data
		// read input data
		// read input data
		// Read input data (starting in column D), create events
		// need to fill in as many parts of TimedRequest mytreq as possible
		mytreq.SetRW(1);  // we're writing input values
		mytreq.SetEventTime(timeThisCycle); // same as test step
		for (size_t i = 0; i < (size_t)inputRowCount; i++) {
			if ((!newBlockCycle) && (sparseEvents) && (iCol > 3)) {	// if sparseEvents check for writing the same value, skip if no change
				if (myWorksheet[i + inputRowIndex][iCol].compare(myWorksheet[i + inputRowIndex][iCol - 1]) == 0) {
					continue;
				}
			}

			mytreq.ResetLine();  // clears out strings and valueLock
			mytreq.SetRW(1);  // we're writing input values
			mytreq.SetEventTime(timeThisCycle); // same as test step
			mytreq.SetFPV(i);  // value sent to controller should be saved in VARlist- this may be overridden later

			cellData = myWorksheet[i + inputRowIndex][iCol];
			if(cellData.size() == 0){
				cs1.Format(L"ERROR: Found zero length cell (%d, %d). This indicates an error with the input file format or content. File import has stopped.", i + inputRowIndex + 1, iCol + 1);
				theApp.m_BACdlg->m_status.AddString(cs1);
				return -1;
			}
			switch (cellData[0]) {
				case 'a':	// ABSENT
				case 'A':
					mytreq.SetValue(0.0);
					if (iCol == 3) VARlist[i].SetValue(0.0);
					break;
				case 'C':	// CLOSED (for window, closed is 1 open is 0) or COOLDOWN
				case 'c':
					switch (cellData[1]) {
						case 'L':	// CLOSED
						case 'l':
							mytreq.SetValue(1.0);
							if (iCol == 3) VARlist[i].SetValue(1.0);
							break;
						case 'O':  // COOLDOWN
						case 'o':
							mytreq.SetValue(2.0);
							if (iCol == 3) VARlist[i].SetValue(2.0);
							break;
						default:
							cs1.Format(L"ERROR: Found cell with unknown content (%S). This indicates an error with the input file. File import has stopped.", cellData.c_str());
							theApp.m_BACdlg->m_status.AddString(cs1);
							return -1;
					}
					break;
				case 'N':	// NULL
				case 'n':
					mytreq.SetValue(0.0);
					if (iCol == 3) VARlist[i].SetValue(0.0);
					break;
				case 'O':	// OCCUPIED, ON, or OPEN (same result),or OFF
				case 'o':
					switch (cellData[1]) {
					case 'F': // OFF
					case 'f': 
					case 'P': // OPEN (for window, closed is 1 open is 0)
					case 'p':
						mytreq.SetValue(0);
						if (iCol == 3) VARlist[i].SetValue(0.0);
						break;
					default:	// Occupied, ON, everything else
						mytreq.SetValue(1.0);
						if (iCol == 3) VARlist[i].SetValue(1.0);
					}
					break;
				case 'P':	// PRESENT (Periodic is handled in '=' below
				case 'p':
					mytreq.SetValue(1.0);
					if (iCol == 3) VARlist[i].SetValue(1.0);
					break;
				case 'S':	// SETBACK or SETUP
				case 's':
					switch (cellData[1]) {
					case 'E': // SETBACK
					case 'e':
						mytreq.SetValue(5);
						if (iCol == 3) VARlist[i].SetValue(5.0);
						break;
					case 'U': // SETUP
					case 'u':
						mytreq.SetValue(3);
						if (iCol == 3) VARlist[i].SetValue(3.0);
						break;
					default:	// Occupied, ON, everything else
						mytreq.SetValue(1.0);
						if (iCol == 3) VARlist[i].SetValue(1.0);
					}
					break;
				case 'T':	// TRUE
				case 't':
					mytreq.SetValue(1.0);
					if (iCol == 3) VARlist[i].SetValue(1.0);
					break;
				case 'U':	// Unpopulated ?unoccupied?
				case 'u':
					mytreq.SetValue(6.0);
					if (iCol == 3) VARlist[i].SetValue(6.0);
					break;
				case 'W':	// WARMUP	TODO- verify 2 is appropriate value for warmup
				case 'w':
					mytreq.SetValue(4.0);
					if (iCol == 3) VARlist[i].SetValue(4.0);
					break;
				case 'Y':	// YES
				case 'y':
					mytreq.SetValue(1.0);
					if (iCol == 3) VARlist[i].SetValue(1.0);
					break;
				case '=':	// found a formula, this is where it gets fun...
					// MAG 241202 TODO!!!! 
					// might be in format "=VariableName" (put in subroutine as case is found in Output too)
					// special case of "=LAST" must also be handled
					parse_string = cellData;
					// remove '=' from beginning
					parse_string.erase(0, 1);
					//if (parse_string.substr(1, 0).compare("=") == 0)  parse_string = parse_string.substr(1, parse_string.size() - 2);
					j = FindVariablePos(parse_string);
					mytreq.SetUseValue(0);		// not using static value, will be variable, ramp, etc...
					if (j >= 0) {
						// writing- will retrieve variable value to write
						mytreq.SetFPV(j);
						mytreq.SetFPVOperator(0);	// no operator, just use value
						mytreq.SetFPVProcvar(i);
					} else {	// could be "=PERIODIC", "=RAMP", "=MULT", "=SUB", "=ADD", ="DIV"
						mytreq.SetFPVOperator(0);	// needed here too
						switch (parse_string[0]) {
							case 'A':
							case 'a': // =ADD
								if (ScanMathString(cellData, 1, &mytreq) == 0) {
									theApp.m_BACdlg->m_status.AddString(L"ERROR: Could not parse ADD statement.");
									fileReport.push_back("ERROR: Could not parse ADD statement:");
									fileReport.push_back(cellData);
									break;
								}
								mytreq.SetFPVProcvar(i);  // writes calculated value back to mytreq VAR if set
								break;
							case 'S':
							case 's': // =SUB
								if (ScanMathString(cellData, 2, &mytreq) == 0) {
									theApp.m_BACdlg->m_status.AddString(L"ERROR: Could not parse SUB statement.");
									fileReport.push_back("ERROR: Could not parse SUB statement:");
									fileReport.push_back(cellData);
									break;
								}
								mytreq.SetFPVProcvar(i);  // writes calculated value back to mytreq VAR if set
								break;
							case 'M':
							case 'm': // =MULT
								if(ScanMathString(cellData, 3, &mytreq) == 0) {
									theApp.m_BACdlg->m_status.AddString(L"ERROR: Could not parse MULT statement.");
									fileReport.push_back("ERROR: Could not parse MULT statement:");
									fileReport.push_back(cellData);
									break;
								}
								mytreq.SetFPVProcvar(i);  // writes calculated value back to mytreq VAR if set
								break;
							case 'D':
							case 'd': // =DIV
								if(ScanMathString(cellData, 4, &mytreq) == 0) {
									theApp.m_BACdlg->m_status.AddString(L"ERROR: Could not parse DIV statement.");
									fileReport.push_back("ERROR: Could not parse DIV statement:");
									fileReport.push_back(cellData);
									break;
								}
								mytreq.SetFPVProcvar(i);  // writes calculated value back to mytreq VAR if set
								break;
							case 'P': // =Periodic
							case 'p':
								if(ScanPeriodicString(cellData, &pi1) == 0){;
								theApp.m_BACdlg->m_status.AddString(L"ERROR: Could not parse Periodic statement.");
									fileReport.push_back("ERROR: Could not parse Periodic statement:");
									fileReport.push_back(cellData);
									break;
								}
								pi1.SetValueIndex(i);
								pi1.SetTRSource(totalEventCount); // MAG 251204 add tracking for event index
								myTestStep.piList.push_back(pi1); // MAG Note- myTestStep.piList needs to be maintained for Output test step (i.e. don't reset it)
								pi1.Reset();
								break;
							case 'r':
							case 'R': // =RAMP
								k = EvalRamp(cellData, mytrList.size() + TRlist[TRactive].size());// not subtracting 1 from size as this event will be appended to list
								if (k == 0) {  
									theApp.m_BACdlg->m_status.AddString(L"ERROR: Ramp specified with invalid parameters:");
									cs1.Format(L"-> %S", cellData.c_str());
									theApp.m_BACdlg->m_status.AddString(cs1);
									fileReport.push_back("ERROR: Ramp specified with invalid parameters");
									fileReport.push_back(cellData);
								} else {
									if (k == 2) { // MAG indicates "RAMP(LAST,...)"
										rampList[TRactive].back().SetVarStart(i);
									}
									mytreq.SetRampID(rampList[TRactive].size() - 1);
								}
								break;
							default:
								theApp.m_BACdlg->m_status.AddString(L"ERROR: Unknown input formula.");
								fileReport.push_back("ERROR: Unknown input formula:");
								fileReport.push_back(cellData);
								break;
						} // end switch parse_string[0]
					}  // end else
					break;  // end case '='
					//case '>':  MAG these are not used, there are not comparisons in the input
					//case '<':
					//	// MAG 241202 TODO!!!! 
					//	// probably a value like "<=50%"
					//	mytreq.SetValue(0.0);// placeholder only!!!
					//	break;
				//case 'R':	// RAMP (deprecated w/o leading =
				//case 'r':
				//	if (EvalRamp(cellData, mytrList.size() + TRlist[TRactive].size()) != 1) {  // not subtracting 1 from size as this event will be appended to list
				//		theApp.m_BACdlg->m_status.AddString(L"ERROR: Ramp specified with invalid parameters:");
				//		cs1.Format(L"-> %S", cellData.c_str());
				//		theApp.m_BACdlg->m_status.AddString(cs1);
				//		fileReport.push_back("ERROR: Ramp specified with invalid parameters:");
				//		fileReport.push_back(cellData);
				//	}
				//	//if (iCol == 3) VARlist[i].SetValue(0.0); MAG ramp not allowed in column 3
				//	break;
				default:  // reading in number, do not use variable
					mytreq.SetValue(stod(cellData));
					mytreq.SetValueLock();
					if (iCol == 3) VARlist[i].SetValue(stod(cellData));
					//mytreq.SetFPV(i); <-- NO to this!
					break;
			}  // end switch cellData[0]
			cs1 = myWorksheet[i + inputRowIndex][0].c_str();
			mytreq.SetComment(cs1);
			mynp = FindNamedPoint(VARlist[i].GetLabel());
			if (mynp.GetBACnetObject() >= 0) {	// check for valid value
				mytreq.SetBACnetObject(mynp.GetBACnetObject());
				mytreq.SetInstance(mynp.GetInstance());
				mytreq.SetDevice(mynp.GetDevice());
			} else {
				cs1.Format(L"NOTE: *** Read point '%S' (%d, %d) with no point mapping. ***", cellData.c_str(), 1+i + inputRowIndex,1+iCol);
				theApp.m_BACdlg->m_status.AddString(cs1);
				fileReport.push_back(std::string(CT2A(cs1)));//"ERROR: NOTE: Read point with no point mapping:");
				fileReport.push_back(cellData);
				mytreq.SetBACnetObject(0);
				mytreq.SetInstance(0);
				mytreq.SetDevice(0);
			}
			mytrList.push_back(mytreq);
			totalEventCount++;
		} // end for loop input count

		mytl.SetRelate(-3); // ==
		mytl.SetResult(-1);

		// MAG 250328 add intermediate write to TRlist and testSteps
		// MAG set Test Step event list ids
		for (size_t i = 0; i < mytrList.size(); i++) myTestStep.AddToList(i + TRlist[curTRlist].size());
		testSteps[TRactive].push_back(myTestStep);
		myTestStep.mvList.clear();
		//myTestStep.piList.clear();  // MAG 251211 let Periodic list from Input fall through to Output as they need to be run there too.
		myTestStep.CleartrList();

		TRlist[curTRlist].insert(TRlist[curTRlist].end(), mytrList.begin(), mytrList.end());
		mytrList.clear();


		// read output data
		// read output data
		// read output data
		// scan outputs and create read events, genreally copied from input version
		mytreq.ResetLine();
		mytreq.SetRW(0); // now we're reading
		stepTotal += stepInterval;  // stepTotal not used for output data timing
	
		// MAG 250912 modify so output data steps start 10s after input data steps. Total cycle time is span.
		myTestStep.SetTimeInterval(stepInterval - tenSeconds);  // Test will not be run until interval (from test start) is over (or step evaluation succeeds)
		myTestStep.SetTest(outputStepTestID);
		myTestStep.SetTime(timeThisCycle + tenSeconds);
		mytreq.SetEventTime(timeThisCycle + tenSeconds); // set time after interval when reading
		for (size_t i = 0; i < (size_t)outputRowCount; i++) {
			if (myWorksheet[i + outputRowIndex][iCol].find("LAST") != std::string::npos) {  // FOR DEBUG ONLY!!!
				j = 0;
			}
			if ((!newBlockCycle) && (sparseEvents) && (iCol > 3)) {	// if sparseEvents check for writing the same value, skip if no change
				if (myWorksheet[i + outputRowIndex][iCol].compare(myWorksheet[i + outputRowIndex][iCol - 1]) == 0) {
					continue;
				}
			}

			mytl.SetRelate(0);  // set test list to == as default
			mytl.SetLevel(1);	// no primary tests in first column

			mytreq.SetFPV(i + inputRowCount);	// start after input variables
			mytreq.SetValue(0.0);				// reset to default value

			cellData = myWorksheet[i + outputRowIndex][iCol];
			if (cellData.size() == 0) {
				cs1.Format(L"ERROR: Found zero length cell (%d, %d). This indicates an error with the input file format or content. File import has stopped.",i+outputRowIndex+1,iCol+1);
				theApp.m_BACdlg->m_status.AddString(cs1);
				return -1;
			}
			switch (cellData[0]) {
			//case 'A':	// ANY  MAG 250430 ANY without leading = deprecated in Draft 9
			//case 'a':
			//	// MAG 241205 TODO maybe set to int min and change relate to lt?
			//	mytreq.SetValue(0.0);
			//	mytl.SetRelate(5); 
			//	break;
			case 'C':	// CLOSED
			case 'c':
				mytreq.SetValue(0.0);
				break;
			//case 'L':	// LAST - last requires = sign
			//case 'l':
			//	mytreq.SetValue(1.0);
			//	break;
			case 'N':	// NULL
			case 'n':
				mytreq.SetValue(0.0);
				break;
			case 'O':	// OCCUPIED
			case 'o':
				mytreq.SetValue(1.0);
				break;
			case 'P':	// PRESENT
			case 'p':
				mytreq.SetValue(1.0);
				break;
			case 'Y':	// YES
			case 'y':
				mytreq.SetValue(1.0);
				break;
			case '=':	// found a formula, this is where it gets fun...
				// MAG 241202 TODO!!!! 
				// might be in format "=VariableName" (put in subroutine as case is found in Output too)
				// special case of "=LAST" must also be handled
				mytreq.SetFPVOperator(0);  // MAG 260113 add line here
				cellData.erase(0, 1);
				mytreq.SetUseValue(0);		// not using static value, will be variable, ramp, etc...
				j = FindVariablePos(cellData);
				if (j >= 0) {  // found a variable
					//mytreq.SetFPV(j);
					mytl.SetVarIndex2(j);
					if (iCol == 3) {
						VARlist[i + inputRowCount].SetValue(VARlist[j].GetValue());
					}
				} else {
					switch (cellData[0])
					{
					case 'A':
					case 'a': // =ADD or =ANY
						switch (cellData[1])
						{
							case 'D':  // ADD
							case 'd':
								ScanMathString(cellData, 0, &mytreq);
								break;
							case 'N':  // ANY
							case 'n':
								mytreq.SetValue(0.0);
								mytl.SetRelate(5);
								break;
							default:
								cs1.Format(L"ERROR: Unknown input formula with '='. %S",cellData.c_str());
								theApp.m_BACdlg->m_status.AddString(cs1);
								fileReport.push_back("ERROR: NOTE: Unknown imput formula:");
								fileReport.push_back(cellData);
								break;
						}
						ScanMathString(cellData, 0, &mytreq);
						break;
					case 'I':
					case 'i': // Interpolate
						// MAG TODO: check that interpolate is connected to mytreq (i.e. so it knows about the interpolate)
						ScanInterpolateString(cellData, &myInterp, &myTestStep);
						mytl.myInterpolate = myInterp;
						myInterp.Reset();
						break;
					case 'l':
					case 'L': // LAST
						if (iCol == 3) { // first column, read init as last value
							mytreq.SetValue(stod(myWorksheet[i + outputRowIndex][iCol]));
							break;
						}
						// else set to var value
						mytreq.SetFPV(i + inputRowCount);
						mytl.SetValue(mytreq.GetValue());  // primes value so prev value is filled below
						mytl.SetUsePrevV1(1);
						break;
					default:
						cs1.Format(L"ERROR: Unknown input formula with '='. %S", cellData.c_str());
						theApp.m_BACdlg->m_status.AddString(cs1);
						fileReport.push_back("ERROR: NOTE: Unknown imput formula:");
						fileReport.push_back(cellData);
						break;
					}
				}
				break;
			case '>':  // NOTE based on case for '='
				cellData.erase(0, 1);
				if (cellData[0] == '=') {
					mytl.SetRelate(1);  // >=
					cellData.erase(0, 1); // remove '='
				} else {
					mytl.SetRelate(2);  // >
				}
				j = FindVariablePos(cellData);

				if (j >= 0) {  // found a variable
					mytl.SetVarIndex2(j);
					if (iCol == 3) {
						VARlist[i + inputRowCount].SetValue(VARlist[j].GetValue());
					}
				} else {
					switch (cellData[0]) {
					case 'I':
					case 'i': // Interpolate
						// MAG TODO: check that interpolate is connected to mytreq (i.e. so it knows about the interpolate)
						ScanInterpolateString(cellData, &myInterp, &myTestStep);
						break;
					case 'l':
					case 'L': // LAST
						if (iCol == 3) { // first column, read init as last value
							mytreq.SetValue(stod(myWorksheet[i + outputRowIndex][iCol]));
							break;
						}
						// else set to var value
						mytreq.SetFPV(i + inputRowCount);
						mytl.SetValue(mytreq.GetValue());  // primes value so prev value is filled below
						mytl.SetUsePrevV1(1);
						break;
					default:// maybe a number?
						mytl.SetValue(stod(cellData));
						mytreq.SetValue(stod(cellData));
						break;
					}
				}
				break;
			case '<': // NOTE based on case for '='
				cellData.erase(0, 1);
				if (cellData[0] == '=') {
					mytl.SetRelate(-1);  // <=
					cellData.erase(0, 1); // remove '='
				} else {
					mytl.SetRelate(-2);  // <
				}
				j = FindVariablePos(cellData);

				if (j >= 0) {  // found a variable
					mytl.SetVarIndex2(j);
					if (iCol == 3) {
						VARlist[i + inputRowCount].SetValue(VARlist[j].GetValue());
					}
				} else {
					switch (cellData[0]) {
						case 'I':
						case 'i': // Interpolate
							// MAG TODO: check that interpolate is connected to mytreq (i.e. so it knows about the interpolate)
							ScanInterpolateString(cellData, &myInterp, &myTestStep);
							break;
						case 'l':
						case 'L': // LAST
							if (iCol == 3) { // first column, read init as last value
								mytreq.SetValue(stod(myWorksheet[i + outputRowIndex][iCol]));
								break;
							}
							// else set to var value
							mytreq.SetFPV(i + inputRowCount);
							mytl.SetValue(mytreq.GetValue());  // primes value so prev value is filled below
							mytl.SetUsePrevV1(1);
							break;
						default:// maybe a number?
							mytl.SetValue(stod(cellData));
							mytreq.SetValue(stod(cellData));
							break;
					}
				}
				break;
			case '&':
				// MAG 241202 TODO!!!! 
				// probably a value like "<=50%" or "&lt;=50%"
				mytreq.SetValue(0.0);// placeholder only!!!
				break;
			default:	// Just a number, probably most frequent case
				if((std::isalpha(cellData[0])) || (std::ispunct(cellData[0]))){  // check for alpha for puncuation that slipped through...
					cs1.Format(L"ERROR: can not parse cell: %S", cellData.c_str());  // %s does not work here
					theApp.m_BACdlg->m_status.AddString(cs1);
					mytreq.SetComment(cs1);
					mytl.SetRelate(4); // flag for manual review
					break;
				}
				
				if(myStyleListGrid[i + outputRowIndex][iCol] == '0')
					mytreq.SetValue(stod(cellData));// normal cell, no %
				else 
					mytreq.SetValue(100 * stod(cellData));// % cell
				mytreq.SetUseValue(1);  // MAG 260114 we are using the value

				if (iCol == 3) {
					VARlist[i + inputRowCount].SetValue(cellData);
				}
				break;
			}
			cs1 = myWorksheet[i + outputRowIndex][0].c_str();
			mytreq.SetComment(cs1);
			cs1 = myWorksheet[i + outputRowIndex][1].c_str();

			// finish config for mytl and add to list
			mytl.Label = cs1;
			if (iCol > 3) {
				if (cellData.compare(myWorksheet[i + outputRowIndex][iCol - 1]) == 0) {
					mytl.SetLevel(0);
				} else {
					mytl.SetLevel(1);
				}
			}
			mytl.SetValue(mytreq.GetValue());
			mytl.SetVarIndex(inputRowCount + i); // index starts after input rows
			mytl.SetSourceRow(i+1); // sRow is used for display, 1 based not 0 based
			mytl.SetSourceColumn(1 + iCol - blockColumnOffset);
			mytlvec.push_back(mytl);
			mytl.Reset();
			mytreq.SetRunTest((TESTlist[TRactive].size() + mytlvec.size()) - 1);  // sets index into TESTlist, accounting for mytlvec

			// finish config for mytreq and add to list
			mynp = FindNamedPoint(VARlist[i + inputRowCount].GetLabel());
			if (mynp.GetBACnetObject() >= 0) {	// check for valid value
				mytreq.SetBACnetObject(mynp.GetBACnetObject());
				mytreq.SetInstance(mynp.GetInstance());
				mytreq.SetDevice(mynp.GetDevice());
			} else {
				cs1.Format(L"NOTE: *** Read point '%S' (%d, %d) with no point mapping. ***", cellData.c_str(), 1 + i + outputRowIndex, 1 + iCol);
				theApp.m_BACdlg->m_status.AddString(cs1);
				fileReport.push_back(std::string(CT2A(cs1)));
				fileReport.push_back(cellData);
				mytreq.SetBACnetObject(0);
				mytreq.SetInstance(0);
				mytreq.SetDevice(0);
			}
			//else { MAG 250418 don't want to reset this here!
			//	mytreq.ResetLine(); // sets to init values, called if point map list is not loaded
			//}
			mytrList.push_back(mytreq);
			totalEventCount++;
			timeThisCycle = baseTime + stepTotal;  // increment for next cycle. Do not use timeThisCycle after this block.
		} // end reading output rows for this column

		// MAG set Test Step event list ids
		for (size_t i = 0; i < mytrList.size(); i++) myTestStep.AddToList(i + TRlist[curTRlist].size());
		testSteps[TRactive].push_back(myTestStep);
		myTestStep.Reset();

		TRlist[curTRlist].insert(TRlist[curTRlist].end(), mytrList.begin(), mytrList.end());
		mytrList.clear();

		// set last event of step as trigger for tests
		j = (TRlist[TRactive].size()) - 1;
		for (std::vector<DualVarTestList>::iterator it = mytlvec.begin();it != mytlvec.end(); it++)
			it->SetTriggerEvent(j);

		// add working test list to global test list
		TESTlist[TRactive].insert(TESTlist[TRactive].end(), mytlvec.begin(), mytlvec.end());
		mytlvec.clear();

	} // end iCol loop

	theApp.m_BACdlg->m_status.AddString(L"Finished reading CalNEXT format input file.");

	CString dts;
	std::string dtss;

	if (!reportT2.is_open()) InitLogFiles();

	if (reportT2.good()) {
		reportT2 << "TestStep report: Input file has " << testSteps.size() << " Blocks." << std::endl;
		for (size_t blocki = 0; blocki < testSteps.size(); blocki++) {
			reportT2 << "Block " << blocki + 1 << " report. There are " << testSteps[blocki].size() << " test steps in this block." << std::endl;
			for (size_t tsi = 0; tsi < testSteps[blocki].size(); tsi++) {
				baseTime = testSteps[blocki][tsi].GetTime();
				dts = theApp.GetDTS(baseTime);
				dts += '\t';
				dtss = std::string(CT2A(dts));

				cs1.Format(L"Block %d:%02d time: %s ## time offset: %d  Event range: %02d-%02d", blocki + 1, tsi, dts.GetString(), (int)testSteps[blocki][tsi].GetTimeInterval().GetTotalSeconds(),
					testSteps[blocki][tsi].GetListMinEvent(), testSteps[blocki][tsi].GetListMaxEvent());
				reportT2 << "Block " << blocki + 1 << ":" << tsi << " time : " << dtss << " ## time offset : ";
				reportT2 << (int)testSteps[blocki][tsi].GetTimeInterval().GetTotalSeconds() << "  Event range : " <<
					testSteps[blocki][tsi].GetListMinEvent() << " -" << testSteps[blocki][tsi].GetListMaxEvent() << std::endl;

				theApp.m_BACdlg->m_status.AddString(cs1);
			}
		}
		reportT2 << "Test Event report: Input file has " << TRlist.size() << " Blocks." << std::endl;
		for (size_t blocki = 0; blocki < TRlist.size(); blocki++) {
			reportT2 << "Block " << blocki + 1 << " report. There are " << TRlist[blocki].size() << " events in block." << std::endl;
			for (size_t tsi = 0; tsi < TRlist[blocki].size(); tsi++) {
				baseTime = TRlist[blocki][tsi].GetEventTime();
				dts.Format(L"%02d/%02d/%02d %02d:%02d:%02d\t", baseTime.GetMonth(), baseTime.GetDay(), baseTime.GetYear() - 2000, baseTime.GetHour(), baseTime.GetMinute(), baseTime.GetSecond());
				dtss = std::string(CT2A(dts));

				reportT2 << "Event " << blocki + 1 << ":" << tsi << " time is ";
				reportT2 << dtss;
				reportT2 << std::endl;
			}
		}
	}

	for (size_t i = 0; i < fileReport.size(); i++){
		cs1 = fileReport[i].c_str();
		theApp.m_BACdlg->m_status.AddString(cs1);
	}

	for (size_t i = 0; i < testSteps.size(); i++){
		cs1.Format(L"Times for test step list %d:", i);
		theApp.m_BACdlg->m_status.AddString(cs1);
		for (size_t j = 0; j < testSteps[i].size(); j++){
			stepInterval = testSteps[i][j].GetTimeInterval();
			cs1.Format(L"TS %d Step %d Interval %d", i, j,(int) stepInterval.GetTotalSeconds());
			theApp.m_BACdlg->m_status.AddString(cs1);
		}
	}

	ChangeActiveTRlist(1); // bumps TR list to 0 (first list)

	// This is important!
	if (noMapPoints > 0) {
		cs1.Format(L"%s NOTE: %d points were read with no mapping! Test will not run properly until these points are assigned!", wallTime.GetString(), noMapPoints);
		theApp.m_BACdlg->m_status.AddString(cs1);
		theApp.m_BACdlg->m_status.AddString(cs1);
		theApp.m_BACdlg->m_status.AddString(cs1);	// I said this is important!
	} else {
		cs1.Format(L"%s NOTE: All points were successfully mapped.", wallTime.GetString());
		theApp.m_BACdlg->m_status.AddString(cs1);
	}

	//// MAG 251209 add these to help with testing- vars not set default to 0
	//VARlist[28].SetValue(70);
	//VARlist[29].SetValue(75);
	//VARlist[30].SetValue(500);
	//VARlist[31].SetValue(100);
	//VARlist[32].SetValue(100);
	//VARlist[33].SetValue(120);
	//VARlist[34].SetValue(200);
	//VARlist[35].SetValue(100);
	//VARlist[36].SetValue(100);
	//VARlist[37].SetValue(55);
	//VARlist[38].SetValue(20);


	return 1;
}  // end ReadCalNEXTScript


// MAG 240528 add to enable device config by reading in BDS settings file
// NOTE- shamelessly copied from the BDS code to read in a settings file.
int
FPTMbase::ReadBDSSettingsFile(xml_element* bdsDataPointList)
{
	mxml			myxml;
	xml_element*	curdev = NULL;
	xml_element*	dpl = NULL;
	xml_props*		myprop = NULL;
	xml_props		nullProp;
	int				i = 0;
	int				pointCount = 0;
	int				deviceCount = 0;
	char			cjunk[MAX_PATH] = {};
	char*			chp = NULL;
	wchar_t			junk[MAX_PATH] = {};
	CString cs1;
	CString cs2;
	unsigned int	defaultPort = 0xBAC0; // MAG 220913 add this, set to 0xBAC0
	BACdev			lbd;	// used with BDS config files
	NamedPoint		mynp;

	// MAG 260227 modify to focus DEV on device object id, other network info not required. Does need either device object id OR MAC.
	nullProp.data = "NULL";
	curdev = myxml.element_by_name(bdsDataPointList, "dev");
	while ((curdev != NULL) && (curdev->name.compare("dev") == 0)) { //if(strcmp(curdev->name,"dev") == 0){  // current node is a dev
		lbd.Reset();

		myprop = myxml.getprop(curdev, "MAC");
		if (myprop == NULL) myprop = myxml.getprop(curdev, "mac");
		//if (myprop == NULL) {  // no recovering from this one!
		//	theApp.m_BACdlg->m_status.AddString(L"ERROR in config file: Device MAC address not found.");
		//	return 0;
		//}
		if (myprop != NULL) {
			sprintf_s(cjunk, "%s", myprop->data.c_str()); // check if this needs %s or %S
			lbd.SetMAC(cjunk);
		}

		myprop = myxml.getprop(curdev, "IP");
		if (myprop == NULL) myprop = myxml.getprop(curdev, "ip");
		if (myprop != NULL) {  // if there's an IP, also read port,  and router info
			sprintf_s(cjunk, "%s", myprop->data.c_str()); // check if this needs %s or %S
			lbd.SetIP(cjunk);

			// this only gets read in if there is an IP address
			myprop = myxml.getprop(curdev, "IPport");  // this defaults to 0xBAC0 if not present
			if (myprop == NULL) myprop = myxml.getprop(curdev, "ipport");
			if (myprop == NULL) lbd.SetIPPort(defaultPort);//0xBAC0;
			else {
				sprintf_s(cjunk, "%s", myprop->data.c_str()); // check if this needs %s or %S
				lbd.SetIPPort(cjunk); // check for hex etc. done in subroutine, how convenient! Also out of range is set to 0xBAC0
			}
		}

		myprop = myxml.getprop(curdev, "routerIP");
		if (myprop == NULL) myprop = myxml.getprop(curdev, "routerip");
		if (myprop != NULL) {  // if there's an IP, also read port
			sprintf_s(cjunk, "%s", myprop->data.c_str()); // check if this needs %s or %S
			lbd.SetRouterIP(cjunk);

			// this only gets read in if there is an IP address
			myprop = myxml.getprop(curdev, "routerIPport");  // this defaults to 0xBAC0 if not present
			if (myprop == NULL) myprop = myxml.getprop(curdev, "routeripport");
			if (myprop == NULL) lbd.SetRouterPort(defaultPort);//0xBAC0;
			if (myprop != NULL) {
				sprintf_s(cjunk, "%s", myprop->data.c_str()); // check if this needs %s or %S
				lbd.SetRouterPort(cjunk); // check for hex etc. done in subroutine, how convenient! Also out of range is set to 0xBAC0
			}
			myprop = myxml.getprop(curdev, "routerMAC");
			if (myprop == NULL) myprop = myxml.getprop(curdev, "routermac");
			if (myprop == NULL) myprop = myxml.getprop(curdev, "RouterMAC");
			if (myprop != NULL) {
				sprintf_s(cjunk, "%s", myprop->data.c_str()); // check if this needs %s or %S
				lbd.SetRouterMAC(cjunk);
			}
			//else {
			//	theApp.m_BACdlg->m_status.AddString(L"ERROR in config file: Router IP found but Router MAC not found.");
			//	return 0;
			//}
		}

		// MAG 260210 TODO: implement devinst in BDS as shown here
		// read device object instance ##
		myprop = myxml.getprop(curdev, "devinst");  // 
		if (myprop == NULL) myprop = myxml.getprop(curdev, "devInst");
		if (myprop != NULL) {
			sprintf_s(cjunk, "%s", myprop->data.c_str()); // check if this needs %s or %S
			lbd.SetDevObjInst(atoi(cjunk));
		} // else will remain at default -1

		// read net
		myprop = myxml.getprop(curdev, "net");  // this defaults to 0xBAC0 if not present
		if (myprop == NULL) myprop = myxml.getprop(curdev, "Net");
		if (myprop != NULL) {
			sprintf_s(cjunk, "%s", myprop->data.c_str()); // check if this needs %s or %S
			lbd.SetNet(cjunk);
		} // else net will remain at zero

		myprop = myxml.getprop(curdev, "label");  // this defaults to 0xBAC0 if not present
		if (myprop == NULL) myprop = myxml.getprop(curdev, "Label");
		if (myprop != NULL) {
			sprintf_s(cjunk, "%s", myprop->data.c_str()); // check if this needs %s or %S
			lbd.SetDeviceLabel(cjunk);
		} else {
			myprop = &nullProp;  // MAG 251203 add to prevent null myprop below
		}

		lbd.AutoConfigID();
		BACnetList.push_back(lbd);
		deviceCount++;

		cs1.Format(L"Read device %02d label '%S'", (int)BACnetList.size(), myprop->data.c_str());
		theApp.m_BACdlg->m_status.AddString(cs1);

		// now search for points in this dev
		// MAG 240527 NOTE- refer to BDS code if reading points is added, bds31.cpp line 6681 starting point
		
		mynp.SetDevice(BACnetList.size() - 1);
		
		dpl = myxml.element_by_name(curdev, "point");
		while (dpl != NULL) {  // found a point!
			myprop = myxml.getprop(dpl, "type");
			if (myprop != NULL) {
				i = FindBACnetObjectType(myprop->data);
				mynp.SetBACnetObject(i);
			} else {
				dpl = dpl->next;
				continue;
			}

			myprop = myxml.getprop(dpl, "label");
			if (myprop != NULL) {
				mynp.SetLabel(myprop->data);
			} else {
				dpl = dpl->next;
				continue;
			}

			myprop = myxml.getprop(dpl, "description");
			if(myprop == NULL) myprop = myxml.getprop(dpl, "Description");
			if (myprop != NULL) {
				mynp.SetDescription(myprop->data);
			} else {
				dpl = dpl->next;
				continue;
			}

			myprop = myxml.getprop(dpl, "alias");  // check for optional alias
			if (myprop == NULL) myprop = myxml.getprop(dpl, "Alias");  // for backwards compatability
			if (myprop != NULL) {
				mynp.SetAlias(myprop->data);
			} else {
				dpl = dpl->next;
				continue;
			}

			myprop = myxml.getprop(dpl, "id"); // instance
			if (myprop != NULL) {
				mynp.SetInstance(stoi(myprop->data));
			} else {
				dpl = dpl->next;
				continue;
			}

			namedPointList.push_back(mynp);	// no need to reset mynp, all fields are rewritten when read is successful

			cs1.Format(L"  Add named point label '%S' type %d  instance %d", mynp.GetLabel().c_str(),mynp.GetBACnetObject(),mynp.GetInstance());
			theApp.m_BACdlg->m_status.AddString(cs1);

			pointCount++;
			dpl = dpl->next;
		}
		if (pointCount > 0) {
			cs1.Format(L"  Added %d named points.", pointCount);
			theApp.m_BACdlg->m_status.AddString(cs1);
			pointCount = 0;
		}

		curdev = curdev->next;
		if (curdev == NULL) break;
	}

	cs1.Format(L"BDS mapping file has been read, found %d devices.", deviceCount);
	theApp.m_BACdlg->m_status.AddString(cs1);

	return 1;
}  // end ReadBDSSettingsFile


 // The event save file name is passed from the GUI.  This sub opens the CFile to be used.
int
FPTMbase::SetEventSavefile(char* fileName)
{
	CString lcs;

	fopen_s(&evOut, fileName, "w");
	if (evOut == NULL) {
		theApp.m_BACdlg->m_status.AddString(L"ERROR- could not open Events file.");
		return 0;
	}
	strcpy_s(evOutLabel, MAX_PATH, fileName);
	lcs.Format(L"Open Events file ");
	lcs += fileName;
	theApp.m_BACdlg->m_status.AddString(lcs);

	return 1;
}


// MAG 241231 add function
void
FPTMbase::AddRamp(double sv, double ev, int rt, int dur, int evid) {
	RampValue rv = RampValue(sv, ev, rt, dur, evid, 0);
	std::vector<RampValue> rvl;

	while ((int)rampList.size() < TRactive + 1) rampList.push_back(rvl);
	rampList[TRactive].push_back(rv);
}


// MAG 241127 
// Upgraded as part of modifications for Taylor/CalNEXT
// On error returns NamedPoint with no valid content
NamedPoint
FPTMbase::FindNamedPoint(std::string si)
{
	NamedPoint mynp;

	for (size_t st = 0; st < namedPointList.size(); st++) {
		if ((si.compare(namedPointList[st].GetDescription()) == 0) || (si.compare(namedPointList[st].GetLabel()) == 0)||
			(si.compare(namedPointList[st].GetAlias()) == 0)) {
			return namedPointList[st];
		}
	}

	return mynp;
}


// MAG 241127 
// Upgraded as part of modifications for Taylor/CalNEXT
// On error returns PointVarMap with no valid content
PointVarMap
FPTMbase::FindPVM(std::string si)
{
	PointVarMap mynp;

	for (size_t st = 0; st < pvmInput.size(); st++) {
		if (pvmInput[st].CheckName(si) > 0) {
			return pvmInput[st];
		}
	}
	for (size_t st = 0; st < pvmOutput.size(); st++) {
		if (pvmOutput[st].CheckName(si) > 0) {
			return pvmOutput[st];
		}
	}

	return mynp;
}


// MAG 241218 
// Upgraded as part of modifications for Taylor/CalNEXT
// On error returns Variable with no valid content
FPvar
FPTMbase::FindVariable(std::string si)
{
	FPvar myfp;

	for (size_t st = 0; st < VARlist.size(); st++) {
		if (VARlist[st].GetLabel().compare(si) == 0) {
			return VARlist[st];
		}
	}

	return myfp;
}


// MAG 241220 
// Upgraded as part of modifications for Taylor/CalNEXT
// On error returns -1
int
FPTMbase::FindVariablePos(std::string si)
{
	FPvar myfp;
	int i = 0;

	// remove leading spaces
	while (si[0] == ' ') si.erase(0, 1);

	// remove trailing )
	i = si.find(')');
	if (i != std::string::npos)	si.erase(i, 1);
	i = si.find(')'); // might be two, i.e. )) at end of input string
	if (i != std::string::npos)	si.erase(i, 1);

	if (si.size() == 0) return -1;

	for (size_t st = 0; st < VARlist.size(); st++) {
		if (VARlist[st].GetLabel().compare(si) == 0) {
			return st;
		}
	}

	return -1;
}


// MAG 241127 copy functionality from readfile, as it is needed in multiple locations
// Upgraded as part of modifications for Taylor/CalNEXT
// Returns the object type indicated in parameter si, or 0 (AI) on error.
int
FPTMbase::FindBACnetObjectType(std::string si)
{
	int i = 0;
	CString cs1;

	if (si[0] > '9') {  // entered as AI or MV or similar text
		// MAG eval for "AI", "bo", etc.
		switch (si[0]) {
			case 'a':
			case 'A': i = 0; break;  // 
			case 'b':
			case 'B': i = 3; break;  // 
			case 'l':
			case 'L': i = 12; break;  // Loop LP
			case 'm':
			case 'M': i = 13; break;  // 
			default:
				i = 0;
				cs1.Format(L"Error- Find event with BACnet object type not specified correctly (%S).  Setting to AI.", si.c_str());
				theApp.m_BACdlg->m_status.AddString(cs1);
				break;
		}  // end switch 1

		switch (si[1]) {
			case 'i':
			case 'I': break;	// need this to avoid error message below
			case 'o':
			case 'O': i++; break;
			case 'p':
			case 'P': break;	// nothing to do for loop
			case 'v':
			case 'V': if (i == 13) i = 19; // check for MSV
					else i += 2;
				break;
			default:
				cs1.Format(L"Error- Find event with BACnet object type not specified correctly (%S).  Setting to AI.", si.c_str());
				theApp.m_BACdlg->m_status.AddString(cs1);
				break; // leave as AI
		}  // end switch 2
	} else {
		i = stoi(si);// read number directly
	}

	return i;
}


int
FPTMbase::SetEventSavefile(std::string fileName)
{
	char fc[MAX_PATH] = {};

	if (fileName.length() == 0) return 0;

	std::copy(fileName.begin(), fileName.end(), fc);
	fc[fileName.size()] = 0;

	return(SetEventSavefile(fc));
}


void
FPTMbase::SetSendOutOfService(int object)
{
	if (object < 0) return;
	if (object >= 20) return;  // set to array length

	if (sendOutOfService[object] == 1) {
		sendOutOfService[object] = 0;
		//theApp.m_BACdlg->m_status.AddString(L"Settings change: Not sending out-of-service commands with write commands");
	}
	else {
		sendOutOfService[object] = 1;
		//theApp.m_BACdlg->m_status.AddString(L"Settings change: Sending out-of-service commands with write commands");
	}

	return;
}


void
FPTMbase::SetWritePriorityLevel(int i)
{
	if (i < 1) return;
	if (i > 16) return;

	writePriorityLevel = i;

	return;
}


int
FPTMbase::GetSendOutOfService(int objectid)	// calling this flips the value of sendOutOfService
{
	if (objectid < 0) return -1;
	if (objectid >= 20) return-1;  // set to array length

	return sendOutOfService[objectid];
}


void
FPTMbase::SetListRepeat(int i)
{
	if (i < 0) return;
	if (i > 1) return;

	TRlistRepeat = i;

	return;
}


int
FPTMbase::SetCheckOOS(int i)
{
	if ((i < 0) || (i > 1)) return 0;
	checkOOS = i;

	return 1;
}

// MAG 251210 this version figures out the value to scale
double
FPTMbase::GetInterpolatedValue(int testID)
{
	int xIndex = -1;
	double xValue = 0;
	std::string testLabel;

	if (TRactive < 0) return -1;
	if (testID < 0) return -1;
	if (testID >= (int)TESTlist[TRactive].size()) return -1;
	
	xIndex = TESTlist[TRactive][testID].myInterpolate.GetVarIndexX();
	if (xIndex >= 0) {
		xValue = VARlist[xIndex].GetValue();
		testLabel = VARlist[xIndex].GetLabel();
	} else {
		xValue = TESTlist[TRactive][testID].GetValue();
		testLabel = CT2A(TESTlist[TRactive][testID].Label);
	}
	reportT2 << "GetInterpolatedValue A test ID: " << testID << " value: " << xValue << " Label: " << testLabel << std::endl;

	return GetInterpolatedValue(testID, xValue);
}


double
FPTMbase::GetInterpolatedValue(int testID, int testTime)
{
	double	x = 0.0;
	double	rv = 0.0;
	double	lx1 = 0.0;
	double	ly0 = 0.0;
	double	ly1 = 0.0;
	double	lx0 = 0.0;
	double	lminOut = 0.0;
	double	lmaxOut = 0.0;
	double	lval = 0.0;
	double	rval = 0.0;
	int		trl = theApp.myFPTM.TRactive;
	std::string labelx0 = "NA";
	std::string labelx1 = "NA";
	std::string labely0 = "NA";
	std::string labely1 = "NA";
	std::string labeli0 = "NA";
	std::string labeli1 = "NA";

	if (TESTlist[trl][testID].myInterpolate.GetVarIndexX0() != -1) {
		lx0 = VARlist[TESTlist[trl][testID].myInterpolate.GetVarIndexX0()].GetValue();
		labelx0 = VARlist[TESTlist[trl][testID].myInterpolate.GetVarIndexX0()].GetLabel();
	} else { 
		lx0 = theApp.myFPTM.TESTlist[trl][testID].myInterpolate.GetX0(); 
		labelx0 = CT2A(theApp.myFPTM.TESTlist[trl][testID].Label);
	}

	if (TESTlist[trl][testID].myInterpolate.GetVarIndexX1() != -1){ 
		lx1 = VARlist[TESTlist[trl][testID].myInterpolate.GetVarIndexX1()].GetValue();
		labelx1 = VARlist[TESTlist[trl][testID].myInterpolate.GetVarIndexX1()].GetLabel();
	} else {
		lx1 = theApp.myFPTM.TESTlist[trl][testID].myInterpolate.GetX1();
		labelx1 = CT2A(theApp.myFPTM.TESTlist[trl][testID].Label);
	}

	if (TESTlist[trl][testID].myInterpolate.GetVarIndexY0() != -1){ 
		ly0 = VARlist[TESTlist[trl][testID].myInterpolate.GetVarIndexY0()].GetValue();
		labely0 = VARlist[TESTlist[trl][testID].myInterpolate.GetVarIndexY0()].GetLabel();
	} else {
		ly0 = theApp.myFPTM.TESTlist[trl][testID].myInterpolate.GetY0();
		labely0 = CT2A(theApp.myFPTM.TESTlist[trl][testID].Label);
	}

	if (TESTlist[trl][testID].myInterpolate.GetVarIndexY1() != -1){ 
		ly1 = VARlist[TESTlist[trl][testID].myInterpolate.GetVarIndexY1()].GetValue(); 
		labely1 = VARlist[TESTlist[trl][testID].myInterpolate.GetVarIndexY1()].GetLabel();
	} else {
		ly1 = theApp.myFPTM.TESTlist[trl][testID].myInterpolate.GetY1(); 
		labely1 = CT2A(theApp.myFPTM.TESTlist[trl][testID].Label);
	}

	if (TESTlist[trl][testID].myInterpolate.GetVarIndexMinOut() != -1) lminOut = VARlist[TESTlist[trl][testID].myInterpolate.GetVarIndexMinOut()].GetValue(); else lminOut = TESTlist[trl][testID].myInterpolate.GetMinOut();
	if (TESTlist[trl][testID].myInterpolate.GetVarIndexMaxOut() != -1) lmaxOut = VARlist[TESTlist[trl][testID].myInterpolate.GetVarIndexMaxOut()].GetValue(); else lmaxOut = TESTlist[trl][testID].myInterpolate.GetMaxOut();

	x = testTime;
	reportT2 << "GetInterpolatedValue B test ID: " << testID << " value: " << x <<  std::endl;
	reportT2 << "GetInterpolatedValue B lx0 ("<< labelx0 << "): " << lx0 << " lx1( " << labelx1 << "): " << lx1 << std::endl;
	reportT2 << "GetInterpolatedValue B ly0 ("<< labely0 << "): " << ly0 << " ly1( " << labely1 << "): " << ly1 << std::endl;
	reportT2 << "GetInterpolatedValue B lminOut: " << lminOut << " lmaxOut: " << lmaxOut << std::endl;
	reportT2 << "GetInterpolatedValue B mode: " << TESTlist[trl][testID].myInterpolate.GetMode() << std::endl;

	// NOTE formulas copied from test script feature reference
	switch (TESTlist[trl][testID].myInterpolate.GetMode()) {
		case 0:	// missing min_out or max_out
			rv = ly0 + (ly1 - ly0) * ((x - lx0) / (lx1 - lx0));
			break;
		case 1:// missing min_out
			lval = lminOut;
			rval = ly0 + (ly1 - ly0) * ((x - lx0) / (lx1 - lx0));
			if (lval > rval) rv = lval;
			else rv = rval;
			//rv = std::max(lminOut, ly0 + (ly1 - ly0) * ((x - lx0) / (lx1 - lx0)));
			break;
		case 2:// missing max_out
			lval = lmaxOut;
			rval = ly0 + (ly1 - ly0) * ((x - lx0) / (lx1 - lx0));
			if (lval > rval) rv = rval;
			else rv = lval;
			//rv = std::min(lmaxOut, ly0 + (ly1 - ly0) * ((x - lx0) / (lx1 - lx0)));
			break;
		case 3:// has min_out and max_out
			lval = lminOut;	// calc rval max
			rval = ly0 + (ly1 - ly0) * ((x - lx0) / (lx1 - lx0));
			if (lval > rval) rval = lval;  // else rval remains the same
			lval = lmaxOut; // now calc outer lval
			if (lval > rval) rv = rval;
			else rv = lval;
			//rv = std::min(lmaxOut, std::max(lminOut, ly0 + (ly1 - ly0) * ((x - lx0) / (lx1 - lx0))));
			break;
		default:
			rv = DBL_MIN;// std::numeric_limits<double>::min();  // ERROR STATE
			break;
	}

	reportT2 << "GetInterpolatedValue B return value: " << rv << " input value: " << x << std::endl;

	return rv;
}


void
FPTMbase::SetModelStartTime(CTime val)
{
	modelStartTime = val;

	return;
}


int
FPTMbase::GetListRepeat()
{
	return TRlistRepeat;
}


// MAG 250619 Evaluates event timer test ending step early (if present)
// aka "threshold" tests
// Returns 0 if the step can continue, 1 if the step should end early
int
FPTMbase::CheckTestStepTest() {

	double	lval = 0.0;
	double	rval = 0.0;
	double	tol = 0.0;
	int		lid = -1;	// left var id
	int		rid = -1;	// right var id
	int		ltr = 0;	// pass/fail
	int		pindex = 0;// eventTimerTestList index
	CString	cs1;
	CString dts;
	CString lvLabel;
	CString	rvLabel;
	CTime	tnow = CTime::GetCurrentTime();
	std::vector<std::string>	testType = { "LT","LTE", "EQ", "GTE", "GT","NE", "MA", "ANY" };
	std::vector<std::string>	testResult = { "Criteria not met", "PASS", "ERROR"};	// MAG 251209 change from "FAIL" to "Criteria not met" Request by Hwakong

	if ((TRactive < 0) || (TSactive < 0)) {
		return 0;
	}

	if(testSteps.size() <= 0) {
		return -1;	// there are no test steps yet
	}

	if (TSactive >= (int)testSteps[TRactive].size()) {
		return -2;	// incremented past last test step
	}
	
	if (tnow < testSteps[TRactive][TSactive].GetTime()) {
		return -3;	// current test step has not started (maybe before first step starts)
	}

	// Get index to eventTimerTestList
	pindex = testSteps[TRactive][TSactive].GetTest();
	if (pindex == -1) {
		return -4;  // no test to fail
	}
		
	// MAG LVAL is ALWAYS a variable index reference (varIndex)
	lid = eventTimerTestList[TRactive][pindex].GetVarIndex();
	if (lid < 0) {  // == -1 for not set, but going with <= to capture other errors (if any)
		return -5;
	} else {// MAG the LVAL is ALWAYS a variable reference!!!
		if (lid > (int)VARlist.size()) {
			return -6; // ERROR LINE- invalid variable reference
		}
		lval = VARlist[lid].GetValue();
		lvLabel = VARlist[lid].GetLabel().c_str();
	}

	// MAG RVAL may be a numerical value (value) OR a variable index reference (varIndex2)
	rid = eventTimerTestList[TRactive][pindex].GetVarIndex2();
	if (rid == -1) {
		rval = eventTimerTestList[TRactive][pindex].GetValue();
		rvLabel = "Value";
	} else if (rid < -1) {  // other error? Probably not but need to capture case.
		return -7;
	} else {  // rval is a variable
		if (rid > (int)VARlist.size()) {
			return -8; // ERROR LINE- invalid variable reference
		}
		rval = VARlist[rid].GetValue();
		rvLabel = VARlist[rid].GetLabel().c_str();
	}

	dts = theApp.GetDTS(tnow);

	switch (eventTimerTestList[TRactive][pindex].GetRelate()){
		case -2: // LT
			//if (VARlist[TESTlist[TRactive][tid].GetVarIndex()].GetValue() < TESTlist[TRactive][tid].GetValue()) 
			if ((lval - tol) < rval) {
				ltr = 1;
				//cs1.Format(L"LT Test %d passed.", tid);
			} else {
				ltr = 0;
				//cs1.Format(L"LT Test %d failed.", tid);
			}
			break;
		case -1: // LTE
			//if (VARlist[TESTlist[TRactive][tid].GetVarIndex()].GetValue() <= TESTlist[TRactive][tid].GetValue()) {
			if ((lval - tol) <= rval) {
				ltr = 1;
				//cs1.Format(L"LTE Test %d passed.", tid);
			} else {
				ltr = 0;
				//cs1.Format(L"LTE Test %d failed.", tid);
			}
			break;
		case 0: // EQ
			//if (abs(VARlist[TESTlist[TRactive][tid].GetVarIndex()].GetValue() - TESTlist[TRactive][tid].GetValue()) < 0.01) {
			if (abs(lval - rval) <= tol) {
				ltr = 1;
				//cs1.Format(L"EQ Test %d passed.", tid);
			} else {
				ltr = 0;
				//cs1.Format(L"EQ Test %d failed.", tid);
			}
			break;
		case 1: // GTE
			//if (VARlist[TESTlist[TRactive][tid].GetVarIndex()].GetValue() >= TESTlist[TRactive][tid].GetValue()) {
			if ((lval + tol) >= rval) {
				ltr = 1;
				//cs1.Format(L"GTE Test %d passed.", tid);
			} else {
				ltr = 0;
				//cs1.Format(L"GTE Test %d failed.", tid);
			}
			break;
		case 2: // GT
			//if (VARlist[TESTlist[TRactive][tid].GetVarIndex()].GetValue() > TESTlist[TRactive][tid].GetValue()) {
			if ((lval + tol) > rval) {
				ltr = 1;
				//cs1.Format(L"GT Test %d passed.", tid);
			} else {
				ltr = 0;
				//cs1.Format(L"GT Test %d failed.", tid);
			}
			break;
		case 3: // NE
			//if (VARlist[TESTlist[TRactive][tid].GetVarIndex()].GetValue() - TESTlist[TRactive][tid].GetValue() > 0.01) {
			if (abs(lval - rval) > tol) {
				ltr = 1;
				//cs1.Format(L"NE Test %d passed.", tid);
			} else {
				ltr = 0;
				//cs1.Format(L"NE Test %d failed.", tid);
			}
			break;
		//case 4: // MA MANUAL REVIEW- can not fail here.
		//	ltr = 1;
		//	cs1.Format(L"Test %d: Manual check VAR value is %5.2f", tid, lval);//VARlist[TESTlist[TRactive][tid].GetVarIndex()].GetValue());
		//	reportT2 << dts << CT2A(cs1) << std::endl;
		//	cs1.Format(L"Test %d: Manual check REL value is %5.2f", tid, rval);//TESTlist[TRactive][tid].GetValue());
		//	//manualCount++;
		//	break;
		case 5: // ANY- can not fail here.
			ltr = 1;
			//cs1.Format(L"Test %d: ANY value no test.", tid);
			break;
		default:
			ltr = 2;  // error
			cs1.Format(L"%s ERROR Test %d: Test type not known. No test was run. LVAL: (%s %5.2f) RVAL: (%s %5.2f)", dts.GetString(), TSactive, 
				lvLabel.GetString(), lval, rvLabel.GetString(), rval);
			theApp.m_BACdlg->m_status.AddString(cs1);
			break;
	}	// end switch

	// MAG 260115 NOTE check line below is printing column correctly, then delete comment
	int i = eventTimerTestList[TRactive][pindex].GetSourceColumn();
	cs1.Format(L"%s\tThreshold Evaluation %c:%d (%s: %5.2f) %S (%s: %5.2f). Test result: %S.",
		dts.GetString(), TRactive + 'A', eventTimerTestList[TRactive][pindex].GetSourceColumn(), lvLabel.GetString(), lval, testType[eventTimerTestList[TRactive][pindex].GetRelate() + 2].c_str(),
		rvLabel.GetString(), rval, testResult[ltr].c_str());
	theApp.m_BACdlg->m_status.AddString(cs1);
	reportT1log.push_back(std::string(CT2A(cs1)));

	return (ltr&1);  // strip out 2 values on error
}


// MAG 241107
// Purpose: To increment or decrement the active TRlist
// Note: GUI must be redrawn from calling subroutine
int
FPTMbase::ChangeActiveTRlist(int updown)
{
	// TODO: Add your implementation code here.
	if (updown == 0) return TRactive;

	if (updown > 0) {
		if (TRactive + 1 >= (int)TRlist.size())  // check for end of TRlist, loop back to zero, GTE in case things have gone whacky...
		{
			TRactive = 0;
		} else {
			TRactive++;	// move to next position
		}
		TSactive = 0;
	} else {
		if (TRactive <= 0)  // check for beginning of TRlist, loop back to end
		{
			TRactive = TRlist.size() - 1;
		} else {
			TRactive--;	// move to lower position
		}
		TSactive = 0;
	}

	return 0;
}


// MAG 241108
// Called from menu item, adds new empty event list
void
FPTMbase::OnRuntimeoptionsNewEventList()
{
	// TODO: Add your command handler code here
	CString stat;

	// copied from App init
	std::vector<TimedRequest>		trInit;
	TimedRequestData				trdInit; // let it go.
	std::vector<RampValue>			rampInit;
	std::vector<DualVarTestList>	tlInit;
	std::vector<DualVarTestList>	dvtlInit;
	std::vector<TestStep>			tsInit;

	TRlist.push_back(trInit);
	TRdata.push_back(trdInit);
	TESTlist.push_back(tlInit);
	rampList.push_back(rampInit);
	eventTimerTestList.push_back(dvtlInit);
	testSteps.push_back(tsInit);

	TRactive = TRlist.size() - 1;
	theApp.m_BACdlg->UpdateEvents();

	stat.Format(L"Add event list #%d. Add and configure events to start testing.", TRactive);
	theApp.m_BACdlg->m_status.AddString(stat);

	return;
}


void
FPTMbase::OnRuntimeoptionsDeleteEventList()
{
	// TODO: Add your command handler code here
	CString stat;

	if (TRlist.size() == 0) {
		stat.Format(L"There are no event lists to delete.");
		theApp.m_BACdlg->m_status.AddString(stat);
		return;
	}

	TRlist.erase(TRlist.begin() + TRactive);
	TRdata.erase(TRdata.begin() + TRactive);
	TESTlist.erase(TESTlist.begin() + TRactive);
	rampList.erase(rampList.begin() + TRactive);
	eventTimerTestList.erase(eventTimerTestList.begin() + TRactive);

	stat.Format(L"Delete event list %d.", TRactive);
	theApp.m_BACdlg->m_status.AddString(stat);

	if (TRactive >= (int)TRlist.size()) TRactive = TRlist.size() - 1;

	if (TRlist.size() == 0) {
		stat.Format(L"There are no event lists remaining.");
		theApp.m_BACdlg->m_status.AddString(stat);
	}

	theApp.m_BACdlg->UpdateEvents();

	return;
}


// MAG 250204 
// Purpose: To allow management of periodic value inputs. Input consists of a value/variable, and an interval.
// Value is sent to controller at specified interval until next time step. Value is recalculated if necessary each time it is sent.
// PeriodicInput is initiated in TestStep structure
int
FPTMbase::ScanPeriodicString(std::string rs, PeriodicInput* pi)
{
	std::stringstream	sstr;
	std::string			parse_string;
	std::string			ps2;
	char				rampDelim = ';';
	int					offset = 0;
	int					mod = -1;
	char*				mtypes = { "+-*/" };  // math types
	CString				cs1;

	// MAG 260616 deliminator removed in V20 as there is no longer an interval parameter
	// determine deliminater
	//if (rs.find(rampDelim) == std::string::npos) {
	//	rampDelim = ',';
	//	if (rs.find(rampDelim) == std::string::npos) {
	//		return 0;
	//	}
	//}

	// clean up rs
	offset = rs.find('(');
	if (offset != std::string::npos) {  // found (
		rs.erase(0, offset + 1);	// this should erase "=PERIODIC("
	}
	// can leave trailing ')'

	//sstr = std::stringstream(rs);
	//std::getline(sstr, parse_string, rampDelim); // get param 1
	//offset = FindVariablePos(parse_string);
	// MAG after v20 no need to parse, is just one string
	offset = FindVariablePos(rs);
	if (offset == -1) {  // ERROR! oh no. Check if it's formatted "var+#"
		// MAG NOTE this is not a recoverable error.
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Can not parse PERIODIC input string. Variable not found.");
		cs1.Format(L"String: %S", rs.c_str());
		theApp.m_BACdlg->m_status.AddString(cs1);
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Review and fix PERIODIC input string.");
		return 0; 

		// attempt to recover? After v20 this is unlikely.
		//for (size_t i = 0; (i < 4) && (mod == -1); i++) { // scan through mtypes
		//	offset = parse_string.find(mtypes[i]);
		//	if (offset != std::string::npos) mod = i;
		//}
		//if (mod == -1) {
		//	theApp.m_BACdlg->m_status.AddString(L"ERROR: Can not parse PERIODIC input string.");
		//	cs1.Format(L"String: %S", rs.c_str());
		//	theApp.m_BACdlg->m_status.AddString(cs1);
		//	return 0; // don't know what's going on here.
		//}
		//
		//pi->SetModType(mod);
		//
		//ps2 = parse_string.substr(0, offset);
		//parse_string.erase(offset + 1); // this should change parse_string to ##

		// ps2 should be name of var
	} else {
		pi->SetExpressionIndex(offset);
		//std::getline(sstr, parse_string, rampDelim); // get param 2
		//pi->SetPeriod(std::stoi(parse_string));
		pi->SetPeriod(10);
	}

	return 1;
}


// MAG 250205
// Purpose: to set up TimedRequest when value is ADD, SUB, MULT, DIV math operation. Based on ScanPeriodicString.
// info from TimedRequest definition
//int		fpvar;		// index to VARlist, -1 for not used
//int		fprelate;	// variable relation to value: 1:add 2:subtract 3:multiply 4:divide by 5:set to 
//double	fpvalue;	// <modvalue> used with operator NOTE NOT VALUE READ/WRITE from/to controller- this modifies write values with fprelate
//int		fpmodvar;	// <modvar> used instead of fpvalue as second term in operation- used only with fprelate
// Inputs:
// rs: input string to read
// mathType: see fprelate
// tr: timed reques pointer to save values to
int
FPTMbase::ScanMathString(std::string rs, int mathType, TimedRequest* tr)
{
	std::stringstream	sstr;
	std::string			parse_string;
	std::string			ps2;
	std::string			rs2;
	CString				cs1;
	char				rampDelim = ';';
	int					offset = 0;
	int					mod = -1;
	double				dv1 = 0.0;
	//char* mtypes = { "+ -*/" };

	// determine deliminater
	if (rs.find(rampDelim) == std::string::npos) {
		rampDelim = ',';
		if (rs.find(rampDelim) == std::string::npos) {
			return 0;
		}
	}

	// clean up rs
	offset = rs.find('(');
	if (offset != std::string::npos) {  // found (
		rs.erase(0, offset + 1);	// this should erase "=XXX(" xxxis one of ADD, SUB, etc.
	}
	// can leave trailing ')'

	sstr = std::stringstream(rs);
	std::getline(sstr, parse_string, rampDelim); // get param 1
	offset = FindVariablePos(parse_string);
	if (offset == -1) {  // ERROR! oh no. Check if it's formatted "var+#"
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Import: Math operation does not have variable as first input.");
		cs1.Format(L"Line is: %S", rs2.c_str());
		theApp.m_BACdlg->m_status.AddString(cs1);
		return 0;
	}
	tr->SetFPV(offset);

	std::getline(sstr, parse_string, rampDelim); // get param 2
	offset = FindVariablePos(parse_string);
	if (offset != -1) {  // Second param is a variable
		tr->SetFPVModvar(offset);
	} else {
		dv1 = std::stod(parse_string);
		tr->SetFPVValue(dv1);
	}

	tr->SetFPVOperator(mathType);

	tr->SetUseValue(0);

	return 1;
}


// MAG 250206
// Purpose: to set up new Variable for is ADD, SUB, MULT, DIV math operation. Based on ScanMathString.
// Called when reading Interpolate or other function
// Returns: index to new variable, or -1 on error
// NOTE: Adds new var to VARlist, does NOT add MathVar
// 
//int		fpvar;		// index to VARlist, -1 for not used
//int		fprelate;	// variable relation to value: 1:add 2:subtract 3:multiply 4:divide by 5:set to 
//double	fpvalue;	// <modvalue> used with operator NOTE NOT VALUE READ/WRITE from/to controller- this modifies write values with fprelate
//int		fpmodvar;	// <modvar> used instead of fpvalue as second term in operation- used only with fprelate
// Inputs:
// rs: input string to read
// tr: timed reques pointer to save values to
// calculates:
// mathType: see fprelate
int
FPTMbase::ScanMathStringToVar(std::string rs, MathVar* tr)
{
	std::stringstream	sstr;
	std::stringstream	sstrv3;
	std::string			parse_string;
	std::string			ps2;
	std::string			rs2;
	std::string			labelV1;
	std::string			labelV2;
	std::string			labelV3; // new output var, added by myfpv
	CString				cs1;
	char				rampDelim = ';';
	int					offset = 0;
	int					mod = -1;
	int					mathType = -1;
	int					v1index = 0;
	int					v2index = 0;
	double				dv1 = 0.0;
	double				initdv3 = 0.0;
	FPvar				myfpv;
	std::vector<std::string> labelOp = { "ADD", "SUB", "MULT", "DIV" };
	//char* mtypes = { "+ -*/" };

	if (tr == NULL) {
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Compound math type format error- formula not sent for evaluation.");
		return -1;
	}

	if (rs.size() < 4) {
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Compound math type format error- formula not found.");
		cs1.Format(L"Input is: %S", rs.c_str());
		theApp.m_BACdlg->m_status.AddString(cs1);
		return -1;
	}

	switch (rs[0]) {
		case 'A':
		case 'a': // =ADD
			mathType = 1;
			cs1 = "add";
			break;
		case 'D':
		case 'd': // =DIV
			mathType = 4;
			cs1 = "div";
			break;
		case 'M':
		case 'm': // =MULT
			mathType = 3;
			cs1 = "mult";
			break;
		case 'S':
		case 's': // =SUB
			mathType = 2;
			cs1 = "sub";
			break;
		default:
			break;
	}

	if (mathType == -1) {
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Compound math type format error- unknown operation requested.");
		cs1.Format(L"Input is: %S", rs.c_str());
		theApp.m_BACdlg->m_status.AddString(cs1);
		return -1;
	}

	tr->SetVarOperator(mathType);

	// determine delimiter
	if (rs.find(rampDelim) == std::string::npos) {
		rampDelim = ',';
		if (rs.find(rampDelim) == std::string::npos) {
			theApp.m_BACdlg->m_status.AddString(L"ERROR: Compound math type format error- delimiter not found. Use , or ;.");
			cs1.Format(L"Input is: %S", rs.c_str());
			theApp.m_BACdlg->m_status.AddString(cs1);
			return -1;
		}
	}

	// clean up rs
	offset = rs.find('(');
	if (offset != std::string::npos) {  // found (
		rs.erase(0, offset + 1);	// this should erase "=PERIODIC("
	}
	// can leave trailing ')'

	// scan two parameters
	sstr = std::stringstream(rs);
	std::getline(sstr, parse_string, rampDelim); // get param 1
	v1index = FindVariablePos(parse_string);
	if (v1index == -1) {  // it's a number, number not allowed here
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Compound math type format error- constant value not allowed.");
		cs1.Format(L"Error in: %S", rs.c_str());
		theApp.m_BACdlg->m_status.AddString(cs1);
		return -1;
	} else {
		tr->SetVar1Index(v1index);
		labelV1 = VARlist[v1index].GetLabel();
	}

	std::getline(sstr, parse_string, rampDelim); // get param 2
	v2index = FindVariablePos(parse_string);
	if (v2index == -1) {  // it's a number, number not allowed here
		dv1 = std::stod(parse_string);
		tr->SetVar2Value(dv1);
		sstrv3 << std::setw(5) << std::setprecision(2) << dv1;
		labelV2 = sstrv3.str();
	} else {
		tr->SetVar2Index(v2index);
		labelV2 = VARlist[v2index].GetLabel();
		dv1 = VARlist[v2index].GetValue();
	} // NOTE dv1 holds value 2
	runtimeVar++; // starts at 0
	sstrv3 << "RV" << std::setfill('0') << std::setw(2) <<  runtimeVar << labelV1 << labelOp[mathType] << labelV2;

	myfpv.SetLabel(sstrv3.str());

	initdv3 = VARlist[v1index].GetValue();

	switch (mathType) {
		// MAG case 0 can be used for pass-thru value, but not applicable here
		case 1: initdv3 += dv1; break;
		case 2: initdv3 -= dv1; break;
		case 3: initdv3 *= dv1; break;
		case 4: initdv3 /= dv1; break;
		default:
			break; // this should never hit as mathtype is checked above
	}
	myfpv.SetValue(initdv3);

	VARlist.push_back(myfpv);
	tr->SetVarOutIndex(VARlist.size() - 1);

	return 1;
}


// MAG 250205
// Purpose: to set up InterpolateValue structure. Based on ScanPeriodicString.
// Inputs:
// rs: input string to read
// tr: interpolate value object pointer to save values to
int
FPTMbase::ScanInterpolateString(std::string rs, InterpolateValue* tr, TestStep* mts)
{
	std::stringstream	sstr;
	std::string			parse_string;
	std::string			ps2;
	std::string			rs2;
	CString				cs1;
	char				rampDelim = ';';
	char				subDelim = ',';
	int					offset = 0;
	int					mod = 0;
	int					mode2flag = 0; // used if min is set to 'NA'
	double				dv1 = 0.0;
	MathVar				mv1;


	if (tr == NULL) {
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Interpolate type format error- value not sent for evaluation.");
		return 0;
	}
	if (mts == NULL) {
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Interpolate type format error- test step not sent for evaluation.");
		return 0;
	}

	// determine deliminater
	if (rs.find(rampDelim) == std::string::npos) {
		rampDelim = ',';
		if (rs.find(rampDelim) == std::string::npos) {
			return 0;
		}
	}

	// clean up rs
	offset = rs.find('(');
	if (offset != std::string::npos) {  // found (
		rs.erase(0, offset + 1);	// this should erase "=PERIODIC("
	}
	// can leave trailing ')'

	// check for nested components like "xx; ADD(x1; x2); xy;"
	for (size_t i = 0; i < rs.size(); i++) {
		if (rs[i] == '(') {
			mod = 1; // set to indicate in a nested component
			continue;
		}
		if ((mod == 1) && (rs[i] == rampDelim)) {
			rs[i] = subDelim;
			continue;
		}
		if (rs[i] == ')') mod = 0; // set to indicate NOT in a nested component
	}

	sstr = std::stringstream(rs);

	std::getline(sstr, parse_string, rampDelim); // get param 1 X
	while (parse_string[0] == ' ') parse_string.erase(0, 1);
	offset = FindVariablePos(parse_string);
	if (offset == -1) {  // number not var
		if (std::isalpha(parse_string[0])) { // probably math function like sum or mult
			mod = ScanMathStringToVar(parse_string, &mv1);
			if (mod == -1) {
				return 0;  // error found! TODO flesh this out
			}
			tr->SetVarIndexX(mod);
			mts->mvList.push_back(mv1);
		} else {	// found a number
			dv1 = std::stod(parse_string);
			tr->SetX(dv1);
		}
	} else {
		tr->SetVarIndexX(offset);
	}

	std::getline(sstr, parse_string, rampDelim); // get param 2 X0
	while (parse_string[0] == ' ') parse_string.erase(0, 1);
	offset = FindVariablePos(parse_string);
	if (offset == -1) {  // number not var
		dv1 = std::stod(parse_string);
		tr->SetX0(dv1);
	} else {
		tr->SetVarIndexX0(offset);
	}

	std::getline(sstr, parse_string, rampDelim); // get param 3 X1
	while (parse_string[0] == ' ') parse_string.erase(0, 1);
	offset = FindVariablePos(parse_string);
	if (offset == -1) {  // number not var
		dv1 = std::stod(parse_string);
		tr->SetX1(dv1);
	} else {
		tr->SetVarIndexX1(offset);
	}

	std::getline(sstr, parse_string, rampDelim); // get param 4 Y0
	while (parse_string[0] == ' ') parse_string.erase(0, 1);
	offset = FindVariablePos(parse_string);
	if (offset == -1) {  // number not var
		dv1 = std::stod(parse_string);
		tr->SetY0(dv1);
	} else {
		tr->SetVarIndexY0(offset);
	}
	tr->SetMode(0); 

	std::getline(sstr, parse_string, rampDelim); // get param 5 Y1
	while (parse_string[0] == ' ') parse_string.erase(0, 1);
	offset = FindVariablePos(parse_string);
	if (offset == -1) {  // number not var
		if (std::isalpha(parse_string[0])) { // probably math function like sum or mult
			mod = ScanMathStringToVar(parse_string, &mv1);
			if (mod == -1) {
				return 0;  // error found! TODO flesh this out
			}
			tr->SetVarIndexY1(mv1.GetVarOutIndex());
			mts->mvList.push_back(mv1);
			offset = parse_string.find(')');
			if (offset != std::string::npos) parse_string.erase(offset, 1); // erase ending ) so param 6 can be read in properly
		} else {	// found a number
			dv1 = std::stod(parse_string);
			tr->SetY1(dv1);
		}
	} else {
		tr->SetVarIndexY1(offset);
	}

	// note this doesn't work if prev was math like ADD(xxx)
	if (parse_string.find(')') == std::string::npos) {
		std::getline(sstr, parse_string, rampDelim); // get param 6 minOut
		while (parse_string[0] == ' ') parse_string.erase(0, 1);
		offset = FindVariablePos(parse_string);
		tr->SetMode(1);
		if (offset == -1) {  // number not var
			if (parse_string.compare("NA") != 0) {	// MAG 260303 add method to check for NA for min to allow max w/o min set (mode 2)
				dv1 = std::stod(parse_string);
				tr->SetMinOut(dv1);
			} else {
				mode2flag = 1;
			}
		} else {
			tr->SetVarIndexMinOut(offset);
		}
	}
	// MAG 251218 TODO- currently always sets to mode 3, as there's no real way to get to mode 2 // set mode 2 or 3 below!!
	if (parse_string.find(')') == std::string::npos) {
		std::getline(sstr, parse_string, rampDelim); // get param 7 maxOut
		while (parse_string[0] == ' ') parse_string.erase(0, 1);
		offset = FindVariablePos(parse_string);
		(mode2flag)?(tr->SetMode(2)):(tr->SetMode(3));	// MAG 260303 add way to get to mode 2 // MAG 251218 there's functionally no way to get to mode 2.
		if (offset == -1) {  // number not var
			if (std::isalpha(parse_string[0])) { // probably math function like sum or mult
				mod = ScanMathStringToVar(parse_string, &mv1);
				if (mod == -1) {
					return 0;  // error found! TODO flesh this out
				}
				tr->SetVarIndexMaxOut(mv1.GetVarOutIndex());
			} else {	// found a number
				dv1 = std::stod(parse_string);
				tr->SetMaxOut(dv1);
			}
		} else {
			tr->SetVarIndexMaxOut(offset);
		}
	}

	return 1;
}


// MAG 250102
// Function evaluates ramp declaration string from Taylor/CalNEXT input file
// ramp string is in format "RAMP(77.9;72;1;6)" or "RAMP(=ZoneTemp;72;1;6)"
// NOTE MAG 260127 add capability for first parameter as "LAST"- returns a 2, 
// indicating first parameter should be set to VAR index
// Returns 1 on success, 0 on fail
// Ramp parameters:
// 1: Start value- may be a variable reference
// 2: End value- may be a variable reference
// XXX 3: Ramp rate /minute (removed in v20)
// 3: Ramp duration
int
FPTMbase::EvalRamp(RampValue* rv, std::string rs)
{
	// TODO: Add your implementation code here.
	std::stringstream	sstr;
	std::string			parse_string;
	std::string			sub_string;
	CString				cs1;
	int					rampVar = -1;
	FPvar				myVar;
	double				dv = 0.0;
	char				rampDelim = ';';
	int					retVal = 1;  // assume normal return, set to 2 if LAST is used for first parameter (?use 3 for second param if needed?).


	if (rv == NULL) {
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Ramp type format error- ramp value not sent for evaluation.");
		return 0;
	}

	// determine deliminater
	if (rs.find(rampDelim) == std::string::npos) {
		rampDelim = ',';
		if (rs.find(rampDelim) == std::string::npos) {
			return 0;
		}
	}
	sstr = std::stringstream(rs);

	// Read in Start Value
	try {
		std::getline(sstr, parse_string, rampDelim); // get param 1
		// remove "RAMP("
		parse_string = parse_string.substr(6, parse_string.size() - 6);
		//std::regex_replace(parse_string, std::regex("RAMP("), "");

		//if (parse_string[0] == '=') { // parse for format like "=ZoneTemp"
		//	parse_string.erase(0, 1);
		myVar = FindVariable(parse_string);
		if (myVar.GetLabel().size() > 0) {  // valid var has label length > 0
			rv->SetVarStart(FindVariablePos(parse_string));
		} else {
			sub_string = parse_string.substr(0, 4);
			if (sub_string.compare("LAST") == 0) {
				retVal = 2;
			} else {	// just a number
				dv = stod(parse_string);
				rv->SetStartValue(dv);
			}
		}
	}
	catch (const std::exception& e) {
		std::stringstream ss1;
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Caught exception in EvalRamp!");
		ss1 << "ERROR: " << e.what();
		cs1 = ss1.str().c_str();
		theApp.m_BACdlg->m_status.AddString(cs1);
		return 0;
	}
	catch (...) {
		cs1.Format(L"ERROR: Could not parse parameter 1 of ramp: %S", rs.c_str());
		theApp.m_BACdlg->m_status.AddString(cs1);
		return 0;
	}

	// Read in End Value
	try {
		std::getline(sstr, parse_string, rampDelim);	// get param 2
		while (parse_string[0] == ' ') parse_string.erase(0, 1);
		if (isalpha(parse_string[0]) != 0) {		// also a var
			if (parse_string[0] == '=') {	// check for format like "=ZoneTemp"- NOTE could also be math like ADD or SUB???
				parse_string.erase(0, 1);	// then erase '='
			}
			myVar = FindVariable(parse_string);
			if (myVar.GetLabel().size() > 0) {  // valid var has label length > 0
				rv->SetVarEnd(FindVariablePos(parse_string));
			} else {
				theApp.m_BACdlg->m_status.AddString(L"ERROR: RAMP specified with invalid end value:");
				cs1.Format(L"%S", parse_string.c_str());
				theApp.m_BACdlg->m_status.AddString(cs1);
				return 0;
			}
		} else {
			dv = stod(parse_string);
			rv->SetEndValue(dv);
		}
	}
	catch (const std::exception& e) {
		std::stringstream ss1;
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Caught exception (2) in EvalRamp!");
		ss1 << "ERROR: " << e.what();
		cs1 = ss1.str().c_str();
		theApp.m_BACdlg->m_status.AddString(cs1);
		return 0;
	}
	catch (...) {
		cs1.Format(L"ERROR: Could not parse parameter 2 of ramp: %S", rs.c_str());
		theApp.m_BACdlg->m_status.AddString(cs1);
		return 0;
	}

	// Read in Duration Value (was rate)
	try {
		std::getline(sstr, parse_string, rampDelim);	// get param 3, always a number
		rampVar = stoi(parse_string);
		rv->SetDuration(rampVar);
	}
	catch (const std::exception& e) {
		std::stringstream ss1;
		theApp.m_BACdlg->m_status.AddString(L"ERROR: Caught exception (3) in EvalRamp!");
		ss1 << "ERROR: " << e.what();
		cs1 = ss1.str().c_str();
		theApp.m_BACdlg->m_status.AddString(cs1);
		return 0;
	}
	catch (...) {
		cs1.Format(L"ERROR: Could not parse parameter 3 of ramp: %S", rs.c_str());
		theApp.m_BACdlg->m_status.AddString(cs1);
		return 0;
	}

	// MAG removed period (update span) value as variable in VAVRH v20, now just const 10 sec
	rv->SetUpdateSpan(10);  // set to default 10 seconds
	//try {
	//	std::getline(sstr, parse_string, rampDelim);	// get param 4, always a number
	//	rampVar = stoi(parse_string);
	//	rv->SetUpdateSpan(rampVar);
	//}
	//catch (const std::exception& e) {
	//	std::stringstream ss1;
	//	theApp.m_BACdlg->m_status.AddString(L"ERROR: Caught exception (4) in EvalRamp!");
	//	ss1 << "ERROR: " << e.what();
	//	cs1 = ss1.str().c_str();
	//	theApp.m_BACdlg->m_status.AddString(cs1);
	//	return 0;
	//}
	//catch (...) {
	//	cs1.Format(L"ERROR: Could not parse parameter 4 of ramp: %S", rs.c_str());
	//	theApp.m_BACdlg->m_status.AddString(cs1);
	//	return 0;
	//}

	return retVal;
} // end FPTMbase::EvalRamp


// MAG Creates RampValue, calls other EvalRamp function, adds new RampValue to rampList
int
FPTMbase::EvalRamp(std::string rs, int evid)
{
	RampValue	rv;
	int			fv = 0;

	fv = EvalRamp(&rv, rs);	// MAG return value could be 0, 1, or 2
	if (fv != 0) {
		rv.SetEventID(evid);
		rampList[TRactive].push_back(rv);
		return fv;
	}

	return  0;
}


void 
FPTMbase::SetCurrentTestStep(int val)
{
	if (val < currentTestStep) {
		CString csx = L"ERROR setting lower test step!!!";
		return;
	}
	currentTestStep = val;
}


void 
FPTMbase::SetActiveColumn(int val)
{
	if (val < activeColumn) {
		CString csx = L"ERROR setting lower test step!!!";
		return;
	}
	activeColumn = val;
}