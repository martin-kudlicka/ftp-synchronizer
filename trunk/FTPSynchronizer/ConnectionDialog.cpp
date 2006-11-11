#include "ConnectionDialog.h"

#include <QFileDialog>

// create dialog
cConnectionDialog::cConnectionDialog()
{
	setupUi(this);
	setWindowFlags(Qt::SubWindow);
} // cConnectionDialog

// display destination password
void cConnectionDialog::on_qcbDisplayPassword_stateChanged(int state)
{
	if (state == Qt::Unchecked) {
		qleDestinationPassword->setEchoMode(QLineEdit::Password);
	} else {
		qleDestinationPassword->setEchoMode(QLineEdit::Normal);
	} // if else
} // on_qcbDisplayPassword_stateChanged

// browse for source directory
void cConnectionDialog::on_qpbBrowseSource_clicked(bool checked /* false */)
{
	QString qsDirectory;

	qsDirectory = QFileDialog::getExistingDirectory(this, tr("Select directory"), "/");
	if (qsDirectory != "") {
		qleSource->setText(qsDirectory);
	} // if
} // on_qpbBrowseSource_clicked
