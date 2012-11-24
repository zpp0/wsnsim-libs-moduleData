include(../libs.pri)

QT       -= gui

INCLUDEPATH += . /usr/include/luajit-2.0
LIBS += -lluajit-5.1

SOURCES += moduleData.cpp
HEADERS += moduleParams.h moduleData.h
