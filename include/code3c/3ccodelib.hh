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
 * @version 2023/03 0.0.1 (alpha)
 * @author Rin "madeshiro" Baudelet
 *
 * ----- << OS/SPECIFICATION >> -----
 * > support OS(arch):
 *   - GNU/Linux distribution (64bit)
 *   - Unix-Like distribution (64bit)
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

#define CODE3C_LIB  202303001L //>! 2023/03 0.0.1
#define CODE3C_LIB_VERSION "2023/03 v0.0.1 (3c-code lib)"
#define CODE3C_LIB_RELEASE 0
#define CODE3C_LIB_MAJOR   0
#define CODE3C_LIB_MINOR   1

// DEFINE CONSTANTS //

#define CODE3C_PIXEL_UNIT 4 // Amount of pixel for each data unit

#endif //HH_LIB_3CCODELIB
