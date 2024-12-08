
TEMPLATE = app
TARGET = uchmviewer

include(../common.pri)

defined(VERSION, var) {
    # Define the version as a string literal
    # https://stackoverflow.com/a/2411008
    DEFINES += 'APP_VERSION=\\"$$VERSION\\"'
} else {
    VERSION = $$getVersion()
}

defined(USE_GETTEXT, var): DEFINES += USE_GETTEXT \
    'APP_INSTALL_DIR_BIN=\\"$${APP_BIN_DIR}\\"' \
    'APP_INSTALL_DIR_LOCALE=\\"$${APP_LOCALE_DIR}\\"' \

QT += \
    xml \
    network \
    widgets \
    printsupport

HEADERS += \
    config.h \
    dialog_chooseurlfromlist.h \
    dialog_setup.h \
    ebookcontentprovider.h \
    mainwindow.h \
    recentfiles.h \
    settings.h \
    tab_bookmarks.h \
    tab_contents.h \
    tab_index.h \
    tab_search.h \
    version.h \
    viewwindowmgr.h \
    navigationpanel.h \
    toolbarmanager.h \
    toolbareditor.h \
    textencodings.h \
    treeitem_toc.h \
    treeitem_index.h \
    mimehelper.h \
    showwaitcursor.h \
    i18n.h \
    webbrowser.h

SOURCES += \
    config.cpp \
    dialog_chooseurlfromlist.cpp \
    dialog_setup.cpp \
    ebookcontentprovider.cpp \
    main.cpp \
    mainwindow.cpp \
    recentfiles.cpp \
    settings.cpp \
    tab_bookmarks.cpp \
    tab_contents.cpp \
    tab_index.cpp \
    tab_search.cpp \
    viewwindowmgr.cpp \
    navigationpanel.cpp \
    toolbarmanager.cpp \
    toolbareditor.cpp \
    textencodings.cpp \
    treeitem_toc.cpp \
    treeitem_index.cpp \
    mimehelper.cpp \
    i18n.cpp

FORMS += \
    tab_bookmarks.ui \
    tab_index.ui \
    tab_contents.ui \
    tab_search.ui \
    dialog_setup.ui \
    dialog_topicselector.ui \
    mainwindow.ui \
    window_browser.ui \
    navigatorpanel.ui \
    dialog_about.ui \
    toolbareditor.ui

RESOURCES += resources/images.qrc

INCLUDEPATH *= ../lib/libebook
LIBS *= -L"../lib/libebook"

INCLUDEPATH *= ../lib/ubrowser/include
LIBS *= -L"../lib/ubrowser"

!isEmpty(USE_STATIC_CHMLIB) {
    INCLUDEPATH *= ../lib/CHMLib/src
    LIBS *= -L"../lib"
} else {
    !isEmpty(CHMLIB_ROOT): INCLUDEPATH *= "$$CHMLIB_ROOT/include"
    !isEmpty(CHMLIB_ROOT): LIBS *= -L"$$CHMLIB_ROOT/lib"
}

!isEmpty(LIBZIP_ROOT): LIBS *= -L"$$LIBZIP_ROOT/lib"

LIBS *= -lebook -lchm -lzip -lubrowser

# This is used by cross-build on 64-bit when building a 32-bit version
linux-g++-32: {
       LIBS *= -L.
}

# General per-platform settings
defined(USE_MAC_APP, var) | macx:{
    HEADERS += uchmviewerapp.h
    SOURCES += uchmviewerapp.cpp
    DEFINES += USE_MAC_APP
}

win32:{
    CONFIG( debug, debug|release ) {
            LIBS *= -L"../lib/libebook/debug" -L"../lib/debug" -L"../lib/ubrowser/debug"
    } else {
            LIBS *= -L"../lib/libebook/release" -L"../lib/release" -L"../lib/ubrowser/release"
    }

    LIBS += -loleaut32
    defined(USE_GETTEXT, var): LIBS *= -lintl

    RC_ICONS = resources/uchmviewer.ico
}

unix:!macx: {
    QT += dbus
    HEADERS += dbus_interface.h
    SOURCES += dbus_interface.cpp
    CONFIG += dbus
    DEFINES += USE_DBUS
}

!isEmpty(USE_WEBENGINE) {
    isEqual(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 9):error("QtWebEnginew requires at least Qt5.9")

    QT += webenginewidgets
    DEFINES += USE_WEBENGINE
    SOURCES += qtwebengine/dataprovider.cpp \
        qtwebengine/webenginebrowser.cpp \
        qtwebengine/webenginewidget.cpp
    HEADERS += qtwebengine/dataprovider.h \
        qtwebengine/webenginebrowser.h \
        qtwebengine/webenginepage.h \
        qtwebengine/webenginewidget.h
	    RESOURCES += qtwebengine/webengine.qrc
} else {
    QT += webkit webkitwidgets
    DEFINES += USE_WEBKIT
    SOURCES += qtwebkit/dataprovider.cpp \
        qtwebkit/webkitbrowser.cpp \
        qtwebkit/webkitwidget.cpp
    HEADERS += qtwebkit/dataprovider.h \
        qtwebkit/webkitbrowser.h \
        qtwebkit/webkitwidget.h
}

greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

target.path = $${PREFIX}/$${APP_BIN_DIR}
INSTALLS += target
