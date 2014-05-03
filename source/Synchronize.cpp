#include "Synchronize.h"

#include "Common/XMLTools.h"

const QString qsDIRECTORIES = "Directories";
const QString qsFILES = "Files";
const QString qsITEM = "Item";
const QString qsLAST_MODIFIED = "LastModified";

// check for directory in buffer
bool cSynchronize::BufferContainsDirectory(const eDirection edDirection, const QString qsName)
{
	QDomNode qdnDirectories, qdnName;

	if (edDirection == Source) {
		qdnDirectories = qdnSourceBuffer.namedItem(qsDIRECTORIES);
	} else {
		qdnDirectories = qdnDestinationBuffer.namedItem(qsDIRECTORIES);
	} // if else

	qdnName = qdnDirectories.firstChild();
	while (!qdnName.isNull()) {
		if (qdnName.toElement().text() == qsName) {
			return true;
		} // if
		qdnName = qdnName.nextSibling();
	} // while

	return false;
} // BufferContainsDirectory

// returns true if can do buffered copy
bool cSynchronize::CanCopy(const eDirection edDirection, const QHashIterator<QString, QDateTime> qhiI)
{
	QDomNode qdnFiles, qdnItem;

	if (qhiI.value().toString() == "") {
		// no date/time stamp
		return true;
	} // if

	if (edDirection == Source) {
		qdnFiles = qdnDestinationBuffer.namedItem(qsFILES);
	} else {
		qdnFiles = qdnSourceBuffer.namedItem(qsFILES);
	} // if else

	qdnItem = qdnFiles.firstChild();
	while (!qdnItem.isNull()) {
		if (qdnItem.namedItem(qsNAME).toElement().text() == qhiI.key()) {
			if (qdnItem.namedItem(qsLAST_MODIFIED).toElement().text() != qhiI.value().toString()) {
				// date/time stamp is differend
				return true;
			} else {
				// date/time stamp equals
				return false;
			} // if else
		} // if
		qdnItem = qdnItem.nextSibling();
	} // while

	return true;
} // CanCopy

// connect to FTP
void cSynchronize::ConnectDestination()
{
#ifdef _DEBUG
	qDebug("FTP host: %s", quDestination.host().toLatin1().constData());
#endif
	qfDestination.connectToHost(quDestination.host());
	qfDestination.login(ccConnections->GetProperty(qdnConnection, cConnections::DestinationUsername),
							  ccConnections->GetProperty(qdnConnection, cConnections::DestinationPassword));
} // ConnectDestination

// copy files to destination
void cSynchronize::CopyFiles(const eDirection edDirection)
{
	if (edDirection == Source) {
		// copy to source
		if (bBufferedDownload) {
			// buffered
			QHashIterator<QString, QDateTime> qhiI(qhDestinationFiles);

			while (qhiI.hasNext() && !bStop) {

				qhiI.next();
				if (CanCopy(edDirection, qhiI)) {
					int iCommand;
					QFile *qfFile;
					sCommand scCommand;

					qfFile = new QFile(quSource.path() + "/" + qhiI.key());
					scCommand.qfFile = qfFile;
					scCommand.qsMessage = tr("Copying: %1").arg(qhiI.key());
					scCommand.qsName = qhiI.key();
					scCommand.qdtDateTime = qhiI.value();
					iCommand = qfDestination.get(quDestination.path() + "/" + qhiI.key(), qfFile);
					qhCommands.insert(iCommand, scCommand);
				} // if
			} // while
		} else {
			// not buffered
			QHashIterator<QString, QDateTime> qhiI(qhDestinationFiles);

			while (qhiI.hasNext() && !bStop) {
				int iCommand;
				QFile *qfFile;
				sCommand scCommand;

				qhiI.next();
				qfFile = new QFile(quSource.path() + "/" + qhiI.key());
				scCommand.qfFile = qfFile;
				scCommand.qsMessage = tr("Copying: %1").arg(qhiI.key());
				iCommand = qfDestination.get(quDestination.path() + "/" + qhiI.key(), qfFile);
				qhCommands.insert(iCommand, scCommand);
			} // while
		} // if else
	} else {
		// copy to destination
		if (bBufferedUpload) {
			// buffered
			QHashIterator<QString, QDateTime> qhiI(qhSourceFiles);

			while (qhiI.hasNext() && !bStop) {
				qhiI.next();
				if (CanCopy(edDirection, qhiI)) {
					int iCommand;
					QFile *qfFile;
					sCommand scCommand;

					qfFile = new QFile(quSource.path() + "/" + qhiI.key());
					qfFile->open(QIODevice::ReadOnly);
					scCommand.qfFile = qfFile;
					scCommand.qsMessage = tr("Copying: %1").arg(qhiI.key());
					scCommand.qsName = qhiI.key();
					scCommand.qdtDateTime = qhiI.value();
					iCommand = qfDestination.put(qfFile, quDestination.path() + "/" + qhiI.key());
					qhCommands.insert(iCommand, scCommand);
				} // if
			} // while
		} else {
			// not buffered
			QHashIterator<QString, QDateTime> qhiI(qhSourceFiles);

			while (qhiI.hasNext() && !bStop) {
				int iCommand;
				QFile *qfFile;
				sCommand scCommand;

				qhiI.next();
				qfFile = new QFile(quSource.path() + "/" + qhiI.key());
				qfFile->open(QIODevice::ReadOnly);
				scCommand.qfFile = qfFile;
				scCommand.qsMessage = tr("Copying: %1").arg(qhiI.key());
				iCommand = qfDestination.put(qfFile, quDestination.path() + "/" + qhiI.key());
				qhCommands.insert(iCommand, scCommand);
			} // while
		} // if else
	} // if else
} // CopyFiles

