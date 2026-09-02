#include "RampValue.h"

double 
RampValue::GetValueAtTime(int evTime)
{
	double rv;

	if (evTime < 0) return startValue;
	if (evTime > duration) return endValue;

	rv = startValue + (endValue - startValue) * (1.0 * evTime / (duration-1));

	return rv;
}