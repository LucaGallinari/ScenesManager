# -------------------------------------------------
# Project created by QtCreator 2015-07-27T11:32:17
# -------------------------------------------------
QT       += core gui widgets
CONFIG   += c++11
#CONFIG   += static

TARGET = ScenesManager
TEMPLATE = app

SOURCES +=  main.cpp \
            mainwindow.cpp \
            QVideoDecoder.cpp \
            PlayerWidget.cpp \
            ImagesBuffer.cpp \
            PreviewsWidget.cpp \
            CompareMarkersDialog.cpp \
            MarkersWidget.cpp \
            MenuBar.cpp \
            TitleBar.cpp \
            WindowTitleFilter.cpp \
            HoverMoveFilter.cpp

HEADERS +=  mainwindow.h \
            QVideoDecoder.h \
            ffmpeg.h \
            PlayerWidget.h \
            ImagesBuffer.h \
            PreviewsWidget.h \
            CompareMarkersDialog.h \
            MarkersWidget.h \
            MenuBar.h \
            TitleBar.h \
            WindowTitleFilter.h \
            HoverMoveFilter.h

FORMS +=    mainwindow.ui \
            comparemarkersdialog.ui
RESOURCES += resource.qrc \
    playericons.qrc \
    windowicons.qrc
DEFINES += DEVELMODE

# ##############################################################################
# Modify the below path so that it point to the folder containing
# .lib, .dll.a and .def files of ffmpeg
# ##############################################################################
win32 {
    FFMPEG_LIBRARY_PATH = ffmpeg_lib_win64
}
unix {
    FFMPEG_LIBRARY_PATH = "/usr/local/lib"
    FFMPEG_INCLUDE_PATH += "/usr/local/include"
}
# ##############################################################################
# Do not modify from here: FFMPEG default settings
# ##############################################################################
win32 {
# Set list of required FFmpeg libraries
    LIBS += -L"$$PWD/$$FFMPEG_LIBRARY_PATH"
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
}
unix {
# Set list of required FFmpeg libraries
    LIBS += -L"$$FFMPEG_LIBRARY_PATH"
    LIBS += -lavcodec \
            -lavdevice \
            -lavfilter \
            -lavformat \
            -lavutil \
            -lpostproc \
            -lswresample \
            -lswscale \
            -lx264 \
            -lz
# Related includes
    INCLUDEPATH += FFMPEG_INCLUDE_PATH
}

# Requied for some C99 defines
DEFINES += __STDC_CONSTANT_MACROS

# ##############################################################################
# FFMPEG: END OF CONFIGURATION
# ##############################################################################
