#include "MainWindow.h"

#include <QStack>

const QString qsBOTH_WAYS = "Both ways";
const QString qsDOWNLOAD = "Download";
const QString qsUPLOAD = "Upload";

const unsigned int uiTOOL_BAR_POSITION = 1;

// create of main window
cMainWindow::cMainWindow()
{
	QMenu *qmConnectionAdd;
	QTreeWidgetItem *qtwiHeader;

	setupUi(this);
	// disable status bar
	setStatusBar(NULL);
	// reposition toolbar into left splitter
	qsLeftSplitter->insertWidget(uiTOOL_BAR_POSITION, toolBar);
	// create qtwFTP context menu
	qmConnection = new QMenu(qtwConnections);
	qmConnectionAdd = new QMenu(tr("&Add"), qmConnection);
	qmConnectionAdd->addAction(tr("&Connection"), this, SLOT(on_qaAddConnection_triggered()));
	qmConnectionAdd->addAction(tr("F&older"), this, SLOT(on_qaAddFolder_triggered()));
	qmConnection->addMenu(qmConnectionAdd);
	qaContextEdit = qmConnection->addAction(tr("&Edit"), this, SLOT(on_qaEdit_triggered()));
	qaContextEdit->setEnabled(false);
	qaContextRemove = qmConnection->addAction(tr("&Remove"), this, SLOT(on_qaRemove_triggered()));
	qaContextRemove->setEnabled(false);
	// show connection tree
	qtwiHeader = new QTreeWidgetItem();
	qtwiHeader->setText(0, "Connections");
	qtwConnections->setHeaderItem(qtwiHeader);
	ShowConnectionTree();
} // cMainWindow

// connection dialog accepted - make changes
void cMainWindow::ConnectionDialogAccepted(const cConnectionDialog *ccdNewConnection, const eModify emModify)
{
	QDomNode qdnNewConnection, qdnSelected;
	QString qsSynchronization;
	QTreeWidgetItem *qtwiNewConnection, *qtwiSelected;

	// send to cConnections
	qtwiSelected = qtwConnections->currentItem();
	if (qtwiSelected) {
		qdnSelected = qhTable.value(qtwiSelected);
	} // if
	if (ccdNewConnection->rbUpload->isChecked()) {
		qsSynchronization = qsUPLOAD;
	} else {
		if (ccdNewConnection->rbDownload->isChecked()) {
			qsSynchronization = qsDOWNLOAD;
		} else {
			qsSynchronization = qsBOTH_WAYS;
		} // if else
	} // if else
	qdnNewConnection = ccConnections.ModifyConnection(emModify,
																	  qdnSelected,
																	  // Connection
																	  ccdNewConnection->qleName->text(),
																	  ccdNewConnection->qleSource->text(),
																	  ccdNewConnection->qleSourceUsername->text(),
																	  ccdNewConnection->qleSourcePassword->text(),
																	  ccdNewConnection->qleDestination->text(),
																	  ccdNewConnection->qleDestinationUsername->text(),
																	  ccdNewConnection->qleDestinationUsername->text(),
																	  // Settings
																	  ccdNewConnection->cbIncludeSubdirectories->isChecked(),
																	  // Synchronization
																	  qsSynchronization,
																	  ccdNewConnection->cbDeleteObsoleteFiles->isChecked());
	if (emModify == Add) {
		// add to TreeWidget
		if (!qtwiSelected) {
			qtwiNewConnection = new QTreeWidgetItem(qtwConnections);
		} else {
			if (ccConnections.GetProperty(qdnSelected, Type) == qsFOLDER) {
				qtwiNewConnection = new QTreeWidgetItem(qtwiSelected);
			} else {
				if (qtwiSelected->parent()) {
					qtwiNewConnection = new QTreeWidgetItem(qtwiSelected->parent(), qtwiSelected);
				} else {
					qtwiNewConnection = new QTreeWidgetItem(qtwConnections, qtwiSelected);
				} // if else
			} // if else
		} // if else
		qtwiNewConnection->setText(0, ccdNewConnection->qleName->text());
	} // if

	// to hash table
	qhTable.insert(qtwiNewConnection, qdnNewConnection);
} // ConnectionDialogAccepted

// add new connection
void cMainWindow::on_qaAddConnection_triggered()
{
	cConnectionDialog *ccdNewConnection;

	ccdNewConnection = new cConnectionDialog();
	if (ccdNewConnection->exec() == QDialog::Accepted) {
		ConnectionDialogAccepted(ccdNewConnection, Add);
	} // if
} // on_qaAddConnection_triggered

// add new folder
void cMainWindow::on_qaAddFolder_triggered()
{
	// TODO on_qaAddFolder_triggered
} // on_qaAddFolder_triggered

