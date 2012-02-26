#-------------------------------------------------
#
# Project created by QtCreator 2012-02-26T22:15:12
#
#-------------------------------------------------

QT       += core gui

TARGET = leterrier-mulot
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    widgetchoixtheme.cpp

HEADERS  += mainwindow.h \
    widgetchoixtheme.h

FORMS    += mainwindow.ui \
    widgetchoixtheme.ui

#AbulEduApplicationV1
include(lib/abuleduapplicationv1/abuleduapplicationv1.pri)

#AbulEduConstantesV1
include(lib/abuleduconstantesv1/abuleduconstantesv1.pri)
