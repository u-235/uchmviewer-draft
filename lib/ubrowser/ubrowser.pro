
TEMPLATE = lib
TARGET = ubrowser
QT *= core widgets

include(../../common.pri)

INCLUDEPATH *= include

DEFINES *= UBROWSER_API_EXPORT

HEADERS += \
    include/ubrowser/browser.hpp \
    include/ubrowser/builder.hpp \
    include/ubrowser/contentprovider.hpp \
    include/ubrowser/export.hpp \
    include/ubrowser/history.hpp \
    include/ubrowser/history-impl.hpp \
    include/ubrowser/settings.hpp \
    include/ubrowser/types.hpp

SOURCES += \
    src/browser.cpp \
    src/history-impl.cpp

target.path = $${PREFIX}/$${APP_LIB_DIR}
INSTALLS += target
