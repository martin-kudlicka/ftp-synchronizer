#include "Synchronize.h"

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
		int iI;

		for (iI = 0; iI < qqDestinationFiles.count() && !bStop; iI++) {
			int iCommand;
			QFile *qfFile;
			sCommand scCommand;

			qfFile = new QFile(quSource.path() + "/" + qqDestinationFiles.at(iI));
			qfFile->open(QIODevice::WriteOnly);
			scCommand.qfFile = qfFile;
			scCommand.qsMessage = tr("Copying: %1").arg(qqDestinationFiles.at(iI));
			iCommand = qfDestination.get(quDestination.path() + "/" + qqDestinationFiles.at(iI), qfFile);
			qhCommands.insert(iCommand, scCommand);
		} // for
	} else {
		// copy to destination
		int iI;

		for (iI = 0; iI < qqSourceFiles.count() && !bStop; iI++) {
			int iCommand;
			QFile *qfFile;
			sCommand scCommand;

			qfFile = new QFile(quSource.path() + "/" + qqSourceFiles.at(iI));
			qfFile->open(QIODevice::ReadOnly);
			scCommand.qfFile = qfFile;
			scCommand.qsMessage = tr("Copying: %1").arg(qqSourceFiles.at(iI));
			iCommand = qfDestination.put(qfFile, quDestination.path() + "/" + qqSourceFiles.at(iI));
			qhCommands.insert(iCommand, scCommand);
		} // for
	} // if else
} // CopyFiles

// create destination directories
void cSynchronize::CreateDirectories(const eDirection edDirection)
{
	if (edDirection == Source) {
		// create on source
		int iI;

		for (iI = 0; iI < qqDestinationDirectories.count() && !bStop; iI++) {
			if (qqSourceDirectories.indexOf(qqDestinationDirectories.at(iI)) == -1) {
				QDir qdDir;

				emit SendMessage(tr("Creating: %1").arg(qqDestinationDirectories.at(iI)));
				qdDir.mkdir(quSource.path() + "/" + qqDestinationDirectories.at(iI));
			} // if
		} // for
	} else {
		// create on destination
		int iI;

		for (iI = 0; iI < qqSourceDirectories.count() && !bStop; iI++) {
			if (qqDestinationDirectories.indexOf(qqSourceDirectories.at(iI)) == -1) {
				emit SendMessage(tr("Creating: %1").arg(qqSourceDirectories.at(iI)));
				qfDestination.mkdir(quDestination.path() + "/" + qqSourceDirectories.at(iI));
			} // if
		} // for
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
		int iI;

		// files
		for (iI = 0; iI < qqSourceFiles.count() && !bStop; iI++) {
			if (qqDestinationFiles.indexOf(qqSourceFiles.at(iI)) == -1) {
				QFile qfFile;

				qfFile.setFileName(quSource.path() + "/" + qqSourceFiles.at(iI));
				emit SendMessage(tr("Removing: %1").arg(qqSourceFiles.at(iI)));
				qfFile.remove();
			} // if
		} // for

		// directories
		for (iI = qqSourceDirectories.count() - 1; iI >= 0 && !bStop; iI--) {
			if (qqDestinationDirectories.indexOf(qqSourceDirectories.at(iI)) == -1) {
				QDir qdDir;

				emit SendMessage(tr("Removing: [%1]").arg(qqSourceDirectories.at(iI)));
				qdDir.rmdir(quSource.path() + "/" + qqSourceDirectories.at(iI));
			} // if
		} // for
	} else {
		// delete on destination
		int iI;

		// files
		for (iI = 0; iI < qqDestinationFiles.count() && !bStop; iI++) {
			if (qqSourceFiles.indexOf(qqDestinationFiles.at(iI)) == -1) {
				emit SendMessage(tr("Removing: %1").arg(qqDestinationFiles.at(iI)));
				qfDestination.remove(quDestination.path() + "/" + qqDestinationFiles.at(iI));
			} // if
		} // for

		// directories
		for (iI = qqDestinationDirectories.count() - 1 && !bStop; iI >= 0; iI--) {
			if (qqSourceDirectories.indexOf(qqDestinationDirectories.at(iI)) == -1) {
				emit SendMessage(tr("Removing: [%1]").arg(qqDestinationDirectories.at(iI)));
				qfDestination.rmdir(quDestination.path() + "/" + qqDestinationDirectories.at(iI));
			} // if
		} // for
	} // if else
} // DeleteObsolete

// disconnect from FTP
void cSynchronize::DisconnectDestination()
{
	qfDestination.close();
} // DisconnectDestination

// fills files and directories info
void cSynchronize::GetFileList(const eDirection edDirection)
{
	if (edDirection == Source) {
		// source
		if (!ccConnections->BufferExists(edDirection, qdnConnection) || ccConnections->GetProperty(qdnConnection, cConnections::SynchronizationType) == qsUPLOAD) {
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
				QString qsDirectory;
				QStringList qslCurrentList;

				// list current directory for files
				qslCurrentList = qdSource.entryList(QDir::Files);
				// add files to global
				for (iI = 0; iI < qslCurrentList.count(); iI++) {
					qqSourceFiles += qsCurrentDirectory + qslCurrentList.at(iI);
					emit SendMessage(tr("Source: %1").arg(qslCurrentList.at(iI)));
				} // for

				// list current directory for directories
				qslCurrentList = qdSource.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
				if (!qslCurrentList.empty()) {
					QQueue<QString> qqCurrentDirectories;

					for (iI = 0; iI < qslCurrentList.count(); iI++) {
						qqSourceDirectories += qsCurrentDirectory + qslCurrentList.at(iI);
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
		} else {
			// get source from buffer
			// TODO GetFileList (get source from buffer)
		} // if else
	} else {
		// destination
		if (!ccConnections->BufferExists(edDirection, qdnConnection) || ccConnections->GetProperty(qdnConnection, cConnections::SynchronizationType) == qsDOWNLOAD) {
			// get destination from disk
			qfDestination.cd(quDestination.path());
			// list current directory
			qfDestination.list();
			// rest as in source in on_qfDestination_commandFinished
		} else {
			// get destination from buffer
			// TODO GetFileList (get destination from buffer)
		} // if else
	} // if else
} // GetFileList

// initialize class
void cSynchronize::Initialization()
{
	if (!ccConnections) {
		ccConnections = new cConnections();
		bGUIRunning = false;
	} else {
		bGUIRunning = true;
	} // if else

	qqDestinationDirectories.clear();
	qqDestinationFiles.clear();
	qqSourceDirectories.clear();
	qqSourceFiles.clear();
	qsCurrentDirectory.clear();
	qsDirectoryLevel.clear();
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
		QString qsDirectory;

		// add files to global
		while (!qqCurrentDestinationFiles.empty()) {
			qqDestinationFiles += qsCurrentDirectory + qqCurrentDestinationFiles.dequeue();
		} // while

		// add directories to global and stack, go to antoher directory
		if (!qqCurrentDestinationDirectories.empty()) {
			qsDirectoryLevel.push(qqCurrentDestinationDirectories);
			while (!qqCurrentDestinationDirectories.empty()) {
				qqDestinationDirectories += qsCurrentDirectory + qqCurrentDestinationDirectories.dequeue();
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
		DisconnectDestination();
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
			qsMessage += i.name();
			qqCurrentDestinationFiles.enqueue(i.name());
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

	qdnConnection = ccConnections->FindConnection(qsName);
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