#include "ConnectionDialog.h"

// create dialog
cConnectionDialog::cConnectionDialog()
{
	setupUi(this);
	setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
} // cConnectionDialog