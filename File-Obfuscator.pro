# using Qt concurrent to faciliate multi-threading easily whilst being easy to scale
# (Qt Concurrent 6.1.0, 2021)
QT       += \
    core gui\
    concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# split the program into multiple files for ease of use
SOURCES += \
    main.cpp \
    fileobfuscator.cpp \
    digitoperations.cpp \
    fileoperations.cpp \
    addciphers.cpp \
# add new cipher files here
    caesarcipher.cpp \
    passwordcipher.cpp

# uses custom solution for unordered maps (robin_hood.h)
# (Ankerl, 2021)
HEADERS += \
    fileobfuscator.h \
    robin_hood.h \
# add new cipher header files here
    caesarcipher.h \
    passwordcipher.h

FORMS += \
    fileobfuscator.ui \
# add new cipher forms here
    caesarcipher.ui \
    passwordcipher.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# defines application details
VERSION = 1.0
QMAKE_TARGET_COMPANY = "gotumjee"
QMAKE_TARGET_PRODUCT = "File Obfuscator"
QMAKE_TARGET_DESCRIPTION = "File Obfuscator"
QMAKE_TARGET_COPYRIGHT = "gotumjee"

# loads custom resources including fonts and themes
# (Duquesnoy, 2021)
RESOURCES += \
    fobsresources/fobsresources.qrc \
    qdarkstyle/dark/dark.qrc \
    qdarkstyle/light/light.qrc

# sets app icon
win32:RC_ICONS = fobsresources/fobs.ico
macx:ICON = fobsresources/fobs.ico
