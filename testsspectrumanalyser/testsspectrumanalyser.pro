#-------------------------------------------------
#
# Project created by QtCreator 2017-10-07T14:32:40
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

QT += multimedia xml

TARGET = tst_testsspectrumanalysertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

CONFIG+=sdk_no_version_check

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$(MYLIBRARY)/include/analyzer
LIBS += -L$$(MYLIBRARY)/lib -l$$qtLibraryTarget(analyzer)

INCLUDEPATH += $$(MYLIBRARY)/include/multimedia
LIBS += -L$$(MYLIBRARY)/lib -l$$qtLibraryTarget(mediadevice)

INCLUDEPATH += "../spectrumanalyser"
LIBS += -L$$(MYLIBRARY)/lib -l$$qtLibraryTarget(spectrumanalyser)

INCLUDEPATH += $$(MYLIBRARY)/include/QmlApplication
LIBS += -L$$(MYLIBRARY)/lib -l$$qtLibraryTarget(QmlApplication)

INCLUDEPATH += $$(MYLIBRARY)/lib/fftreal.framework/include
LIBS += -F$$(MYLIBRARY)/lib
LIBS += -framework fftreal

INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib -lavcodec -lavformat -lavutil -lswscale -lswresample

SOURCES += \
        tst_testsspectrumanalysertest.cpp 

DEFINES += SRCDIR=\\\"$$PWD/\\\"
