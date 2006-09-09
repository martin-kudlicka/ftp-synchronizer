CONFIG += qt warn_on
QT += xml
//RC_FILE = Sequence.rc
//RESOURCES = MainWindow.qrc
FORMS = MainWindow.ui \
		ConnectionDialog.ui \
		FolderDialog.ui
HEADERS = MainWindow.h \
		  ConnectionDialog.h \
		  Connections.h \
		  Common/XMLTools.h \
		  FolderDialog.h
SOURCES = FTPSynchronizer.cpp \
		  MainWindow.cpp \
		  ConnectionDialog.cpp \
		  Connections.cpp \
		  Common/XMLTools.cpp \
		  FolderDialog.cpp
//TRANSLATIONS = Sequence_cs_CZ.ts