// create destination directories
void cSynchronize::CreateDirectories(const eDirection edDirection)
{
	if (edDirection == Source) {
		// create on source
		if (bBufferedDownload) {
			// buffered
			int iI;

			for (iI = 0; iI < qqDestinationDirectories.count() && !bStop; iI++) {
				if (!BufferContainsDirectory(Destination, qqDestinationDirectories.at(iI))) {
					QDir qdDir;

					emit SendMessage(tr("Creating: %1").arg(qqDestinationDirectories.at(iI)), ForSource);
					qdDir.mkdir(quSource.path() + "/" + qqDestinationDirectories.at(iI));
					CreateDirectoryInBuffer(Destination, qqDestinationDirectories.at(iI));
				} // if
			} // for
		} else {
			// not buffered
			int iI;

			for (iI = 0; iI < qqDestinationDirectories.count() && !bStop; iI++) {
				if (qqSourceDirectories.indexOf(qqDestinationDirectories.at(iI)) == -1) {
					QDir qdDir;

					emit SendMessage(tr("Creating: %1").arg(qqDestinationDirectories.at(iI)), ForSource);
					qdDir.mkdir(quSource.path() + "/" + qqDestinationDirectories.at(iI));
				} // if
			} // for
		} // if else
	} else {
		// create on destination
		if (bBufferedUpload) {
			// buffered
			int iI;

			for (iI = 0; iI < qqSourceDirectories.count() && !bStop; iI++) {
				if (!BufferContainsDirectory(Source, qqSourceDirectories.at(iI))) {
					sCommand scCommand;

					emit SendMessage(tr("Creating: %1").arg(qqSourceDirectories.at(iI)), ForDestination);
					scCommand.qsMessage = qqSourceDirectories.at(iI);
					qhCommands.insert(qfDestination.mkdir(quDestination.path() + "/" + qqSourceDirectories.at(iI)), scCommand);
				} // if
			} // for
		} else {
			// not buffered
			int iI;

			for (iI = 0; iI < qqSourceDirectories.count() && !bStop; iI++) {
				if (qqDestinationDirectories.indexOf(qqSourceDirectories.at(iI)) == -1) {
					emit SendMessage(tr("Creating: %1").arg(qqSourceDirectories.at(iI)), ForDestination);
					qfDestination.mkdir(quDestination.path() + "/" + qqSourceDirectories.at(iI));
				} // if
			} // for
		} // if else
	} // if else
} // CreateDirectories

