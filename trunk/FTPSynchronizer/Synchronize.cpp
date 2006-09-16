#include "Synchronize.h"

#include "Common/XMLTools.h"

const QString qsDIRECTORY = "Directory";
const QString qsFILE = "File";
const QString qsITEM = "Item";
const QString qsLAST_MODIFIED = "LastModified";

// build XML buffer for edDirection
void cSynchronize::BuildBuffer(const eDirection edDirection)
{
	QDomNode qdnBuffer;
	QHash<QString, sFileInfo *> *qhDirectories, *qhFiles;
	QHash<QString, sFileInfo *>::const_iterator qhDirectoriesIterator, qhFilesIterator;

	qdnBuffer = ccConnections->GetBuffer(qdnConnection, edDirection);
	if (edDirection == Source) {
		qhFiles = &qhSourceFiles;
		qhDirectories = &qhSourceDirectories;
	} else {
		qhFiles = &qhDestinationFiles;
		qhDirectories = &qhDestinationDirectories;
	} // if else
	qhFilesIterator = qhFiles->constBegin();
	qhDirectoriesIterator = qhDirectories->constBegin();

	// directories
	while (qhDirectoriesIterator != qhDirectories->constEnd()) {
		QDomElement qdeItem, qdeName;

		qdeItem = ccConnections->qddXML.createElement(qsITEM);
		qdnBuffer.appendChild(qdeItem);
		qdeItem.setAttribute(qsTYPE, qsDIRECTORY);
		qdeName = ccConnections->qddXML.createElement(qsNAME);
		qdeItem.appendChild(qdeName);
		cXMLTools::SetText(ccConnections->qddXML, &qdeName, qhDirectoriesIterator.key());
		qhDirectoriesIterator.value()->qdnXMLItem = qdeItem;

		qhDirectoriesIterator++;
	} // while

	// files
	while (qhFilesIterator != qhFiles->constEnd()) {
		QDomElement qdeItem, qdeLastModified, qdeName;

		qdeItem = ccConnections->qddXML.createElement(qsITEM);
		qdnBuffer.appendChild(qdeItem);
		qdeItem.setAttribute(qsTYPE, qsFILE);
		qdeName = ccConnections->qddXML.createElement(qsNAME);
		qdeItem.appendChild(qdeName);
		cXMLTools::SetText(ccConnections->qddXML, &qdeName, qhFilesIterator.key());
		qdeLastModified = ccConnections->qddXML.createElement(qsLAST_MODIFIED);
		qdeItem.appendChild(qdeLastModified);
		cXMLTools::SetText(ccConnections->qddXML, &qdeLastModified, qhFilesIterator.value()->qdtLastModified.toString());
		qhFilesIterator.value()->qdnXMLItem = qdeItem;

		qhFilesIterator++;
	} // while
} // BuildBuffer

// return true if file can be copied in chosed direction
bool cSynchronize::CanCopy(const QHashIterator<QString, sFileInfo *> qhiI, const eDirection edDirection)
{
	QHash<QString, sFileInfo *> *qhTargetFiles;
	sFileInfo *sfiTarget;

	if (edDirection == Source) {
		qhTargetFiles = &qhSourceFiles;
	} else {
		qhTargetFiles = &qhDestinationFiles;
	} // if else

	if (!qhTargetFiles->contains(qhiI.key())) {
		// file doesn't exists on target -> copy
		return true;
	} // if

	sfiTarget = qhTargetFiles->value(qhiI.key());
	
	// compare last modified stamp
	if (sfiTarget->qdtLastModified != qhiI.value()->qdtLastModified) {
		return true;
	} else {
		return false;
	} // if else
} // CanCopy

// connect to FTP
void cSynchronize::ConnectDestination()
{
	qfDestination.connectToHost(quDestination.host());
	qfDestination.login(ccConnections->GetProperty(qdnConnection, cConnections::DestinationUsername),
							  ccConnections->GetProperty(qdnConnection, cConnections::DestinationPassword));
} // ConnectDestination

