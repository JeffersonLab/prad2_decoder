cmake_minimum_required(VERSION 3.5)

PROJECT(EvChannel CXX)

if(USE_EXTERNAL_EVIO)
    find_package(evio REQUIRED)
else()
    add_subdirectory(evio-5.2)
endif()

if(USE_EXTERNAL_ET)
    find_package(coda_et REQUIRED)
else()
    add_subdirectory(et-16.2)
endif()

#----------------------------------------------------------------------------
# Install in GNU-style directory layout
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/${MAIN_PROJECT_NAME_LC})

#----------------------------------------------------------------------------
# Sources and headers
set(src
    EvChannel.cpp
    EtChannel.cpp
    CompositeData.cpp
)

set(headers
    EvStruct.h
    EvChannel.h
    EtChannel.h
    EtConfigWrapper.h
    CompositeData.h
)

set(LIBNAME evc)
add_library(${LIBNAME} SHARED ${src})

target_include_directories(${LIBNAME}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

target_link_libraries(${LIBNAME}
PUBLIC
    evio
    et
    ${CMAKE_DL_LIBS}
)

install(TARGETS ${LIBNAME}
    EXPORT ${MAIN_PROJECT_NAME_LC}-exports
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})


