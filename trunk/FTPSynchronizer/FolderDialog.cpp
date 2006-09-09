#include "FolderDialog.h"

// create dialog
cFolderDialog::cFolderDialog()
{
	setupUi(this);
	setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
} // cFolderDialog