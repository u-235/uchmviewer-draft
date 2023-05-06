
TEMPLATE = lib
TARGET = uchmviewerbrowser
CONFIG *= c++11 warn_on sharedlib
QT *= core widgets

INCLUDEPATH *= include

DEFINES *= BROWSER_API_EXPORT

HEADERS += \
    include/browser/abstract-browser-controller.hpp \
    include/browser/browser-builder.hpp \
    include/browser/browser-controller.hpp \
    include/browser/browser-history.hpp \
    include/browser/browser-history-impl.hpp \
    include/browser/browser-settings.hpp \
    include/browser/browser-types.hpp \
    include/browser/content-provider.hpp \
    include/browser/export.hpp

SOURCES += \
    abstract-browser-controller.cpp \
    browser-controller.cpp \
    browser-history-impl.cpp
