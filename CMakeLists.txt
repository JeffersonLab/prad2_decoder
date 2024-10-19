cmake_minimum_required(VERSION 3.5)

PROJECT(PRad2Decoder C CXX)

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
add_subdirectory(evchannel)
add_subdirectory(decoder)


# build executable
set(exe prad2_decoder)
add_executable(${exe}
    src/prad2_decoder.cpp
)

target_include_directories(${exe}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    ${ROOT_INCLUDE_DIRS}
)

target_link_libraries(${exe}
LINK_PUBLIC
    ${ROOT_LIBRARIES}
    evc
    conf
    fdec
)
target_link_libraries(${exe} PRIVATE  nlohmann_json::nlohmann_json)

install(TARGETS ${exe} DESTINATION ${CMAKE_INSTALL_BINDIR})

