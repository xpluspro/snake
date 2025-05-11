QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = snake-qt
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    game.cpp \
    snake.cpp \
    food.cpp

HEADERS += \
    mainwindow.h \
    game.h \
    snake.h \
    food.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target 