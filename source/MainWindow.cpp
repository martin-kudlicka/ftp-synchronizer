#include "MainWindow.h"

#include <QStack>
#include <QScrollBar>
#include <QMessageBox>
#include <QResizeEvent>
#include "Common/CommandLine.h"
#include "Common/About.h"

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
	qmConnectionAdd->setIcon(QIcon(":/menubar/Images/MainWindow/menubar/Add.png"));
	qmConnectionAdd->addAction(QIcon(":/menubar/Images/MainWindow/menubar/Add Connection.png"), tr("&Connection"), this, SLOT(on_qaAddConnection_triggered()));
	qmConnectionAdd->addAction(QIcon(":/menubar/Images/MainWindow/menubar/Add Folder.png"), tr("F&older"), this, SLOT(on_qaAddFolder_triggered()));
	qmConnection->addMenu(qmConnectionAdd);
	qaContextEdit = qmConnection->addAction(QIcon(":/menubar/Images/MainWindow/menubar/Edit.png"), tr("&Edit"), this, SLOT(on_qaEdit_triggered()));
	qaContextEdit->setEnabled(false);
	qaContextRemove = qmConnection->addAction(QIcon(":/menubar/Images/MainWindow/menubar/Remove.png"), tr("&Remove"), this, SLOT(on_qaRemove_triggered()));
	qaContextRemove->setEnabled(false);

	// show connection tree
	qtwiHeader = new QTreeWidgetItem();
	qtwiHeader->setText(0, tr("Connections"));
	qtwConnections->setHeaderItem(qtwiHeader);
	ShowConnectionTree();

	// connect signals
	connect((QObject *)&csSynchronize, SIGNAL(SendMessage(const QString, const eMessageType)),
			  this, SLOT(on_cSynchronize_Message(const QString, const eMessageType)));
	connect(this, SIGNAL(StopSynchronization()),
			  (QObject *)&csSynchronize, SLOT(on_StopSynchronization()));
	connect((QObject *)&csSynchronize, SIGNAL(Done()), this, SLOT(on_cSynchronize_Done()));
	connect((QObject *)&csSynchronize, SIGNAL(FTPStateChanged(const int)),
			  this, SLOT(on_cSynchronize_FTPStateChanged(const int)));
	connect((QObject *)&csSynchronize, SIGNAL(Progress(const qint64, const qint64)),
			  this, SLOT(on_cSynchronize_Progress(const qint64, const qint64)));

	// initialize variables
	csSynchronize.ccConnections = &ccConnections;
} // cMainWindow

