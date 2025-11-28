QT       += core gui
QT += core gui widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

RC_ICONS = favicon.ico


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    apidialog.cpp \
    autoscroller.cpp \
    bosskeymanager.cpp \
    interfacesetting.cpp \
    main.cpp \
    mainwindow.cpp \
    novelapiclient.cpp \
    novelwindow.cpp \
    searchitemwidget.cpp \
    throttle.cpp \
    tiplabel.cpp

HEADERS += \
    apidialog.h \
    autoscroller.h \
    bosskeymanager.h \
    interfacesetting.h \
    mainwindow.h \
    novelapiclient.h \
    noveltypes.h \
    novelwindow.h \
    searchitemwidget.h \
    throttle.h \
    tiplabel.h

FORMS += \
    interfacesetting.ui \
    mainwindow.ui \
    novelwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES += \
    dll/libcrypto-1_1-x64.dll \
    dll/libssl-1_1-x64.dll \
    favicon.ico
