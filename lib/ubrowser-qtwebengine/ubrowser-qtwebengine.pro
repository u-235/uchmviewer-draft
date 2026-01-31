
TEMPLATE = lib
TARGET = ubrowser-qtwebengine
QT *= core widgets webenginewidgets
CONFIG += plugin

!isEmpty(USE_STATIC_QTWEBENGINE) {
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
    src/qtwebenginebrowser.h \
    src/qtwebenginebuilder.h \
    src/qtwebenginepage.h \
    src/qtwebengineurlschemehandler.h \
    src/qtwebenginewidget.h

SOURCES += \
    src/qtwebenginebrowser.cpp \
    src/qtwebenginebuilder.cpp \
    src/qtwebengineurlschemehandler.cpp \
    src/qtwebenginewidget.cpp

#   src/qtwebenginebrowser.js
#   src/qtwebengine.json

RESOURCES += \
    src/qtwebengine.qrc

isEmpty(USE_STATIC_QTWEBENGINE) {
    target.path = $${PREFIX}/$${APP_LIB_DIR}
    INSTALLS += target
}
