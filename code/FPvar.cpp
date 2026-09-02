#include "FPvar.h"

int
FPvar::IsVar(char *inlab)
{
	if (inlab != NULL) {
		if (label.compare(inlab) == 0)
			return 1;
	}

	return 0;
}


// MAG  250324 move from inline version to catch debug
void
FPvar::SetValue(double v1)
{
	//prevValue = value; 
	value = v1;
}


// MAG 241126 Modify from inline version to support Taylor/CalNEXT script files, as some values are text
void
FPvar::SetValue(char* inlab)
{
	if (inlab == NULL) {
		//prevValue = value;
		value = 0;
		return;
	}

	if (inlab[0] == '=') {  // set to a variable or other value that needs more processing
		//MAG 241126 TODO: this may point to a variable name or other comparison (i.e. gt, lt than value, formula)
		//prevValue = value;
		value = 0;
		return;
	}
	
	//prevValue = value;

	if (isalpha(inlab[0]) != 0) {
		switch (inlab[0]) {
			case 'n':	// NULL
			case 'N':
				//prevValue = value;
				value = 0;
				break;
			case 'p':	// Present
			case 'P':
				//prevValue = value;
				value = 1;
				break;
			case 'c':	// Closed
			case 'C':
				//prevValue = value;
				value = 0;
				break;
			case 'o':	// Occupied or On
			case 'O':
				//prevValue = value;
				value = 1;
			case 'y':	// Yes
			case 'Y':
				//prevValue = value;
				value = 1;
				break;
			default:
				//prevValue = value;
				value = 0;
				break;
		}
		return;
	}

	//prevValue = value;
	value = std::stod(inlab);

	return;
}


// MAG 241126 Modify from inline version to support Taylor/CalNEXT script files, as some values are text
// copied from SetValue- not all functionality needed but could be in future versions
void
FPvar::SetErrBound(char* inlab)
{
	if (inlab == NULL) {
		errBound = 0;
		return;
	}

	if (inlab[0] == '=') {  // set to a variable or other value that needs more processing
		//MAG 241126 TODO: this may point to a variable name or other comparison (i.e. gt, lt than value, formula)
		errBound = 0;
		return;
	}

	if (isalpha(inlab[0]) != 0) {
		switch (inlab[0]) {
		case 'n':	// NULL
		case 'N':
			errBound = 0;
			break;
		case 'p':	// Present
		case 'P':
			errBound = 1;
			break;
		case 'c':	// Closed
		case 'C':
			errBound = 0;
			break;
		case 'o':	// Occupied or On
		case 'O':
			errBound = 1;
		case 'y':	// Yes
		case 'Y':
			errBound = 1;
			break;
		default:
			errBound = 0;
			break;
		}
		return;
	}

	errBound = std::stod(inlab);

	// check for % values, reduce accordingly
	if (strchr(inlab, '%') != NULL) {
		errBound = errBound / 100.0;
	}

	return;
}
