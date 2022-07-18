QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    A2168_interface.cpp \
    Buttons_QuickCommands.cpp \
    Tab_ExtendedUnseal.cpp \
    Tab_Flash_Memory.cpp \
    Tab_Read_Write.cpp \
    Tab_Standart_SBS.cpp \
    cp2112.cpp \
    hid_smbus_errors.cpp \
    main.cpp \
    mainwindow.cpp \
    progress.cpp \
    smbus.cpp \
    statusbar.cpp

HEADERS += \
    A2168_interface.h \
    Library/Windows/SLABCP2112.h \
    cp2112.h \
    cp2112_config.h \
    hid_smbus_errors.h \
    mainwindow.h \
    progress.h \
    smbus.h \
    statusbar.h \
    types.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/Library/Windows/x64/ -lSLABHIDtoSMBus

INCLUDEPATH += $$PWD/Library/Windows
DEPENDPATH += $$PWD/Library/Windows

DISTFILES += \
	Library/Windows/x64/SLABHIDDevice.dll \
	Library/Windows/x64/SLABHIDtoSMBus.dll \
	Library/Windows/x64/SLABHIDtoSMBus.lib
