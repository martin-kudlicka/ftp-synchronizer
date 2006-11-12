#include "Connections.h"

#include "Common/XMLTools.h"
#include <QStack>
#include "Common/System.h"

const QString qsAPPLICATION_NAME = "FTPSynchronizer";
const QString qsBUFFER = "Buffer";
const QString qsBUFFERED = "Buffered";
const QString qsCONNECTIONS_FILE = "Connections.xml";
const QString qsDATE_TIME = "DateTime";
const QString qsDELETE_OBSOLETE_FILES = "DeleteObsoleteFiles";
const QString qsDESTINATION = "Destination";
const QString qsINCLUDE_SUBDIRECTORIES = "IncludeSubdirectories";
const QString qsFALSE = "False";
const QString qsLAST_RUN = "LastRun";
const QString qsMESSAGE = "Message";
const QString qsPASSWORD = "Password";
const QString qsPATH = "Path";
const QString qsSETTINGS = "Settings";
const QString qsSOURCE = "Source";
const QString qsSYNCHRONIZATION = "Synchronization";
const QString qsUSERNAME = "Username";

// save changes into XML
void cConnections::ApplyChanges(const eModify emModify, QDomNode qdnParent, const QDomNode qdnNewItem)
{
	if (emModify == Add) {
		if (qdnParent.isNull()) {
			qddXML.documentElement().appendChild(qdnNewItem);
		} else {
			if (GetProperty(qdnParent, Type) == qsFOLDER) {
				qdnParent.appendChild(qdnNewItem);
			} else {
				QDomNode qdnRealParent;

				qdnRealParent = qdnParent.parentNode();
				qdnRealParent.insertAfter(qdnNewItem, qdnParent);
			} // if else
		} // if else
	} else {
		if (GetProperty(qdnParent, Type) == qsFOLDER) {
			// folder can't be replaced due to lost of his childrens
			cXMLTools::SetText(qddXML, &qdnParent.namedItem(qsNAME).toElement(), qdnNewItem.namedItem(qsNAME).toElement().text());
		} else {
			qdnParent.parentNode().replaceChild(qdnNewItem, qdnParent);
		} // if else
	} // if else

	Save();
} // ApplyChanges

// checks if exists the file and folder buffer
bool cConnections::BufferExists(const QDomNode qdnConnection, const eDirection edDirection)
{
	QDomNode qdnDirection;

	if (edDirection == Source) {
		qdnDirection = qdnConnection.namedItem(qsSOURCE);
	} else {
		qdnDirection = qdnConnection.namedItem(qsDESTINATION);
	} // if else

	if (qdnDirection.namedItem(qsBUFFER).isNull()) {
		return false;
	} else {
		return true;
	} // if else
} // BufferExists

// load XML file with connections
cConnections::cConnections()
{
	// open data file
	qfFile.setFileName(cSystem::GetApplicationSettingsPath(qsAPPLICATION_NAME) + "/" + qsCONNECTIONS_FILE);
	qfFile.open(QIODevice::ReadWrite);
	if (!qddXML.setContent(&qfFile)) {
		// create XML structure
		qddXML.setContent((QString)"<?xml version=\"1.0\" standalone=\"yes\" ?><Connections></Connections>");
		Save();
	} // if
} // cConnections

// find connection in XML file by it's name
QDomNode cConnections::FindConnection(const QString qsName)
{
	QDomNode qdnNode;
	QStack<QDomNode> qsXMLLevel;

	qdnNode = qddXML.documentElement().firstChild();

	while (!qdnNode.isNull()) {
		if (GetProperty(qdnNode, Type) == qsFOLDER && !qdnNode.namedItem(qsCONNECTION).isNull()) {
			// non-empty folder
			qsXMLLevel.push(qdnNode);
			qdnNode = qdnNode.firstChild();
		} else {
			// empty folder or connection
			if (GetProperty(qdnNode, Type) == qsCONNECTION && GetProperty(qdnNode, Name) == qsName) {
				// found
				break;
			} // if

			qdnNode = qdnNode.nextSibling();
			while (qdnNode.isNull() && !qsXMLLevel.isEmpty()) {
				qdnNode = qsXMLLevel.pop();
				qdnNode = qdnNode.nextSibling();
			} // while
		} // if else
	} // while

	return qdnNode;
} // FindConnection

