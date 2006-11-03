#include "System.h"

#ifdef Q_WS_WIN
#include <QSettings>
#endif

// get common program files directory
QString cSystem::GetCommonFilesPath()
{
	// TODO GetCommonFilesPath - Windows only
	QString qsResult;

#ifdef Q_WS_WIN
	QSettings qsRegistry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion", QSettings::NativeFormat);

	qsResult = qsRegistry.value("CommonFilesDir").toString();
#endif

	return qsResult;
} // GetCommonFilesPath

// get Isshou/Common/QT directory
QString cSystem::GetIsshouCommonPath()
{
	QString qsResult;

	qsResult = GetCommonFilesPath();
	qsResult += "/Isshou";

	return qsResult;
} // GetIsshouCommonPath

// get program files directory
QString cSystem::GetProgramFilesPath()
{
	// TODO GetProgramFilesPath - Windows only
	QString qsResult;

#ifdef Q_WS_WIN
	QSettings qsRegistry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion", QSettings::NativeFormat);

	qsResult = qsRegistry.value("ProgramFilesDir").toString();
#endif

	return qsResult;
} // GetProgramFilesPath