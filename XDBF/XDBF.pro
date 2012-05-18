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
    RC_FILE = app.rc
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
    synclistdialog.cpp \
    avatarawarddialog.cpp \
    achievementinjectordialog.cpp \
    FileIO.cpp \
    newentrychooser.cpp \
    titleinjectordialog.cpp \
    settinginjectorint.cpp \
    addressconverter.cpp \
    imageinjectordialog.cpp \
    newgpddialog.cpp \
    achievementgpd.cpp \
    stringinjector.cpp

HEADERS  += mainwindow.h \
    xdbf.h \
    imagedialog.h \
    xdbfhelper.h \
    achievementviewer.h \
    binarydialog.h \
    synclistdialog.h \
    avatarawarddialog.h \
    achievementinjectordialog.h \
    FileIO.h \
    newentrychooser.h \
    titleinjectordialog.h \
    settinginjectorint.h \
    addressconverter.h \
    imageinjectordialog.h \
    newgpddialog.h \
    achievementgpd.h \
    stringinjector.h

FORMS    += mainwindow.ui \
    imagedialog.ui \
    achievementviewer.ui \
    binarydialog.ui \
    synclistdialog.ui \
    avatarawarddialog.ui \
    achievementinjectordialog.ui \
    newentrychooser.ui \
    titleinjectordialog.ui \
    settinginjectorint.ui \
    addressconverter.ui \
    imageinjectordialog.ui \
    newgpddialog.ui \
    achievementgpd.ui \
    stringinjector.ui

RESOURCES += \
    images.qrc

OTHER_FILES += \
    app.rc