// create new directory entry in buffer
void cSynchronize::CreateDirectoryInBuffer(const eDirection edDirection, const QString qsName)
{
	QDomNode qdnDirectories, qdnName;

	if (edDirection == Source) {
		qdnDirectories = qdnSourceBuffer.namedItem(qsDIRECTORIES);
	} else {
		qdnDirectories = qdnDestinationBuffer.namedItem(qsDIRECTORIES);
	} // if else

	qdnName = ccConnections->qddXML.createElement(qsNAME);
	qdnDirectories.appendChild(qdnName);
	cXMLTools::SetText(ccConnections->qddXML, &qdnName.toElement(), qsName);
} // CreateDirectoryInBuffer

// create new buffer on edTarget from opposite
void cSynchronize::CreateNewBuffer(const eDirection edTarget)
{
	int iI;
	QDomElement qdeDirectories, qdeFiles;
	QDomNode qdnBuffer;
	QHash<QString, QDateTime> qhFiles;
	QHash<QString, QDateTime>::const_iterator qhFilesIterator;
	QQueue<QString> qqDirectories;

	qdnBuffer = ccConnections->GetBuffer(qdnConnection, edTarget);
	if (edTarget == Source) {
		qhFiles = qhDestinationFiles;
		qqDirectories = qqDestinationDirectories;
	} else {
		qhFiles = qhSourceFiles;
		qqDirectories = qqSourceDirectories;
	} // if else
	qhFilesIterator = qhFiles.constBegin();

	// directories
	qdeDirectories = ccConnections->qddXML.createElement(qsDIRECTORIES);
	qdnBuffer.appendChild(qdeDirectories);
	for (iI = 0; iI < qqDirectories.count(); iI++) {
		QDomElement qdeName;

		qdeName = ccConnections->qddXML.createElement(qsNAME);
		qdeDirectories.appendChild(qdeName);
		cXMLTools::SetText(ccConnections->qddXML, &qdeName, qqDirectories.at(iI));
	} // for

	// files
	qdeFiles = ccConnections->qddXML.createElement(qsFILES);
	qdnBuffer.appendChild(qdeFiles);
	while (qhFilesIterator != qhFiles.constEnd()) {
		QDomElement qdeItem, qdeLastModified, qdeName;

		qdeItem = ccConnections->qddXML.createElement(qsITEM);
		qdeFiles.appendChild(qdeItem);
		qdeName = ccConnections->qddXML.createElement(qsNAME);
		qdeItem.appendChild(qdeName);
		cXMLTools::SetText(ccConnections->qddXML, &qdeName, qhFilesIterator.key());
		qdeLastModified = ccConnections->qddXML.createElement(qsLAST_MODIFIED);
		qdeItem.appendChild(qdeLastModified);
		cXMLTools::SetText(ccConnections->qddXML, &qdeLastModified, qhFilesIterator.value().toString());

		qhFilesIterator++;
	} // while
} // CreateNewBuffer

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
		if (bBufferedDownload) {
			// buffered
			QDomNode qdnDirectories, qdnFiles, qdnItem;

			// files
			qdnFiles = qdnDestinationBuffer.namedItem(qsFILES);
			qdnItem = qdnFiles.firstChild();
			while (!qdnItem.isNull()) {
				bool bRemove;
				QString qsName;

				bRemove = false;
				qsName = qdnItem.namedItem(qsNAME).toElement().text();
				if (!qhDestinationFiles.contains(qsName)) {
					QFile qfFile;

					qfFile.setFileName(quSource.path() + "/" + qsName);
					emit SendMessage(tr("Removing: %1").arg(qsName), ForDestination);
					qfFile.remove();
					// remove from buffer
					bRemove = true;
				} // if
				if (bRemove) {
					QDomNode qdnRemove;

					qdnRemove = qdnItem;
					qdnItem = qdnItem.nextSibling();
					qdnRemove.parentNode().removeChild(qdnRemove);
				} else {
					qdnItem = qdnItem.nextSibling();
				} // if else
			} // while

			// directories
			qdnDirectories = qdnDestinationBuffer.namedItem(qsDIRECTORIES);
			qdnItem = qdnDirectories.lastChild();
			while (!qdnItem.isNull()) {
				bool bRemove;
				QString qsName;

				bRemove = false;
				qsName = qdnItem.toElement().text();
				if (qqDestinationDirectories.indexOf(qsName) == -1) {
					QDir qdDir;

					emit SendMessage(tr("Removing: [%1]").arg(qsName), ForSource);
					qdDir.rmdir(quSource.path() + "/" + qsName);
					// remove from buffer
					bRemove = true;
				} // if
				if (bRemove) {
					QDomNode qdnRemove;

					qdnRemove = qdnItem;
					qdnItem = qdnItem.previousSibling();
					qdnRemove.parentNode().removeChild(qdnRemove);
				} else {
					qdnItem = qdnItem.previousSibling();
				} // if else
			} // while
		} else {
			// not buffered
			int iI;
			QHashIterator<QString, QDateTime> qhiI(qhSourceFiles);

			// files
			while (qhiI.hasNext() && !bStop) {
				qhiI.next();
				if (!qhDestinationFiles.contains(qhiI.key())) {
					QFile qfFile;

					qfFile.setFileName(quSource.path() + "/" + qhiI.key());
					emit SendMessage(tr("Removing: %1").arg(qhiI.key()), ForDestination);
					qfFile.remove();
				} // if
			} // while

			// directories
			for (iI = qqSourceDirectories.count() - 1; iI >= 0 && !bStop; iI--) {
				if (qqDestinationDirectories.indexOf(qqSourceDirectories.at(iI)) == -1) {
					QDir qdDir;

					emit SendMessage(tr("Removing: [%1]").arg(qqSourceDirectories.at(iI)), ForSource);
					qdDir.rmdir(quSource.path() + "/" + qqSourceDirectories.at(iI));
				} // if
			} // for
		} // if else
	} else {
		// delete on destination
		if (bBufferedUpload) {
			// buffered
			QDomNode qdnDirectories, qdnFiles, qdnItem;

			// files
			qdnFiles = qdnSourceBuffer.namedItem(qsFILES);
			qdnItem = qdnFiles.firstChild();
			while (!qdnItem.isNull()) {
				QString qsName;

				qsName = qdnItem.namedItem(qsNAME).toElement().text();
				if (!qhSourceFiles.contains(qsName)) {
					sCommand scCommand;

					scCommand.qdnItem = qdnItem;
					emit SendMessage(tr("Removing: %1").arg(qsName), ForDestination);
					qhCommands.insert(qfDestination.remove(quDestination.path() + "/" + qsName), scCommand);
				} // if
				qdnItem = qdnItem.nextSibling();
			} // while

			// directories
			qdnDirectories = qdnSourceBuffer.namedItem(qsDIRECTORIES);
			qdnItem = qdnDirectories.lastChild();
			while (!qdnItem.isNull()) {
				QString qsName;

				qsName = qdnItem.toElement().text();
				if (qqSourceDirectories.indexOf(qsName) == -1) {
					sCommand scCommand;

					scCommand.qdnItem = qdnItem;
					emit SendMessage(tr("Removing: [%1]").arg(qsName), ForDestination);
					qhCommands.insert(qfDestination.rmdir(quDestination.path() + "/" + qsName), scCommand);
				} // if
				qdnItem = qdnItem.previousSibling();
			} // while
		} else {
			// not buffered
			int iI;
			QHashIterator<QString, QDateTime> qhiI(qhDestinationFiles);

			// files
			while (qhiI.hasNext() && !bStop) {
				qhiI.next();
				if (!qhSourceFiles.contains(qhiI.key())) {
					emit SendMessage(tr("Removing: %1").arg(qhiI.key()), ForDestination);
					qfDestination.remove(quDestination.path() + "/" + qhiI.key());
				} // if
			} // while

			// directories
			for (iI = qqDestinationDirectories.count() - 1; iI >= 0 && !bStop; iI--) {
				if (qqSourceDirectories.indexOf(qqDestinationDirectories.at(iI)) == -1) {
					emit SendMessage(tr("Removing: [%1]").arg(qqDestinationDirectories.at(iI)), ForDestination);
					qfDestination.rmdir(quDestination.path() + "/" + qqDestinationDirectories.at(iI));
				} // if
			} // for
		} // if else
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
		if (!bBufferedDownload || bCreateDestinationBuffer) {
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
					qhSourceFiles.insert(qsCurrentDirectory + qfilCurrentList.at(iI).fileName(),
												qfilCurrentList.at(iI).lastModified());
#ifdef _DEBUG
					qDebug("Source time: %s", qfilCurrentList.at(iI).lastModified().toString().toLatin1().constData());
#endif
					emit SendMessage(tr("Source: %1").arg(qfilCurrentList.at(iI).fileName()), ForSource);
				} // for

				// list current directory for directories
				qslCurrentList = qdSource.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
				if (!qslCurrentList.empty()) {
					QQueue<QString> qqCurrentDirectories;

					for (iI = 0; iI < qslCurrentList.count(); iI++) {
						qqSourceDirectories += qsCurrentDirectory + qslCurrentList.at(iI);
						qqCurrentDirectories.enqueue(qslCurrentList.at(iI));
						emit SendMessage(tr("Source: [%1]").arg(qslCurrentList.at(iI)), ForSource);
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

			// create new destination buffer
			if (bCreateDestinationBuffer) {
				CreateNewBuffer(Destination);
			} // if
		} // if
		if (bBufferedUpload) {
			// get source buffer
			qdnSourceBuffer = ccConnections->GetBuffer(qdnConnection, edDirection);
		} // if
	} else {
		// destination
		if (!bBufferedUpload || bCreateSourceBuffer) {
			// get destination from disk
			qfDestination.cd(quDestination.path());
			// list current directory
			qfDestination.list();
			// rest as in source in on_qfDestination_commandFinished
		} // if
		if (bBufferedUpload && !bCreateSourceBuffer) {
			// continue in synchronization
			Synchronize2();
		} // if
		if (bBufferedDownload) {
			// get destination buffer
			qdnDestinationBuffer = ccConnections->GetBuffer(qdnConnection, edDirection);
		} // if
	} // if else
} // GetFileList

