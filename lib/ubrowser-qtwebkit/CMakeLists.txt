
if (NOT DEFINED QT)
    find_package(Qt ${APP_QT_VERSION} NAMES Qt5 Qt6 REQUIRED)
    set(QT Qt${Qt_VERSION_MAJOR})
endif ()

find_package(${QT} REQUIRED Core Widgets WebKit WebKitWidgets)

# see https://doc.qt.io/Qt-6/qt-add-plugin.html
if (USE_STATIC_QTWEBKIT)
    add_plugin(ubrowser-qtwebkit STATIC)
    target_compile_definitions(ubrowser-qtwebkit PRIVATE UBROWSER_API_STATIC PUBLIC USE_STATIC_QTWEBKIT)
else ()
    add_plugin(ubrowser-qtwebkit SHARED)
    target_compile_definitions(ubrowser-qtwebkit PRIVATE UBROWSER_API_EXPORT)
    install(TARGETS ubrowser-qtwebkit DESTINATION ${APP_LIB_INSTALL_DIR})
endif ()

target_sources(ubrowser-qtwebkit PRIVATE
#   src/qtwebkit.json
    src/qtwebkit.qrc
    src/qtwebkitbrowser.cpp
    src/qtwebkitbrowser.h
#   src/qtwebkitbrowser.js
    src/qtwebkitbuilder.cpp
    src/qtwebkitbuilder.h
    src/qtwebkitnetwork.cpp
    src/qtwebkitnetwork.h
    src/qtwebkitwidget.cpp
    src/qtwebkitwidget.h
)

add_library(UBrowser::QWebKit ALIAS ubrowser-qtwebkit)
target_link_libraries(ubrowser-qtwebkit PUBLIC UBrowser::API)
target_link_libraries(ubrowser-qtwebkit PUBLIC ${QT}::Core ${QT}::Widgets ${QT}::WebKit ${QT}::WebKitWidgets)
set_target_properties(ubrowser-qtwebkit PROPERTIES AUTOMOC ON AUTORCC ON CLASS_NAME QtWebKitBuilder)
