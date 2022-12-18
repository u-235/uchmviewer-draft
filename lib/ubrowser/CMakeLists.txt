#
# Shared library ubrowser
# Alias: UBrowser::API
#===========================================================

cmake_minimum_required(VERSION 3.11)

add_library(ubrowser SHARED)
add_library(UBrowser::API ALIAS ubrowser)
target_include_directories(ubrowser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(ubrowser PRIVATE UBROWSER_API_EXPORT)

target_sources(ubrowser PRIVATE
    include/ubrowser/browser.hpp
    include/ubrowser/contentprovider.hpp
    include/ubrowser/export.hpp
    include/ubrowser/history.hpp
    include/ubrowser/settings.hpp
    include/ubrowser/types.hpp
)
 
target_sources(ubrowser PRIVATE
    src/browser.cpp
    src/contentprovider.cpp
)


# Qt
#-----------------------------------------------------------
if (NOT DEFINED QT)
    if (NOT DEFINED APP_QT_VERSION)
        set(APP_QT_VERSION 5.0 CACHE STRING "Required version of Qt")
    endif ()

    find_package(QT ${APP_QT_VERSION} NAMES Qt5 Qt6 REQUIRED)
    set(QT Qt${QT_VERSION_MAJOR})
endif ()

find_package(${QT} REQUIRED Core Widgets)
target_link_libraries(ubrowser PUBLIC ${QT}::Core PRIVATE ${QT}::Widgets)
set_target_properties(ubrowser PROPERTIES AUTOMOC ON)


# Install
#-----------------------------------------------------------
install(TARGETS ubrowser DESTINATION ${APP_LIB_INSTALL_DIR})