// initialize class
void cSynchronize::Initialization()
{
	// connect to XML
	if (!ccConnections) {
		ccConnections = new cConnections();
		bGUIRunning = false;
	} else {
		bGUIRunning = true;
	} // if else

	// clear from previous run
	qhCommands.clear();
	qqDestinationDirectories.clear();
	qhDestinationFiles.clear();
	qqSourceDirectories.clear();
	qhSourceFiles.clear();
	qsCurrentDirectory.clear();
	qsDirectoryLevel.clear();

	// find connection in XML
	qdnConnection = ccConnections->FindConnection(qsName);

	// initialize variables
	if (ccConnections->GetProperty(qdnConnection, cConnections::Buffered) == qsTRUE) {
		if (ccConnections->GetProperty(qdnConnection, cConnections::SynchronizationType) == qsUPLOAD) {
			bBufferedUpload = true;
			bBufferedDownload = false;
		} else {
			bBufferedUpload = false;
			bBufferedDownload = true;
		} // if else
	} else {
		bBufferedUpload = false;
		bBufferedDownload = false;
	} // if else
	bStop = false;
} // Initialization

// insert copied file in buffer
void cSynchronize::InsertFileInBuffer(const eDirection edDirection, const QString qsName, const QDateTime qdtLastModified)
{
	QDomElement qdeLastModified, qdeName;
	QDomNode qdnFiles, qdnItem;

	if (edDirection == Source) {
		qdnFiles = qdnSourceBuffer.namedItem(qsFILES);
	} else {
		qdnFiles = qdnDestinationBuffer.namedItem(qsFILES);
	} // if else

	qdnItem = qdnFiles.firstChild();
	while (!qdnItem.isNull()) {
		qdeName = qdnItem.namedItem(qsNAME).toElement();
		if (qdeName.text() == qsName) {
			// file already in buffer -> actualize
			cXMLTools::SetText(ccConnections->qddXML, &qdnItem.namedItem(qsLAST_MODIFIED).toElement(), qdtLastModified.toString());
			return;
		} // if

		qdnItem = qdnItem.nextSibling();
	} // while

	// file not in buffer -> create new entry
	qdnItem = ccConnections->qddXML.createElement(qsITEM);
	qdnFiles.appendChild(qdnItem);
	qdeName = ccConnections->qddXML.createElement(qsNAME);
	qdnItem.appendChild(qdeName);
	cXMLTools::SetText(ccConnections->qddXML, &qdeName, qsName);
	qdeLastModified = ccConnections->qddXML.createElement(qsLAST_MODIFIED);
	qdnItem.appendChild(qdeLastModified);
	cXMLTools::SetText(ccConnections->qddXML, &qdeLastModified, qdtLastModified.toString());
} // InsertFileInBuffer

