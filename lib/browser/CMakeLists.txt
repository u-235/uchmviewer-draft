#
# Shared library uchmviewerbrowser
#===========================================================

cmake_minimum_required(VERSION 3.11)
project(BrowserAPI VERSION 0.3)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(HEADER_DIR ${INCLUDE_DIR}/browser)

add_library(uchmviewerbrowser SHARED)
target_include_directories(uchmviewerbrowser PUBLIC ${INCLUDE_DIR})
target_compile_definitions(uchmviewerbrowser PRIVATE BROWSER_API_EXPORT)
set_target_properties(uchmviewerbrowser PROPERTIES VERSION ${PROJECT_VERSION})

target_sources(uchmviewerbrowser
    PUBLIC
    ${HEADER_DIR}/abstract-browser-controller.hpp
    ${HEADER_DIR}/browser-builder.hpp
    ${HEADER_DIR}/browser-controller.hpp
    ${HEADER_DIR}/browser-history.hpp
    ${HEADER_DIR}/browser-history-impl.hpp
    ${HEADER_DIR}/browser-settings.hpp
    ${HEADER_DIR}//browser-types.hpp
    ${HEADER_DIR}/content-provider.hpp
    ${HEADER_DIR}/export.hpp
    PRIVATE
    abstract-browser-controller.cpp
    browser-controller.cpp
    browser-history-impl.cpp
)

set(MOC_HEADERS
    ${HEADER_DIR}/abstract-browser-controller.hpp
    ${HEADER_DIR}/browser-controller.hpp
    ${HEADER_DIR}/browser-history.hpp
    ${HEADER_DIR}/browser-history-impl.hpp
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

if (QT_VERSION VERSION_LESS 5.15)
    macro(qt_wrap_cpp)
        qt5_wrap_cpp(${ARGV})
    endmacro()
endif ()

target_link_libraries(uchmviewerbrowser PUBLIC ${QT}::Core PRIVATE ${QT}::Widgets)
qt_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
target_sources(uchmviewerbrowser PRIVATE ${MOC_SOURCES})

# Install
#-----------------------------------------------------------
install(TARGETS uchmviewerbrowser RUNTIME DESTINATION ${APP_LIB_INSTALL_DIR})
add_library(Browser::API ALIAS uchmviewerbrowser)
