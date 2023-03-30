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
#ifndef HH_LIB_GWINDOW_3CCODE
#define HH_LIB_GWINDOW_3CCODE
#include "code3c/bitmat.hh"
#include <png.h>

namespace code3c
{
    struct MouseEvent
    {
        int mouseX, mouseY;
        int pmouseX, pmouseY;
        int mouseButton;
        int wheelCount;
    };
    
    class DisplayManager /* abstract */
    {
        const matb& m_data;
        int m_width, m_height;
    public:
        typedef bool (*key_listener)(char key, int keycode);
        typedef bool (*mouse_listener)(MouseEvent event);
        
        key_listener onKeyPressed;
        key_listener onKeyReleased;
        key_listener onKeyTyped;
        
        mouse_listener onMouseMoved;
        mouse_listener onMouseWheel;
        mouse_listener onMouseClicked;
        mouse_listener onMousePressed;
        mouse_listener onMouseReleased;
        
        DisplayManager(int width, int height, const matb& data);
        virtual ~DisplayManager() noexcept = default;
        
        virtual int height() const final;
        virtual int width() const final;
        virtual const matb& getData() const final;
        
        virtual void show(bool b = true) = 0;
        virtual void setTitle(const char*) = 0;
        
        virtual void setup() = 0;
        virtual void draw() = 0;
        virtual void exit() = 0;
        
        /**
         *
         * @param name
         */
        virtual void savePNG(const char* name) const = 0;
    };

#ifdef CODE3C_UNIX
    class X11Display final : public DisplayManager
    {
    public:
        X11Display(int width, int height, const matb& data);
        virtual ~X11Display() noexcept override;
        
        virtual void show(bool b = true) override;
        virtual void setTitle(const char*) override;
        
        virtual void setup() override;
        virtual void draw() override;
        virtual void exit() override;
        
        virtual void savePNG(const char* name) const override;
    };
#endif //CODE3C_UNIX
#ifdef CODE3C_WIN32
#error "Undefined class, Windows 1X not available"
#endif //CODE3C_WIN32
}

#endif //HH_LIB_GWINDOW_3CCODE
