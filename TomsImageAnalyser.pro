#-------------------------------------------------
#
# Project created by QtCreator 2016-05-09T11:36:48
#
#-------------------------------------------------

QT += core gui
QT += sql
QT += concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TomsImageAnalyser
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mainui.cpp \
    flowlayout.cpp \
    caseselect.cpp \
    imagefunctions.cpp \
    databasefunctions.cpp \
    imageinspector.cpp \
    faceanalysis.cpp \
    skinanalysis.cpp \
    exif.cpp \
    autoadjustcontrast.cpp
INCLUDEPATH += "C:\opencv\build\include"

LIBS += -L"C:\opencv\build\x64\vc14\lib"
LIBS += -lopencv_world310


HEADERS  += mainwindow.h \
    mainui.h \
    flowlayout.h \
    caseselect.h \
    imagefunctions.h \
    databasefunctions.h \
    matconversion.h \
    imageinspector.h \
    faceanalysis.h \
    skinanalysis.h \
    exif.h \
    autoadjustcontrast.h

FORMS    += mainwindow.ui \
    mainui.ui \
    caseselect.ui \
    imageinspector.ui

DISTFILES +=

RESOURCES += \
    resources.qrc
