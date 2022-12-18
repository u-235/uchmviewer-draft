#
# Shared library uchmviewerbrowser
#===========================================================

cmake_minimum_required(VERSION 3.11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(HEADER_DIR ${INCLUDE_DIR}/browser)

add_library(uchmviewerbrowser SHARED)
target_link_libraries(uchmviewerbrowser Qt::Core)
target_include_directories(uchmviewerbrowser PUBLIC ${INCLUDE_DIR})
target_compile_definitions(uchmviewerbrowser PRIVATE BROWSER_API_EXPORT)
set_target_properties(uchmviewerbrowser PROPERTIES VERSION 0.3)

target_sources(uchmviewerbrowser PUBLIC
#   ${HEADER_DIR}/browser-builder.hpp
    ${HEADER_DIR}/browser-controller.hpp
    ${HEADER_DIR}/browser-history.hpp
    ${HEADER_DIR}/browser-history-impl.hpp
    ${HEADER_DIR}/browser-settings.hpp
    ${HEADER_DIR}//browser-types.hpp
    ${HEADER_DIR}/content-provider.hpp
    ${HEADER_DIR}/export.hpp
)

qt_wrap_cpp(MOC_SOURCES
#   ${HEADER_DIR}/browser-builder.hpp
    ${HEADER_DIR}/browser-controller.hpp
    ${HEADER_DIR}/browser-history.hpp
    ${HEADER_DIR}/browser-history-impl.hpp
)

target_sources(uchmviewerbrowser PRIVATE
    ${MOC_SOURCES}
    browser-controller.cpp
    browser-history-impl.cpp
)

install(TARGETS uchmviewerbrowser RUNTIME DESTINATION ${APP_LIB_INSTALL_DIR})
