#include "Connections.h"

#include "../../Common/XMLTools.h"

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

// returns information about connection
QString cConnections::GetProperty(const QDomNode qdnConnection,
											 const eProperty epProperty)
{
	QString qsResult;

	switch (epProperty) {
		case DeleteObsoleteFiles:		qsResult = qdnConnection.namedItem(qsSYNCHRONIZATION).namedItem(qsDELETE_OBSOLETE_FILES).toElement().text();
												break;
		case Destination:					qsResult = qdnConnection.namedItem(qsDESTINATION).namedItem(qsPATH).toElement().text();
												break;
		case DestinationPassword:		qsResult = qdnConnection.namedItem(qsDESTINATION).namedItem(qsPASSWORD).toElement().text();
												break;
		case DestinationUsername:		qsResult = qdnConnection.namedItem(qsDESTINATION).namedItem(qsUSERNAME).toElement().text();
												break;
		case IncludeSubdirectories:	qsResult = qdnConnection.namedItem(qsSETTINGS).namedItem(qsINCLUDE_SUBDIRECTORIES).toElement().text();
												break;
		case Name:							qsResult = qdnConnection.namedItem(qsNAME).toElement().text();
												break;
		case Source:						qsResult = qdnConnection.namedItem(qsSOURCE).namedItem(qsPATH).toElement().text();
												break;
		case SourcePassword:				qsResult = qdnConnection.namedItem(qsSOURCE).namedItem(qsPASSWORD).toElement().text();
												break;
		case SourceUsername:				qsResult = qdnConnection.namedItem(qsSOURCE).namedItem(qsUSERNAME).toElement().text();
												break;
		case SynchronizationType:		qsResult = qdnConnection.namedItem(qsSYNCHRONIZATION).namedItem(qsTYPE).toElement().text();
												break;
		case Type:							qsResult = qdnConnection.toElement().attributeNode(qsTYPE).value();
												break;
	}; // switch

	return qsResult;
} // GetProperty

// adds or edit connection in table
QDomNode cConnections::ModifyConnection (const eModify emModify,
													  QDomNode qdnParent,
													  // Connection
													  const QString qsName,
													  const QString qsSource,
													  const QString qsSourceUsername,
													  const QString qsSourcePassword,
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
	// Connection
	qdeProperty = qddXML.createElement(qsNAME);
	qdnNewConnection.appendChild(qdeProperty);
	cXMLTools::SetText(qddXML, &qdeProperty, qsName);
	qdeSubProperty = qddXML.createElement(qsSOURCE);
	qdnNewConnection.appendChild(qdeSubProperty);
	qdeProperty = qddXML.createElement(qsPATH);
	qdeSubProperty.appendChild(qdeProperty);
	cXMLTools::SetText(qddXML, &qdeProperty, qsSource);
	qdeProperty = qddXML.createElement(qsUSERNAME);
	qdeSubProperty.appendChild(qdeProperty);
	cXMLTools::SetText(qddXML, &qdeProperty, qsSourceUsername);
	qdeProperty = qddXML.createElement(qsPASSWORD);
	qdeSubProperty.appendChild(qdeProperty);
	cXMLTools::SetText(qddXML, &qdeProperty, qsSourcePassword);
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

	// connect or replace new connection
	if (emModify == Add) {
		if (qdnParent.isNull()) {
			qddXML.documentElement().appendChild(qdnNewConnection);
		} else {
			if (GetProperty(qdnParent, Type) == qsFOLDER) {
				qdnParent.appendChild(qdnNewConnection);
			} else {
				QDomNode qdnRealParent;

				qdnRealParent = qdnParent.parentNode();
				qdnRealParent.insertAfter(qdnNewConnection, qdnParent);
			} // if else
		} // if else
	} else {
		qdnParent.parentNode().replaceChild(qdnNewConnection, qdnParent);
	} // if else

	Save();

	return qdnNewConnection;
} // ModifyConnection

void cConnections::Save()
{
	qfFile.resize(0);
	qfFile.write(qddXML.toByteArray());
} // Save