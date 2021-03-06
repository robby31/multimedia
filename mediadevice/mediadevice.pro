#-------------------------------------------------
#
# Project created by QtCreator 2017-05-27T22:39:38
#
#-------------------------------------------------

QT       -= gui
QT       += xml multimedia

TARGET = $$qtLibraryTarget(mediadevice)
TEMPLATE = lib

CONFIG += debug_and_release
CONFIG += shared_and_static build_all

CONFIG += c++14

CONFIG += sdk_no_version_check

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

!exists($$(MYLIBRARY)) {
    error("variable MYLIBRARY not set.")
}

#DEFINES += PROFILING

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
    qffmpeg.cpp \
    device.cpp \
    transcodeprocess.cpp \
    ffmpegtranscoding.cpp \
    streamingfile.cpp \
    mencodertranscoding.cpp \
    wavfile.cpp \
    qffmpegmedia.cpp \
    qffmpegstream.cpp \
    qffmpegcodec.cpp \
    qffmpegframe.cpp \
    qffmpegpacket.cpp \
    qffmpegtranscoding.cpp \
    transcodedevice.cpp \
    qffmpegbuffer.cpp \
    qffmpeginputmedia.cpp \
    qffmpegoutputmedia.cpp \
    qffmpegdecoder.cpp \
    qffmpegencoder.cpp \
    qffmpeginputstream.cpp \
    qffmpegoutputstream.cpp \
    qffmpegaudioencoder.cpp \
    qffmpegvideoencoder.cpp \
    qffmpegaudiodecoder.cpp \
    qffmpegvideodecoder.cpp

HEADERS += \
    qffmpeg.h \
    device.h \
    transcodeprocess.h \
    ffmpegtranscoding.h \
    streamingfile.h \
    mencodertranscoding.h \
    wavfile.h \
    qffmpegmedia.h \
    qffmpegstream.h \
    qffmpegcodec.h \
    qffmpegframe.h \
    qffmpegpacket.h \
    qffmpegtranscoding.h \
    transcodedevice.h \
    qffmpegbuffer.h \
    qffmpeginputmedia.h \
    qffmpegoutputmedia.h \
    qffmpegdecoder.h \
    qffmpegencoder.h \
    qffmpeginputstream.h \
    qffmpegoutputstream.h \
    qffmpegaudioencoder.h \
    qffmpegvideoencoder.h \
    qffmpegaudiodecoder.h \
    qffmpegvideodecoder.h

DISTFILES +=

INCLUDEPATH += $$(MYLIBRARY)/include/analyzer
LIBS += -L$$(MYLIBRARY)/lib -l$$qtLibraryTarget(analyzer)

INCLUDEPATH += $$(MYLIBRARY)/include/QmlApplication
LIBS += -L$$(MYLIBRARY)/lib -l$$qtLibraryTarget(QmlApplication)

INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib -lavcodec -lavformat -lavutil -lswscale -lswresample

installPath = $$(MYLIBRARY)
target.path = $$installPath/lib

installIncludePath = $$installPath/include/multimedia

h_include.files = $${HEADERS}
h_include.path = $$installIncludePath

INSTALLS += target h_include

macx {
    CONFIG += lib_bundle

    FRAMEWORK_HEADERS.version = Versions
    FRAMEWORK_HEADERS.files = $${HEADERS}
    FRAMEWORK_HEADERS.path = include
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS

    QMAKE_FRAMEWORK_BUNDLE_NAME = mediadevice
}
