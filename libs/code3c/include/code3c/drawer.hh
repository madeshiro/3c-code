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
#include "code3c/pixelmap.hh"

#include <cstdint>
#include <png.h>
#include <map>

#ifdef CODE3C_UNIX
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#endif // CODE3C_UNIX

#ifdef CODE3C_WIN32
#include <windef.h>
#include <winuser.h>
#include <wingdi.h>
#include <windowsx.h>
#endif // CODE3C_WIN32

// DRAWER_KEY_cc format: xckk
// x: special key id (e.g 1 = F#)
// c: ctrl key
// kk: key char
#define DRAWER_KEY_CTRL     0x0100
#define DRAWER_KEY_SHIFT    0x0200
#define DRAWER_KEY_ALT      0x0400
#define DRAWER_KEY_ESC      0x0800
#define DRAWER_KEY_F1       0x1000
#define DRAWER_KEY_F2       0x2000
#define DRAWER_KEY_F3       0x3000
#define DRAWER_KEY_F4       0x4000
#define DRAWER_KEY_F5       0x5000
#define DRAWER_KEY_F6       0x6000
#define DRAWER_KEY_F7       0x7000
#define DRAWER_KEY_F8       0x8000
#define DRAWER_KEY_F9       0x9000
#define DRAWER_KEY_F10      0xA000
#define DRAWER_KEY_F11      0xB000
#define DRAWER_KEY_F12      0xC000

namespace code3c
{
    /**
     *
     * @param r
     * @param g
     * @param b
     * @return
     */
    unsigned long rgb(int r, int g, int b);
    
    class Drawer;
    struct MouseEvent
    {
        int mouseX, mouseY;
        int pmouseX, pmouseY;
        unsigned int mouseButton;
        int wheelCount;

        bool button1Pressed;
        bool button2Pressed;
    };
    
    class Drawer /* abstract */
    {
    protected:
        // Display variables
        const mat8_t& m_data;
        int m_width, m_height;
        
        // FPS management
        unsigned long m_fps = 30;
    
        // Event variables
        char key;
        unsigned int keyCode;
        MouseEvent mouseEvent;

        // Key mapping
        int keys_pressed;

        bool register_key(int mask);
        void delete_key(int mask);

        // Key binding
        typedef void (Drawer::*delegate)();
        std::map<int, delegate> m_keybind;

        void bindKey(int mask, delegate fn);
        void unbindKey(int mask);
    private:
        int delta_r;
        int delta_t;
    public:
        Drawer(int width, int height, const mat8_t& data);
        Drawer(const Drawer&);
        virtual ~Drawer() = default;
        
        virtual unsigned long fps() const final;
        virtual int height() const final;
        virtual int width() const final;
        virtual const mat8_t& getData() const final;
        
        virtual void show(bool b) = 0;
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
        virtual void onMouseDragged();
        virtual void onMouseClicked();
        virtual void onMousePressed();
        virtual void onMouseReleased();
        
        /**
         *
         * @param name
         */
        virtual void savePNG(const char *name) const = 0;
        
        /* Draw functions */
        virtual void foreground(unsigned long color) = 0;

        virtual void background(unsigned long color) = 0;

        virtual void draw_pixel(unsigned long color, int x, int y) = 0;
        virtual void draw_text(const char* str, int x, int y)  = 0;
        virtual void draw_slice(int origin_x, int origin_y, int radius, int degree,
                                int rotation) = 0;

        virtual void fill_circle(int x, int y, int radius) = 0;
        virtual void draw_line(int x1, int y1, int x2, int y2) = 0;

        virtual void draw_pixelmap(const PixelMap& pixelMap, int x, int y);

        virtual uint64_t hash() const;
    };

#ifdef CODE3C_UNIX
    class X11Drawer : public Drawer
    {
        // X11 Window variables
        XSetWindowAttributes m_attributes;
        XGCValues m_gcvalues;
        XFontStruct *m_font;
        
        XkbDescPtr m_keyboard;
        Display *m_display;
        Window m_window;
        int m_screen;
        Pixmap m_db; // Double Buffer
        GC m_gc;

        // Miscellanous
        Atom wmDeleteWindow;
        KeySym keySym;
        unsigned int keyMod;
        unsigned long m_frameRate;

        void key_binding(bool _register);

    public:
        X11Drawer(int width, int height, const mat8_t&data);
        X11Drawer(const X11Drawer &x11Drawer);
        ~X11Drawer() noexcept override;
        
        void show(bool b) override;
        void setTitle(const char *) override;
        void setHeigh(int height) override;
        void setWidth(int width) override;
        
        void run() override;
        void exit() override;
        void clear() override;
        virtual void cleardb();

        unsigned long frameRate() const override;
        
        void setup() override = 0;
        void draw() override = 0;
        
        void savePNG(const char *name) const override;
        
        /* draw functions */
        
        void background(unsigned long color) override;
        void foreground(unsigned long color) override;

        void draw_pixel(unsigned long color, int x, int y) override;
        void draw_text(const char *str, int x, int y) override;
        void draw_slice(
                int origin_x, int origin_y, int radius, int degree,
                int rotation
        ) override;
        void fill_circle(int x, int y, int radius) override;
        void draw_line(int x1, int y1, int x2, int y2) override;
    };

    typedef X11Drawer Code3CDrawer;
#endif //CODE3C_UNIX
#ifdef CODE3C_WIN32
    class Win32Drawer : public Drawer
    {
        friend LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);

        // Win32 Window variables
        HINSTANCE m_instance;
        PAINTSTRUCT m_paint;
        HWND m_window;
        HDC m_hdc;
        HPEN m_pen;
        HBRUSH m_brush;

        // Double Buffering
        HDC m_hdcdb;
        HBITMAP m_bitmap;
        BITMAPINFO m_bmi;
        BLENDFUNCTION m_bf;
        void* m_bitmapBits;

        // Miscellanous
        int m_offH, m_offW;
        unsigned long m_frameRate;
        bool done = false;

        void key_binding(bool _register);
    public:
        Win32Drawer(int width, int height, const mat8_t& data);
        Win32Drawer(const Win32Drawer& w32Drawer);
        ~Win32Drawer() noexcept override;

        void show(bool b) override;
        void setTitle(const char*) override;
        void setHeigh(int height) override;
        void setWidth(int width) override;

        void run() override;
        void exit() override;
        void clear() override;

        unsigned long frameRate() const override;

        void setup() override = 0;
        void draw() override = 0;

        void savePNG(const char* name) const override;

        /* draw functions */

        void background(unsigned long color) override;
        void foreground(unsigned long color) override;
        void draw_pixel(unsigned long color, int x, int y) override;
        void draw_text(const char* str, int x, int y) override;
        void draw_slice(int origin_x, int origin_y, int radius, int degree,
                        int rotation) override;
        void fill_circle(int x, int y, int radius) override;
        void draw_line(int x1, int y1, int x2, int y2) override;
    };
    typedef Win32Drawer Code3CDrawer;
#endif //CODE3C_WIN32
}

#endif //HH_LIB_GDRAWER_3CCODE
