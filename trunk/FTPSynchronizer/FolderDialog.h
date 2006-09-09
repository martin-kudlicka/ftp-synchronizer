#ifndef FOLDER_DIALOG_H
#define FOLDER_DIALOG_H

#include "ui_FolderDialog.h"

class cFolderDialog : public QDialog, public Ui::qdFolderDialog
{
	Q_OBJECT;

	public:
		cFolderDialog();
}; // cFolderDialog

#endif