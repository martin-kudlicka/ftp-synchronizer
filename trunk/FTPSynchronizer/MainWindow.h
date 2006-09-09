#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui_MainWindow.h"

#include "Connections.h"
#include <QHash>
#include "ConnectionDialog.h"
#include "FolderDialog.h"
#include "Synchronize.h"

class cMainWindow : public QMainWindow, private Ui::qmwMainWindow
{
	Q_OBJECT;

	public:
		cMainWindow();

	private:
		const static unsigned int uiTOOL_BAR_POSITION = 1;

		cConnections ccConnections;
		QAction *qaContextEdit, *qaContextRemove;
		QHash <QTreeWidgetItem *, QDomNode> qhTable;
		QMenu *qmConnection;

		void ConnectionOrFolderDialogAccepted(const cConnectionDialog *ccdNewConnection,
														  const cFolderDialog *cfdNewFolder,
														  const cConnections::eModify emModify);
		bool IsFolder(QTreeWidgetItem *qtwiItem);
		void ShowConnectionTree();
		void ShowInfo(QTreeWidgetItem *qtwiSelected);

	private slots:
		void on_cSynchronize_Message(const QString qsMessage);
		void on_qaAddConnection_triggered();
		void on_qaAddFolder_triggered();
		void on_qaEdit_triggered();
		void on_qaRemove_triggered();
		void on_qaStart_triggered();
		void on_qfDestination_stateChanged(int state);
		void on_qtwConnections_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
		void on_qtwConnections_customContextMenuRequested(const QPoint &pos);
}; // cMainWindow

#endif