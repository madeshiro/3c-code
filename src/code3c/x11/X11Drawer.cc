#include "code3c/drawer.hh"

#include <unistd.h>
#include <ctime>
#include <cstring>
#include <stdexcept>

namespace code3c
{
    X11Drawer::X11Drawer(int width, int height, const code3c::matb &data):
            Drawer(width, height, data), m_frameRate(fps()),
            m_gcvalues(), m_attributes()
    {
        m_display = XOpenDisplay(NULL);
        m_screen = DefaultScreen(m_display);
        m_attributes.background_pixel = WhitePixel(m_display, DefaultScreen(m_display));
        m_window = XCreateSimpleWindow(m_display, RootWindow(m_display,m_screen),
                                       500, 500, width, height, 2,
                                       BlackPixel(m_display, m_screen),
                                       WhitePixel(m_display, m_screen));
        m_keyboard = XkbGetMap(m_display, XkbAllClientInfoMask,
                               XkbUseCoreKbd);
        if (!m_keyboard) throw std::runtime_error("Unable to alloc X11 keyboard");
        long mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask |
                    StructureNotifyMask |PointerMotionMask | ButtonReleaseMask;
        XSelectInput(m_display, m_window, mask);
        XAutoRepeatOff(m_display);
        
        m_gcvalues.foreground = BlackPixel(m_display, m_screen);
        m_gcvalues.background = WhitePixel(m_display, m_screen);
        m_gc = XCreateGC(m_display, RootWindow(m_display, m_screen),
                         (GCForeground | GCBackground), &m_gcvalues);
        
        m_db = XCreatePixmap(m_display, m_window, width, height,
                             DefaultDepth(m_display, m_screen));
        
        wmDeleteWindow = XInternAtom(m_display, "WM_DELETE_WINDOW", true);
        XSetWMProtocols(m_display, m_window, &wmDeleteWindow, 1);
        
        if ((m_font = XLoadQueryFont(m_display, "9x15")) == nullptr)
            throw std::runtime_error("Unable to find \"9x15\" font");
    }
    
    X11Drawer::X11Drawer(const X11Drawer &x11Drawer):
            X11Drawer(x11Drawer.m_width, x11Drawer.m_height, x11Drawer.m_data)
    {
    }
    
    X11Drawer::~X11Drawer() noexcept
    {
        XFreeGC(m_display, m_gc);
        XFreeFont(m_display, m_font);
        XCloseDisplay(m_display);
    }
    
    void X11Drawer::key_binding(bool _register)
    {
        int mask(0);
        switch(keySym)
        {
            case XK_Control_L:
                mask = DRAWER_KEY_CTRLL;
                break;
            case XK_Control_R:
                mask = DRAWER_KEY_CTRLR;
                break;
            case XK_Alt_L:
                mask = DRAWER_KEY_ALTL;
                break;
            case XK_Alt_R:
                mask = DRAWER_KEY_ALTR;
                break;
            case XK_Shift_L:
            case XK_Shift_R:
                mask = DRAWER_KEY_SHIFT;
                break;
            default:
            {
                if (key > 0)
                    mask = (int) key; // NOLINT(cert-str34-c)
                break;
            }
        }
        
        if (_register)
            register_key(mask);
        else
            delete_key(mask);
    }
    
    void X11Drawer::show(bool b)
    {
        if (b)
        {
            XMapWindow(m_display, m_window);
        } else
        {
            XUnmapWindow(m_display, m_window);
        }
    }
    
    void X11Drawer::setTitle(const char * title)
    {
        XStoreName(m_display, m_window, title);
    }
    
    void X11Drawer::setHeigh(int height)
    {
    }
    
    void X11Drawer::setWidth(int width)
    {
    
    }
    
    unsigned long X11Drawer::frameRate() const
    {
        return m_frameRate;
    }
    
    void X11Drawer::setup()
    {
    }
    
