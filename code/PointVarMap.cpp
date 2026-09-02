#include "PointVarMap.h"

int
PointVarMap::CheckName(std::string ip)
{
	if (ip.length() == 0) return 0;

	if (longName.compare(ip) == 0) return variableNameIndex;
	if (pointName.compare(ip) == 0) return variableNameIndex;

	return 0;
}