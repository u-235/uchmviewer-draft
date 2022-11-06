
TEMPLATE = lib
TARGET = ubrowser
CONFIG *= c++11 warn_on
QT *= core

include(../../common.pri)

INCLUDEPATH *= include

DEFINES *= UBROWSER_API_EXPORT

HEADERS += \
    include/ubrowser/export.hpp \
    include/ubrowser/settings.hpp \
    include/ubrowser/types.hpp

target.path = $${PREFIX}/$${APP_LIB_DIR}
INSTALLS += target
