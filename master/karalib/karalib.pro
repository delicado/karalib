#-------------------------------------------------
#
# Project created by QtCreator 2016-06-14T13:02:16
#
#-------------------------------------------------

QT       += core

CONFIG += C++14

TARGET = karalib
TEMPLATE = lib

DEFINES += KARALIB_LIBRARY

SOURCES += karalib.cpp \
    src/chunk.cpp \
    src/defprocedures.cpp \
    src/error.cpp \
    src/event.cpp \
    src/fileformat.cpp \
    src/karaoke.cpp \
    src/ksong.cpp \
    src/midi.cpp \
    src/midiout.cpp \
    src/scheduler.cpp \
    src/song.cpp \
    src/tracks.cpp \
    src/utils.cpp

HEADERS += karalib.h\
        karalib_global.h \
    src/buffer.h \
    src/chunk.h \
    src/defprocedures.h \
    src/error.h \
    src/event.h \
    src/fileformat.h \
    src/flags.h \
    src/karaoke.h \
    src/ksong.h \
    src/ktypes.h \
    src/midi.h \
    src/midiout.h \
    src/out.h \
    src/scheduler.h \
    src/song.h \
    src/tracks.h \
    src/utils.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32 {
    LIBS += -lwinmm
}
