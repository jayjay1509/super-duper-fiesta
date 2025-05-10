file(GLOB_RECURSE LB_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(LoadBalancing-cpp STATIC ${LB_SRC})

target_include_directories(LoadBalancing-cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_link_libraries(LoadBalancing-cpp PUBLIC Common-cpp)
