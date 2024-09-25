cmake_minimum_required(VERSION 3.5)

PROJECT(simpleLib C)

#----------------------------------------------------------------------------
# Install in GNU-style directory layout
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/${MAIN_PROJECT_NAME_LC})

#----------------------------------------------------------------------------
# Sources and headers
set(src
    simpleLib.c
)

set(headers
    simpleLib.h
)

set(LIBNAME simple)

add_library(${LIBNAME} SHARED ${src})

target_include_directories(${LIBNAME}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)


install(TARGETS ${LIBNAME}
    EXPORT ${MAIN_PROJECT_NAME_LC}-exports
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

