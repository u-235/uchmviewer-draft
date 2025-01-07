#
# Library ubrowser
# Alias: UBrowser::API
#===========================================================

if (USE_STATIC_UBROWSER)
    add_library(ubrowser STATIC)
    target_compile_definitions(ubrowser PRIVATE UBROWSER_API_STATIC)
else ()
    add_library(ubrowser SHARED)
    target_compile_definitions(ubrowser PRIVATE UBROWSER_API_EXPORT)
endif ()

add_library(UBrowser::API ALIAS ubrowser)
set_target_properties(ubrowser PROPERTIES VERSION 0.8)
target_include_directories(ubrowser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_sources(ubrowser PRIVATE
    include/ubrowser/browser.hpp
    include/ubrowser/contentprovider.hpp
    include/ubrowser/export.hpp
    include/ubrowser/settings.hpp
)


# Qt
#-----------------------------------------------------------
if (NOT DEFINED QT)
    find_package(Qt ${APP_QT_VERSION} NAMES Qt5 Qt6 REQUIRED)
    set(QT Qt${Qt_VERSION_MAJOR})
endif ()

find_package(${QT} REQUIRED Core Widgets)
target_link_libraries(ubrowser PUBLIC ${QT}::Core PRIVATE ${QT}::Widgets)
set_target_properties(ubrowser PROPERTIES AUTOMOC ON)


# Install
#-----------------------------------------------------------
if (NOT USE_STATIC_UBROWSER)
    install(TARGETS ubrowser DESTINATION ${APP_LIB_INSTALL_DIR})
endif ()

