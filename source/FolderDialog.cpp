#include "FolderDialog.h"

// create dialog
cFolderDialog::cFolderDialog(QWidget *qwParent)
{
	setParent(qwParent, windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	setupUi(this);
} // cFolderDialog
