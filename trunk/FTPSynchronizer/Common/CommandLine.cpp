#include "CommandLine.h"

// add option to parse
void cCommandLine::AddKeyValue(const QString qsOption, QString *qsValue)
{
	qhKeyValues.insert("-" + qsOption, qsValue);
} // AddKeyValue

// check if parametr is keyvalue
bool cCommandLine::IsKeyValue(const QString qsKey)
{
	if (qhKeyValues.contains(qsKey)) {
		return true;
	} else {
		return false;
	} // if else
} // IsKeyValue

// parse added options
void cCommandLine::Parse() {
	int iArgument;

	iArgument = 1;

	while (iArgument < iArguments) {
		if (IsKeyValue(cArguments[iArgument])) {
			iArgument++;
			SetKeyValue(cArguments[iArgument - 1], cArguments[iArgument]);
		} // if

		iArgument++;
	} // while
} // Parse

// set value for found key
void cCommandLine::SetKeyValue(const QString qsKey, const QString qsValue)
{
	*qhKeyValues.value(qsKey) = qsValue;
} // SetKeyValue