// return pointer to buffer
QDomNode cConnections::GetBuffer(const QDomNode qdnConnection, const eDirection edDirection)
{
	QDomNode qdnDirection;

	if (edDirection == Source) {
		qdnDirection = qdnConnection.namedItem(qsSOURCE);
	} else {
		qdnDirection = qdnConnection.namedItem(qsDESTINATION);
	} // if else

	if (qdnDirection.namedItem(qsBUFFER).isNull()) {
		QDomElement qdeBuffer;

		qdeBuffer = qddXML.createElement(qsBUFFER);
		qdnDirection.appendChild(qdeBuffer);
		return qdeBuffer;
	} else {
		return qdnDirection.namedItem(qsBUFFER);
	} // if else
} // GetBuffer

// returns information about connection
QString cConnections::GetProperty(const QDomNode qdnConnection,
											 const eProperty epProperty)
{
	switch (epProperty) {
		case Buffered:						return qdnConnection.namedItem(qsSYNCHRONIZATION).namedItem(qsBUFFERED).toElement().text();
		case DeleteObsoleteFiles:		return qdnConnection.namedItem(qsSYNCHRONIZATION).namedItem(qsDELETE_OBSOLETE_FILES).toElement().text();
		case DestinationPath:			return qdnConnection.namedItem(qsDESTINATION).namedItem(qsPATH).toElement().text();
		case DestinationPassword:		return qdnConnection.namedItem(qsDESTINATION).namedItem(qsPASSWORD).toElement().text();
		case DestinationUsername:		return qdnConnection.namedItem(qsDESTINATION).namedItem(qsUSERNAME).toElement().text();
		case IncludeSubdirectories:	return qdnConnection.namedItem(qsSETTINGS).namedItem(qsINCLUDE_SUBDIRECTORIES).toElement().text();
		case LastRun:						if (qdnConnection.namedItem(qsLAST_RUN).isNull()) {
													return "";
												} else {
													return qdnConnection.namedItem(qsLAST_RUN).namedItem(qsDATE_TIME).toElement().text()
													+ " (" +
													qdnConnection.namedItem(qsLAST_RUN).namedItem(qsMESSAGE).toElement().text()
													+ ")";
												} // if else
		case Name:							return qdnConnection.namedItem(qsNAME).toElement().text();
		case SourcePath:					return qdnConnection.namedItem(qsSOURCE).namedItem(qsPATH).toElement().text();
		case SynchronizationType:		return qdnConnection.namedItem(qsSYNCHRONIZATION).namedItem(qsTYPE).toElement().text();
		case Type:							return qdnConnection.toElement().attributeNode(qsTYPE).value();
	}; // switch

	Q_ASSERT(false);
	return NULL;
} // GetProperty

