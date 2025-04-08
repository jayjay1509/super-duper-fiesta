file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)


add_library(api STATIC ${SRC_FILES} ${HEADER_FILES})
target_include_directories(api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)


target_link_libraries(api PRIVATE sfml-graphics sfml-window sfml-system)  # Link SFML to `api`
target_include_directories(api PUBLIC externals/SFML/include)  # Include SFML headers


target_include_directories(api PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../crackitos_physics/physics/include
)

file(GLOB_RECURSE DATA_FILES
        "data/*.json"
        "data/*.png"
        "data/*.jpg"
        "data/*.jpeg"
        "data/*.bmp"
        "data/*.hdr"
        "data/*.obj"
        "data/*.mtl"
        "data/*.gltf"
        "data/*.bin"
)

set(Data_OUTPUT_FILES "")  # Make sure this list is empty before appending

foreach(DATA ${DATA_FILES})
    get_filename_component(FILE_NAME ${DATA} NAME)
    get_filename_component(PATH_NAME ${DATA} DIRECTORY)
    get_filename_component(EXTENSION ${DATA} EXT)
    file(RELATIVE_PATH PATH_NAME "${CMAKE_CURRENT_SOURCE_DIR}" ${PATH_NAME})

    set(DATA_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${PATH_NAME}/${FILE_NAME}")

    # Ensure the directory exists
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${PATH_NAME}")

    add_custom_command(
            OUTPUT ${DATA_OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${DATA} ${DATA_OUTPUT}
            DEPENDS ${DATA}
    )

    list(APPEND Data_OUTPUT_FILES ${DATA_OUTPUT})
endforeach()

# Create a custom target to copy all data files
add_custom_target(copy_data ALL DEPENDS ${Data_OUTPUT_FILES})
add_dependencies(api copy_data)

if(MSVC)
    target_compile_options(api PRIVATE "/W3" "/WX" "/permissive-")
endif ()


#add_executable(game tmp/game.cc)
#target_link_libraries(game PRIVATE api)