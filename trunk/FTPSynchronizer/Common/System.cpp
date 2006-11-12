#include "System.h"

#include <QSettings>
#include <QDir>

const QString qsCOMPANY = "Isshou";

// get settings path for application
QString cSystem::GetApplicationSettingsPath(const QString qsApplication)
{
	QDir qdDir;
	QString qsPath;
	
	qsPath = GetIsshouSettingsPath();
	qdDir.mkdir(qsPath + "/" + qsApplication);
	return qsPath + "/" + qsApplication;
} // GetApplicationSettingsPath

// get common program files path
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

// get Isshou/Common/QT path
QString cSystem::GetIsshouCommonPath()
{
	QString qsResult;

	qsResult = GetCommonFilesPath();
	qsResult += "/Isshou";

	return qsResult;
} // GetIsshouCommonPath

// get settings path for applications
QString cSystem::GetIsshouSettingsPath()
{
	QDir qdDir;
	QFileInfo qfiFile;
	QSettings qsSettings(QSettings::IniFormat, QSettings::UserScope, qsCOMPANY);
	
	qfiFile.setFile(qsSettings.fileName());
	qdDir.mkdir(qfiFile.absolutePath() + "/" + qsCOMPANY);
	
	return qfiFile.absolutePath() + "/" + qsCOMPANY;
} // GetIsshouSettingsPath

// get program files path
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
