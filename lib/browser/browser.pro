
TEMPLATE = lib
TARGET = uchmviewerbrowser
CONFIG *= c++11 warn_on sharedlib
QT *= core widgets

include(../../common.pri)

INCLUDEPATH *= include

DEFINES *= BROWSER_API_EXPORT

HEADERS += \
    include/browser/abstract-controller.hpp \
    include/browser/builder.hpp \
    include/browser/content-provider.hpp \
    include/browser/controller.hpp \
    include/browser/export.hpp \
    include/browser/history.hpp \
    include/browser/history-impl.hpp \
    include/browser/settings.hpp \
    include/browser/types.hpp

SOURCES += \
    src/abstract-controller.cpp \
    src/controller.cpp \
    src/history-impl.cpp

