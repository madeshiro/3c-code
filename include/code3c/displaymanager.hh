/* 3C-CODE Library
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
#ifndef HH_LIB_GWINDOW_3CCODE
#define HH_LIB_GWINDOW_3CCODE
#include "3ccodelib.hh"
#include <png.h>

namespace code3c
{
    class DisplayManager /* abstract */
    {
    public:
        virtual ~DisplayManager() noexcept = default;
        
        // ? savePNG(const char* name) const;
    };

#ifdef CODE3C_UNIX
    class X11Display final : public DisplayManager
    {
    public:
        X11Display();
        virtual ~X11Display() noexcept override;
    };
#endif //CODE3C_UNIX
#ifdef CODE3C_WIN32

#endif //CODE3C_WIN32
}

#endif //HH_LIB_GWINDOW_3CCODE
