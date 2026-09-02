#pragma once

// MAG 250206
// Class MathVar
// Purpose: to enable functioning of compound tests in Taylor/CalNEXT files, such as when an ADD or other math function
// is embedded in another function declaration, such as this actual line from V8 test script:
// '=INTERPOLATE(HeatLoopOut; 0; 50; AHUSupAirTempSp; ADD(OccHeatSp; MaxDeltaT); AHUSupAirTempSp; ADD(OccHeatSp; MaxDeltaT))
// Method: Scan function will determine variable index or value for each parameter. Scan function will determine math operation
// required. Will create a new variable for the results of the operation. This MathVar struct will record required inputs.
// This will be part of TestStep, and values will be recalculated after read step of each TestStep.
class MathVar
{
public:
	MathVar();
	~MathVar() {};

	const int		GetVar1Index() { return var1index; };	// first one is always a var (index)
	const int		GetVar2Index() { return var2index; };	// second one may be var (index) or value
	const double	GetVar2Value() { return var2value; };
	const int		GetVarOutIndex() { return varOutIndex; };
	const int		GetVarOperator() { return varOperator; };

	void	SetVar1Index(int var) { var1index = var; };
	void	SetVar2Index(int var) { var2index = var; };
	void	SetVar2Value(double var) { var2value = var; };
	void	SetVarOutIndex(int var){ varOutIndex = var; };
	void	SetVarOperator(int var) { if (var < 0) return; if (var > 4) return; varOperator = var; };

protected:
	int var1index;
	int var2index;
	double var2value;
	int varOutIndex;
	int	varOperator;// { "+-*/(invert /)" }; +=1, -=2, *=3, /=4, 5 for invert (not used) and 0 for pass-thru (no math, not used)
};

