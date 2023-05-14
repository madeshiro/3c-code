# HTF Generator -- Command Line Interface (software)
# Copyright (C) 2023 - Rin "madeshiro" Baudelet
#
# This program is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation, either
# version 3 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with this
# program. If not, see <https://www.gnu.org/licenses/>.
set(TARGET ${HTFGEN_TARGET})

include_directories(.)
include_directories(./include)

set(SOURCES
        src/main.cc)

set(HEADERS
        )

message("> add target {'name': '${TARGET}', 'type': 'executable'}")
add_executable(${TARGET} ${SOURCES} ${HEADERS})
target_link_libraries(${TARGET} ${3CCODE_TARGET})

# Windows Resources
if (WIN32)
    target_sources(${TARGET} PRIVATE ${CMAKE_HOME_DIRECTORY}/resources/htfgen.rc)
endif ()

# Install
install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
install(FILES ${SOURCES} DESTINATION src/code3c/htfgen-cli)