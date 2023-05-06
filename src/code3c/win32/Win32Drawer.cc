#include <unistd.h>
#include <stdexcept>
#include "code3c/drawer.hh"

namespace code3c
{
    LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        auto *win = (Win32Drawer *) GetProp(hWnd, "Win32Drawer");
        switch (msg)
        {
            case WM_PAINT:
            {
                BeginPaint(hWnd, &win->m_paint);
                {
                    win->draw();
                }
                EndPaint(hWnd, &win->m_paint);
                break;
            }
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                win->keyCode = wParam;
                if (msg == WM_KEYDOWN)
                {
                    win->key_binding(true);
                    win->onKeyPressed();
                }
                if (msg == WM_KEYUP)
                {
                    win->onKeyReleased();
                    win->key_binding(false);
                }
                break;
            }
            case WM_MOUSEMOVE:
            {
                int mouseX, mouseY;
                mouseX = GET_X_LPARAM(lParam);
                mouseY = GET_Y_LPARAM(lParam);
                
                win->mouseEvent.pmouseX = win->mouseEvent.mouseX - mouseX;
                win->mouseEvent.pmouseY = win->mouseEvent.mouseY - mouseY;
                win->mouseEvent.mouseX = mouseX;
                win->mouseEvent.mouseY = mouseY;
                win->onMouseMoved();
                if (win->mouseEvent.button1Pressed)
                    win->onMouseDragged();
                break;
            }
            case WM_LBUTTONDOWN:
                win->mouseEvent.button1Pressed = true;
                win->keyCode = VK_LBUTTON;
                win->onMousePressed();
                break;
            case WM_RBUTTONDOWN:
                win->mouseEvent.button2Pressed = true;
                win->keyCode = VK_RBUTTON;
                win->onMousePressed();
                break;
            case WM_LBUTTONUP:
                win->mouseEvent.button1Pressed = false;
                win->keyCode = VK_LBUTTON;
                win->onMouseReleased();
                break;
            case WM_RBUTTONUP:
                win->mouseEvent.button2Pressed = false;
                win->keyCode = VK_RBUTTON;
                win->onMousePressed();
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                win->done = true;
                break;
            case WM_QUIT:
                win->done = true;
                break;
            default:
                return DefWindowProc(hWnd, msg, wParam, lParam);
        }
        
        return 0;
    }
    
    Win32Drawer::Win32Drawer(int width, int height, const matb &data):
            Drawer(width, height, data)
    {
        m_instance = GetModuleHandle(NULL);
        
        // Compute real width and height
        RECT rc = {0, 0, width, height};
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        m_offH = rc.bottom-rc.top-height;
        m_offW  = rc.right-rc.left-width;
        
        // Register Win32 class
        WNDCLASS wc = { };
        if (!GetClassInfo(m_instance, "Win32Drawer", &wc))
        {
            wc.lpfnWndProc = code3c::WndProc;
            wc.hInstance = m_instance;
            wc.lpszClassName = "Win32Drawer";
            
            RegisterClass(&wc);
        }
        
        m_window = CreateWindowEx(0,
                "Win32Drawer",
                "Win32Drawer Window",
                WS_OVERLAPPED |
                    WS_CAPTION |
                    WS_SYSMENU |
                    WS_THICKFRAME |
                    WS_MINIMIZEBOX |
                    WS_VISIBLE,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                width + m_offW,
                height + m_offH,
                NULL, // Parent Window
                NULL, // Menu: Try to create one for the future
                m_instance,
                NULL  // Additional application data
                );
        if (!m_window)
            throw std::runtime_error("Unable to create Win32 Window");
        if (!SetProp(m_window, "Win32Drawer", this))
            throw std::runtime_error("Unable to attach Win32Drawer to Win32 HWND");
        m_hdc   = GetDC(m_window);
        m_hdcdb = CreateCompatibleDC(m_hdc);
        
        ZeroMemory(&m_bmi, sizeof(BITMAPINFO));
        m_bmi.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
        m_bmi.bmiHeader.biWidth    = width;
        m_bmi.bmiHeader.biHeight   = height;
        m_bmi.bmiHeader.biPlanes   = 1;
        m_bmi.bmiHeader.biBitCount = 32;
        m_bitmap = CreateDIBSection(m_hdc, &m_bmi, DIB_RGB_COLORS,
                                    &m_bitmapBits,
                                    NULL, 0);
        
        m_bf.BlendOp = AC_SRC_OVER;
        m_bf.BlendFlags = 0;
        m_bf.SourceConstantAlpha = 0xff;
        m_bf.AlphaFormat = AC_SRC_ALPHA;
        
        m_pen = CreatePen(PS_SOLID, 1, 0x0);
        SelectObject(m_hdc, m_pen);

        m_brush = CreateSolidBrush(0x0);
        SelectObject(m_hdc, m_brush);
    }
    
    Win32Drawer::Win32Drawer(const Win32Drawer &w32Drawer):
            Win32Drawer(w32Drawer.width(), w32Drawer.height(), w32Drawer.m_data)
    {
    }
    
    Win32Drawer::~Win32Drawer() noexcept
    {
        RemoveProp(m_window, "Win32Drawer");
        DestroyWindow(m_window);
        DeleteObject(m_bitmap);
        DeleteDC(m_hdcdb);
        DeletePen(m_pen);
    }
    
    void Win32Drawer::key_binding(bool _register)
    {
        int mask(0);
        switch (keyCode)
        {
            case VK_CONTROL:
                mask = DRAWER_KEY_CTRL;
                break;
            case VK_MENU:
                mask = DRAWER_KEY_ALT;
                break;
            case VK_SHIFT:
                mask = DRAWER_KEY_SHIFT;
                break;
            case VK_ESCAPE:
                mask = DRAWER_KEY_ESC;
            default:
                if (keyCode >= 0x41 && keyCode <= 0x5a)
                    mask = 'a' + (keyCode-0x41);
                if (VK_F1 <= keyCode && VK_F12 >= keyCode)
                    mask = (keyCode-VK_F1+1) << 12;
                break;
        }
        
        if (_register)
            register_key(mask);
        else
            delete_key(mask);
    }
    
    void Win32Drawer::show(bool b)
    {
        ShowWindow(m_window, b ? SW_SHOW : SW_HIDE);
    }
    
    void Win32Drawer::setTitle(const char * title)
    {
        SetWindowText(m_window, title);
    }
    
    void Win32Drawer::setHeigh(int height)
    {
        m_height = height;
        SetWindowPos(m_window, NULL, 0, 0,
                     m_width+m_offW, m_height+m_offH,
                     SWP_NOMOVE | SWP_NOZORDER);
    }
    
    void Win32Drawer::setWidth(int width)
    {
        m_width = width;
        SetWindowPos(m_window, NULL, 0, 0,
                     m_width+m_offW, m_height+m_offH,
                     SWP_NOMOVE | SWP_NOZORDER);
    }
    
    unsigned long Win32Drawer::frameRate() const
    {
        return m_frameRate;
    }
    
    void Win32Drawer::run()
    {
        MSG msg;
        
        this->setup();
        this->show(true);
        do
        {
            clock_t begin(clock());
            m_hdcdb = CreateCompatibleDC(m_hdc);
            
            /* run */ {
                while (PeekMessage(&msg, m_window, 0, 0, PM_REMOVE) > 0)
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                
                unsigned long millis((clock() - begin) * 1000 / CLOCKS_PER_SEC);
                if ((1000 / fps() > millis))
                {
                    usleep((1000 / fps() - millis) * 1000);
                    millis += (1000 / fps() - millis);
                }
                
                double _frameRate(1000.0 / (double) millis);
                m_frameRate = (unsigned long) _frameRate;
            }
            
            HGDIOBJ oldBmp = SelectObject(m_hdcdb, m_bitmap);
            AlphaBlend(m_hdc, 0, 0, m_width, m_height, m_hdcdb,
                              0, 0, m_width, m_height, m_bf);
            SelectObject(m_hdcdb, oldBmp);
            DeleteDC(m_hdcdb);
        }
        while (!done);
    }
    
    void Win32Drawer::exit()
    {
    }
    
    void Win32Drawer::clear()
    {
        // Default behaviour in Win32
        // Non-persistent drawing surface
        RedrawWindow(m_window, NULL, NULL,
                     RDW_ERASE | RDW_INVALIDATE | RDW_FRAME);
    }
    
    void Win32Drawer::savePNG(const char *name) const
    {
        PixelMap pixelMap(width(), height());
        for (int x(0); x < width(); x++)
        {
            for (int y(0); y < height(); y++)
            {
                COLORREF color(GetPixel(m_hdc, x, y));
                pixelMap[x,y] = {
                        x, y,
                        rgb(GetRValue(color), GetGValue(color), GetBValue(color)),
                        0xff
                };
            }
        }
        FILE * dest = fopen(name, "wb");
        if (dest)
        {
            PixelMap::saveInPng(pixelMap, dest);
            fclose(dest);
        }
    }
    
    void Win32Drawer::background(unsigned long color)
    {
        RECT rc = {0, 0, m_width, m_height};
        HBRUSH brush = CreateSolidBrush(RGB(color>>16&0xff, color>>8&0xff, color&0xff));
        FillRect(m_hdc, &rc, brush);
    }
    
    void Win32Drawer::foreground(unsigned long color)
    {
        COLORREF cr = RGB(color>>16&0xff, color>>8&0xff, color&0xff);
        SetBkColor(m_hdc, cr);
        SetBkMode(m_hdc, TRANSPARENT);
        SetTextColor(m_hdc, color);

        DeletePen(m_pen);
        m_pen = CreatePen(PS_SOLID, 1, cr);
        SelectPen(m_hdc, m_pen);

        DeleteBrush(m_brush);
        m_brush = CreateSolidBrush(cr);
        SelectBrush(m_hdc, m_brush);
    }
    
    void Win32Drawer::draw_pixel(unsigned long color, int x, int y)
    {
        SetPixel(m_hdc, x, y, RGB((color>>16)&0xff, (color>>8)&0xff, color&0xff));
        // uint32_t index = (width()*height()) - (width()*y+(width()-x));
        // ((UINT*)m_bitmapBits)[index] = (UINT) color;
    }
    
    void Win32Drawer::draw_text(const char *str, int x, int y)
    {
        RECT rc = {x, y, 0, 0};
        DrawText(m_hdc, str, -1, &rc, DT_NOCLIP );
    }
    
    void Win32Drawer::draw_slice(
            int origin_x, int origin_y, int radius, int degree, int rotation
    )
    {
        rotation -= (degree/2);

        BeginPath(m_hdc);
        MoveToEx(m_hdc, origin_x, origin_y, NULL);
        AngleArc(m_hdc, origin_x, origin_y, radius, rotation, degree);
        LineTo(m_hdc, origin_x, origin_y);
        EndPath(m_hdc);
        StrokeAndFillPath(m_hdc);
    }
    
    void Win32Drawer::fill_circle(int x, int y, int radius)
    {
        Ellipse(m_hdc, x-radius, y-radius, x+radius, y+radius);
    }
    
    void Win32Drawer::draw_line(int x1, int y1, int x2, int y2)
    {
        POINT points[2] {
                {x1, y1},
                {x2, y2}
        };
        
        Polyline(m_hdc, points, 2);
    }
}