// copy files to destination
void cSynchronize::CopyFiles(const eDirection edDirection)
{
	if (edDirection == Source) {
		// copy to source
		QHashIterator<QString, sFileInfo *> qhiI(qhDestinationFiles);

		while (qhiI.hasNext() && !bStop) {
			int iCommand;
			QFile *qfFile;
			sCommand scCommand;

			qhiI.next();
			if (!bBufferedDestination || CanCopy(qhiI, edDirection)) {
				qfFile = new QFile(quSource.path() + "/" + qhiI.key());
				scCommand.qfFile = qfFile;
				scCommand.qsMessage = tr("Copying: %1").arg(qhiI.key());
				iCommand = qfDestination.get(quDestination.path() + "/" + qhiI.key(), qfFile);
				qhCommands.insert(iCommand, scCommand);
			} else {
				emit SendMessage(tr("Skipping: %1").arg(qhiI.key()));
			} // if else
		} // while
	} else {
		// copy to destination
		QHashIterator<QString, sFileInfo *> qhiI(qhSourceFiles);

		while (qhiI.hasNext() && !bStop) {
			int iCommand;
			QFile *qfFile;
			sCommand scCommand;

			qhiI.next();
			if (!bBufferedSource || CanCopy(qhiI, edDirection)) {
				qfFile = new QFile(quSource.path() + "/" + qhiI.key());
				qfFile->open(QIODevice::ReadOnly);
				scCommand.qfFile = qfFile;
				scCommand.qsMessage = tr("Copying: %1").arg(qhiI.key());
				iCommand = qfDestination.put(qfFile, quDestination.path() + "/" + qhiI.key());
				qhCommands.insert(iCommand, scCommand);
			} else {
				emit SendMessage(tr("Skipping: %1").arg(qhiI.key()));
			} // if else
		} // while
	} // if else
} // CopyFiles

// create destination directories
void cSynchronize::CreateDirectories(const eDirection edDirection)
{
	if (edDirection == Source) {
		// create on source
		QHashIterator<QString, sFileInfo *> qhiI(qhDestinationDirectories);

		while (qhiI.hasNext() && !bStop) {
			qhiI.next();
			if (!qhSourceDirectories.contains(qhiI.key())) {
				QDir qdDir;

				emit SendMessage(tr("Creating: %1").arg(qhiI.key()));
				qdDir.mkdir(quSource.path() + "/" + qhiI.key());
			} // if
		} // while
	} else {
		// create on destination
		QHashIterator<QString, sFileInfo *> qhiI(qhSourceDirectories);

		while (qhiI.hasNext() && !bStop) {
			qhiI.next();
			if (!qhDestinationDirectories.contains(qhiI.key())) {
				emit SendMessage(tr("Creating: %1").arg(qhiI.key()));
				qfDestination.mkdir(quDestination.path() + "/" + qhiI.key());
			} // if
		} // while
	} // if else
} // CreateDirectories

// constructor
cSynchronize::cSynchronize()
{
	connect(&qfDestination, SIGNAL(listInfo(const QUrlInfo &)),
			  SLOT(on_qfDestination_listInfo(const QUrlInfo &)));
	connect(&qfDestination, SIGNAL(done(bool)), SLOT(on_qfDestination_done(bool)));
	connect(&qfDestination, SIGNAL(commandFinished(int, bool)),
			  SLOT(on_qfDestination_commandFinished(int, bool)));
	connect(&qfDestination, SIGNAL(commandStarted(int)),
			  SLOT(on_qfDestination_commandStarted(int)));
	connect(&qfDestination, SIGNAL(stateChanged(int)),
			  SLOT(on_qfDestination_stateChanged(int)));
	connect(&qfDestination, SIGNAL(dataTransferProgress(qint64, qint64)),
			  SLOT(on_qfDestination_dataTransferProgress(qint64, qint64)));
} // cSynchronize

