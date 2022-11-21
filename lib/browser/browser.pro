
TEMPLATE = lib
TARGET = uchmviewerbrowser
CONFIG *= c++11 warn_on sharedlib
QT *= core

INCLUDEPATH *= include

DEFINES *= BROWSER_API_EXPORT

HEADERS += \
#   include/browser/abstract-browser-page.hpp \
#   include/browser/browser-builder.hpp \
    include/browser/browser-history.hpp \
    include/browser/browser-history-impl.hpp \
#   include/browser/browser-page.hpp \
    include/browser/browser-settings.hpp \
    include/browser/content-provider.hpp \
    include/browser/export.hpp

SOURCES += \
#   abstract-browser-page.cpp \
    browser-history-impl.cpp
