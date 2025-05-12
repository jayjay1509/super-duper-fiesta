cmake_minimum_required(VERSION 3.2)
project(sdf)

set(CMAKE_CXX_STANDARD 20)

# Forcer le bon runtime (/MD en Release, /MDd en Debug)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# ─── Sous-dossiers externes ────────────────────────────────────────────────
add_subdirectory(externals/SFML)
add_subdirectory(game)
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/externals/imgui/" CACHE STRING "")
add_subdirectory(externals/imgui-sfml)
add_subdirectory(externals/Physics_923)

# ─── Bibliothèque Photon ───────────────────────────────────────────────────
file(GLOB_RECURSE PHOTON_SRC_FILES
        externals/LoadBalancing-cpp/inc/*.h
        externals/LoadBalancing-cpp/src/*.cpp
)
add_library(photon ${PHOTON_SRC_FILES})
set_target_properties(photon PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(photon PUBLIC externals/)

# Lien conditionnel Debug / Release
target_link_libraries(photon PUBLIC
        debug ${CMAKE_CURRENT_SOURCE_DIR}/externals/Common-cpp/lib/Common-cpp_vc17_debug_windows_md_x64.lib
        debug ${CMAKE_CURRENT_SOURCE_DIR}/externals/Photon-cpp/lib/Photon-cpp_vc17_debug_windows_md_x64.lib
        optimized ${CMAKE_CURRENT_SOURCE_DIR}/externals/Common-cpp/lib/Common-cpp_vc17_release_windows_md_x64.lib
        optimized ${CMAKE_CURRENT_SOURCE_DIR}/externals/Photon-cpp/lib/Photon-cpp_vc17_release_windows_md_x64.lib
)

# ─── Serveur ───────────────────────────────────────────────────────────────
add_executable(server main/server.cc)
target_include_directories(server PRIVATE externals/SFML/include include)
target_link_libraries(server PRIVATE sfml-network api)

# ─── Client ────────────────────────────────────────────────────────────────
set(NET_DIR externals)

add_executable(client main/client.cc)
target_include_directories(client PUBLIC ${NET_DIR}/include)
link_directories(${NET_DIR}/lib/windows/)

target_include_directories(client PRIVATE
        externals/SFML/include
        externals/imgui-sfml
        externals/imgui
        externals/Physics_923
)
target_link_libraries(client PRIVATE
        sfml-network
        sfml-graphics
        ImGui-SFML
        api
        crackitos_physics
        photon
)

if(MSVC)
    target_compile_options(client PRIVATE "/W3" "/WX" "/permissive-")
endif ()

get_target_property(client_includes client INCLUDE_DIRECTORIES)
message(STATUS "Client includes after photon: ${client_includes}")

# ─── Network Test ──────────────────────────────────────────────────────────
add_executable(network_test main/network_test.cc)

target_include_directories(network_test PRIVATE
        externals/SFML/include
        externals/imgui-sfml
        externals/imgui
        externals/Physics_923
        ${NET_DIR}/include
)
target_link_libraries(network_test PRIVATE
        sfml-network
        sfml-graphics
        ImGui-SFML
        api
        crackitos_physics
        photon
)

if(MSVC)
    target_compile_options(network_test PRIVATE "/W3" "/WX" "/permissive-")
endif ()

# ─── Copie des fichiers data ───────────────────────────────────────────────
file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})

# ─── Infos debug ───────────────────────────────────────────────────────────
get_target_property(client_includes client INCLUDE_DIRECTORIES)
message(STATUS "Client includes: ${client_includes}")