// deinitialization class
void cSynchronize::Deinitialization()
{
	if (!bGUIRunning) {
		delete ccConnections;
	} // if
} // Deinitialization

// delete obsolete files and folders
void cSynchronize::DeleteObsolete(const eDirection edDirection)
{
	if (edDirection == Source) {
		// delete on source
		QHashIterator<QString, sFileInfo *> qhiDirectories(qhSourceDirectories);
		QHashIterator<QString, sFileInfo *> qhiFiles(qhSourceFiles);

		// files
		while (qhiFiles.hasNext() && !bStop) {
			qhiFiles.next();
			if (!qhDestinationFiles.contains(qhiFiles.key())) {
				QFile qfFile;

				qfFile.setFileName(quSource.path() + "/" + qhiFiles.key());
				emit SendMessage(tr("Removing: %1").arg(qhiFiles.key()));
				qfFile.remove();
			} // if
		} // while

		// directories
		while (qhiDirectories.hasNext() && !bStop) {
			qhiDirectories.next();
			if (!qhDestinationDirectories.contains(qhiDirectories.key())) {
				QDir qdDir;

				emit SendMessage(tr("Removing: [%1]").arg(qhiDirectories.key()));
				qdDir.rmdir(quSource.path() + "/" + qhiDirectories.key());
			} // if
		} // while
	} else {
		// delete on destination
		QHashIterator<QString, sFileInfo *> qhiDirectories(qhDestinationDirectories);
		QHashIterator<QString, sFileInfo *> qhiFiles(qhDestinationFiles);

		// files
		while (qhiFiles.hasNext() && !bStop) {
			qhiFiles.next();
			if (!qhSourceFiles.contains(qhiFiles.key())) {
				emit SendMessage(tr("Removing: %1").arg(qhiFiles.key()));
				qfDestination.remove(quDestination.path() + "/" + qhiFiles.key());
			} // if
		} // while

		// directories
		while (qhiDirectories.hasNext() && !bStop) {
			qhiDirectories.next();
			if (!qhSourceDirectories.contains(qhiDirectories.key())) {
				emit SendMessage(tr("Removing: [%1]").arg(qhiDirectories.key()));
				qfDestination.rmdir(quDestination.path() + "/" + qhiDirectories.key());
			} // if
		} // while
	} // if else
} // DeleteObsolete

// disconnect from FTP
void cSynchronize::DisconnectDestination()
{
	qfDestination.close();
} // DisconnectDestination

// fill source files and directories tables from buffer
void cSynchronize::FillSourceLists(const eDirection edDirection)
{
	QDomNode qdnBuffer, qdnItem;
	QHash<QString, sFileInfo *> *qhDirectories, *qhFiles;

	if (edDirection == Source) {
		qhDirectories = &qhSourceDirectories;
		qhFiles = &qhSourceFiles;
	} else {
		qhDirectories = &qhDestinationDirectories;
		qhFiles = &qhDestinationFiles;
	} // if else

	qdnBuffer = ccConnections->GetBuffer(qdnConnection, edDirection);
	qdnItem = qdnBuffer.firstChild();
	
	while (!qdnItem.isNull()) {
		sFileInfo *sfiValue;

		sfiValue = new sFileInfo();
		sfiValue->qdnXMLItem = qdnItem;
		if (qdnItem.toElement().attribute(qsTYPE) == qsDIRECTORY) {
			// directory
			qhDirectories->insert(qdnItem.namedItem(qsNAME).toElement().text(), sfiValue);
		} else {
			// file
			sfiValue->qdtLastModified = QDateTime::fromString(qdnItem.namedItem(qsLAST_MODIFIED).toElement().text());
			qhFiles->insert(qdnItem.namedItem(qsNAME).toElement().text(), sfiValue);
		} // if else

		qdnItem = qdnItem.nextSibling();
	} // while
} // FillSourceLists

