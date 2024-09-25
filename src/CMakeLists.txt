# build analyzer binary
set(exe analyze)
add_executable(${exe}
    analyze.cpp
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
    fdec
    ssp
    conf
)
target_link_libraries(${exe} PRIVATE  nlohmann_json::nlohmann_json)

install(TARGETS ${exe} DESTINATION ${CMAKE_INSTALL_BINDIR})

