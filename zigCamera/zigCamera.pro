#-------------------------------------------------
#
# Project created by QtCreator 2016-03-26T17:30:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11

TARGET = zigCamera
TEMPLATE = app


INCLUDEPATH += D:\opencv\opencv249\build\include

CONFIG(debug, debug|release): {

LIBS += -LD:\opencv\opencv249\build\x64\vc12\lib \
            -lopencv_calib3d249d \
            -lopencv_contrib249d \
            -lopencv_core249d \
            -lopencv_highgui249d \
            -lopencv_imgproc249d \
            -lopencv_objdetect249d \
            -lopencv_features2d249d \
            -lopencv_flann249d \
            -lopencv_gpu249d \
            -lopencv_legacy249d \
            -lopencv_ml249d \
            -lopencv_ts249d \
            -lopencv_video249d
}

CONFIG(release, debug|release): {

LIBS += -LD:\opencv\opencv249\build\x64\vc12\lib \
            -lopencv_calib3d249 \
            -lopencv_contrib249 \
            -lopencv_core249 \
            -lopencv_highgui249 \
            -lopencv_imgproc249 \
            -lopencv_objdetect249 \
            -lopencv_features2d249 \
            -lopencv_flann249 \
            -lopencv_gpu249 \
            -lopencv_legacy249 \
            -lopencv_ml249 \
            -lopencv_ts249 \
            -lopencv_video249
}


SOURCES += main.cpp\
        mainwindow.cpp \
    fhog.cpp \
    kcftracker.cpp \
    tools.cpp \
    multiobjtracker.cpp

HEADERS  += mainwindow.h \
    ffttools.hpp \
    fhog.hpp \
    kcftracker.hpp \
    labdata.hpp \
    MultiObjTracker.h \
    recttools.hpp \
    tools.h \
    tracker.h \
    multiobjtracker.h

RESOURCES += \
    res.qrc

