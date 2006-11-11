#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include <QDomDocument>
#include <QFile>
#include <QDateTime>

const QString qsCONNECTION = "Connection";
const QString qsDOWNLOAD = "Download";
const QString qsFOLDER = "Folder";
const QString qsNAME = "Name";
const QString qsTRUE = "True";
const QString qsTYPE = "Type";

enum eDirection {Destination, Source};

class cConnections
{
	public:
		enum eModify {Add, Modify};
		enum eProperty {Buffered,
							 DeleteObsoleteFiles,
							 DestinationPath,
							 DestinationPassword,
							 DestinationUsername,
							 IncludeSubdirectories,
							 LastRun,
							 Name,
							 SourcePath,
							 SynchronizationType,
							 Type};

		QDomDocument qddXML;

		cConnections();

		bool BufferExists(const QDomNode qdnConnection, const eDirection edDirection);
		QDomNode FindConnection(const QString qsName);
		QDomNode GetBuffer(const QDomNode qdnConnection, const eDirection edDirection);
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
										  const bool bBuffered,
										  const bool bDeleteObsoleteFiles);
		QDomNode ModifyFolder(const eModify emModify,
									 QDomNode qdnParent,
									 // properties
									 const QString qsName);
		void Remove(const QDomNode qdnConnection);
		void SetLastRun(QDomNode qdnConnection, const QDateTime qdtDateTime, const QString qsMessage);

	private:
		QFile qfFile;

		void ApplyChanges(const eModify emModify, QDomNode qdnParent, const QDomNode qdnNewItem);
		void Save();
}; // cConnections

#endif
