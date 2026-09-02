#pragma once
#include <string>
#include "fpvar.h"

// MAG 241112
// Class will hold "program BACnet point names" and "variable names" 

class PointVarMap {
protected:
	std::string longName; // string from first column of XML config file
	std::string pointName; // string from second column of XML config file
	int variableNameIndex; // index to variable with name matching string from third column of XML config file (index from list of variables)
public:
	PointVarMap() { variableNameIndex = -1; };
	void SetLongName(std::string ip) { longName = ip; };
	void SetPointName(std::string ip) { pointName = ip; };
	void SetVariableNameIndex(int i) { variableNameIndex = i; };

	std::string GetLongName() { return longName; };
	std::string GetPoinName() { return pointName; };
	int CheckName(std::string ip); // checks string ip against longName and pointName, returns variableNameIndex if they match or 0 if they don't

};