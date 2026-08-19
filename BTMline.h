#pragma once
#include "MDateTimeCtrl.h"
#include "EditDevice.h"
#include "MComboBox.h"

// Class BTMline
// represents one line on the BTM display
class BTMline
	{
	public:
		BTMline();
		BTMline(int id, int width);  // id = which lien, also gives vertical location, width = (width of window - 14) for border
		~BTMline();

		//int SetValue(int id, CString text);  // id is number of cedit, note not available for all data
		
		void SetDisplayID(int val);	// sets the line number (update to reset to position when add capability to delete lines)
		void SetStart();	// set all display elements _inactive_ when started
		void SetStop();		// set all display elements _active_ when started

		int AddDevice(CString ips);
		int RemoveDevices(); // MAG 260305 removes all devices from device combo boxes. Next call UpdateDevices to refill. Used when changing display options.
		int GetDeviceCount();

		CTime GetActivationTime();

	protected:
		CEditDevice		*line;
		CMDateTimeCtrl	*date;
		CMDateTimeCtrl	*time;
		CMComboBox		*mode;		// read or write - read=0, write=1
		CMComboBox		*device;	// mac / router
		CMComboBox		*object;	// BACnet object
		CEditDevice		*instance;
		CEditDevice		*value;
		CEditDevice		*comment;

		CTime			startTime;  // values from date and time controls combined- add mods should use this variable
		int				lineID;		// the line number in the display.  This should not change.
		int				displayID;	// the ID of the Task currently displayed on this line, which may be different from the lineID

	public:
		int SetActivationTime(CTime ntime);
		int GetActivationTime(CTime *ntime);	// MAG 170329

		// Returns the current value.  Could be value entered for write, value read from device, or -999.0
		double GetValue();

		// Sets the value of the request
		int SetValue(double nval);

		// Changes display states to match read/write state.  0 = read, 1 = write
		int SetRW(int rw);

		// Returns the current line ID
		int GetLine();

		// Returns the current display ID
		int GetDisplayID();

		// Sets visibility of referenced line
		int SetVisible(int isvis);

		// Updates the display showing nline
		int DisplayLine(int nline);
	};
