CONFIG += qt warn_on
QT += xml network
RC_FILE = FTPSynchronizer.rc
RESOURCES = MainWindow.qrc
FORMS = MainWindow.ui \
		ConnectionDialog.ui \
		FolderDialog.ui
HEADERS = MainWindow.h \
		  ConnectionDialog.h \
		  Connections.h \
		  Common/XMLTools.h \
		  FolderDialog.h \
		  Synchronize.h
SOURCES = FTPSynchronizer.cpp \
		  MainWindow.cpp \
		  ConnectionDialog.cpp \
		  Connections.cpp \
		  Common/XMLTools.cpp \
		  FolderDialog.cpp \
		  Synchronize.cpp
//TRANSLATIONS = Sequence_cs_CZ.ts