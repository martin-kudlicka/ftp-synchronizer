#ifndef SYNCHRONIZE_H
#define SYNCHRONIZE_H

#include <QString>
#include "Connections.h"
#include <QMainWindow>
#include <QFtp>
#include <QUrl>
#include <QDir>
#include <QQueue>
#include <QStack>
#include <QHash>

const QString qsDOWNLOAD = "Download";

class cSynchronize : private QObject
{
	Q_OBJECT

	public:
		cConnections *ccConnections;
		QDomNode qdnConnection;
		QMainWindow *qmwGUI;
		QString qsName;
		QUrl quSource, quDestination;

		void Start();

	private:
		enum eDirection {Destination, Source};
		struct sCommand {QString qsMessage; QFile *qfFile;};

		bool bGUIRunning, bStop;
		QFtp qfDestination;
		QHash<int, sCommand> qhCommands;
		QStack<QQueue<QString> > qsDirectoryLevel;
		QString qsCurrentDirectory;
		QQueue<QString> qqCurrentDestinationDirectories, qqCurrentDestinationFiles, qqDestinationDirectories,
							 qqDestinationFiles, qqSourceDirectories, qqSourceFiles;

		void ConnectDestination();
		void ConnectSignals();
		void CopyFiles(const eDirection edDirection);
		void CreateDirectories(const eDirection edDirection);
		void Deinitialization();
		void DeleteObsolete(const eDirection edDirection);
		void DisconnectDestination();
		void DisconnectSignals();
		void GetFileList(const eDirection edDirection);
		void Initialization();
		QString SetDirectory(const eDirection edDirection, QDir *qdDir = NULL);
		void Synchronize();
		void Synchronize2();

	signals:
		void SendGUIMessage(const QString qsMessage);
		void Done();

	private slots:
		void on_qfDestination_commandFinished(int id, bool error);
		void on_qfDestination_commandStarted(int id);
		void on_qfDestination_done(bool error);
		void on_qfDestination_listInfo(const QUrlInfo &i);
		void on_qmwGUI_StopSynchronization();
}; // cSynchronize

#endif