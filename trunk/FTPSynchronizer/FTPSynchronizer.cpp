#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include "MainWindow.h"

int main(int argc, char **argv)
{
	cMainWindow *cmwMainWindow;
	QApplication qaApplication(argc, argv);
	QTranslator qtTranslator;

#ifdef Q_OS_LINUX
	// change style on Linux - Motif is not very nice
	qaApplication.setStyle("plastique");
#endif
	qtTranslator.load(QString("FTPSynchronizer_") + QLocale::system().name());
	qaApplication.installTranslator(&qtTranslator);
	cmwMainWindow = new cMainWindow();
	
	cmwMainWindow->show();

	return qaApplication.exec();
} // main