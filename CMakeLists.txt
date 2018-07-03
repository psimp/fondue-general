cmake_minimum_required (VERSION 2.8)
cmake_policy(VERSION 2.8)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

link_directories(${CMAKE_SOURCE_DIR}/lib)

################### Variables. ####################
# Change if you want modify path or other values. #
###################################################

set(PROJECT_NAME FondueEngine)
# Output Variables
set(OUTPUT_DEBUG bin/)
set(OUTPUT_REL bin/)
# Folders files
set(CPP_DIR_1 interfaces)
set(CPP_DIR_2 src/graphics/3d/batches)
set(CPP_DIR_3 src/graphics/3d/renderers)
set(CPP_DIR_4 src/graphics/3d/state)
set(CPP_DIR_5 src/graphics/3d/rasterizables)
set(CPP_DIR_6 src/graphics/3d/scenes)
set(CPP_DIR_7 src/graphics/3d)
set(CPP_DIR_8 src/graphics/glwrappers)
set(CPP_DIR_9 src/utils)
set(CPP_DIR_10 src/physics/bodies)
set(CPP_DIR_11 src/graphics)
set(HEADER_DIR_1 src/physics/bodies)
set(HEADER_DIR_2 src/graphics/3d/renderers)
set(HEADER_DIR_3 src/graphics/3d/batches)
set(HEADER_DIR_4 src/graphics/3d/rasterizables)
set(HEADER_DIR_5 src/graphics/3d)
set(HEADER_DIR_6 src/graphics/3d/scenes)
set(HEADER_DIR_7 src/graphics/glwrappers)
set(HEADER_DIR_8 src/maths)
set(HEADER_DIR_9 src)
set(HEADER_DIR_10 src/utils)
set(HEADER_DIR_11 src/graphics/3d/state)
set(HEADER_DIR_12 src/graphics)
set(HEADER_DIR_13 src/physics)

############## CMake Project ################
#        The main options of project        #
#############################################

project(${PROJECT_NAME})

# Define Release by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "Build type not specified: Use Release by default.")
endif(NOT CMAKE_BUILD_TYPE)

# Definition of Macros
add_definitions(
   -DFT2_BUILD_LIBRARY
   -D_LIB
   -D_CRT_NONSTDC_NO_DEPRECATE
   -D_CRT_SECURE_NO_WARNINGS
)

############## Artefacts Output #################
# Defines outputs , depending Debug or Release. #
#################################################

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DEBUG}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DEBUG}")
  set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${OUTPUT_DEBUG}")
else()
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_REL}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_REL}")
  set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${OUTPUT_REL}")
endif()

################# Flags ################
# Defines Flags for Windows and Linux. #
########################################

if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W3 /MD /Od /EHsc")
   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W3 /GL /Od /Oi /Gy /EHsc")
endif(MSVC)
if(NOT MSVC)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
   if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
   endif()
endif(NOT MSVC)

################ Files ################
#   --   Add files to project.   --   #
#######################################

file(GLOB SRC_FILES
    ${CPP_DIR_1}/*.cpp
    ${CPP_DIR_2}/*.cpp
    ${CPP_DIR_3}/*.cpp
    ${CPP_DIR_4}/*.cpp
    ${CPP_DIR_5}/*.cpp
    ${CPP_DIR_6}/*.cpp
    ${CPP_DIR_7}/*.cpp
    ${CPP_DIR_8}/*.cpp
    ${CPP_DIR_9}/*.cpp
    ${CPP_DIR_10}/*.cpp
    ${CPP_DIR_11}/*.cpp
    ${HEADER_DIR_1}/*.h
    ${HEADER_DIR_2}/*.h
    ${HEADER_DIR_3}/*.h
    ${HEADER_DIR_4}/*.h
    ${HEADER_DIR_5}/*.h
    ${HEADER_DIR_6}/*.h
    ${HEADER_DIR_7}/*.h
    ${HEADER_DIR_8}/*.h
    ${HEADER_DIR_9}/*.h
    ${HEADER_DIR_10}/*.h
    ${HEADER_DIR_11}/*.h
    ${HEADER_DIR_12}/*.h
    ${HEADER_DIR_13}/*.h
    ${HEADER_DIR_14}/*.h
)

############ Dependencies #############

include(FindPackageHandleStandardArgs)

if (WIN32)
        # Find include files
        find_path(
                FREEIMAGE_INCLUDE_DIR
                NAMES FreeImage.h
                PATHS
                        $ENV{PROGRAMFILES}/include
                        ${FREEIMAGE_ROOT_DIR}/include
                DOC "The directory where FreeImage.h resides")

        # Find library files
        find_library(
                FREEIMAGE_LIBRARY
                NAMES FreeImage
                PATHS
                        $ENV{PROGRAMFILES}/lib
                        ${FREEIMAGE_ROOT_DIR}/lib)
else()
        # Find include files
        find_path(
                FREEIMAGE_INCLUDE_DIR
                NAMES FreeImage.h
                PATHS
                        /usr/include
                        /usr/local/include
                        /sw/include
                        /opt/local/include
                DOC "The directory where FreeImage.h resides")

        # Find library files
        find_library(
                FREEIMAGE_LIBRARY
                NAMES freeimage
                PATHS
                        /usr/lib64
                        /usr/lib
                        /usr/local/lib64
                        /usr/local/lib
                        /sw/lib
                        /opt/local/lib
                        ${FREEIMAGE_ROOT_DIR}/lib
                DOC "The FreeImage library")
endif()

# Handle REQUIRED argument, define *_FOUND variable
find_package_handle_standard_args(FreeImage DEFAULT_MSG FREEIMAGE_INCLUDE_DIR FREEIMAGE_LIBRARY)

if (FREEIMAGE_FOUND)
        set(FREEIMAGE_LIBRARIES ${FREEIMAGE_LIBRARY})
        set(FREEIMAGE_INCLUDE_DIRS ${FREEIMAGE_INCLUDE_DIR})
endif()

# Hide some variables
mark_as_advanced(FREEIMAGE_INCLUDE_DIR FREEIMAGE_LIBRARY)

include_directories($(FREEIMAGE_INCLUDE_DIRS) ${FREETYPE_INCLUDE_DIRS})

include_directories(Dependencies)
include_directories(Dependencies/emscripten/include)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(Dependencies/glfw-3.2.1)
add_subdirectory(Dependencies/assimp-4.0.1/)

if(WIN32)
  set(LIBS glfw3 opengl32 assimp)
elseif(UNIX AND NOT APPLE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  find_package(OpenGL REQUIRED)
  add_library(GLAD "src/glad.c")
  add_definitions(${OPENGL_DEFINITIONS})
  find_package(X11 REQUIRED)
  set(LIBS GLAD glfw X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread assimp)
elseif(APPLE)
  INCLUDE_DIRECTORIES(/System/Library/Frameworks)
  FIND_LIBRARY(COCOA_LIBRARY Cocoa)
  FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
  FIND_LIBRARY(IOKit_LIBRARY IOKit)
  FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
  MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
  SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
  SET(APPLE_LIBS ${APPLE_LIBS} ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY})
  set(LIBS ${LIBS} ${APPLE_LIBS})
else()
  set(LIBS )
endif(WIN32)

################ Flags ################

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")

# Add executable to build.
add_executable(${PROJECT_NAME}
   ${SRC_FILES}
)

configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

target_link_libraries(${PROJECT_NAME} ${LIBS} ${FREEIMAGE_LIBRARIES})

if(MSVC)
   target_link_libraries(${PROJECT_NAME} opengl32.lib glfw3.lib FreeImage.lib )
endif(MSVC)
