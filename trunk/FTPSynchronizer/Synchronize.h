#ifndef SYNCHRONIZE_H
#define SYNCHRONIZE_H

#include <QString>
#include "Connections.h"
#include <QFtp>
#include <QUrl>
#include <QDir>
#include <QQueue>
#include <QStack>
#include <QHash>

const QString qsDOWNLOAD = "Download";
const QString qsUPLOAD = "Upload";

class cSynchronize : private QObject
{
	Q_OBJECT

	public:
		cConnections *ccConnections;
		QDomNode qdnConnection;
		QString qsName;
		QUrl quSource, quDestination;

		cSynchronize();

		void Start();

	private:
		struct sCommand {QString qsMessage;	// CopyFiles, CreateDirectories
							  QFile *qfFile;		// CopyFiles
							  QDomNode qdnItem;	// DeleteObsolete
		}; // sCommand

		bool bBufferedDownload, bBufferedUpload, bCreateDestinationBuffer, bCreateSourceBuffer, bGUIRunning, bStop;
		QDomNode qdnDestinationBuffer, qdnSourceBuffer;
		QFtp qfDestination;
		QHash<int, sCommand> qhCommands;
		QHash<QString, QDateTime> qhCurrentDestinationFiles, qhDestinationFiles, qhSourceFiles;
		QStack<QQueue<QString> > qsDirectoryLevel;
		QString qsCurrentDirectory;
		QQueue<QString> qqCurrentDestinationDirectories, qqDestinationDirectories, qqSourceDirectories;

		bool BufferContainsDirectory(const eDirection edDirection, const QString qsName);
		void ConnectDestination();
		void CopyFiles(const eDirection edDirection);
		void CreateDirectories(const eDirection edDirection);
		void CreateDirectoryInBuffer(const eDirection edDirection, const QString qsName);
		void CreateNewBuffer(const eDirection edTarget);
		void Deinitialization();
		void DeleteObsolete(const eDirection edDirection);
		void DisconnectDestination();
		void GetFileList(const eDirection edDirection);
		void Initialization();
		QString SetDirectory(const eDirection edDirection, QDir *qdDir = NULL);
		void SynchronizationEnd(const QString qsMessage);
		void Synchronize();
		void Synchronize2();

	signals:
		void FTPStateChanged(const int iState);
		void Progress(const qint64 qi64Done, const qint64 qi64Total);
		void SendMessage(const QString qsMessage);
		void Done();

	private slots:
		void on_qfDestination_commandFinished(int id, bool error);
		void on_qfDestination_commandStarted(int id);
		void on_qfDestination_dataTransferProgress(qint64 done, qint64 total);
		void on_qfDestination_done(bool error);
		void on_qfDestination_listInfo(const QUrlInfo &i);
		void on_qfDestination_stateChanged(int state);
		void on_StopSynchronization();
}; // cSynchronize

#endif