// adds or edits connection
QDomNode cConnections::ModifyConnection(const eModify emModify,
													 QDomNode qdnParent,
													 // Connection
													 const QString qsName,
													 const QString qsSource,
													 const QString qsDestination,
													 const QString qsDestinationUsername,
													 const QString qsDestinationPassword,
													 // Settings
													 const bool bIncludeSubdirectories,
													 // Synchronization
													 const QString qsSynchronization,
													 const bool bBuffered,
													 const bool bDeleteObsoleteFiles)
{
	QDomElement qdeProperty, qdeSubProperty;
	QDomNode qdnNewConnection;

	// create new connection with properties
	qdnNewConnection = qddXML.createElement(qsCONNECTION);
	qdnNewConnection.toElement().setAttribute(qsTYPE, qsCONNECTION);
	// Connection
	qdeProperty = qddXML.createElement(qsNAME);
	qdnNewConnection.appendChild(qdeProperty);
	cXMLTools::SetText(qddXML, &qdeProperty, qsName);
	qdeSubProperty = qddXML.createElement(qsSOURCE);
	qdnNewConnection.appendChild(qdeSubProperty);
	qdeProperty = qddXML.createElement(qsPATH);
	qdeSubProperty.appendChild(qdeProperty);
	cXMLTools::SetText(qddXML, &qdeProperty, qsSource);
	qdeSubProperty = qddXML.createElement(qsDESTINATION);
	qdnNewConnection.appendChild(qdeSubProperty);
	qdeProperty = qddXML.createElement(qsPATH);
	qdeSubProperty.appendChild(qdeProperty);
	cXMLTools::SetText(qddXML, &qdeProperty, qsDestination);
	qdeProperty = qddXML.createElement(qsUSERNAME);
	qdeSubProperty.appendChild(qdeProperty);
	cXMLTools::SetText(qddXML, &qdeProperty, qsDestinationUsername);
	qdeProperty = qddXML.createElement(qsPASSWORD);
	qdeSubProperty.appendChild(qdeProperty);
	cXMLTools::SetText(qddXML, &qdeProperty, qsDestinationPassword);
	// Settings
	qdeSubProperty = qddXML.createElement(qsSETTINGS);
	qdnNewConnection.appendChild(qdeSubProperty);
	qdeProperty = qddXML.createElement(qsINCLUDE_SUBDIRECTORIES);
	qdeSubProperty.appendChild(qdeProperty);
	cXMLTools::SetText(qddXML, &qdeProperty, bIncludeSubdirectories ? qsTRUE : qsFALSE);
	// Synchronization
	qdeSubProperty = qddXML.createElement(qsSYNCHRONIZATION);
	qdnNewConnection.appendChild(qdeSubProperty);
	qdeProperty = qddXML.createElement(qsTYPE);
	qdeSubProperty.appendChild(qdeProperty);
	cXMLTools::SetText(qddXML, &qdeProperty, qsSynchronization);
	qdeProperty = qddXML.createElement(qsBUFFERED);
	qdeSubProperty.appendChild(qdeProperty);
	cXMLTools::SetText(qddXML, &qdeProperty, bBuffered ? qsTRUE : qsFALSE);
	qdeProperty = qddXML.createElement(qsDELETE_OBSOLETE_FILES);
	qdeSubProperty.appendChild(qdeProperty);
	cXMLTools::SetText(qddXML, &qdeProperty, bDeleteObsoleteFiles ? qsTRUE : qsFALSE);

	// copy some information when modyfying
	if (emModify == Modify) {
		qdnNewConnection.appendChild(qdnParent.namedItem(qsLAST_RUN).cloneNode());
		if (bBuffered) {
			if (qsSynchronization == qsDOWNLOAD) {
				qdnNewConnection.namedItem(qsDESTINATION).appendChild(qdnParent.namedItem(qsDESTINATION).namedItem(qsBUFFER).cloneNode());
			} else {
				qdnNewConnection.namedItem(qsSOURCE).appendChild(qdnParent.namedItem(qsSOURCE).namedItem(qsBUFFER).cloneNode());
			} // if else
		} // if
	} // if

	ApplyChanges(emModify, qdnParent, qdnNewConnection);

	return qdnNewConnection;
} // ModifyConnection

// adds or edits folder
QDomNode cConnections::ModifyFolder(const eModify emModify,
												QDomNode qdnParent,
												// properties
												const QString qsName)
{
	QDomElement qdeProperty;
	QDomNode qdnNewFolder;

	// create new folder with properties
	qdnNewFolder = qddXML.createElement(qsCONNECTION);
	qdnNewFolder.toElement().setAttribute(qsTYPE, qsFOLDER);
	// properties
	qdeProperty = qddXML.createElement(qsNAME);
	qdnNewFolder.appendChild(qdeProperty);
	cXMLTools::SetText(qddXML, &qdeProperty, qsName);

	ApplyChanges(emModify, qdnParent, qdnNewFolder);

	return qdnNewFolder;
} // ModifyFolder

// removes connection or folder (with appended childrens)
void cConnections::Remove(const QDomNode qdnConnection)
{
	qdnConnection.parentNode().removeChild(qdnConnection);
	Save();
} // Remove

// saves changes into file
void cConnections::Save()
{
	qfFile.resize(0);
	qfFile.write(qddXML.toByteArray());
} // Save

// set last run in connection
void cConnections::SetLastRun(QDomNode qdnConnection, const QDateTime qdtDateTime, const QString qsMessage)
{
	QDomNode qdnDateTime, qdnLastRun, qdnMessage;

	qdnLastRun = qdnConnection.namedItem(qsLAST_RUN);
	if (qdnLastRun.isNull()) {
		qdnLastRun = qddXML.createElement(qsLAST_RUN);
		qdnConnection.appendChild(qdnLastRun);
	} // if

	qdnDateTime = qdnLastRun.namedItem(qsDATE_TIME);
	if (qdnDateTime.isNull()) {
		qdnDateTime = qddXML.createElement(qsDATE_TIME);
		qdnLastRun.appendChild(qdnDateTime);
	} // if
	cXMLTools::SetText(qddXML, &qdnDateTime.toElement(), qdtDateTime.toString());

	qdnMessage = qdnLastRun.namedItem(qsMESSAGE);
	if (qdnMessage.isNull()) {
		qdnMessage = qddXML.createElement(qsMESSAGE);
		qdnLastRun.appendChild(qdnMessage);
	} // if
	cXMLTools::SetText(qddXML, &qdnMessage.toElement(), qsMessage);

	Save();
} // SetLastRun
