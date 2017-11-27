#-------------------------------------------------
#
# Project created by QtCreator 2017-10-07T11:53:59
#
#-------------------------------------------------

QT       -= gui
QT       += multimedia

TARGET = $$qtLibraryTarget(spectrumanalyser)

TEMPLATE = lib
CONFIG += staticlib

!exists($$(MYLIBRARY)) {
    error("variable MYLIBRARY not set.")
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += "../../fftreal"

SOURCES += \
        spectrumanalyser.cpp \
        frequencyspectrum.cpp \
        utils.cpp

HEADERS += \
        spectrumanalyser.h  \
        spectrum.h  \
        frequencyspectrum.h   \
        utils.h

installPath = $$(MYLIBRARY)/$$QT_VERSION
target.path = $$installPath

installIncludePath = $$installPath/include/multimedia

h_includes.files = $${HEADERS}
h_includes.path = $$installIncludePath

INSTALLS += target h_includes
