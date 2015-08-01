# -------------------------------------------------
# Project created by QtCreator 2015-07-27T11:32:17
# -------------------------------------------------
QT       += core gui widgets

TARGET = ScenesManager
TEMPLATE = app

SOURCES +=  main.cpp \
            mainwindow.cpp \
            QVideoDecoder.cpp \
            PlayerWidget.cpp \
            ImagesBuffer.cpp \
            PreviewsWidget.cpp

HEADERS +=  mainwindow.h \
            QVideoDecoder.h \
            ffmpeg.h \
            PlayerWidget.h \
            ImagesBuffer.h \
            PreviewsWidget.h

FORMS += mainwindow.ui
RESOURCES += resource.qrc \
    playericons.qrc
DEFINES += DEVELMODE

# ##############################################################################
# Modify the below path so that it point to the folder containing
# .lib, .dll.a and .def files of ffmpeg
# ##############################################################################
FFMPEG_LIBRARY_PATH = ffmpeg_lib_win64

# ##############################################################################
# Do not modify from here: FFMPEG default settings
# ##############################################################################
# Add the path
LIBS += -L$$PWD/$$FFMPEG_LIBRARY_PATH

# Set list of required FFmpeg libraries
LIBS += -lavutil \
        -lavcodec \
        -lavformat \
        -lswscale

# Related includes
INCLUDEPATH +=  $$PWD/libavutil \
                $$PWD/libavcodec \
                $$PWD/libavdevice \
                $$PWD/libavformat \
                $$PWD/libswscale
DEPENDPATH +=   $$PWD/libavutil \
                $$PWD/libavcodec \
                $$PWD/libavdevice \
                $$PWD/libavformat \
                $$PWD/libswscale

# Requied for some C99 defines
DEFINES += __STDC_CONSTANT_MACROS

# ##############################################################################
# FFMPEG: END OF CONFIGURATION
# ##############################################################################


