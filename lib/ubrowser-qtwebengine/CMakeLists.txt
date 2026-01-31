
if (NOT DEFINED QT)
    find_package(Qt ${APP_QT_VERSION} NAMES Qt5 Qt6 REQUIRED)
    set(QT Qt${Qt_VERSION_MAJOR})
endif ()

find_package(${QT} REQUIRED Core Widgets WebEngineWidgets)

# see https://doc.qt.io/Qt-6/qt-add-plugin.html
if (USE_STATIC_QTWEBENGINE)
    add_plugin(ubrowser-qtwebengine STATIC CLASS_NAME QtWebEngineBuilder)
    target_compile_definitions(ubrowser-qtwebengine PRIVATE UBROWSER_API_STATIC PUBLIC USE_STATIC_QTWEBENGINE)
else ()
    add_plugin(ubrowser-qtwebengine SHARED CLASS_NAME QtWebEngineBuilder)
    target_compile_definitions(ubrowser-qtwebengine PRIVATE UBROWSER_API_EXPORT)
    install(TARGETS ubrowser-qtwebengine DESTINATION ${APP_LIB_INSTALL_DIR})
endif ()

target_sources(ubrowser-qtwebengine PRIVATE
#   src/qtwebengine.json
    src/qtwebengine.qrc
    src/qtwebenginebrowser.cpp
    src/qtwebenginebrowser.h
#   src/qtwebenginebrowser.js
    src/qtwebenginebuilder.cpp
    src/qtwebenginebuilder.h
    src/qtwebenginepage.h
    src/qtwebengineurlschemehandler.cpp
    src/qtwebengineurlschemehandler.h
    src/qtwebenginewidget.cpp
    src/qtwebenginewidget.h
)

add_library(UBrowser::QWebEngine ALIAS ubrowser-qtwebengine)
target_link_libraries(ubrowser-qtwebengine PUBLIC UBrowser::API)
target_link_libraries(ubrowser-qtwebengine PUBLIC ${QT}::Core ${QT}::Widgets ${QT}::WebEngineWidgets)
set_target_properties(ubrowser-qtwebengine PROPERTIES AUTOMOC ON AUTORCC ON)
