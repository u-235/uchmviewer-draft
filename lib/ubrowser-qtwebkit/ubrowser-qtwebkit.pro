
TEMPLATE = lib
TARGET = ubrowser-qtwebkit
QT *= core widgets webkit webkitwidgets
CONFIG += plugin

!isEmpty(USE_STATIC_QTWEBKIT) {
    CONFIG += static
    DEFINES *= UBROWSER_API_STATIC
} else {
    DEFINES *= UBROWSER_API_EXPORT
}

MODULES = ubrowser

include(../../common.pri)

INCLUDEPATH *= include

# Private
HEADERS += \
    src/qtwebkitbrowser.h \
    src/qtwebkitbuilder.h \
    src/qtwebkitnetwork.h \
    src/qtwebkitwidget.h

SOURCES += \
    src/qtwebkitbrowser.cpp \
    src/qtwebkitbuilder.cpp \
    src/qtwebkitnetwork.cpp \
    src/qtwebkitwidget.cpp
    
#   src/qtwebkitbrowser.js
#   src/qtwebkit.json

RESOURCES += \
    src/qtwebkit.qrc

isEmpty(USE_STATIC_QTWEBKIT) {
    target.path = $${PREFIX}/$${APP_LIB_DIR}
    INSTALLS += target
}
