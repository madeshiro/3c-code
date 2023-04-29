cmake_minimum_required(VERSION 3.23)
project(code3c C CXX)
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

# Headers directories
include_directories("${CMAKE_HOME_DIRECTORY}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/code3c/include")

# Targets
set(3CCODE_TARGET 3ccode)
set(CODE3C_TARGET code3c)
set(HTFGEN_TARGET htfgen)

# Resources
set(RESOURCES
        resources/3ccode-wb.png
        resources/3ccode-wb2c.png
        resources/3ccode-wb6c.png
        )

file(COPY ${RESOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)

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

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/code3c)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/exec)