// edit existing connection
void cMainWindow::on_qaEdit_triggered()
{
	cConnectionDialog *ccdNewConnection;
	QDomNode qdnConnection;
	QString qsProperty;

	qdnConnection = qhTable.value(qtwConnections->currentItem());
	ccdNewConnection = new cConnectionDialog();

	// fill values
	// Connection
	ccdNewConnection->qleName->setText(ccConnections.GetProperty(qdnConnection, Name));
	ccdNewConnection->qleSource->setText(ccConnections.GetProperty(qdnConnection, Source));
	ccdNewConnection->qleSourceUsername->setText(ccConnections.GetProperty(qdnConnection, SourceUsername));
	ccdNewConnection->qleSourcePassword->setText(ccConnections.GetProperty(qdnConnection, SourcePassword));
	ccdNewConnection->qleDestination->setText(ccConnections.GetProperty(qdnConnection, Destination));
	ccdNewConnection->qleDestinationUsername->setText(ccConnections.GetProperty(qdnConnection, DestinationUsername));
	ccdNewConnection->qleDestinationPassword->setText(ccConnections.GetProperty(qdnConnection, DestinationPassword));
	// Settings
	qsProperty = ccConnections.GetProperty(qdnConnection, IncludeSubdirectories);
	if (qsProperty == qsTRUE) {
		ccdNewConnection->cbIncludeSubdirectories->setChecked(true);
	} else {
		ccdNewConnection->cbIncludeSubdirectories->setChecked(false);
	} // if else
	// Synchronization
	qsProperty = ccConnections.GetProperty(qdnConnection, SynchronizationType);
	if (qsProperty == qsUPLOAD) {
		ccdNewConnection->rbUpload->setChecked(true);
	} else {
		if (qsProperty == qsDOWNLOAD) {
			ccdNewConnection->rbDownload->setChecked(true);
		} else {
			ccdNewConnection->rbBothWays->setChecked(true);
		} // if else
	} // if else
	qsProperty = ccConnections.GetProperty(qdnConnection, DeleteObsoleteFiles);
	if (qsProperty == qsTRUE) {
		ccdNewConnection->cbDeleteObsoleteFiles->setChecked(true);
	} else {
		ccdNewConnection->cbDeleteObsoleteFiles->setChecked(false);
	} // if else

	if (ccdNewConnection->exec() == QDialog::Accepted) {
		ConnectionDialogAccepted(ccdNewConnection, Modify);
	} // if
} // on_qaEdit_triggered

// remove folder or connection
void cMainWindow::on_qaRemove_triggered()
{
	// TODO on_qaRemove_triggered
} // on_qaRemove_triggered

// another item selcted in tree view
void cMainWindow::on_qtwConnections_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	//if (current) {
		// context menu
		qaContextEdit->setEnabled(true);
		qaContextRemove->setEnabled(true);
		// main menu
		qaEdit->setEnabled(true);
		qaRemove->setEnabled(true);
	/*} else {
		// context menu
		qaContextEdit->setEnabled(false);
		qaContextRemove->setEnabled(false);
		// main menu
		qaEdit->setEnabled(false);
		qaRemove->setEnabled(false);
	} // if else*/
} // on_qtwConnections_currentItemChanged

// context menu on connection tree view
void cMainWindow::on_qtwConnections_customContextMenuRequested(const QPoint &pos)
{
	qmConnection->exec(qtwConnections->viewport()->mapToGlobal(pos));
} // on_qtwFTPTree_customContextMenuRequested

// fill qtwConnection with connections from XML and fill qhTable
void cMainWindow::ShowConnectionTree()
{
	QDomNode qdnConnection;
	QStack<QDomNode> qsXMLLevel;
	QStack<QTreeWidgetItem *> qsTreeLevel;

	qdnConnection = ccConnections.qddXML.documentElement().firstChild();
	while (!qdnConnection.isNull()) {
		QTreeWidgetItem *qtwiItem;

		// insert to tree
		if (qsTreeLevel.empty()) {
			qtwiItem = new QTreeWidgetItem(qtwConnections);
		} else {
			qtwiItem = new QTreeWidgetItem(qsTreeLevel.top());
		} // if else
		qtwiItem->setText(0, ccConnections.GetProperty(qdnConnection, Name));

		// insert to table
		qhTable.insert(qtwiItem, qdnConnection);

		// go to the next one
		if (ccConnections.GetProperty(qdnConnection, Type) == qsFOLDER) {
			qsXMLLevel.push(qdnConnection);
			qsTreeLevel.push(qtwiItem);
			qdnConnection = qdnConnection.namedItem(qsCONNECTION);
		} else {
			qdnConnection = qdnConnection.nextSibling();
			while (qdnConnection.isNull() && !qsXMLLevel.isEmpty()) {
				qdnConnection = qsXMLLevel.pop();
				qdnConnection = qdnConnection.nextSibling();
				qsTreeLevel.pop();
			} // while
		} // if else
	} // while
} // ShowConnectionTree