#ifndef ABOUT_H
#define ABOUT_H

#include "ui_About.h"

class cAbout : public QWidget, public Ui::qwAbout
{
	Q_OBJECT

	public:
		cAbout();

		void SetApplication(QString qsName);
		void SetVersion(QString qsVersion);
}; // cAbout

#endif