#include "About.h"

// create dialog
cAbout::cAbout(QWidget *qwParent)
{
	setParent(qwParent, windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	setupUi(this);
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
