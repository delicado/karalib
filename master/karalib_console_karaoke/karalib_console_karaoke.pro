
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += C++14

#CONFIG -= qt
QT += core

SOURCES += main.cpp

INCLUDEPATH += $$PWD/../karalib/src
win32: LIBS += -L$$OUT_PWD/../karalib/debug -lkaralib
PRE_TARGETDEPS += $$OUT_PWD/../karalib/debug/libkaralib.a




include(deployment.pri)
qtcAddDeployment()


