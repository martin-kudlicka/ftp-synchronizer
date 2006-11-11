#ifndef SYSTEM_H
#define SYSTEM_H

#include <QString>

class cSystem
{
	public:
		static QString GetCommonFilesPath();
		static QString GetIsshouCommonPath();
		static QString GetProgramFilesPath();
}; // cSystem

#endif
