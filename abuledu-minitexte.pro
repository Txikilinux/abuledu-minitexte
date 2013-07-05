#-------------------------------------------------
#
# Project created by QtCreator 2012-10-07T13:27:11
#
#-------------------------------------------------
system(ccache -V):QMAKE_CXX = ccache g++

QT       += core gui

TARGET = abuledu-minitexte
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
            version.h

FORMS    += mainwindow.ui

RESOURCES += resources.qrc

win32{
  OTHER_FILES += windows/windows.rc
  RC_FILE = windows/windows.rc
}

macx {
 ICON = macos/icones/abuledu-minitexte.icns
}

include(lib/abuleduapplicationv1/abuleduapplicationv1.pri)
include(lib/abuleduconstantesv1/abuleduconstantesv1.pri)
include(lib/abuledunetworkaccessmanagerv1/abuledunetworkaccessmanagerv1.pri)
include(lib/abuledumessageboxv1/abuledumessageboxv1.pri)
include(lib/abuleduidentitesv1/abuleduidentitesv1.pri)
include(lib/abuleduflatboutonv1/abuleduflatboutonv1.pri)
include(lib/maia/maia.pri)
include(lib/abuledufilev1/abuledufilev1.pri)

#include(futurelib/abuledutextev1/abuledutextev1.pri)
include(lib/abuledupicottsv1/abuledupicottsv1.pri)
include(lib/abuledusplashscreenv1/abuledusplashscreenv1.pri)
include(lib/abuledulangv1/abuledulangv1.pri)
