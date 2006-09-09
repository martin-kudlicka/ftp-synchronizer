#include "Connections.h"

#include "../../Common/XMLTools.h"
#include <QStack>

const QString qsCONNECTIONS_FILE = "Connections.xml";
const QString qsDELETE_OBSOLETE_FILES = "DeleteObsoleteFiles";
const QString qsDESTINATION = "Destination";
const QString qsINCLUDE_SUBDIRECTORIES = "IncludeSubdirectories";
const QString qsFALSE = "False";
const QString qsNAME = "Name";
const QString qsPASSWORD = "Password";
const QString qsPATH = "Path";
const QString qsSETTINGS = "Settings";
const QString qsSOURCE = "Source";
const QString qsSYNCHRONIZATION = "Synchronization";
const QString qsTYPE = "Type";
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

// load XML file with connections
cConnections::cConnections()
{
	qfFile.setFileName(qsCONNECTIONS_FILE);
	qfFile.open(QIODevice::ReadWrite);
	if (!qddXML.setContent(&qfFile)) {
		// create XML structure
		qddXML.setContent((QString)"<Connections></Connections>");
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

// returns information about connection
QString cConnections::GetProperty(const QDomNode qdnConnection,
											 const eProperty epProperty)
{
	switch (epProperty) {
		case DeleteObsoleteFiles:		return qdnConnection.namedItem(qsSYNCHRONIZATION).namedItem(qsDELETE_OBSOLETE_FILES).toElement().text();
		case DestinationPath:			return qdnConnection.namedItem(qsDESTINATION).namedItem(qsPATH).toElement().text();
		case DestinationPassword:		return qdnConnection.namedItem(qsDESTINATION).namedItem(qsPASSWORD).toElement().text();
		case DestinationUsername:		return qdnConnection.namedItem(qsDESTINATION).namedItem(qsUSERNAME).toElement().text();
		case IncludeSubdirectories:	return qdnConnection.namedItem(qsSETTINGS).namedItem(qsINCLUDE_SUBDIRECTORIES).toElement().text();
		case Name:							return qdnConnection.namedItem(qsNAME).toElement().text();
		case SourcePath:					return qdnConnection.namedItem(qsSOURCE).namedItem(qsPATH).toElement().text();
		case SynchronizationType:		return qdnConnection.namedItem(qsSYNCHRONIZATION).namedItem(qsTYPE).toElement().text();
		case Type:							return qdnConnection.toElement().attributeNode(qsTYPE).value();
	}; // switch

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
	qdeProperty = qddXML.createElement(qsDELETE_OBSOLETE_FILES);
	qdeSubProperty.appendChild(qdeProperty);
	cXMLTools::SetText(qddXML, &qdeProperty, bDeleteObsoleteFiles ? qsTRUE : qsFALSE);

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