#-------------------------------------------------
#
# Project created by QtCreator 2012-02-18T22:53:56
#
#-------------------------------------------------

QT       += core gui
QT       += network

TARGET = XDBF
TEMPLATE = app

#ifdef _WIN32 | _WIN64
RC_FILE = app.rc #Setting icon for Windows
#elif __APPLE__
ICON = app.icns
#endif

SOURCES += main.cpp\
        mainwindow.cpp \
    xdbf.cpp \
    imagedialog.cpp \
    xdbfhelper.cpp \
    achievementviewer.cpp \
    binarydialog.cpp \
    titleentrydialog.cpp \
    synclistdialog.cpp \
    avatarawarddialog.cpp

HEADERS  += mainwindow.h \
    xdbf.h \
    imagedialog.h \
    xdbfhelper.h \
    achievementviewer.h \
    binarydialog.h \
    titleentrydialog.h \
    synclistdialog.h \
    avatarawarddialog.h

FORMS    += mainwindow.ui \
    imagedialog.ui \
    achievementviewer.ui \
    binarydialog.ui \
    titleentrydialog.ui \
    synclistdialog.ui \
    avatarawarddialog.ui

RESOURCES += \
    images.qrc