// connection dialog accepted - make changes
void cMainWindow::ConnectionOrFolderDialogAccepted(const cConnectionDialog *ccdNewConnection,
																	const cFolderDialog *cfdNewFolder,
																	const cConnections::eModify emModify)
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
		// correction
		if (!ccdNewConnection->qleDestination->text().startsWith("ftp://", Qt::CaseInsensitive)) {
			// add ftp:// prefix
			ccdNewConnection->qleDestination->setText("ftp://" + ccdNewConnection->qleDestination->text());
		} // if
		if (!ccdNewConnection->qleDestination->text().endsWith('/')) {
			// add last slash
			ccdNewConnection->qleDestination->setText(ccdNewConnection->qleDestination->text() + '/');
		} // if
		qdnNewItem = ccConnections.ModifyConnection(emModify,
																  qdnSelected,
																  // Connection
																  ccdNewConnection->qleName->text(),
																  ccdNewConnection->qleSource->text(),
																  ccdNewConnection->qleDestination->text(),
																  ccdNewConnection->qleDestinationUsername->text(),
																  ccdNewConnection->qleDestinationPassword->text(),
																  // Settings
																  ccdNewConnection->cbIncludeSubdirectories->isChecked(),
																  // Synchronization
																  qsSynchronization,
																  ccdNewConnection->cbBuffered->isChecked(),
																  ccdNewConnection->cbDeleteObsoleteFiles->isChecked());
	} else {
		// folder
		qdnNewItem = ccConnections.ModifyFolder(emModify,
															 qdnSelected,
															 // properties
															 cfdNewFolder->qleName->text());
	} // if else

	if (emModify == cConnections::Add) {
		// add to QTreeWidget
		if (!qtwiSelected) {
			qtwiNewItem = new QTreeWidgetItem(qtwConnections);
		} else {
			if (ccConnections.GetProperty(qdnSelected, cConnections::Type) == qsFOLDER) {
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
			qtwiNewItem->setIcon(0, QIcon(":/Connections/Images/MainWindow/Connections/Connection.png"));
		} else {
			qtwiNewItem->setText(0, cfdNewFolder->qleName->text());
			qtwiNewItem->setIcon(0, QIcon(":/Connections/Images/MainWindow/Connections/Folder.png"));
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

// custom event handler
void cMainWindow::customEvent(QEvent *event)
{
	cCommandLine cclOptions;
	QList<QTreeWidgetItem *> qltwiItems;
	QString qsConnection;

	cclOptions.qslArguments = QCoreApplication::arguments();

	cclOptions.AddKeyValue("c", &qsConnection);

	// parse
	cclOptions.Parse();

	// find connection in tree
	qltwiItems = qtwConnections->findItems(qsConnection, Qt::MatchRecursive);
	if (qltwiItems.count() == 0 || qltwiItems.at(0)->childCount() > 0) {
		// skip if not exists or is folder
		close();
		return;
	} // if
	qtwConnections->setCurrentItem(qltwiItems.at(0));

	// synchronize
	csSynchronize.qsName = qsConnection;
	csSynchronize.Start();
} // event

// returns true if tree item is folder
bool cMainWindow::IsFolder(QTreeWidgetItem *qtwiItem)
{
	QDomNode qdnNode;

	qdnNode = qhTable.value(qtwiItem);
	if (ccConnections.GetProperty(qdnNode, cConnections::Type) == qsFOLDER) {
		return true;
	} else {
		return false;
	} // if else
} // IsFolder

// synchronization done
void cMainWindow::on_cSynchronize_Done()
{
	// enable controls
	qaStart->setEnabled(true);
	qaStop->setEnabled(false);

	on_cSynchronize_Message(tr("Done\n"), Information);
	qpbProgress->setValue(0);

	if (bCommandLine) {
		close();
	} // if

	ShowInfo(qtwConnections->currentItem());
} // on_cSynchronize_Done

// destination FTP state change
void cMainWindow::on_cSynchronize_FTPStateChanged(int iState)
{
	QString qsStatus;

	switch (iState) {
		case QFtp::Unconnected:	qsStatus = tr("There is no connection to the host.");
										break;
		case QFtp::HostLookup:	qsStatus = tr("A host name lookup is in progress.");
										break;
		case QFtp::Connecting:	qsStatus = tr("An attempt to connect to the host is in progress.");
										break;
		case QFtp::Connected:	qsStatus = tr("Connection to the host has been achieved.");
										break;
		case QFtp::LoggedIn:		qsStatus = tr("Connection and user login have been achieved.");
										break;
		case QFtp::Closing:		qsStatus = tr("The connection is closing down, but it is not yet closed.");
										break;
	} // switch

	this->statusBar()->showMessage(qsStatus);
} // on_cSynchronize_FTPStateChanged

// message from Synchronize class
void cMainWindow::on_cSynchronize_Message(const QString qsMessage, const eMessageType emtMessageType)
{
	switch (emtMessageType) {
		case Error:				qteLog->setTextColor("red");
									break;
		case ForDestination:	qteLog->setTextColor("blue");
									break;
		case ForSource:		qteLog->setTextColor("green");
									break;
		case Information:		qteLog->setTextColor("black");
									break;
	} // switch

	qteLog->insertPlainText(QString("%1\n").arg(qsMessage));
	qteLog->verticalScrollBar()->setValue(qteLog->verticalScrollBar()->maximum());
} // on_cSynchronize_Message

// synchronization progress slot
void cMainWindow::on_cSynchronize_Progress(qint64 qi64Done, qint64 qi64Total)
{
	qpbProgress->setMaximum(qi64Total);
	qpbProgress->setValue(qi64Done);
} // on_cSynchronize_Progress

// show about window
void cMainWindow::on_qaAbout_triggered()
{
	cAbout *caAbout;

	caAbout = new cAbout(this);
	caAbout->SetApplication(qsAPPLICATION);
	caAbout->SetVersion(qsVERSION);
	caAbout->show();
} // on_qaAbout_triggered

// add new connection
void cMainWindow::on_qaAddConnection_triggered()
{
	cConnectionDialog *ccdNewConnection;

	ccdNewConnection = new cConnectionDialog(this);
	if (ccdNewConnection->exec() == QDialog::Accepted) {
		ConnectionOrFolderDialogAccepted(ccdNewConnection, NULL, cConnections::Add);
	} // if

	ccdNewConnection->deleteLater();
} // on_qaAddConnection_triggered

// add new folder
void cMainWindow::on_qaAddFolder_triggered()
{
	cFolderDialog *cfdNewFolder;

	cfdNewFolder = new cFolderDialog(this);
	if (cfdNewFolder->exec() == QDialog::Accepted) {
		ConnectionOrFolderDialogAccepted(NULL, cfdNewFolder, cConnections::Add);
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
		cfdNewFolder = new cFolderDialog(this);

		// fill values
		cfdNewFolder->qleName->setText(ccConnections.GetProperty(qdnFolder, cConnections::Name));

		if (cfdNewFolder->exec() == QDialog::Accepted) {
			ConnectionOrFolderDialogAccepted(NULL, cfdNewFolder, cConnections::Modify);
		} // if

		cfdNewFolder->deleteLater();
	} else {
		// connection
		cConnectionDialog *ccdNewConnection;
		QDomNode qdnConnection;
		QString qsProperty;

		qdnConnection = qhTable.value(qtwConnections->currentItem());
		ccdNewConnection = new cConnectionDialog(this);

		// fill values
		// Connection
		ccdNewConnection->qleName->setText(ccConnections.GetProperty(qdnConnection, cConnections::Name));
		ccdNewConnection->qleSource->setText(ccConnections.GetProperty(qdnConnection, cConnections::SourcePath));
		ccdNewConnection->qleDestination->setText(ccConnections.GetProperty(qdnConnection, cConnections::DestinationPath));
		ccdNewConnection->qleDestinationUsername->setText(ccConnections.GetProperty(qdnConnection, cConnections::DestinationUsername));
		ccdNewConnection->qleDestinationPassword->setText(ccConnections.GetProperty(qdnConnection, cConnections::DestinationPassword));
		// Settings
		qsProperty = ccConnections.GetProperty(qdnConnection, cConnections::IncludeSubdirectories);
		if (qsProperty == qsTRUE) {
			ccdNewConnection->cbIncludeSubdirectories->setChecked(true);
		} else {
			ccdNewConnection->cbIncludeSubdirectories->setChecked(false);
		} // if else
		// Synchronization
		qsProperty = ccConnections.GetProperty(qdnConnection, cConnections::SynchronizationType);
		if (qsProperty == qsUPLOAD) {
			ccdNewConnection->rbUpload->setChecked(true);
		} else {
			ccdNewConnection->rbDownload->setChecked(true);
		} // if else
		qsProperty = ccConnections.GetProperty(qdnConnection, cConnections::Buffered);
		if (qsProperty == qsTRUE) {
			ccdNewConnection->cbBuffered->setChecked(true);
		} else {
			ccdNewConnection->cbBuffered->setChecked(false);
		} // if else
		qsProperty = ccConnections.GetProperty(qdnConnection, cConnections::DeleteObsoleteFiles);
		if (qsProperty == qsTRUE) {
			ccdNewConnection->cbDeleteObsoleteFiles->setChecked(true);
		} else {
			ccdNewConnection->cbDeleteObsoleteFiles->setChecked(false);
		} // if else

		if (ccdNewConnection->exec() == QDialog::Accepted) {
			ConnectionOrFolderDialogAccepted(ccdNewConnection, NULL, cConnections::Modify);
		} // if

		ccdNewConnection->deleteLater();
	} // if else
} // on_qaEdit_triggered

// remove folder or connection
void cMainWindow::on_qaRemove_triggered()
{
	if (QMessageBox::question(this, tr("Remove"), tr("Are you sure?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
		QDomNode qdnSelected;
		QTreeWidgetItem *qtwiSelected;

		qtwiSelected = qtwConnections->currentItem();
		qdnSelected = qhTable.value(qtwiSelected);
		ccConnections.Remove(qdnSelected);
		delete qtwiSelected;
	} // if
} // on_qaRemove_triggered

// start synchronization
void cMainWindow::on_qaStart_triggered()
{
	// disable controls
	qaStart->setEnabled(false);
	qaStop->setEnabled(true);

	on_cSynchronize_Message(tr("Synchronization started"), Information);

	csSynchronize.qsName = qtwConnections->currentItem()->text(0);
	csSynchronize.Start();
} // on_qsStart_triggered

// interrupt synchronization
void cMainWindow::on_qaStop_triggered()
{
	emit StopSynchronization();
} // on_qaStop_triggered

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

// catch main window resizing
void cMainWindow::resizeEvent(QResizeEvent *event)
{
	qsSplitter->setGeometry(iBORDER, iBORDER, width() - 2 * iBORDER, height() - 6 * iBORDER);
} // resizeEvent

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
		qtwiItem->setText(0, ccConnections.GetProperty(qdnConnection, cConnections::Name));

		// insert to table
		qhTable.insert(qtwiItem, qdnConnection);

		// go to the next one
		if (ccConnections.GetProperty(qdnConnection, cConnections::Type) == qsFOLDER && !qdnConnection.namedItem(qsCONNECTION).isNull()) {
			// non-empty folder
			qsXMLLevel.push(qdnConnection);
			qsTreeLevel.push(qtwiItem);
			qtwiItem->setIcon(0, QIcon(":/Connections/Images/MainWindow/Connections/Folder.png"));
			qdnConnection = qdnConnection.namedItem(qsCONNECTION);
		} else {
			// empty folder or connection
			if (ccConnections.GetProperty(qdnConnection, cConnections::Type) == qsCONNECTION) {
				qtwiItem->setIcon(0, QIcon(":/Connections/Images/MainWindow/Connections/Connection.png"));
			} else {
				qtwiItem->setIcon(0, QIcon(":/Connections/Images/MainWindow/Connections/Folder.png"));
			} // if else
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
		if (ccConnections.GetProperty(qdnSelected, cConnections::Type) == qsCONNECTION) {
			QString qsBuffered, qsDeleteObsoleteFiles, qsDestination, qsIncludeSubdirectories, qsLastRun, qsSource, qsType;

			qsSource = "<b>" + tr("Source") + ":</b> " + ccConnections.GetProperty(qdnSelected, cConnections::SourcePath);
			qsDestination = "<b>" + tr("Destination") + ":</b> " + ccConnections.GetProperty(qdnSelected, cConnections::DestinationPath);
			qsIncludeSubdirectories = "<b>" + tr("Include subdirectories") + ":</b> ";
			if (ccConnections.GetProperty(qdnSelected, cConnections::IncludeSubdirectories) == qsTRUE) {
				qsIncludeSubdirectories += "<img src=\":/ConnectionInfo/Images/MainWindow/ConnectionInfo/Yes.png\">";
			} else {
				qsIncludeSubdirectories += "<img src=\":/ConnectionInfo/Images/MainWindow/ConnectionInfo/No.png\">";
			} // if else
			qsType = "<b>" + tr("Type") + ":</b> " + ccConnections.GetProperty(qdnSelected, cConnections::SynchronizationType);
			qsBuffered = "<b>" + tr("Buffered") + ":</b> ";
			if (ccConnections.GetProperty(qdnSelected, cConnections::Buffered) == qsTRUE) {
				qsBuffered += "<img src=\":/ConnectionInfo/Images/MainWindow/ConnectionInfo/Yes.png\">";
			} else {
				qsBuffered += "<img src=\":/ConnectionInfo/Images/MainWindow/ConnectionInfo/No.png\">";
			} // if else
			qsDeleteObsoleteFiles = "<b>" + tr("Delete obsolete files") + ":</b> ";
			if (ccConnections.GetProperty(qdnSelected, cConnections::DeleteObsoleteFiles) == qsTRUE) {
				qsDeleteObsoleteFiles += "<img src=\":/ConnectionInfo/Images/MainWindow/ConnectionInfo/Yes.png\">";
			} else {
				qsDeleteObsoleteFiles += "<img src=\":/ConnectionInfo/Images/MainWindow/ConnectionInfo/No.png\">";
			} // if else
			qsLastRun = "<b>" + tr("Last run") + ":</b> ";
			qsLastRun += ccConnections.GetProperty(qdnSelected, cConnections::LastRun);

			qteConnectionInfo->setHtml(qsSource + "<br />" + qsDestination + "<br /><br />" +
												qsIncludeSubdirectories + "<br /><br />" +
												qsType + "<br />" + qsBuffered + "<br />" + qsDeleteObsoleteFiles + "<br /><br />" +
												qsLastRun);
		} else {
			qteConnectionInfo->clear();
		} // if else
	} else {
		qteConnectionInfo->clear();
	} // if else
} // ShowInfo
