#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include <QDomDocument>
#include <QFile>

const QString qsCONNECTION = "Connection";
const QString qsFOLDER = "Folder";
const QString qsTRUE = "True";

class cConnections
{
	public:
		enum eModify {Add, Modify};
		enum eProperty {DeleteObsoleteFiles,
							 DestinationPath,
							 DestinationPassword,
							 DestinationUsername,
							 IncludeSubdirectories,
							 Name,
							 SourcePath,
							 SynchronizationType,
							 Type};

		QDomDocument qddXML;

		cConnections();

		QDomNode FindConnection(const QString qsName);
		QString GetProperty(const QDomNode qdnConnection,
								  const eProperty epProperty);
		QDomNode ModifyConnection(const eModify emModify,
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
										  const bool bDeleteObsoleteFiles);
		QDomNode ModifyFolder(const eModify emModify,
									 QDomNode qdnParent,
									 // properties
									 const QString qsName);
		void Remove(const QDomNode qdnConnection);

	private:
		QFile qfFile;

		void ApplyChanges(const eModify emModify, QDomNode qdnParent, const QDomNode qdnNewItem);
		void Save();
}; // cConnections

#endif