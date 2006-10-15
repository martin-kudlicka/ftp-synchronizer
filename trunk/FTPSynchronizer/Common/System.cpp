#include "System.h"

#include <stdlib.h>

// get Isshou/Common/QT directory
QString cSystem::GetIsshouQTPath()
{
	QString qsResult;

	qsResult = GetProgramFilesDirectory();
	qsResult += "/Isshou/Common/QT";

	return qsResult;
} // GetIsshouQTPath

// get program files directory (Windows only for now)
QString cSystem::GetProgramFilesDirectory()
{
	// TODO GetProgramFilesDirectory - Linux and Macintosh version
	QString qsResult;

#ifdef Q_WS_WIN
	char *cBuffer;

	cBuffer = getenv("PROGRAMFILES");
	qsResult = cBuffer;
#endif

	return qsResult;
} // GetProgramFilesDirectory