// single FTP command finished
void cSynchronize::on_qfDestination_commandFinished(int id, bool error)
{
	// Close
	if (qfDestination.currentCommand() == QFtp::Close && !bStop) {
		SynchronizationEnd(tr("OK"));
		return;
	} // if

	// Get
	if (qfDestination.currentCommand() == QFtp::Get) {
		sCommand scCommand;

		scCommand = qhCommands.value(id);
		scCommand.qfFile->close();
		scCommand.qfFile->deleteLater();
		if (bBufferedDownload) {
			InsertFileInBuffer(Destination, scCommand.qsName, scCommand.qdtDateTime);
		} // if
		return;
	} // if

	// List
	if (qfDestination.currentCommand() == QFtp::List) {
		bool bIncludeSubdirectories;
		QHashIterator<QString, QDateTime> qhiI(qhCurrentDestinationFiles);
		QString qsDirectory;

		// add files to global
		while (qhiI.hasNext()) {
			qhiI.next();
			qhDestinationFiles.insert(qsCurrentDirectory + qhiI.key(), qhiI.value());
		} // while
		qhCurrentDestinationFiles.clear();

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
		emit SendMessage(tr("Change: %1").arg(qsDirectory), ForDestination);
		qDebug("GuildFTPd");
#endif
			qfDestination.cd(qsDirectory);
			qfDestination.list();
		} else {
			// create new source buffer
			if (bCreateSourceBuffer) {
				CreateNewBuffer(Source);
			} // if

			// continue in synchronization
			Synchronize2();
		} // if else

		return;
	} // if

	// Mkdir
	if (qfDestination.currentCommand() == QFtp::Mkdir && !error) {
		sCommand scCommand;

		scCommand = qhCommands.value(id);
		CreateDirectoryInBuffer(Source, scCommand.qsMessage);
		return;
	} // if

	// Put
	if (qfDestination.currentCommand() == QFtp::Put) {
		sCommand scCommand;

		scCommand = qhCommands.value(id);
		scCommand.qfFile->close();
		scCommand.qfFile->deleteLater();
		if (bBufferedUpload) {
			InsertFileInBuffer(Source, scCommand.qsName, scCommand.qdtDateTime);
		} // if
		return;
	} // if

	// Remove, Rmdir
	if ((qfDestination.currentCommand() == QFtp::Remove || qfDestination.currentCommand() == QFtp::Rmdir) && !error) {
		sCommand scCommand;

		scCommand = qhCommands.value(id);
		scCommand.qdnItem.parentNode().removeChild(scCommand.qdnItem);
		return;
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
			emit SendMessage(scCommand.qsMessage, ForDestination);
		} else {
			emit SendMessage(scCommand.qsMessage, ForSource);
		} // if else

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
		emit SendMessage(tr("Error: %1").arg(qfDestination.errorString()), ForDestination);
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
			qsMessage += i.name();
