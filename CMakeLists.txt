# 3C-CODE -- Colored and Compressed Circular CODE
# Copyright (C) 2023 - Rin "madeshiro" Baudelet
#
# This program is free software: you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation, either
# version 3 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with this
# program. If not, see <https://www.gnu.org/licenses/>.
cmake_minimum_required(VERSION 3.23)
project(3c-code C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

# Setup CTest
include(CTest)
enable_testing()

# Resolve error concerning 'MultiThreadDll' for MSVC
set(CMAKE_MSVC_RUNTIME_LIBRARY "")

message("--- GRAPHICAL PROTOCOL[CMAKE] CONFIGURATION ---")
message("> version: ${CMAKE_VERSION} (minimum required: ${CMAKE_MINIMUM_REQUIRED_VERSION})")
message("> os: {'name': '${CMAKE_SYSTEM_NAME}', 'arch': '${CMAKE_SYSTEM_PROCESSOR}'}")
message("> check OS/architecture compatibility...")
set(3CCODE_SYSTEM ${CMAKE_SYSTEM_NAME})
if ("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "(64)+")
    set(3CCODE_ARCH 64)
else()
    set(3CCODE_ARCH 32)
endif()
message("> detecting ${3CCODE_ARCH}bit...")
if (${3CCODE_ARCH} EQUAL 64)
    message("> architecture OK")
endif()
message("> detecting ${3CCODE_SYSTEM}...")
if (UNIX OR WIN32)
    message("> operating system OK")
else()
    message(FATAL_ERROR "> operating system FAILED")
endif()
if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    add_definitions(
            -DCODE3C_DEBUG
    )
    message("> debug mode OK")
else()
    add_definitions(
            -DCODE3C_RELEASE
    )
    message("> release mode OK")
endif()

add_definitions(
        -DCODE3C_${3CCODE_ARCH}
        -DCODE3C_${3CCODE_ARCH}
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

set(3CCODE_TARGET 3ccode)

set(SOURCES
        src/main.cc
        )

set(HEADERS
        )

set(LIBSOURCES
        src/code3c/3ccode.cc
        src/code3c/drawer.cc
        src/code3c/huffman.cc
        src/code3c/bitmat.cc
        src/code3c/in3ccode.cc
        src/code3c/pixelmap.cc
        src/code3c/pixelmap.c)

set(LIBHEADERS
        include/code3c/3ccode.hh
        include/code3c/drawer.hh
        include/code3c/huffman.hh
        include/code3c/3ccodelib.hh
        include/code3c/bitmat.hh
        include/code3c/in3ccode.hh
        include/code3c/pixelmap.hh)

if(UNIX)
    add_definitions(-DCODE3C_UNIX)
    set(LIBSOURCES ${LIBSOURCES}
            src/code3c/x11/X11Drawer.cc)
elseif(WIN32)
    add_definitions(-DCODE3C_WIN32)
    set(LIBSOURCES ${LIBSOURCES}
            src/code3c/win32/Win32Drawer.cc)
endif()

# Add Dependencies
find_package(PNG REQUIRED)
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(GTK REQUIRED gtk4)

set(CODE3C_DEPENDENCIES
        PNG::PNG
        -lm
        )
if(UNIX)
    find_package(X11 REQUIRED)
    set(CODE3C_DEPENDENCIES ${CODE3C_DEPENDENCIES}
            X11::X11)
endif()
if (WIN32)
    set(CODE3C_DEPENDENCIES ${CODE3C_DEPENDENCIES}
            -lMsimg32)
endif()

include_directories(${PNG_INCLUDE_DIRS})
# include_directories(${GTK_INCLUDE_DIRS})
# link_directories(${GTK_LIBRARY_DIR})

# Setup Targets
message("> add target {'name': '${3CCODE_TARGET}', 'type': 'library'}")
add_library(${3CCODE_TARGET} STATIC ${LIBSOURCES} ${LIBHEADERS})
add_library(${3CCODE_TARGET}.so SHARED ${LIBSOURCES} ${LIBHEADERS})
set_target_properties(${3CCODE_TARGET}.so PROPERTIES OUTPUT_NAME ${3CCODE_TARGET})

target_link_libraries(${3CCODE_TARGET} ${CODE3C_DEPENDENCIES})
target_link_libraries(${3CCODE_TARGET}.so ${CODE3C_DEPENDENCIES})

if (WIN32)
    target_sources(${3CCODE_TARGET} PRIVATE resources/winres.rc)
    target_sources(${3CCODE_TARGET}.so PRIVATE resources/winres.rc)
endif ()

message("> add target {'name': '${3CCODE_TARGET}_gen', 'type': 'executable'}")
add_executable(${3CCODE_TARGET}_gen ${SOURCES} ${HEADERS})
target_link_libraries(${3CCODE_TARGET}_gen ${3CCODE_TARGET})


# Instanciate Test Applications
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/test")
if(${CMAKE_ENV_GUI} MATCHES False)
    add_definitions(
        -DCMAKE_CTEST_ENV_NOGUI
    )
endif ()
create_test_sourcelist(ctest_testmodule ${3CCODE_TARGET}_testmodule.cxx
        test/3c_generation.cxx
        test/3c_drawer.cxx
        test/mat_operation.cxx
        test/png_in_out.cxx
)

add_executable(${3CCODE_TARGET}_testmodule ${ctest_testmodule})
target_link_libraries(${3CCODE_TARGET}_testmodule ${3CCODE_TARGET})

set(ctest_runtest
        ${ctest_testmodule}
        )
list(REMOVE_ITEM ctest_runtest ${3CCODE_TARGET}_testmodule.cxx)

foreach(test ${ctest_runtest})
    get_filename_component(test_name ${test} NAME_WE)
    add_test(NAME ${3CCODE_TARGET}.${test_name} COMMAND ${3CCODE_TARGET}_testmodule test/${test_name})
    set_property(TEST ${3CCODE_TARGET}.${test_name} PROPERTY LABELS ${3CCODE_TARGET})
    message("> add test ${3CCODE_TARGET}.${test_name}")
endforeach()