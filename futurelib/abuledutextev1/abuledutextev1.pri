QT += core gui network xml

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += abuledutextev1.h

SOURCES += abuledutextev1.cpp

FORMS += abuledutextev1.ui

!exists($$PWD/../../data/abuledutextev1/abuledutextev1.qrc) {
  RESOURCES += abuledutextev1.qrc
}
