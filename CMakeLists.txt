cmake_minimum_required(VERSION 3.2)
project(super-duper-fiesta)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(externals/SFML)
add_subdirectory(game)
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/externals/imgui/" CACHE STRING "")
add_subdirectory(externals/imgui-sfml)
add_subdirectory(externals/Physics_923)



#serv
add_executable(server main/server.cc)
target_include_directories(server PRIVATE externals/SFML/include include)
target_link_libraries(server PRIVATE sfml-network)
target_link_libraries(server PRIVATE api)

#client
add_executable(client main/client.cc)
target_include_directories(client PRIVATE
        externals/SFML/include
        externals/imgui-sfml
        externals/imgui
        externals/Physics_923
        externals/LoadBalancing-cpp
        externals/Photon-cpp
)
target_link_libraries(client PRIVATE sfml-network sfml-graphics ImGui-SFML api crackitos_physics)

if(MSVC)
    target_compile_options(client PRIVATE "/W3" "/WX" "/permissive-")
endif ()


#copy resources in build
file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})