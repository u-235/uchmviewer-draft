
TEMPLATE = lib
TARGET = ubrowser
CONFIG *= c++11 warn_on
QT *= core

include(../../common.pri)

INCLUDEPATH *= include

DEFINES *= UBROWSER_API_EXPORT

HEADERS += \
    include/ubrowser/browser.hpp \
    include/ubrowser/builder.hpp \
    include/ubrowser/content-provider.hpp \
    include/ubrowser/export.hpp \
    include/ubrowser/history.hpp \
    include/ubrowser/settings.hpp \
    include/ubrowser/types.hpp

SOURCES += \
    src/browser.cpp \
    src/content-provider.cpp

target.path = $${PREFIX}/$${APP_LIB_DIR}
INSTALLS += target