// fills files and directories info
void cSynchronize::GetFileList(const eDirection edDirection)
{
	if (edDirection == Source) {
		// source
		if (!bBufferedSource || !ccConnections->BufferExists(edDirection, qdnConnection)) {
			// get source from disk
			bool bIncludeSubdirectories;
			QDir qdSource;

			if (ccConnections->GetProperty(qdnConnection, cConnections::IncludeSubdirectories) == qsTRUE) {
				bIncludeSubdirectories = true;
			} else {
				bIncludeSubdirectories = false;
			} // if else

			qdSource.setPath(quSource.path());

			do {
				int iI;
				QFileInfoList qfilCurrentList;
				QString qsDirectory;
				QStringList qslCurrentList;

				// list current directory for files
				qfilCurrentList = qdSource.entryInfoList(QDir::Files);
				// add files to global
				for (iI = 0; iI < qfilCurrentList.count(); iI++) {
					sFileInfo *sfiValue;

					sfiValue = new sFileInfo();
					sfiValue->qdtLastModified = qfilCurrentList.at(iI).lastModified();
					qhSourceFiles.insert(qsCurrentDirectory + qfilCurrentList.at(iI).fileName(), sfiValue);
					emit SendMessage(tr("Source: %1").arg(qfilCurrentList.at(iI).fileName()));
				} // for

				// list current directory for directories
				qslCurrentList = qdSource.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
				if (!qslCurrentList.empty()) {
					QQueue<QString> qqCurrentDirectories;

					for (iI = 0; iI < qslCurrentList.count(); iI++) {
						sFileInfo *sfiValue;

						sfiValue = new sFileInfo();
						qhSourceDirectories.insert(qsCurrentDirectory + qslCurrentList.at(iI), sfiValue);
						qqCurrentDirectories.enqueue(qslCurrentList.at(iI));
						emit SendMessage(tr("Source: [%1]").arg(qslCurrentList.at(iI)));
					} // for
					qsDirectoryLevel.push(qqCurrentDirectories);
				} else {
					// no directories -> go back
					qdSource.cd("..");
					qsCurrentDirectory +=  "../";
				} // if else

				qsDirectory = SetDirectory(Source, &qdSource);
				qsCurrentDirectory = QDir::cleanPath(qsCurrentDirectory);
				if (qsCurrentDirectory.endsWith("..")) {
					qsCurrentDirectory.clear();
				} else {
					if (qsCurrentDirectory != "") {
						qsCurrentDirectory += "/";
					} // if
				} // if else
				qdSource.cd(qsDirectory);
				if (qsDirectory != "") {
					qsCurrentDirectory += qsDirectory + "/";
				} // if
			} while (!qsDirectoryLevel.empty() && bIncludeSubdirectories && !bStop);

			// buffer
			if (bBufferedSource) {
				BuildBuffer(edDirection);
			} // if
		} else {
			// get source from buffer
			FillSourceLists(edDirection);
		} // if else
	} else {
		// destination
		if (!bBufferedDestination || !ccConnections->BufferExists(edDirection, qdnConnection)) {
			// get destination from disk
			qfDestination.cd(quDestination.path());
			// list current directory
			qfDestination.list();
			// rest as in source in on_qfDestination_commandFinished
		} else {
			// get destination from buffer
			FillSourceLists(edDirection);
		} // if else
	} // if else
} // GetFileList

// initialize class
void cSynchronize::Initialization()
{
	// connect XML
	if (!ccConnections) {
		ccConnections = new cConnections();
		bGUIRunning = false;
	} else {
		bGUIRunning = true;
	} // if else

	// clear from previous run
	qhDestinationDirectories.clear();
	qhDestinationFiles.clear();
	qhSourceDirectories.clear();
	qhSourceFiles.clear();
	qsCurrentDirectory.clear();
	qsDirectoryLevel.clear();

	// find connection in XML
	qdnConnection = ccConnections->FindConnection(qsName);

	// initialize variables
	if (ccConnections->GetProperty(qdnConnection, cConnections::Buffered) == qsTRUE) {
		if (ccConnections->GetProperty(qdnConnection, cConnections::SynchronizationType) == qsUPLOAD) {
			bBufferedSource = true;
			bBufferedDestination = false;
		} else {
			bBufferedSource = false;
			bBufferedDestination = true;
		} // if else
	} else {
		bBufferedSource = false;
		bBufferedDestination = false;
	} // if else
	bStop = false;
} // Initialization

