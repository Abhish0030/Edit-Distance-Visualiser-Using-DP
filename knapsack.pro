QT += core gui widgets

CONFIG += c++17

TARGET = SmartSpellChecker
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    editsolver.cpp

HEADERS += \
    mainwindow.h \
    editsolver.h

win32:CONFIG(release, debug|release): DESTDIR = $$PWD/bin
else:DESTDIR = $$PWD/bin
