#include "code3c/drawer.hh"

namespace code3c
{
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
    
    Drawer::Drawer(int width, int height, const code3c::matb &data):
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
    
    const matb& Drawer::getData() const
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
}
