#-------------------------------------------------
#
# Project created by QtCreator 2017-05-26T22:55:59
#
#-------------------------------------------------

QT       -= gui
QT       += xml

TARGET = chromaprintwrapper
TARGET = $$qtLibraryTarget($$TARGET)

TEMPLATE = lib
CONFIG += staticlib

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

!exists($$(MYLIBRARY)) {
    error("variable MYLIBRARY not set.")
}

DESTDIR = $$(MYLIBRARY)/$$QT_VERSION

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chromaprint_wrapper.cpp

HEADERS += \
    chromaprint_wrapper.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += $$PWD/../../libchromaprint-1.1/src

DISTFILES += \
    chromaprintwrapper.prf
