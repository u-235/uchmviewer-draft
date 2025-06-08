
TEMPLATE = lib
TARGET = ubrowser
QT *= core widgets

include(../../common.pri)

INCLUDEPATH *= include

DEFINES *= UBROWSER_API_EXPORT

HEADERS += \
    include/ubrowser/contentprovider.hpp \
    include/ubrowser/export.hpp \
    include/ubrowser/settings.hpp \
    include/ubrowser/types.hpp

SOURCES += \
    src/contentprovider.cpp

target.path = $${PREFIX}/$${APP_LIB_DIR}
INSTALLS += target
