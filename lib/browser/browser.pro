
TEMPLATE = lib
TARGET = uchmviewerbrowser
CONFIG *= c++11 warn_on
QT *= core

include(../../common.pri)

INCLUDEPATH *= include

HEADERS += \
    include/browser/content-provider.hpp \
    include/browser/history.hpp \
    include/browser/settings.hpp

