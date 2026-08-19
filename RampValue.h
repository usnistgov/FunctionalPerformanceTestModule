#pragma once

#define RAMPTIMER	31
#define NEXTLIST	32
// MAG 241231 create class to enable RAMP feature in Taylor/CalNEXT data files
// Array of RampValue objects will be created for each TRlist.
class RampValue
{
protected:
	double	startValue;
	double	endValue;
	// MAG 240115 RAMP will be updated wait for modifications
	int		duration;	//NOTE update time in seconds, default to 10 with v20
	int		updateSpan;	// time span interval at which value is updated (written to controller)
	int		eventID;	// index in TRlist, the TR event this ramp is associated with
	int		varStart;	// if > 0 variable value is used instead of startValue
	int		varEnd;		// if > 0 variable value is used instead of endValue
	int		activity;	// 0 = waiting, 1 = running, 2 = finished
	// start time add object as saved in main, or just get eventID->time
public:
	RampValue() { startValue = 0; endValue = 0; duration = 0; updateSpan = 10; eventID = varStart = varEnd = -1; activity = 0; };
	RampValue(double sv, double ev, int dur, int upd, int evid, int act) 
	{
		startValue = sv; endValue = ev; duration = dur; updateSpan = upd; eventID = evid; varStart = varEnd = -1; 
		if ((act < 0) || (act > 2)) activity = 0; else activity = act;
	};

	// Setter functions
	void SetStartValue(double val) { startValue = val; };
	void SetEndValue(double val) { endValue = val; };
	void SetDuration(int val) { duration = val; };
	void SetUpdateSpan(int val) { updateSpan = val; };
	void SetEventID(int val) { eventID = val; };
	void SetVarStart(int val) { varStart = val; };
	void SetVarEnd(int val) { varEnd = val; };
	void SetActivity(int val) { if (val < 0) return; if (val > 2) return; activity = val; };

	// Getter functions
	double GetStartValue() { return startValue; };
	double GetEndValue() { return endValue; };
	int GetDuration() { return duration; };
	int GetUpdateSpan() { return updateSpan; };
	int GetEventID() { return eventID; };
	int GetVarStart() { return varStart; };
	int GetVarEnd() {   return varEnd; };
	int GetActivity() { return activity; };

	double GetValueAtTime(int evTime);
};

