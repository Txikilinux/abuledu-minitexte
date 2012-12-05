#-------------------------------------------------
#
# Project created by QtCreator 2012-10-07T13:27:11
#
#-------------------------------------------------
system(ccache -V):QMAKE_CXX = ccache g++

#a activer ou pas si on souhaite compiler pour tablette
tablette {
    DEFINES += __ABULEDUTABLETTEV1__MODE__=1
    #AbulEduVirtualKeyboardV1
    include(lib/abuleduvirtualkeyboardv1/abuleduvirtualkeyboardv1.pri)
}

QT       += core gui

TARGET = leterrier-minitexte
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

win32{
  OTHER_FILES += windows/windows.rc
  RC_FILE = windows/windows.rc
}

macx {
 ICON = macos/icones/macos.icns
}

include(futurelib/abuledutextev1/abuledutextev1.pri)
include(lib/abuleduapplicationv1/abuleduapplicationv1.pri)
include(lib/abuleduconstantesv1/abuleduconstantesv1.pri)
include(lib/abuledunetworkaccessmanagerv1/abuledunetworkaccessmanagerv1.pri)
include(lib/abuledumessageboxv1/abuledumessageboxv1.pri)
include(lib/abuleduidentitesv1/abuleduidentitesv1.pri)
include(lib/abuleduflatboutonv1/abuleduflatboutonv1.pri)
include(lib/maia/maia.pri)
include(lib/abuledufilev1/abuledufilev1.pri)
