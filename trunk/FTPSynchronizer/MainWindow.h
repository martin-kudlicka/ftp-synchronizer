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
	Q_OBJECT

	public:
		bool bCommandLine;
		const static QEvent::Type qetCOMMANDLINE_EVENT = QEvent::User;

		cMainWindow();

	private:
		const static int iBORDER = 10;
		const static unsigned int uiTOOL_BAR_POSITION = 1;

		cConnections ccConnections;
		cSynchronize csSynchronize;
		QAction *qaContextEdit, *qaContextRemove;
		QHash <QTreeWidgetItem *, QDomNode> qhTable;
		QMenu *qmConnection;

		void ConnectionOrFolderDialogAccepted(const cConnectionDialog *ccdNewConnection,
														  const cFolderDialog *cfdNewFolder,
														  const cConnections::eModify emModify);
		void customEvent(QEvent *event);
		bool IsFolder(QTreeWidgetItem *qtwiItem);
		void resizeEvent(QResizeEvent *event);
		void ShowConnectionTree();
		void ShowInfo(QTreeWidgetItem *qtwiSelected);

	signals:
		void StopSynchronization();

	private slots:
		void on_cSynchronize_Done();
		void on_cSynchronize_FTPStateChanged(const int iState);
		void on_cSynchronize_Message(const QString qsMessage, const eMessageType emtMessageType);
		void on_cSynchronize_Progress(const qint64 qi64Done, const qint64 qi64Total);
		void on_qaAddConnection_triggered();
		void on_qaAddFolder_triggered();
		void on_qaEdit_triggered();
		void on_qaRemove_triggered();
		void on_qaStart_triggered();
		void on_qaStop_triggered();
		void on_qtwConnections_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
		void on_qtwConnections_customContextMenuRequested(const QPoint &pos);
}; // cMainWindow

#endif