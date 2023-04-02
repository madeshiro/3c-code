#include "code3c/drawer.hh"
#include <unistd.h>
#include <time.h>
#include <string.h>

namespace code3c
{
    X11Drawer::X11Drawer(int width, int height, const code3c::matb &data):
            Drawer(width, height, data), m_frameRate(fps())
    {
        m_display = XOpenDisplay(NULL);
        m_screen = DefaultScreen(m_display);
        m_attributes.background_pixel = WhitePixel(m_display, DefaultScreen(m_display));
        m_window = XCreateSimpleWindow(m_display, RootWindow(m_display,m_screen),
                                       400, 500, width, height, 3,
                                       BlackPixel(m_display, m_screen),
                                       WhitePixel(m_display, m_screen));
        long mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask |
                    PointerMotionMask | ButtonReleaseMask;
        XSelectInput(m_display, m_window, mask);
        
        m_gcvalues.foreground = BlackPixel(m_display, m_screen);
        m_gcvalues.background = WhitePixel(m_display, m_screen);
        m_gc = XCreateGC(m_display, RootWindow(m_display, m_screen),
                         (GCForeground | GCBackground), &m_gcvalues);
        
        wmDeleteWindow = XInternAtom(m_display, "WM_DELETE_WINDOW", true);
        XSetWMProtocols(m_display, m_window, &wmDeleteWindow, 1);
        
        if ((m_font = XLoadQueryFont(m_display, "9x15")) == NULL)
            throw "Unable to find \"9x15\" font";
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
    
    void X11Drawer::loop()
    {
        char buf[256];
        bool done(false);
        XEvent event;
        
        this->setup();
        this->show();
        do
        {
            clock_t begin(clock());
            /* loop */ {
                XNextEvent(m_display, &event);
                this->clear();
                switch (event.type)
                {
                    case KeyPress:
                    {
                        XSetForeground(m_display, m_gc, 0xff0000);
                        sprintf(buf, "You press the button %c", (char) event.xkey
                        .keycode);
                        this->draw_text(buf, 0, 54);
                        XSetForeground(m_display, m_gc, 0x0);
                        break;
                    }
                    case ClientMessage:
                    {
                        if (event.xclient.data.l[0] == wmDeleteWindow)
                            done = true;
                        break;
                    }
                }
                this->draw();
            }
            
            unsigned long millis((clock()-begin)*1000/CLOCKS_PER_SEC);
            if ((1000/fps() > millis))
            {
                usleep((1000 / fps() - millis) * 1000);
                millis += (1000/fps() - millis);
            }
            
            float _frameRate(1000.0f/millis);
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
    
    void X11Drawer::savePNG(const char *name) const
    {
    
    }
    
    /* draw functions */
    
    void X11Drawer::draw_text(const char *str, int x, int y)
    {
        XDrawString(m_display, m_window, m_gc, x, y, str, strlen(str));
    }
}