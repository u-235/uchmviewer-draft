
TEMPLATE = lib
TARGET = ubrowser
QT *= core widgets
USE_STATIC_UBROWSER=1

!isEmpty(USE_STATIC_UBROWSER) {
    CONFIG += static
    DEFINES *= UBROWSER_API_STATIC
} else {
    DEFINES *= UBROWSER_API_EXPORT
}

include(../../common.pri)

INCLUDEPATH *= include
VERSION = 0.8.0.0

HEADERS += \
    include/ubrowser/browser.hpp \
    include/ubrowser/contentprovider.hpp \
    include/ubrowser/export.hpp \
    include/ubrowser/history.hpp \
    include/ubrowser/settings.hpp

SOURCES += \
    src/browser.cpp

isEmpty(USE_STATIC_UBROWSER) {
    target.path = $${PREFIX}/$${APP_LIB_DIR}
    INSTALLS += target
}
