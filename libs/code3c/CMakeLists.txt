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
set(TARGET ${3CCODE_TARGET})

set(SOURCES
        src/3ccode.cc
        src/drawer.cc
        src/huffman.cc
        src/bitmat.cc
        src/in3ccode.cc
        src/pixelmap.cc
        src/pixelmap.c
        src/hamming743.cc)

set(HEADERS
        include/code3c/3ccode.hh
        include/code3c/drawer.hh
        include/code3c/huffman.hh
        include/code3c/3ccodelib.hh
        include/code3c/bitmat.hh
        include/code3c/in3ccode.hh
        include/code3c/pixelmap.hh
        include/code3c/hamming743.hh)

if(UNIX)
    add_definitions(-DCODE3C_UNIX)
    set(SOURCES ${SOURCES}
            src/x11/X11Drawer.cc)
elseif(WIN32)
    add_definitions(-DCODE3C_WIN32)
    set(SOURCES ${SOURCES}
            src/win32/Win32Drawer.cc)
endif()

file(COPY ${RESOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)

# Setup Targets
message("> add target {'name': '${TARGET}', 'type': 'library'}")
add_library(${TARGET} STATIC ${SOURCES} ${HEADERS})
add_library(${TARGET}.so SHARED ${SOURCES} ${HEADERS})
set_target_properties(${TARGET}.so PROPERTIES OUTPUT_NAME ${TARGET})

target_link_libraries(${TARGET} ${CODE3C_DEPENDENCIES})
target_link_libraries(${TARGET}.so ${CODE3C_DEPENDENCIES})

# Windows Resources
if (WIN32)
    target_sources(${TARGET} PRIVATE ${CMAKE_HOME_DIRECTORY}/resources/lib3ccode.rc)
    target_sources(${TARGET}.so PRIVATE ${CMAKE_HOME_DIRECTORY}/resources/lib3ccode.rc)
endif ()

# Instanciate Test Applications
include_directories("test")
if(${CMAKE_ENV_GUI} MATCHES False)
    add_definitions(
        -DCMAKE_CTEST_ENV_NOGUI
    )
endif ()

create_test_sourcelist(ctest_testmodule ${TARGET}_testmodule.cxx
        test/3c_generation.cxx
        test/3c_drawer.cxx
        test/mat_operation.cxx
        test/png_in_out.cxx
        test/hamming.cxx
)

add_executable(${TARGET}_testmodule ${ctest_testmodule})
target_link_libraries(${TARGET}_testmodule ${TARGET})

set(ctest_runtest
        ${ctest_testmodule}
        )
list(REMOVE_ITEM ctest_runtest ${TARGET}_testmodule.cxx)

foreach(test ${ctest_runtest})
    get_filename_component(test_name ${test} NAME_WE)
    add_test(NAME ${TARGET}.${test_name} COMMAND ${TARGET}_testmodule test/${test_name})
    set_property(TEST ${TARGET}.${test_name} PROPERTY LABELS ${TARGET})
    message("> add test ${TARGET}.${test_name}")
endforeach()