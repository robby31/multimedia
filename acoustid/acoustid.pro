#-------------------------------------------------
#
# Project created by QtCreator 2017-05-27T08:57:03
#
#-------------------------------------------------

QT       -= gui
QT       += network xml

TARGET = $$qtLibraryTarget(acoustid)

TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release
CONFIG += build_all

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

!exists($$(MYLIBRARY)) {
    error("variable MYLIBRARY not set.")
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += acoustid.cpp \
    acoustidanswer.cpp \
    acoustidclient.cpp

HEADERS += acoustid.h \
    acoustidanswer.h \
    acoustidclient.h

INCLUDEPATH += $$(MYLIBRARY)/$$QT_VERSION/include
INCLUDEPATH += $$(MYLIBRARY)/$$QT_VERSION/include/multimedia

installPath = $$(MYLIBRARY)/$$QT_VERSION
target.path = $$installPath

installIncludePath = $$installPath/include/multimedia

h_includes.files = acoustid.h \
    acoustidanswer.h \
    acoustidclient.h
h_includes.path = $$installIncludePath

INSTALLS += target h_includes
