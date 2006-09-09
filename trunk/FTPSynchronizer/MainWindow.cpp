#include "MainWindow.h"

#include <QStack>

const QString qsDOWNLOAD = "Download";
const QString qsUPLOAD = "Upload";

const unsigned int uiTOOL_BAR_POSITION = 1;

// create of main window
cMainWindow::cMainWindow()
{
	QMenu *qmConnectionAdd;
	QTreeWidgetItem *qtwiHeader;

	setupUi(this);
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
	qtwiHeader->setText(0, tr("Connections"));
	qtwConnections->setHeaderItem(qtwiHeader);
	ShowConnectionTree();
} // cMainWindow

// connection dialog accepted - make changes
void cMainWindow::ConnectionOrFolderDialogAccepted(const cConnectionDialog *ccdNewConnection,
																	const cFolderDialog *cfdNewFolder,
																	const eModify emModify)
{
	QDomNode qdnNewItem, qdnSelected;
	QTreeWidgetItem *qtwiNewItem, *qtwiSelected;

	// get selected item in tree and XML
	qtwiSelected = qtwConnections->currentItem();
	if (qtwiSelected) {
		qdnSelected = qhTable.value(qtwiSelected);
	} // if

	if (ccdNewConnection) {
		// connection
		QString qsSynchronization;

		if (ccdNewConnection->rbUpload->isChecked()) {
			qsSynchronization = qsUPLOAD;
		} else {
			qsSynchronization = qsDOWNLOAD;
		} // if else
		qdnNewItem = ccConnections.ModifyConnection(emModify,
																  qdnSelected,
																  // Connection
																  ccdNewConnection->qleName->text(),
																  ccdNewConnection->qleSource->text(),
																  ccdNewConnection->qleDestination->text(),
																  ccdNewConnection->qleDestinationUsername->text(),
																  ccdNewConnection->qleDestinationUsername->text(),
																  // Settings
																  ccdNewConnection->cbIncludeSubdirectories->isChecked(),
																  // Synchronization
																  qsSynchronization,
																  ccdNewConnection->cbDeleteObsoleteFiles->isChecked());
	} else {
		// folder
		qdnNewItem = ccConnections.ModifyFolder(emModify,
															 qdnSelected,
															 // properties
															 cfdNewFolder->qleName->text());
	} // if else

	if (emModify == Add) {
		// add to QTreeWidget
		if (!qtwiSelected) {
			qtwiNewItem = new QTreeWidgetItem(qtwConnections);
		} else {
			if (ccConnections.GetProperty(qdnSelected, Type) == qsFOLDER) {
				qtwiNewItem = new QTreeWidgetItem(qtwiSelected);
			} else {
				if (qtwiSelected->parent()) {
					qtwiNewItem = new QTreeWidgetItem(qtwiSelected->parent(), qtwiSelected);
				} else {
					qtwiNewItem = new QTreeWidgetItem(qtwConnections, qtwiSelected);
				} // if else
			} // if else
		} // if else
		if (ccdNewConnection) {
			qtwiNewItem->setText(0, ccdNewConnection->qleName->text());
		} else {
			qtwiNewItem->setText(0, cfdNewFolder->qleName->text());
		} // if else

		// to hash table
		qhTable.insert(qtwiNewItem, qdnNewItem);
	} else {
		// modify QTreeWidget
		if (ccdNewConnection) {
			qtwiSelected->setText(0, ccdNewConnection->qleName->text());
		} else {
			qtwiSelected->setText(0, cfdNewFolder->qleName->text());
		} // if else

		// to hash table
		qhTable.insert(qtwiSelected, qdnNewItem);
		// actualize information about selected connection
		ShowInfo(qtwiSelected);
	} // if else
} // ConnectionOrFolderDialogAccepted

// returns true if tree item is folder
bool cMainWindow::IsFolder(QTreeWidgetItem *qtwiItem)
{
	QDomNode qdnNode;

	qdnNode = qhTable.value(qtwiItem);
	if (ccConnections.GetProperty(qdnNode, Type) == qsFOLDER) {
		return true;
	} else {
		return false;
	} // if else
} // IsFolder

// add new connection
void cMainWindow::on_qaAddConnection_triggered()
{
	cConnectionDialog *ccdNewConnection;

	ccdNewConnection = new cConnectionDialog();
	if (ccdNewConnection->exec() == QDialog::Accepted) {
		ConnectionOrFolderDialogAccepted(ccdNewConnection, NULL, Add);
	} // if

	ccdNewConnection->deleteLater();
} // on_qaAddConnection_triggered

