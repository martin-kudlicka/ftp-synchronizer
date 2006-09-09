#ifndef CONNECTION_DIALOG_H
#define CONNECTION_DIALOG_H

#include "ui_ConnectionDialog.h"

class cConnectionDialog : public QDialog, public Ui::qdConnectionDialog
{
	Q_OBJECT;

	public:
		cConnectionDialog();

	private slots:
		//void on_qaAddFTP_triggered();
}; // cConnectionDialog

#endif