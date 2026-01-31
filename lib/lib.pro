
TEMPLATE = subdirs
SUBDIRS = libebook ubrowser
libebook.depends = ubrowser

!isEmpty(USE_STATIC_CHMLIB) {
    exists(CHMLib/src/chm_lib.h) {
        SUBDIRS += CHMLib
        CHMLib.file = CHMLib.pro
        libebook.depends *= CHMLib
    } else {
        error(The lib/CHMLib submodule was not found. Use 'git submodule update --init' for static linking with chmlib.)
    }
}

!isEmpty(USE_STATIC_QTWEBENGINE) {
    exists(ubrowser-qtwebengine/src/qtwebenginebrowser.h) {
        SUBDIRS += ubrowser-qtwebengine
        ubrowser-qtwebengine.depends = ubrowser
    } else {
        error(The lib/ubrowser-qtwebengine submodule was not found.)
    }
}
