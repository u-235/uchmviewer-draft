
TEMPLATE = lib
TARGET = uchmviewerbrowser
CONFIG *= c++11 warn_on
QT *= core

include(../../common.pri)

INCLUDEPATH *= include

HEADERS += \
    include/browser/settings.hpp

