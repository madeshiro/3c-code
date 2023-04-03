#include "code3c/drawer.hh"
#include <unistd.h>
#include <ctime>
#include <cstring>
#include <X11/Xutil.h>
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
        long mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask |
                    PointerMotionMask | ButtonReleaseMask;
        XSelectInput(m_display, m_window, mask);
        
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
    
    X11Drawer::~X11Drawer() noexcept
    {
        XFreeGC(m_display, m_gc);
        XFreeFont(m_display, m_font);
        XCloseDisplay(m_display);
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
                            XLookupString(&event.xkey, &key, 1, NULL, NULL);
                            keyCode = event.xkey.keycode;
                            if (event.type == KeyPress)
                                onKeyPressed();
                            if (event.type == KeyRelease)
                                onKeyReleased();
                            break;
                        }
                        case MotionNotify:
                        {
                            mouseEvent.pmouseX = mouseEvent.mouseX - event.xbutton.x;
                            mouseEvent.pmouseY = mouseEvent.mouseY - event.xbutton.y;
                            mouseEvent.mouseX = event.xbutton.x;
                            mouseEvent.mouseY = event.xbutton.y;
                            onMouseMoved();
                            break;
                        }
                        case ButtonRelease:
                        {
                            keyCode = event.xbutton.button;
                            onMouseReleased();
                            break;
                        }
                        case ButtonPress:
                        {
                            mouseEvent.mouseButton = event.xbutton.button;
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
        set_color(0xffffff);
        XFillRectangle(m_display, m_db, m_gc, 0, 0, width(), height());
        set_color(0);
    }
    
    void X11Drawer::savePNG(const char *name) const
    {
    
    }
    
    /* draw functions */
    
    void X11Drawer::background(unsigned long color)
    {
        set_color(color);
        XFillRectangle(m_display, m_db, m_gc, 0, 0, width(), height());
    }
    
    void X11Drawer::set_color(unsigned long color)
    {
        XSetForeground(m_display, m_gc, color);
    }
    
    void X11Drawer::draw_text(const char *str, int x, int y)
    {
        XDrawString(m_display, m_db, m_gc, x, y, str, (int) strlen(str));
    }
    
    void X11Drawer::draw_slice(int origin_x, int origin_y, int radius, int degree,
                               double rotation)
    {
        int precision = 8;
        rotation -= (M_PI*((double)degree)/360.0);
        
        XPoint xPoints[precision*degree+2];
        veclf coord(2, new double*[2] { new double(0), new double(0) });
        
        xPoints[0] = {(short) origin_x, (short) origin_y};
        xPoints[(precision*degree)+1] = {(short) origin_x, (short) origin_y};
        for (int _ = 1; _ <= (precision*degree); _++)
        {
            double delta_degree = ((double) _-1)/(360.0);
            
            coord[0] = radius;
            coord[1] = 0;
            
            matlf rot = mat2rotate<double>(rotation+(delta_degree*2*M_PI));
            coord = rot * coord;
            
            xPoints[_].x = static_cast<short>(coord[0] + origin_x);
            xPoints[_].y = static_cast<short>(-coord[1] + origin_y);
        }
        
        XFillPolygon(m_display, m_db, m_gc, xPoints, degree+2, Convex,
                     CoordModeOrigin);
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