#ifdef _DEBUG
			qDebug("Destination time: %s", i.lastModified().toString().toLatin1().constData());
#endif
			qhCurrentDestinationFiles.insert(i.name(), i.lastModified());
		} // if else
		emit SendMessage(qsMessage, ForDestination);
	} // if
} // on_qfDestination_listInfo

// FTP state change
void cSynchronize::on_qfDestination_stateChanged(int state)
{
#ifdef _DEBUG
	qDebug("FTP state: %d", state);
#endif
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
void cSynchronize::SynchronizationEnd(const QString qsMessage)
{
	ccConnections->SetLastRun(qdnConnection, QDateTime::currentDateTime(), qsMessage);
	emit Done();
	Deinitialization();
} // SynchronizationEnd

// get file and folder lists
void cSynchronize::Synchronize()
{
	// get source and destination file list
	emit SendMessage(tr("Searching for files and folders..."), Information);
	// TODO infinite progress bar
	// buffer create check
	if (bBufferedDownload && !ccConnections->BufferExists(qdnConnection, Destination)) {
		bCreateDestinationBuffer = true;
	} else {
		bCreateDestinationBuffer = false;
	} // if else
	if (bBufferedUpload && !ccConnections->BufferExists(qdnConnection, Source)) {
		bCreateSourceBuffer = true;
	} else {
		bCreateSourceBuffer = false;
	} // if else
	// get files and directories
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
