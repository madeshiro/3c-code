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
    
    void Drawer::onMouseDragged()
    {
    }
    
    Drawer::Drawer(int width, int height, const code3c::matb &data) :
            m_data(data), m_width(width), m_height(height),
            m_keybind(), key(0), keyCode(0), keys_pressed(0)
    {
    }
    
    Drawer::Drawer(const Drawer & drawer):
        m_data(drawer.m_data), m_width(drawer.m_width), m_height(drawer.m_height),
        m_fps(drawer.m_fps),
        key(0), keyCode(0), keys_pressed(0)
    {
    }
    
    bool Drawer::register_key(int mask)
    {
        // Invalid mask
        if ((mask & keys_pressed) != 0)
            return false;
        
        keys_pressed |= mask;
        if (m_keybind.contains(keys_pressed))
        {
            (this->*(m_keybind[keys_pressed]))();
        }
        return true;
    }
    
    void Drawer::delete_key(int mask)
    {
        if ((keys_pressed & mask) != mask)
            return;
        
        keys_pressed -= mask;
    }
    
    void Drawer::bindKey(int mask, code3c::Drawer::delegate fn)
    {
        m_keybind.insert(std::pair<int, delegate>(mask, fn));
    }
    
    void Drawer::unbindKey(int mask)
    {
        m_keybind.erase(mask);
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
}
