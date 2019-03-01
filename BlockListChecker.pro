#-------------------------------------------------
#
# Project created by QtCreator 2015-08-19T18:35:20
#
#-------------------------------------------------

QT       += core gui network

win32:RC_ICONS += images\up-green.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BlockListChecker
TEMPLATE = app

SOURCES += main.cpp\
        blocklistchecker.cpp \
    log.cpp \
    settings.cpp \
    singleapplication.cpp \
    ustawienia.cpp \
    download.cpp

HEADERS  += blocklistchecker.h \
    log.h \
    settings.h \
    singleapplication.h \
    ustawienia.h \
    download.h

FORMS    += blocklistchecker.ui \
    settings.ui

RESOURCES += \
    images.qrc

Release:DESTDIR = release
Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.rcc
Release:UI_DIR = release/.ui

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.rcc
Debug:UI_DIR = debug/.ui
