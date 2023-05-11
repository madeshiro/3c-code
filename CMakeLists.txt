cmake_minimum_required(VERSION 3.23)
project(code3c LANGUAGES C CXX VERSION 1.0.0)
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

if(UNIX)
    add_definitions(-DCODE3C_UNIX)
elseif(WIN32)
    add_definitions(-DCODE3C_WIN32)
endif()


# Headers directories
include_directories("${CMAKE_HOME_DIRECTORY}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/code3c/include")

# Targets
set(3CCODE_TARGET 3ccode)
set(CODE3C_TARGET code3c)
set(HTFGEN_TARGET htfgen)

# Resources
set(RESOURCES
        ${CMAKE_HOME_DIRECTORY}/resources/3ccode-wb.png
        ${CMAKE_HOME_DIRECTORY}/resources/3ccode-wb2c.png
        ${CMAKE_HOME_DIRECTORY}/resources/3ccode-wb6c.png
        ${CMAKE_HOME_DIRECTORY}/resources/code3c-marker.png
        ${CMAKE_HOME_DIRECTORY}/resources/training_set.en_EN.txt
        ${CMAKE_HOME_DIRECTORY}/resources/training_set.fr_FR.txt
        ${CMAKE_HOME_DIRECTORY}/resources/en_EN.htf
        ${CMAKE_HOME_DIRECTORY}/resources/fr_FR.htf
        )

# file(COPY ${RESOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)

# Add Dependencies
find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS})

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

# Set-up CPack
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "code3c")
set(CPACK_VENDOR_NAME "Rin Baudelet")
set(CPACK_PACKAGE_INSTALL_DIRECTORY)
set(CPACK_PACKAGE_FILE_NAME code3c-1.0.0rc-debian-amd64)
set(CPACK_PACKAGE_CONTACT rin.baudelet@etu.uca.fr)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_SYSTEM_NAME Debian-AMD64)

# Setup CPack
include(CPack)

# Resources
foreach (rfile ${RESOURCES})
    install(FILES "${rfile}" DESTINATION share/${PROJECT_NAME})
endforeach()

# Add sub-projects
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/exec)

