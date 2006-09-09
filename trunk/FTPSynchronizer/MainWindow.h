#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui_MainWindow.h"

#include "Connections.h"
#include <QHash>
#include "ConnectionDialog.h"

class cMainWindow : public QMainWindow, private Ui::qmwMainWindow
{
	Q_OBJECT;

	public:
		cMainWindow();

	private:
		cConnections ccConnections;
		QAction *qaContextEdit, *qaContextRemove;
		QHash <QTreeWidgetItem *, QDomNode> qhTable;
		QMenu *qmConnection;

		void ConnectionDialogAccepted(const cConnectionDialog *ccdNewConnection, const eModify emModify);
		void ShowConnectionTree();

	private slots:
		void on_qaAddConnection_triggered();
		void on_qaAddFolder_triggered();
		void on_qaEdit_triggered();
		void on_qaRemove_triggered();
		void on_qtwConnections_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
		void on_qtwConnections_customContextMenuRequested(const QPoint &pos);
}; // cMainWindow

#endif