# third party librarie

# add_subdirectory(simple)
add_subdirectory(conf)
add_subdirectory(evchannel)

if(USE_EXTERNAL_JSON)
    find_package(nlohmann_json 3.2.0 REQUIRED)
else()
    set(JSON_BuildTests OFF CACHE INTERNAL "")
    add_subdirectory(nlohmann_json)
endif()

