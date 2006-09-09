#include "Synchronize.h"

#include <QCoreApplication>

// connect to FTP
void cSynchronize::ConnectDestination()
{
	qfDestination->connectToHost(quDestination.host());
	qfDestination->login(ccConnections->GetProperty(qdnConnection, cConnections::DestinationUsername),
								ccConnections->GetProperty(qdnConnection, cConnections::DestinationPassword));
} // ConnectDestination

// connect signals for GUI
void cSynchronize::ConnectSignals()
{
	QObject::connect(qfDestination, SIGNAL(listInfo(const QUrlInfo &)),
						  SLOT(on_qfDestination_listInfo(const QUrlInfo &)));
	QObject::connect(qfDestination, SIGNAL(done(bool)), SLOT(on_qfDestination_done(bool)));

	// GUI only signals
	if (bGUIRunning) {
		QObject::connect(qfDestination, SIGNAL(stateChanged(int)),
							  qmwGUI, SLOT(on_qfDestination_stateChanged(int)));
		QObject::connect(this, SIGNAL(SendGUIMessage(const QString)),
							  qmwGUI, SLOT(on_cSynchronize_Message(const QString)));
	} // if
} // ConnectSignals

// initialization
cSynchronize::cSynchronize()
{
	qfDestination = new QFtp(this);
	bFTPDestinationDone = false;
} // cSynchronize

// disconnect from FTP
void cSynchronize::DisconnectDestination()
{
	qfDestination->close();
} // DisconnectDestination

// fills files and directories info
void cSynchronize::GetFileList(const eDirection edDirection)
{
	bool bIncludeSubdirectories;
	QString qsIncludeSubdirectories;

	qsIncludeSubdirectories = ccConnections->GetProperty(qdnConnection, cConnections::IncludeSubdirectories);
	if (qsIncludeSubdirectories == qsTRUE) {
		bIncludeSubdirectories = true;
	} else {
		bIncludeSubdirectories = false;
	} // if else

	if (edDirection == Source) {
		// source
		QDir qdSource;
		QStack<QQueue<QString> > qsDirectoryLevel;
		QString qsCurrentDirectory;
		QStringList qslCurrentList;

		qdSource.setPath(quSource.path());

		do {
			int iI;
			QString qsDirectory;

			// list current directory for files
			qslCurrentList = qdSource.entryList(QDir::Files);
			// add files to global
			for (iI = 0; iI < qslCurrentList.count(); iI++) {
				qqSourceFiles += qsCurrentDirectory + qslCurrentList.at(iI);
				emit SendGUIMessage(tr("Source: %1").arg(qslCurrentList.at(iI)));
			} // for

			// list current directory for directories
			qslCurrentList = qdSource.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
			if (!qslCurrentList.empty()) {
				QQueue<QString> qqCurrentDirectories;

				for (iI = 0; iI < qslCurrentList.count(); iI++) {
					qqSourceDirectories += qsCurrentDirectory + qslCurrentList.at(iI);
					qqCurrentDirectories.enqueue(qslCurrentList.at(iI));
					emit SendGUIMessage(tr("Source: [%1]").arg(qslCurrentList.at(iI)));
				} // for
				qsDirectoryLevel.push(qqCurrentDirectories);
			} else {
				// no directories -> go back
				qdSource.cd("..");
				qsCurrentDirectory +=  "../";
			} // if else

			qsDirectory = SetDirectory(&qsDirectoryLevel, &qsCurrentDirectory, NULL, &qdSource);
			qdSource.cd(qsDirectory);
			qsCurrentDirectory += qsDirectory + "/";
			qsCurrentDirectory = QDir::cleanPath(qsCurrentDirectory);
			if (qsCurrentDirectory.endsWith("..")) {
				qsCurrentDirectory.clear();
			} else {
				qsCurrentDirectory += "/";
			} // if else
		} while (!qsDirectoryLevel.empty() && bIncludeSubdirectories);
	} else {
		// destination
		QStack<QQueue<QString> > qsDirectoryLevel;
		QString qsCurrentDirectory;

		qfDestination->cd(quDestination.path());

		do {
			QString qsDirectory;

			// list current directory and wait for result
			qfDestination->list();
			WaitForDestinationFTP();

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
				qfDestination->cd("..");
				qsCurrentDirectory +=  "../";
			} // if else

			qsDirectory = SetDirectory(&qsDirectoryLevel, &qsCurrentDirectory, qfDestination, NULL);
			qfDestination->cd(qsDirectory);
			qsCurrentDirectory += qsDirectory + "/";
			qsCurrentDirectory = QDir::cleanPath(qsCurrentDirectory);
			if (qsCurrentDirectory.endsWith("..")) {
				qsCurrentDirectory.clear();
			} else {
				qsCurrentDirectory += "/";
			} // if else
		} while (!qsDirectoryLevel.empty() && bIncludeSubdirectories);
	} // if else
} // GetFileList

