#include "About.h"

// create dialog
cAbout::cAbout()
{
	setupUi(this);
	setWindowFlags(Qt::SubWindow);
	setWindowModality(Qt::ApplicationModal);
} // cAbout

// set application name
void cAbout::SetApplication(QString qsName)
{
	qlApplication->setText(qsName);
} // SetApplication

// set application version
void cAbout::SetVersion(QString qsVersion)
{
	qlVersion->setText(qsVersion);
} // SetVersion