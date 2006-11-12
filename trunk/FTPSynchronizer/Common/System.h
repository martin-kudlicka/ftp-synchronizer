#ifndef SYSTEM_H
#define SYSTEM_H

#include <QString>

class cSystem
{
	public:
		static QString GetApplicationSettingsPath(const QString qsApplication);
		static QString GetCommonFilesPath();
		static QString GetIsshouCommonPath();
		static QString GetIsshouSettingsPath();
		static QString GetProgramFilesPath();
}; // cSystem

#endif
