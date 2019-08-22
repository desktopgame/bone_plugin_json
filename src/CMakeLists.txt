cmake_minimum_required(VERSION 2.8)
project(json C)

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif()

if(LINUX)
  set(CMAKE_C_COMPILER gcc)
  set(CMAKE_CXX_COMPILER gcc++)
else()
  set(CMAKE_C_COMPILER clang)
  set(CMAKE_CXX_COMPILER clang++)
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
if(APPLE)
  set(CMAKE_C_FLAGS_DEBUG "-g -fsanitize=address -O0 -DDEBUG -DVMDEBUG -MMD -Wall")
elseif(LINUX)
  set(CMAKE_C_FLAGS_DEBUG "-d -O0 -std=c99 -DDEBUG -DVMDEBUG -MMD -Wall")
endif()
set(CMAKE_C_FLAGS_RELEASE "-g -O2 -MMD -w -DNDEBUG")
set(APP_EXECUTABLE "json" CACHE STRING "executable file name")
set(APP_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/json CACHE PATH "Path to install")
set(APP_IGNORE_COMPILE "CMakeCCompilerId.c" "feature_tests.c")
set(BONE_INCLUDE "/Users/koya/Work/BoneWork/bone/src" CACHE PATH "Path to bone header")
set(BONE_LIBRARY_DIRS "/Users/koya/Work/BoneWork/bone/lib" CACHE PATH "Path to bone library")
set(BONE_LIBRARIES "bone")
set(CJSON_INCLUDE "/usr/local/include" CACHE PATH "Path to cjson header")
set(CJSON_LIBRARY_DIRS "/usr/local/lib" CACHE PATH "Path to cjson library")
set(CJSON_LIBRARIES "cjson")
message(STATUS "BONE_INCLUDE=${BONE_INCLUDE}")
message(STATUS "BONE_LIBRARY_DIRS=${BONE_LIBRARY_DIRS}")
message(STATUS "BONE_LIBRARIES=${BONE_LIBRARIES}")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)

if(NOT APPLE)
    if(LINUX AND NOT IS_DIRECTORY ${GLIB_LIBRARY_DIRS})
    set(GLIB_LIBRARY_DIRS /usr/lib)
    endif()
endif()

add_custom_target(_uninstall rm -r ${APP_INSTALL_DIR})
add_custom_target(_clean rm -rf ./CMakeCache.txt && rm -r CMakeFiles)

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "**/*.c")
foreach(e IN LISTS SOURCES)
    foreach(pat IN LISTS APP_IGNORE_COMPILE)
        if(${e} MATCHES ${pat})
            message("remove compile target: ${e}")
            list(REMOVE_ITEM SOURCES ${e})
            break()
        endif()
    endforeach()
endforeach()

link_directories (
    ${GLIB_LIBRARY_DIRS}
    ${BONE_LIBRARY_DIRS}
    ${CJSON_LIBRARY_DIRS}
)
add_library(${APP_EXECUTABLE} SHARED ${SOURCES})
add_definitions (
    ${GLIB_CFLAGS_OTHER}
)
target_include_directories(
    ${APP_EXECUTABLE}
    PRIVATE
    ${GLIB_INCLUDE_DIRS}
    ${BONE_INCLUDE}
    ${CJSON_INCLUDE}
)
target_link_libraries(
    ${APP_EXECUTABLE}
    PRIVATE
    ${GLIB_LIBRARIES}
    ${BONE_LIBRARIES}
    ${CMAKE_DL_LIBS}
    ${CJSON_LIBRARIES}
    m
)
set_target_properties(json
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "../lib"
    LIBRARY_OUTPUT_DIRECTORY "../lib"
    RUNTIME_OUTPUT_DIRECTORY "../bin"
)
