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

	while (iArgument < qslArguments.count()) {
		if (IsKeyValue(qslArguments.at(iArgument))) {
			iArgument++;
			SetKeyValue(qslArguments.at(iArgument - 1), qslArguments.at(iArgument));
		} // if

		iArgument++;
	} // while
} // Parse

// set value for found key
void cCommandLine::SetKeyValue(const QString qsKey, const QString qsValue)
{
	*qhKeyValues.value(qsKey) = qsValue;
} // SetKeyValue
