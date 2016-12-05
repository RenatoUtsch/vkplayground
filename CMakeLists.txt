##
 # vkplayground - Playing around with Vulkan
 #
 # Copyright 2016 Renato Utsch
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #     http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
 ##

# By default generate a release build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: Debug, Release."
        FORCE)
endif()

# Our project
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(vkplayground CXX)

# Get rid of MinSizeRel and RelWithDebInfo
if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING
        "Available configuration types" FORCE)
endif()

# Global macros
set(GLOBAL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(GLOBAL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(GLOBAL_DEFINITIONS "")
set(GLOBAL_INCLUDE_DIRS "")
set(GLOBAL_LIBRARIES "")
set(GLOBAL_COMPILER_FLAGS "")
set(GLOBAL_TARGET "")
set(GLOBAL_DEBUG OFF)

# Always output to bin
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${GLOBAL_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GLOBAL_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GLOBAL_BINARY_DIR}/bin)
foreach(CONF_TYPE ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER "${CONF_TYPE}" CONF_TYPE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF_TYPE} ${GLOBAL_BINARY_DIR}/bin)
endforeach()

# Vulkan
find_package(Vulkan REQUIRED)
set(GLOBAL_INCLUDE_DIRS ${GLOBAL_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})
set(GLOBAL_LIBRARIES ${GLOBAL_LIBRARIES} ${Vulkan_LIBRARIES})

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${GLOBAL_SOURCE_DIR}/3rdparty/glfw)
set(GLOBAL_LIBRARIES ${GLOBAL_LIBRARIES} glfw)

# GLM
set(GLOBAL_INCLUDE_DIRS ${GLOBAL_INCLUDE_DIRS} "${GLOBAL_SOURCE_DIR}/3rdparty/glm")

# stb collection
set(GLOBAL_INCLUDE_DIRS ${GLOBAL_INCLUDE_DIRS} "${GLOBAL_SOURCE_DIR}/3rdparty/stb")

# tinyobjloader
set(GLOBAL_INCLUDE_DIRS ${GLOBAL_INCLUDE_DIRS} "${GLOBAL_SOURCE_DIR}/3rdparty/tinyobjloader")

# Debug flag
if("${CMAKE_BUILD_TYPE}" MATCHES "(Debug|RelWithDebInfo)")
    set(GLOBAL_DEBUG ON)
    message(STATUS "Adding DEBUG macro")
    set(GLOBAL_DEFINITIONS "${GLOBAL_DEFINITIONS} -DDEBUG")

    string(LENGTH "${GLOBAL_SOURCE_DIR}/" DEBUG_PROJECT_PATH_SIZE)
    set(GLOBAL_DEFINITIONS "${GLOBAL_DEFINITIONS} -DDEBUG_PROJECT_PATH_SIZE=${DEBUG_PROJECT_PATH_SIZE}")
endif()

# Compiler Flags
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Clang/GCC Compiler Flags
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "(Clang|GNU)")
    add_compile_options(-Wall -Wextra -Werror -pedantic)

    if((NOT GLOBAL_DEBUG) AND WIN32)
        add_compile_options(-mwindows)
    endif()
endif()

# MSVC Compiler Flags
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    if(CMAKE_CXX_FLAGS MATCHES "/W([0-4]|all)")
        STRING(REGEX REPLACE "/W([0-4]|all)" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()
    add_compile_options(/W4 /WX /Za /wd4100)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows /entry:mainCRTStartup /MANIFEST:NO")
endif()

# OS-dependent target
if(NOT GLOBAL_DEBUG)
    if(WIN32)
        set(GLOBAL_TARGET WIN32)
    elseif(APPLE)
        set(GLOBAL_TARGET MACOSX_BUNDLE)
    endif()
endif()

# res2cpp tool compilation
add_executable(res2cpp "${GLOBAL_SOURCE_DIR}/tools/res2cpp.cpp")
set_target_properties(res2cpp PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${GLOBAL_BINARY_DIR}/tools")

# Shader Sources
set(SHADER_SOURCE_DIR "${GLOBAL_SOURCE_DIR}/source/shaders")
set(SHADER_SOURCE_NAMES "shader.vert" "shader.frag")

# Shader generated source
set(SHADER_GENERATED_DIR "${GLOBAL_BINARY_DIR}/generated/shaders")
set(SHADER_GENERATED_SOURCE "${SHADER_GENERATED_DIR}/shaders_generated.cpp")
file(MAKE_DIRECTORY ${SHADER_GENERATED_DIR})

# Custom commands to compile shaders.
set(SHADER_COMPILED_FILES "")
foreach(SHADER_SOURCE ${SHADER_SOURCE_NAMES})
    set(SHADER_COMPILED_FILES ${SHADER_COMPILED_FILES} "${SHADER_GENERATED_DIR}/${SHADER_SOURCE}.spv")
    add_custom_command(OUTPUT "${SHADER_GENERATED_DIR}/${SHADER_SOURCE}.spv"
        COMMAND glslangValidator -V -o "${SHADER_GENERATED_DIR}/${SHADER_SOURCE}.spv"
            "${SHADER_SOURCE_DIR}/${SHADER_SOURCE}"
        DEPENDS "${SHADER_SOURCE_DIR}/${SHADER_SOURCE}"
        COMMENT "Compiling GLSL shader ${SHADER_SOURCE_DIR}/${SHADER_SOURCE}")
endforeach()

# Build generated file with shader contents.
add_custom_command(OUTPUT ${SHADER_GENERATED_SOURCE}
    COMMAND res2cpp ${SHADER_COMPILED_FILES} ${SHADER_GENERATED_SOURCE}
    DEPENDS ${SHADER_COMPILED_FILES} res2cpp
    COMMENT "Generating shader resource")
add_custom_target(compileShaders DEPENDS ${SHADER_GENERATED_SOURCE}
    COMMENT "Checking if shader regeneration is needed")

# Source Files
set(GLOBAL_SOURCE_FILES "${GLOBAL_SOURCE_DIR}/source/main.cpp"
                        "${GLOBAL_SOURCE_DIR}/source/shaders/shaders.cpp"
                        ${SHADER_GENERATED_SOURCE})

# Compile
include_directories(${GLOBAL_INCLUDE_DIRS})
add_definitions(${GLOBAL_DEFINITIONS})
add_executable(vkplayground ${GLOBAL_TARGET} ${GLOBAL_SOURCE_FILES})
target_link_libraries(vkplayground ${GLOBAL_LIBRARIES})

# Copy resource folder
add_custom_command(TARGET vkplayground POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${GLOBAL_SOURCE_DIR}/resource ${GLOBAL_BINARY_DIR}/resource
    COMMENT "Copying the resource folder")
