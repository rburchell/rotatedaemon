######################################################################
# Automatically generated by qmake (2.01a) Mon Apr 19 19:29:54 2010
######################################################################

TEMPLATE = app
TARGET = rotatedaemon
DEPENDPATH += .
INCLUDEPATH += .

LIBS += -lX11 -lXrandr

# Input
SOURCES += main.cpp \
    rotationdaemon.cpp

HEADERS += \
    rotationdaemon.h

target.path = /usr/bin

INSTALLS += target

maemo5:* {
    INSTALLS += eventfile
    eventfile.path =  /etc/event.d/
    eventfile.files += rotated
}
