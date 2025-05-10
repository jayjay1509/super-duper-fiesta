file(GLOB_RECURSE COMMON_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(Common-cpp STATIC ${COMMON_SRC})

target_include_directories(Common-cpp PUBLIC ../Photon-cpp/inc)
