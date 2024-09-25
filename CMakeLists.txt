cmake_minimum_required(VERSION 3.5)

PROJECT(Analyzer C CXX)

option(USE_EXTERNAL_JSON "Use an external JSON library" OFF)
option(USE_EXTERNAL_EVIO "Use an external EVIO library" OFF)
option(USE_EXTERNAL_ET "Use an external ET library" OFF)

add_compile_options(-O2)

include(cmake/os.cmake)
include(FindThreads)

# required packages
find_package(ROOT 6.0 REQUIRED CONFIG)
include(${ROOT_USE_FILE})

#----------------------------------------------------------------------------
# Install in GNU-style directory layout
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/${MAIN_PROJECT_NAME_LC})

#----------------------------------------------------------------------------
add_subdirectory(third_party)
add_subdirectory(decoder)
add_subdirectory(ssp)
add_subdirectory(src)
add_subdirectory(tools)