// single FTP command finished
void cSynchronize::on_qfDestination_commandFinished(int id, bool error)
{
	// Close
	if (qfDestination.currentCommand() == QFtp::Close && !bStop) {
		SynchronizationEnd(tr("OK"));
		return;
	} // if

	// Get, Put
	if (qfDestination.currentCommand() == QFtp::Get || qfDestination.currentCommand() == QFtp::Put) {
		sCommand scCommand;

		scCommand = qhCommands.value(id);
		scCommand.qfFile->close();
		scCommand.qfFile->deleteLater();
		return;
	} // if

	// List
	if (qfDestination.currentCommand() == QFtp::List) {
		bool bIncludeSubdirectories;
		QHashIterator<QString, sFileInfo *> qhiI(qhCurrentDestinationFiles);
		QString qsDirectory;

		// add files to global
		while (qhiI.hasNext()) {
			qhiI.next();
			qhDestinationFiles.insert(qsCurrentDirectory + qhiI.key(), qhiI.value());
		} // while

		// add directories to global and stack, go to antoher directory
		if (!qqCurrentDestinationDirectories.empty()) {
			qsDirectoryLevel.push(qqCurrentDestinationDirectories);
			while (!qqCurrentDestinationDirectories.empty()) {
				sFileInfo *sfiValue;

				sfiValue = new sFileInfo();
				qhDestinationDirectories.insert(qsCurrentDirectory + qqCurrentDestinationDirectories.dequeue(), sfiValue);
			} // while
		} else {
			// no directories -> go back
			qfDestination.cd("..");
			qsCurrentDirectory +=  "../";
		} // if else

		qsDirectory = SetDirectory(Destination);
		qsCurrentDirectory = QDir::cleanPath(qsCurrentDirectory);
		if (qsCurrentDirectory.endsWith("..")) {
			qsCurrentDirectory.clear();
		} else {
			if (qsCurrentDirectory != "") {
				qsCurrentDirectory += "/";
			} // if
		} // if else
		if (qsDirectory != "") {
			qsCurrentDirectory += qsDirectory + "/";
		} // if

		if (ccConnections->GetProperty(qdnConnection, cConnections::IncludeSubdirectories) == qsTRUE) {
			bIncludeSubdirectories = true;
		} else {
			bIncludeSubdirectories = false;
		} // if else

		if (!qsDirectoryLevel.empty() && bIncludeSubdirectories && !bStop) {
			// next round...
#ifdef _DEBUG
		emit SendMessage(tr("Change: %1").arg(qsDirectory));
#endif
			qfDestination.cd(qsDirectory);
			qfDestination.list();
		} else {
			// buffer
			if (bBufferedDestination) {
				BuildBuffer(Destination);
			} // if

			// continue in synchronization
			Synchronize2();
		} // if else
	} // if
} // on_qfDestination_commandFinished

// single FTP command started
void cSynchronize::on_qfDestination_commandStarted(int id)
{
	// Get, Put
	if (qfDestination.currentCommand() == QFtp::Get || qfDestination.currentCommand() == QFtp::Put) {
		sCommand scCommand;

		scCommand = qhCommands.value(id);
		if (qfDestination.currentCommand() == QFtp::Get) {
			// open file for writing here (in CopyFiles it create all files at first)
			scCommand.qfFile->open(QIODevice::WriteOnly);
		} // if
		emit SendMessage(scCommand.qsMessage);

		return;
	} // if
} // on_qfDestination_commandStarted

// synchronization progress signal
void cSynchronize::on_qfDestination_dataTransferProgress(qint64 done, qint64 total)
{
	emit Progress(done, total);
} // on_qfDestination_dataTransferProgress

