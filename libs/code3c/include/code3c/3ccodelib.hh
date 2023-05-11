/*
 * 3C-CODE Library
 * Copyright (C) 2023 - Rin "madeshiro" Baudelet
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this
 * program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef HH_LIB_3CCODELIB
#define HH_LIB_3CCODELIB

/**
 * 3C-CODE Library -=- Colored and Compressed Circular CODE
 *
 * @file 3ccodelib.hh
 * @version 2023/05 1.0.0 (release-candidate)
 * @author Rin "madeshiro" Baudelet
 *
 * ----- << OS/SPECIFICATION >> -----
 * > support OS(arch):
 *   - GNU/Linux distribution (64bit)
 *   - Unix-Like distribution (64bit)
 *   - Windows 10 and latest  (64bit)
 *
 * > Project made with CMake (@see CMakeLists.txt)
 * > C Standard:    c11
 * > CXX Standard:  c++23
 */
#if defined(BELI_LIB_UTILSLIB)
#   define BLUTILSLIB_DIR __declspec(dllexport)
#else
#   define BLUTILSLIB_DIR __declspec(dllimport)
#endif

#define CODE3C_LIB  202305100L //>! 2023/05 1.0.0-rc
#define CODE3C_LIB_VERSION "2023/05 v1.0.0-rc (3c-code lib)"
#define CODE3C_LIB_RELEASE 1
#define CODE3C_LIB_MAJOR   0
#define CODE3C_LIB_MINOR   0

#ifdef CODE3C_DEBUG
#define cDebug(x...) printf("[debug] " x)
#else
#define cDebug(x...)
#endif

#ifdef CODE3C_UNIX
#include "signal.h"
#include "unistd.h"
#define C3CRC(fname) []() -> const char* { \
        if (access("/usr/share/code3c/" fname, F_OK) == 0) { \
            cDebug("ressources: /usr/share/code3c/" fname "\n"); \
            return "/usr/share/code3c/" fname;               \
        }  \
        if (access("/usr/local/share/code3c/" fname, F_OK) == 0) { \
            cDebug("ressources: /usr/local/share/code3c/" fname "\n");  \
            return "/usr/local/share/code3c/" fname;               \
        }  \
        else if (access("resources/" fname, F_OK) == 0)     \
            return "resources/" fname;                      \
        else  {                                             \
            fprintf(stderr, "unable to find resources '%s'\n", fname); \
            return fname;                                   \
        }                                              \
    }()
#endif // CODE3C_UNIX

#ifdef CODE3C_WIN32
// #include <io.h>
// #define F_OK 0
// #define access _access
#define C3CRC(fname) "resources/" fname
#endif // CODE3C_WIN32

// DEFINE CONSTANTS //

#define CODE3C_PIXEL_UNIT 3 // Amount of pixel for each data unit


#endif //HH_LIB_3CCODELIB
