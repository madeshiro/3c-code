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
#ifndef HH_LIB_GDRAWER_3CCODE
#define HH_LIB_GDRAWER_3CCODE
#include "code3c/bitmat.hh"
#include <png.h>

#ifdef CODE3C_UNIX
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

namespace code3c
{
    unsigned long rgb(const char* hex);
    unsigned long rgb(int r, int g, int b);
    
    class Drawer;
    struct MouseEvent
    {
        int mouseX, mouseY;
        int pmouseX, pmouseY;
        int mouseButton;
        int wheelCount;
    };
    
    class Drawer /* abstract */
    {
    protected:
        // Display variables
        const matb& m_data;
        int m_width, m_height;
        
        // FPS management
        unsigned long m_fps = 30;
    
        // Event variables
        char key;
        int keyCode;
        MouseEvent mouseEvent;
    private:
        int delta_r;
        int delta_t;
    public:
        Drawer(int width, int height, const matb& data);
        virtual ~Drawer();
        
        virtual unsigned long fps() const final;
        virtual int height() const final;
        virtual int width() const final;
        virtual const matb& getData() const final;
        
        virtual void show(bool b = true) = 0;
        virtual void clear() = 0;
        
        virtual void setTitle(const char*) = 0;
        virtual void setDimension(int width, int height);
        virtual void setHeigh(int height) = 0;
        virtual void setWidth(int width) = 0;
        virtual void setFrameRate(int fps);
        
        virtual unsigned long frameRate() const = 0;
        
        virtual void setup() = 0;
        virtual void run() = 0;
        virtual void exit() = 0;
        virtual void draw() = 0;
        
        virtual void onKeyPressed();
        virtual void onKeyReleased();
        virtual void onKeyTyped();
        
        virtual void onMouseMoved();
        virtual void onMouseWheel();
        virtual void onMouseClicked();
        virtual void onMousePressed();
        virtual void onMouseReleased();
        
        /**
         *
         * @param name
         */
        virtual void savePNG(const char* name) const = 0;
        
        /* Draw functions */
        virtual void set_color(unsigned long color) = 0;
        virtual void draw_text(const char* str, int x, int y)  = 0;
        virtual void draw_slice(int origin_x, int origin_y, int radius, int degree,
                                double rotation) = 0;
    };

#ifdef CODE3C_UNIX
    class X11Drawer : public Drawer
    {
        // X11 Window variables
        XSetWindowAttributes m_attributes;
        XGCValues m_gcvalues;
        XFontStruct * m_font;
        
        Display *m_display;
        Window m_window;
        int m_screen;
        Pixmap m_db; // Double Buffer
        GC m_gc;
        
        
        // Miscellanous
        Atom wmDeleteWindow;
        unsigned long m_frameRate;
    public:
        X11Drawer(int width, int height, const matb& data);
        virtual ~X11Drawer() noexcept override;
        
        virtual void show(bool b = true) override;
        virtual void setTitle(const char*) override;
        virtual void setHeigh(int height) override;
        virtual void setWidth(int width) override;
        
        virtual void run() override;
        virtual void exit() override;
        virtual void clear() override;
        virtual void cleardb();
        
        virtual unsigned long frameRate() const override;
        
        virtual void setup() override = 0;
        virtual void draw() override = 0;
        
        virtual void savePNG(const char* name) const override;
        
        /* draw functions */
        
        virtual void background(unsigned long color);
        virtual void set_color(unsigned long color) override;
        virtual void draw_text(const char* str, int x, int y) override;
        virtual void draw_slice(int origin_x, int origin_y, int radius, int degree,
                                double rotation) override;
        virtual void fill_circle(int x, int y, int radius);
        virtual void draw_line(int x1, int y1, int x2, int y2);
    };
    typedef X11Drawer Code3CDrawer;
#endif //CODE3C_UNIX
#ifdef CODE3C_WIN32
#error "Undefined class, Windows 1X not available"
#endif //CODE3C_WIN32
}

#endif //HH_LIB_GDRAWER_3CCODE