// pending operations on FTP done
void cSynchronize::on_qfDestination_done(bool error)
{
	if (error) {
		// finish on error
		emit SendMessage(tr("Error: %1").arg(qfDestination.errorString()));
		SynchronizationEnd(qfDestination.errorString());
		bStop = true;
		if (qfDestination.error() != QFtp::ConnectionRefused) {
			DisconnectDestination();
		} // if
		return;
	} // if
} // on_qfDestination_done

// searching for files and directories
void cSynchronize::on_qfDestination_listInfo(const QUrlInfo &i)
{
	if (!i.isSymLink() && i.name() != "." && i.name() != "..") {
		QString qsMessage;

		qsMessage = tr("Destination: ");

		if (i.isDir()) {
			qsMessage += QString("[%1]").arg(i.name());
			qqCurrentDestinationDirectories.enqueue(i.name());
		} else {
			sFileInfo *sfiValue;

			qsMessage += i.name();
			sfiValue = new sFileInfo();
			sfiValue->qdtLastModified = i.lastModified();
			qhCurrentDestinationFiles.insert(i.name(), sfiValue);
		} // if else
		emit SendMessage(qsMessage);
	} // if
} // on_qfDestination_listInfo

// FTP state change
void cSynchronize::on_qfDestination_stateChanged(int state)
{
	emit FTPStateChanged(state);
} // on_qfDestination_stateChanged

// stop synchronization by external signal
void cSynchronize::on_StopSynchronization()
{
	bStop = true;
	qfDestination.abort();
	DisconnectDestination();
	SynchronizationEnd(tr("Stopped"));
} // on_StopSynchronization

// get next directory from queue and optionally set right directory string with FTP level
QString cSynchronize::SetDirectory(const eDirection edDirection, QDir *qdDir /* NULL */)
{
	QString qsNewDirectory;

	while (!qsDirectoryLevel.empty() && qsDirectoryLevel.top().empty()) {
		qsDirectoryLevel.pop();
		if (edDirection == Source) {
			qdDir->cd("..");
		} else {
			qfDestination.cd("..");
		} // if else
		qsCurrentDirectory += "../";
	} // while
	if (!qsDirectoryLevel.empty()) {
		qsNewDirectory = qsDirectoryLevel.top().dequeue();
	} // if

	return qsNewDirectory;
} // SetDirectory

// input class method - start of synchronization process
void cSynchronize::Start()
{
	Initialization();

	quSource = ccConnections->GetProperty(qdnConnection, cConnections::SourcePath);
	quDestination = ccConnections->GetProperty(qdnConnection, cConnections::DestinationPath);

	ConnectDestination();

	Synchronize();
} // Start

// end of synchronization process
void cSynchronize::SynchronizationEnd(QString qsMessage)
{
	ccConnections->SetLastRun(qdnConnection, QDateTime::currentDateTime(), qsMessage);
	emit Done();
	Deinitialization();
} // SynchronizationEnd

// get file and folder lists
void cSynchronize::Synchronize()
{
	// get source and destination file list
	emit SendMessage(tr("Searching for files and folders..."));
	// TODO infinite progress bar
	GetFileList(Source);
	GetFileList(Destination);
} // Synchronize

// the rest of synchronization process
void cSynchronize::Synchronize2()
{
	eDirection edDirection;

	if (ccConnections->GetProperty(qdnConnection, cConnections::SynchronizationType) == qsDOWNLOAD) {
		edDirection = Source;
	} else {
		edDirection = Destination;
	} // if else

	// delete obsolete files and folders first
	if (ccConnections->GetProperty(qdnConnection, cConnections::DeleteObsoleteFiles) == qsTRUE) {
		DeleteObsolete(edDirection);
	} // if

	// create directories
	CreateDirectories(edDirection);

	// copy files
	CopyFiles(edDirection);

	if (bStop) {
		qfDestination.clearPendingCommands();
	} // if

	// disconnect
	DisconnectDestination();
} // Synchronize2