    void X11Drawer::run()
    {
        bool done(false);
        XEvent event;
        
        this->show(true);
        this->setup();
        do
        {
            clock_t begin(clock());
            /* run */ {
                // Waiting for event
                while (XPending(m_display))
                {
                    XNextEvent(m_display, &event);
                    switch (event.type)
                    {
                        case KeyPress:
                        case KeyRelease:
                        {
                            keyCode = event.xkey.keycode;
                            XLookupString(&event.xkey, &key, 1, NULL, NULL);
                            XkbLookupKeySym(m_display, keyCode, 0, &keyMod, &keySym);
                            if (event.type == KeyPress)
                            {
                                key_binding(true);
                                onKeyPressed();
                            }
                            if (event.type == KeyRelease)
                            {
                                onKeyReleased();
                                key_binding(false);
                            }
                            break;
                        }
                        case MotionNotify:
                        {
                            mouseEvent.pmouseX = mouseEvent.mouseX - event.xbutton.x;
                            mouseEvent.pmouseY = mouseEvent.mouseY - event.xbutton.y;
                            mouseEvent.mouseX = event.xbutton.x;
                            mouseEvent.mouseY = event.xbutton.y;
                            onMouseMoved();
                            if (mouseEvent.button1Pressed)
                                onMouseDragged();
                            break;
                        }
                        case ButtonRelease:
                        {
                            keyCode = event.xbutton.button;
                            switch (keyCode)
                            {
                                case 1:
                                    mouseEvent.button1Pressed = false;
                                    break;
                                case 2:
                                    mouseEvent.button2Pressed = false;
                                    break;
                            }
                            onMouseReleased();
                            break;
                        }
                        case ButtonPress:
                        {
                            mouseEvent.mouseButton = event.xbutton.button;
                            switch (keyCode)
                            {
                                case 1:
                                    mouseEvent.button1Pressed = true;
                                    break;
                                case 2:
                                    mouseEvent.button2Pressed = true;
                                    break;
                            }
                            if (mouseEvent.mouseButton < 4)
                            {
                                onMousePressed();
                            }
                            else if (mouseEvent.mouseButton == 4
                                || mouseEvent.mouseButton == 5)
                            {
                                mouseEvent.wheelCount = (keyCode == 4 ? -1 : 1);
                                onMouseWheel();
                            }
                            break;
                        }
                        case ClientMessage:
                        {
                            if (event.xclient.data.l[0] == wmDeleteWindow)
                                done = true;
                            break;
                        }
                        default:
                            break;
                    }
                }
                this->draw();
                
                XCopyArea(m_display, m_db, m_window, m_gc, 0, 0, width(), height(),0, 0);
                XFlush(m_display);
            }
            
            unsigned long millis((clock()-begin)*1000/CLOCKS_PER_SEC);
            if ((1000/fps() > millis))
            {
                usleep((1000 / fps() - millis) * 1000);
                millis += (1000/fps() - millis);
            }
            
            double _frameRate(1000.0/ (double)millis);
            m_frameRate = (unsigned long) _frameRate;
        } while (!done);
    }
    
    void X11Drawer::exit()
    {
    }
    
    void X11Drawer::clear()
    {
        XClearWindow(m_display, m_window);
    }
    
    void X11Drawer::cleardb()
    {
        // Clear double buffer
        foreground(0xffffff);
        XFillRectangle(m_display, m_db, m_gc, 0, 0, width(), height());
        foreground(0);
    }
    
    void X11Drawer::savePNG(const char *name) const
    {
        XImage * img = XGetImage(m_display, m_db, 0, 0, m_width, m_height, AllPlanes, XYPixmap);
        if (img)
        {
            PixelMap pixelMap(width(), height());
            for (int x(0); x < width(); x++)
            {
                for (int y(0); y < height(); y++)
                {
                    pixelMap[x,y] = {
                            x, y,
                            XGetPixel(img, x, y),
                            0xff
                    };
                }
            }
            FILE * dest = fopen(name, "wb");
            if (dest)
                PixelMap::saveInPng(pixelMap, dest);
            XFree(img);
        }
    }
    
    /* draw functions */
    
    void X11Drawer::background(unsigned long color)
    {
        foreground(color);
        XFillRectangle(m_display, m_db, m_gc, 0, 0, width(), height());
    }
    
    void X11Drawer::foreground(unsigned long color)
    {
        XSetForeground(m_display, m_gc, color);
    }
    
    void X11Drawer::draw_pixel(unsigned long color, int x, int y)
    {
        foreground(color);
        XDrawPoint(m_display, m_db, m_gc, x, y);
    }
    
    void X11Drawer::draw_text(const char *str, int x, int y)
    {
        XDrawString(m_display, m_db, m_gc, x, y, str, (int) strlen(str));
    }

    void X11Drawer::draw_slice(
            int origin_x, int origin_y, int radius, int degree, int rotation
            )
    {
        rotation -= degree/2;
        XFillArc(m_display, m_db, m_gc, origin_x-radius, origin_y-radius,
                 radius*2,radius*2,
                 64*rotation, 64*degree);
    }

    void X11Drawer::fill_circle(int x, int y, int radius)
    {
        XFillArc(m_display, m_db, m_gc, x-radius, y-radius, radius*2, radius*2, 0,
                 360*64);
    }
    
    void X11Drawer::draw_line(int x1, int y1, int x2, int y2)
    {
        XDrawLine(m_display, m_db, m_gc, x1, y1, x2, y2);
    }
}