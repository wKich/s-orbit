#-------------------------------------------------
#
# Project created by QtCreator 2013-11-21T14:41:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = s-orbit
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp\
        mainwindow.cpp \
    orbitcalculator.cpp \
    imagerender.cpp \
    statusdialog.cpp \
    datafile.cpp \
    bruteforcecalculator.cpp \
    barneshutcalculator.cpp \
    quadtree.cpp

HEADERS  += mainwindow.h \
    orbitcalculator.h \
    imagerender.h \
    statusdialog.h \
    datafile.h \
    bruteforcecalculator.h \
    barneshutcalculator.h \
    pointdouble2d.h \
    quadtree.h

FORMS    += mainwindow.ui

RESOURCES += \
    main.qrc

OTHER_FILES += \
    main.frag \
    main.vert
