#ifndef ABOUT_H
#define ABOUT_H

#include "ui_About.h"

class cAbout : public QDialog, public Ui::qdAbout
{
	Q_OBJECT

	public:
		cAbout(QWidget *qwParent);

		void SetApplication(QString qsName);
		void SetVersion(QString qsVersion);
}; // cAbout

#endif
