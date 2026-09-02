#include "PeriodicInput.h"

PeriodicInput::PeriodicInput() { 
	varExpression = -1; 
	period = 0; 
	mod = 0.0; 
	modType = -1; 
	varSet = -1; 
	trSource = -1;
}

void
PeriodicInput::Reset()
{
	varExpression = -1;
	period = 10; // default value starting v20
	mod = 0.0;
	modType = -1;
	varSet = -1;
	trSource = -1;
}
