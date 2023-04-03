#include "code3c/drawer.hh"

namespace code3c
{
    unsigned long rgb(int r, int g, int b)
    {
        return (r << 16) | (g << 8) | b;
    }
    
    void Drawer::onKeyPressed()
    {
    }
    
    void Drawer::onKeyReleased()
    {
    }
    
    void Drawer::onKeyTyped()
    {
    }
    
    void Drawer::onMouseMoved()
    {
    }
    
    void Drawer::onMouseWheel()
    {
    }
    
    void Drawer::onMouseClicked()
    {
    }
    
    void Drawer::onMousePressed()
    {
    }
    
    void Drawer::onMouseReleased()
    {
    }
    
    Drawer::Drawer(int width, int height, const code3c::matb &data) :
            m_data(data), m_width(width), m_height(height)
    {
    }
    
    Drawer::~Drawer()
    {
    }
    
    unsigned long Drawer::fps() const
    {
        return m_fps;
    }
    
    int Drawer::height() const
    {
        return m_height;
    }
    
    int Drawer::width() const
    {
        return m_width;
    }
    
    const matb &Drawer::getData() const
    {
        return m_data;
    }
    
    void Drawer::setFrameRate(int fps)
    {
        m_fps = fps;
    }
    
    void Drawer::setDimension(int width, int height)
    {
        this->setHeigh(height);
        this->setWidth(width);
    }
    
    void Drawer::draw_angle(int t)
    {
        for (int r(0); r < m_data.m(); r++)
        {
            char _byte(m_data[t, r]);
            int red = 0xff * (_byte | (1 << 2));
            int green = 0xff * (_byte | (1 << 1));
            int blue = 0xff * (_byte | (1 << 0));
            
            set_color(0xffffff & ~rgb(red, green, blue));
            //draw_slice(width() / 2, height() / 2, /**/ delta_r, delta_t);
        }
    }
}
