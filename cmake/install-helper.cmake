# The location of the installation directories. Only relative paths may be used.
#
#       Common:
#
# APP_BIN_INSTALL_DIR    folder to install executable.
# APP_LIB_INSTALL_DIR    folder to install shared libraries.
# APP_PLUGIN_INSTALL_DIR folder to install plugins.
# APP_ICONS_INSTALL_DIR  folder to install application icons.
# APP_LOCALE_INSTALL_DIR folder to install translations.
#
#       System-specific:
#
# APP_DEF_INSTALL_DIR    installation folder for a desktop entry in GNU/Linux.
# APP_ROOT_INSTALL_DIR   macOS bundle folder for Info.plist.

include(GNUInstallDirs)

set(APP_BIN_INSTALL_DIR    ${CMAKE_INSTALL_BINDIR})
set(APP_DEF_INSTALL_DIR    ${CMAKE_INSTALL_DATADIR}/applications)
set(APP_ICONS_INSTALL_DIR  ${CMAKE_INSTALL_DATADIR}/icons)
set(APP_LOCALE_INSTALL_DIR ${CMAKE_INSTALL_LOCALEDIR})
set(APP_LIB_INSTALL_DIR    ${CMAKE_INSTALL_LIBDIR})
set(APP_PLUGIN_INSTALL_DIR ${APP_LIB_INSTALL_DIR}/${PROJECT_NAME}/plugins)

# From https://cmake.org/cmake/help/latest/command/install.html
#
# As absolute paths are not supported by cpack installer generators, it is
# preferable to use relative paths throughout. In particular, there is no need
# to make paths absolute by prepending CMAKE_INSTALL_PREFIX; this prefix is
# used by default if the DESTINATION is a relative path.
if (${CMAKE_SYSTEM_NAME}  MATCHES "Windows")
    set(APP_BIN_INSTALL_DIR    bin)
    set(APP_LOCALE_INSTALL_DIR locale)
    set(APP_ICONS_INSTALL_DIR  icons)
    set(APP_LIB_INSTALL_DIR    ${APP_BIN_INSTALL_DIR})
    set(APP_PLUGIN_INSTALL_DIR plugins)
elseif (${CMAKE_SYSTEM_NAME}  MATCHES "Darwin" AND USE_MACOS_BUNDLE)
    set(APP_ROOT_INSTALL_DIR   ${PROJECT_NAME}.app/Contents)
    set(APP_BIN_INSTALL_DIR    ${APP_ROOT_INSTALL_DIR}/MacOS)
    set(APP_DATA_ROOT_DIR      ${APP_ROOT_INSTALL_DIR}/Resources)
    set(APP_LOCALE_INSTALL_DIR ${APP_DATA_ROOT_DIR}/locale)
    set(APP_ICONS_INSTALL_DIR  ${APP_DATA_ROOT_DIR})
    set(APP_LIB_INSTALL_DIR    ${APP_ROOT_INSTALL_DIR}/Frameworks)
    set(APP_PLUGIN_INSTALL_DIR ${APP_ROOT_INSTALL_DIR}/PlugIns)

    # maybe use CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT?
    if(${CMAKE_INSTALL_PREFIX} MATCHES "/usr/.*")
        set(CMAKE_INSTALL_PREFIX "~/Applications" CACHE PATH "" FORCE)
    endif()
endif ()
