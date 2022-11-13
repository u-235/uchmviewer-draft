#
# Shared library uchmviewerbrowser
# Alias: Browser::API
#===========================================================

cmake_minimum_required(VERSION 3.11)

add_library(uchmviewerbrowser SHARED)
add_library(Browser::API ALIAS uchmviewerbrowser)
target_include_directories(uchmviewerbrowser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_sources(uchmviewerbrowser PRIVATE
    include/browser/content-provider.hpp
    include/browser/settings.hpp
    include/browser/types.hpp
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
target_link_libraries(uchmviewerbrowser PUBLIC ${QT}::Core PRIVATE ${QT}::Widgets)
set_target_properties(uchmviewerbrowser PROPERTIES AUTOMOC ON)


# Install
#-----------------------------------------------------------
install(TARGETS uchmviewerbrowser DESTINATION ${APP_LIB_INSTALL_DIR})

