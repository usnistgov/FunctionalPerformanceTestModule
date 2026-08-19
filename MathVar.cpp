#include "MathVar.h"

MathVar::MathVar() {
	// value of -1 indicates not set
	var1index = -1;
	var2index = -1;
	var2value = 0;
	varOutIndex = -1;
	varOperator = -1;// { "+-*/" }; 0-3
}