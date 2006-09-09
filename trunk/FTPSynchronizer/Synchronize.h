#ifndef SYNCHRONIZE_H
#define SYNCHRONIZE_H

#include <QString>
#include "Connections.h"
#include <QFtp>
#include <QMainWindow>
#include <QUrl>
#include <QQueue>
#include <QStack>
#include <QDir>

const QString qsDOWNLOAD = "Download";

class cSynchronize : private QObject
{
	Q_OBJECT;

	public:
		cConnections *ccConnections;
		QMainWindow *qmwGUI;
		QString qsName;

		cSynchronize();

		void Start();

	private:
		enum eDirection {Destination, Source};

		bool bFTPDestinationDone, bGUIRunning;
		QDomNode qdnConnection;
		QFtp *qfDestination;
		QQueue<QString> qqCurrentDestinationDirectories, qqCurrentDestinationFiles, qqDestinationDirectories,
							 qqDestinationFiles, qqSourceDirectories, qqSourceFiles;
		QUrl quSource, quDestination;

		void ConnectDestination();
		void ConnectSignals();
		void DisconnectDestination();
		void GetFileList(const eDirection edDirection);
		QString SetDirectory(QStack<QQueue<QString> > *qsDirectoryLevel,
									QString *qsCurrentDirectory,
									QFtp *qfFTP,
									QDir *qdDir);
		void Synchronize();
		void WaitForDestinationFTP();

	signals:
		void SendGUIMessage(const QString qsMessage);

	private slots:
		void on_qfDestination_done(bool error);
		void on_qfDestination_listInfo(const QUrlInfo &i);
}; // cSynchronize

#endif