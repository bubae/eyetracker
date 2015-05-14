#-------------------------------------------------
#
# Project created by QtCreator 2015-03-16T05:59:37
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Eye
CONFIG   += console
CONFIG += link_pkgconfig
CONFIG   -= app_bundle
PKGCONFIG += opencv\
            cvblob
INCLUDEPATH += /usr/include/opencv2
LIBS += -L/usr/lib \
-lopencv_core\
-lopencv_highgui\
-lopencv_imgproc

LIBS += -L/usr/local/lib\

TEMPLATE = app

SOURCES += main.cpp \
    Headers/Segment.cpp \
    Headers/Corner.cpp \
    Headers/Ellipse.cpp \
    Headers/Candidate.cpp \
    Headers/ImageProcess.cpp

HEADERS += \
    Headers/Segment.h \
    Headers/Corner.h \
    Headers/Ellipse.h \
    Headers/Candidate.h \
    Headers/ImageProcess.h
