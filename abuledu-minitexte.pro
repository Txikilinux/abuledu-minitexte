#-------------------------------------------------
#
# Project created by QtCreator 2012-10-07T13:27:11
#
#-------------------------------------------------

!android {
  system(ccache -V):QMAKE_CXX = ccache g++
}
android {
  system(ccache -V):QMAKE_CXX = ccache arm-linux-gnueabihf-g++-4.6
}

INCLUDEPATH += $$PWD
DEPENDPATH  += $$PWD

QT       += core gui script xml network

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

include(lib/abuleduapplicationv1/abuleduapplicationv1.  pri)
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

OTHER_FILES += \
    android/version.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/values-id/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-et/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-es/strings.xml \
    android/src/org/qtproject/qt5/android/bindings/QtApplication.java \
    android/src/org/qtproject/qt5/android/bindings/QtActivity.java \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/AndroidManifest.xml \
    android/version.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/values-id/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-et/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-es/strings.xml \
    android/src/org/qtproject/qt5/android/bindings/QtApplication.java \
    android/src/org/qtproject/qt5/android/bindings/QtActivity.java \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/AndroidManifest.xml

