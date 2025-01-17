cmake_minimum_required(VERSION 3.12...3.30)
project(economia CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 5.0)

FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    # URL https://github.com/raysan5/raylib/archive/55e83468c9bb515a5f2725751e5f187a7b9b5afd.zip # 9/26/2024
    URL https://github.com/raysan5/raylib/archive/ae50bfa2cc569c0f8d5bc4315d39db64005b1b08.zip # v 5.0
    FIND_PACKAGE_ARGS
)

FetchContent_Declare(
    raygui
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    GIT_REPOSITORY https://github.com/raysan5/raygui.git
    SOURCE_SUBDIR projects/CMake
    FIND_PACKAGE_ARGS
)

FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(raylib raygui unity)

# Our Project
add_executable(${PROJECT_NAME})
add_subdirectory(src)
add_subdirectory(test)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)

if ("${PLATFORM}" STREQUAL "Web")
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/../resources
    )
    #DEPENDS ${PROJECT_NAME}
else()
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
    )
    #DEPENDS ${PROJECT_NAME}
endif()

#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib raygui)

# Web Configurations
if ("${PLATFORM}" STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_link_options(${PROJECT_NAME} PUBLIC -sUSE_GLFW=3 PUBLIC --preload-file resources)
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
