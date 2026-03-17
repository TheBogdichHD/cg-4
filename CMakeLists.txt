cmake_minimum_required(VERSION 3.16)

project(OpenGLTasks LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW3 REQUIRED IMPORTED_TARGET glfw3)
find_package(assimp REQUIRED)

find_package(glm CONFIG QUIET)
if (NOT TARGET glm::glm)
    find_path(GLM_INCLUDE_DIR glm/glm.hpp REQUIRED)
endif()

add_executable(opengl_tasks
    src/main.cpp
    src/glad.c
    src/stb_image_impl.cpp
)

target_include_directories(opengl_tasks PRIVATE include)

target_link_libraries(opengl_tasks PRIVATE PkgConfig::GLFW3 OpenGL::GL assimp::assimp ${CMAKE_DL_LIBS})

if (TARGET glm::glm)
    target_link_libraries(opengl_tasks PRIVATE glm::glm)
else()
    target_include_directories(opengl_tasks PRIVATE ${GLM_INCLUDE_DIR})
endif()

add_custom_command(
    TARGET opengl_tasks POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/shaders
            $<TARGET_FILE_DIR:opengl_tasks>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/models
            $<TARGET_FILE_DIR:opengl_tasks>/models
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/textures
        $<TARGET_FILE_DIR:opengl_tasks>/textures
)