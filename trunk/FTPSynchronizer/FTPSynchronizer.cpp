#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include "MainWindow.h"

int main(int argc, char **argv)
{
	cMainWindow *cmwMainWindow;
	QTranslator qtTranslator;

	QApplication qaApplication(argc, argv);

	qtTranslator.load(QString("FTPSynchronizer_") + QLocale::system().name());
	qaApplication.installTranslator(&qtTranslator);

#ifdef Q_OS_LINUX
		// change style on Linux - Motif is not very nice
		qaApplication.setStyle("plastique");
#endif
	cmwMainWindow = new cMainWindow();

	if (argc == 1) {
		// GUI

		cmwMainWindow->bCommandLine = false;
		cmwMainWindow->show();
	} else {
		QEvent *qeCommandLine;
		
		cmwMainWindow->setWindowState(Qt::WindowMinimized);
		cmwMainWindow->show();
		cmwMainWindow->bCommandLine = true;
		qeCommandLine = new QEvent(cMainWindow::qetCOMMANDLINE_EVENT);
		qApp->postEvent(cmwMainWindow, qeCommandLine);
	} // if else

	return qaApplication.exec();
} // main