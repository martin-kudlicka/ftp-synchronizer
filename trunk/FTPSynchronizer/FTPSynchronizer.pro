CONFIG += qt warn_on
QT += xml network
RC_FILE = FTPSynchronizer.rc
RESOURCES = MainWindow.qrc \
			Common/About.qrc
FORMS = MainWindow.ui \
		ConnectionDialog.ui \
		FolderDialog.ui \
		Common/About.ui
HEADERS = MainWindow.h \
		  ConnectionDialog.h \
		  Connections.h \
		  Common/XMLTools.h \
		  FolderDialog.h \
		  Synchronize.h \
		  Common/CommandLine.h \
		  Common/About.h \
		  Common/System.h
SOURCES = FTPSynchronizer.cpp \
		  MainWindow.cpp \
		  ConnectionDialog.cpp \
		  Connections.cpp \
		  Common/XMLTools.cpp \
		  FolderDialog.cpp \
		  Synchronize.cpp \
		  Common/CommandLine.cpp \
		  Common/About.cpp \
		  Common/System.cpp
TRANSLATIONS = FTPSynchronizer_cs_CZ.ts