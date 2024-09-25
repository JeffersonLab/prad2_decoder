cmake_minimum_required(VERSION 3.5)

PROJECT(etLib C)

#----------------------------------------------------------------------------
# Install in GNU-style directory layout
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/${MAIN_PROJECT_NAME_LC})

#----------------------------------------------------------------------------
# Sources and headers
set(src
    et_bridge.c
    etCommonNetwork.c
    et_data.c
    et_event.c
    et_init.c
    et_local.c
    et_network.c
    et_noshare.c
    et_remote.c
    et_server.c
    et_station.c
    et_system.c
    et_attachment.c
    et_common.c
    # et_jni.c
    et_mem.c
    et_openconfig.c
    et_readwrite.c
    # et_remoteclient.c
    et_statconfig.c
    et_sysconfig.c
)

set(headers
    et_data.h
    et_network.h
    et_private.h
    et.h
    etCommonNetwork.h
    # org_jlab_coda_et_EtJniAccess.h
)

set(LIBNAME et)

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