// add new folder
void cMainWindow::on_qaAddFolder_triggered()
{
	cFolderDialog *cfdNewFolder;

	cfdNewFolder = new cFolderDialog();
	if (cfdNewFolder->exec() == QDialog::Accepted) {
		ConnectionOrFolderDialogAccepted(NULL, cfdNewFolder, Add);
	} // if

	cfdNewFolder->deleteLater();
} // on_qaAddFolder_triggered

// edit existing connection
void cMainWindow::on_qaEdit_triggered()
{
	if (IsFolder(qtwConnections->currentItem())) {
		// folder
		cFolderDialog *cfdNewFolder;
		QDomNode qdnFolder;

		qdnFolder = qhTable.value(qtwConnections->currentItem());
		cfdNewFolder = new cFolderDialog();

		// fill values
		cfdNewFolder->qleName->setText(ccConnections.GetProperty(qdnFolder, Name));

		if (cfdNewFolder->exec() == QDialog::Accepted) {
			ConnectionOrFolderDialogAccepted(NULL, cfdNewFolder, Modify);
		} // if

		cfdNewFolder->deleteLater();
	} else {
		// connection
		cConnectionDialog *ccdNewConnection;
		QDomNode qdnConnection;
		QString qsProperty;

		qdnConnection = qhTable.value(qtwConnections->currentItem());
		ccdNewConnection = new cConnectionDialog();

		// fill values
		// Connection
		ccdNewConnection->qleName->setText(ccConnections.GetProperty(qdnConnection, Name));
		ccdNewConnection->qleSource->setText(ccConnections.GetProperty(qdnConnection, Source));
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
			ccdNewConnection->rbDownload->setChecked(true);
		} // if else
		qsProperty = ccConnections.GetProperty(qdnConnection, DeleteObsoleteFiles);
		if (qsProperty == qsTRUE) {
			ccdNewConnection->cbDeleteObsoleteFiles->setChecked(true);
		} else {
			ccdNewConnection->cbDeleteObsoleteFiles->setChecked(false);
		} // if else

		if (ccdNewConnection->exec() == QDialog::Accepted) {
			ConnectionOrFolderDialogAccepted(ccdNewConnection, NULL, Modify);
		} // if

		ccdNewConnection->deleteLater();
	} // if else
} // on_qaEdit_triggered

// remove folder or connection
void cMainWindow::on_qaRemove_triggered()
{
	QDomNode qdnSelected;
	QTreeWidgetItem *qtwiSelected;

	qtwiSelected = qtwConnections->currentItem();
	qdnSelected = qhTable.value(qtwiSelected);
	ccConnections.Remove(qdnSelected);
	delete qtwiSelected;
} // on_qaRemove_triggered

// another item selcted in tree view
void cMainWindow::on_qtwConnections_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current) {
		// context menu
		qaContextEdit->setEnabled(true);
		qaContextRemove->setEnabled(true);
		// main menu
		qaEdit->setEnabled(true);
		qaRemove->setEnabled(true);
		// toolbar
		if (IsFolder(current)) {
			qaStart->setEnabled(false);
		} else {
			qaStart->setEnabled(true);
		} // if else
		// show info about selected item
		ShowInfo(current);
	} else {
		// context menu
		qaContextEdit->setEnabled(false);
		qaContextRemove->setEnabled(false);
		// main menu
		qaEdit->setEnabled(false);
		qaRemove->setEnabled(false);
		// toolbar
		qaStart->setEnabled(false);
		// clear info
		ShowInfo(NULL);
	} // if else
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
		if (ccConnections.GetProperty(qdnConnection, Type) == qsFOLDER && !qdnConnection.namedItem(qsCONNECTION).isNull()) {
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

// show info about selected item
void cMainWindow::ShowInfo(QTreeWidgetItem *qtwiSelected)
{
	if (qtwiSelected) {
		QDomNode qdnSelected;

		qdnSelected = qhTable.value(qtwiSelected);
		if (ccConnections.GetProperty(qdnSelected, Type) == qsCONNECTION) {
			qteConnectionInfo->setPlainText(tr("Source: ") + ccConnections.GetProperty(qdnSelected, Source) + "\n" +
													  tr("Destination: ") + ccConnections.GetProperty(qdnSelected, Destination));
		} else {
			qteConnectionInfo->clear();
		} // if else
	} else {
		qteConnectionInfo->clear();
	} // if else
} // ShowInfo