// pending operations on FTP done
void cSynchronize::on_qfDestination_done(bool error)
{
#ifdef _DEBUG
	if (error) {
		emit SendGUIMessage(tr("Error: %1").arg(qfDestination->errorString()));
	} // if
#endif
	bFTPDestinationDone = true;
} // on_qfDestination_done

// searching for files and directories
void cSynchronize::on_qfDestination_listInfo(const QUrlInfo &i)
{
	QString qsMessage;

	qsMessage = tr("Destination: ");
	if (i.isDir()) {
		qsMessage += QString("[%1]").arg(i.name());
		qqCurrentDestinationDirectories.enqueue(i.name());
	} else {
		qsMessage += i.name();
		qqCurrentDestinationFiles.enqueue(i.name());
	} // if else
	emit SendGUIMessage(qsMessage);
} // on_qfDestination_listInfo

// get next directory from queue and optionally set right directory string with FTP level
QString cSynchronize::SetDirectory(QStack<QQueue<QString> > *qsDirectoryLevel,
											  QString *qsCurrentDirectory,
											  QFtp *qfFTP,
											  QDir *qdDir)
{
	QString qsNewDirectory;

	while (!qsDirectoryLevel->empty() && qsDirectoryLevel->top().empty()) {
		qsDirectoryLevel->pop();
		if (qfFTP) {
			qfFTP->cd("..");
		} else {
			qdDir->cd("..");
		} // if else
		*qsCurrentDirectory += "../";
	} // while
	if (!qsDirectoryLevel->empty()) {
		qsNewDirectory = qsDirectoryLevel->top().dequeue();
	} // if

	return qsNewDirectory;
} // SetDirectory

// input class method - start of synchronization process
void cSynchronize::Start()
{
	if (!ccConnections) {
		ccConnections = new cConnections();
		bGUIRunning = false;
	} else {
		bGUIRunning = true;
	} // if else

	ConnectSignals();

	qdnConnection = ccConnections->FindConnection(qsName);
	quSource = ccConnections->GetProperty(qdnConnection, cConnections::SourcePath);
	quDestination = ccConnections->GetProperty(qdnConnection, cConnections::DestinationPath);

	ConnectDestination();

	// get source and destination file list
	emit SendGUIMessage(tr("Searching for files and folders..."));
	// TODO infinite progress bar
	GetFileList(Source);
	GetFileList(Destination);

	Synchronize();

	// wait till file lists are filled
	WaitForDestinationFTP();

	DisconnectDestination();

	if (!bGUIRunning) {
		delete ccConnections;
	} // if
} // Start

// main synchronization process
void cSynchronize::Synchronize()
{
	// delete obsolete files and folders first
	if (ccConnections->GetProperty(qdnConnection, cConnections::DeleteObsoleteFiles) == qsTRUE) {
		if (ccConnections->GetProperty(qdnConnection, cConnections::SynchronizationType) == qsDOWNLOAD) {
			// download
			int iI;

			// files
			for (iI = 0; iI < qqSourceFiles.count(); iI++) {
				if (qqDestinationFiles.indexOf(qqSourceFiles.at(iI)) == -1) {
					QFile qfFile;

					qfFile.setFileName(quSource.path() + "/" + qqSourceFiles.at(iI));
					emit SendGUIMessage(tr("Removing: %1").arg(qqSourceFiles.at(iI)));
					qfFile.remove();
				} // if
			} // for

			// directories
		} else {
			// upload
			int iI;

			// files
			for (iI = 0; iI < qqDestinationFiles.count(); iI++) {
				if (qqSourceFiles.indexOf(qqDestinationFiles.at(iI)) == -1) {
					emit SendGUIMessage(tr("Removing: %1").arg(qqDestinationFiles.at(iI)));
					qfDestination->remove(quDestination.path() + "/" + qqDestinationFiles.at(iI));
				} // if
			} // for

			// directories
		} // if else
	} // if
} // Synchronize

// wait till destination FTP makes all the work
void cSynchronize::WaitForDestinationFTP()
{
	while (!bFTPDestinationDone) {
		QCoreApplication::processEvents();
	} // while
	bFTPDestinationDone = false;
} // WaitForDestinationFTP