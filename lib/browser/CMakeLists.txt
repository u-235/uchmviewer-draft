#
# Shared library uchmviewerbrowser
#===========================================================

cmake_minimum_required(VERSION 3.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(uchmviewerbrowser SHARED)
target_link_libraries(uchmviewerbrowser Qt::Core)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(HEADER_DIR ${INCLUDE_DIR}/browser)

target_include_directories(uchmviewerbrowser PUBLIC ${INCLUDE_DIR})

target_sources(uchmviewerbrowser PUBLIC
#   ${HEADER_DIR}/abstract-browser-page.hpp
#   ${HEADER_DIR}/browser-builder.hpp
    ${HEADER_DIR}/browser-history.hpp
    ${HEADER_DIR}/browser-history-impl.hpp
#   ${HEADER_DIR}/browser-page.hpp
    ${HEADER_DIR}/browser-settings.hpp
    ${HEADER_DIR}/content-provider.hpp
)

qt_wrap_cpp(MOC_SOURCES
#   ${HEADER_DIR}/abstract-browser-page.hpp
#   ${HEADER_DIR}/browser-builder.hpp
    ${HEADER_DIR}/browser-history.hpp
    ${HEADER_DIR}/browser-history-impl.hpp
#   ${HEADER_DIR}/browser-page.hpp
)

target_sources(uchmviewerbrowser PRIVATE
    ${MOC_SOURCES}
    abstract-browser-page.cpp
    browser-history-impl.cpp
)

install(TARGETS uchmviewerbrowser DESTINATION ${APP_LIB_INSTALL_DIR})
