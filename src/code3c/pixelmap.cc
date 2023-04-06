#include "code3c/pixelmap.hh"

namespace code3c
{
    PixelMap::PixelMap(int width, int height):
        m_width(width), m_height(height), m_size(width*height),
        m_pixels(new Pixel[width*height])
    {
        for (int y(0); y < height; y++)
        {
            int _off_i(y*width);
            for (int x(0); x < width; x++)
            {
                m_pixels[_off_i+x] = {x, y, 0};
            }
        }
    }
    
    PixelMap::PixelMap(const PixelMap &map):
        m_width(map.m_width), m_height(map.m_height), m_size(map.m_size),
        m_pixels(new Pixel[map.m_size])
    {
        for (uint32_t i(0); i < map.size(); i++)
            m_pixels[i] = map.m_pixels[i];
    }
    
    PixelMap::~PixelMap()
    {
        delete[] m_pixels;
    }
    
    int PixelMap::height() const
    {
        return m_height;
    }
    
    int PixelMap::width() const
    {
        return m_width;
    }
    
    uint32_t PixelMap::size() const
    {
        return m_size;
    }
    
    void PixelMap::resize(float scale)
    {
        // Todo PixelMap::resize(float)
    }
    
    void PixelMap::resize(int width, int height)
    {
        // Todo PixelMap::resize(int, int)
    }
    
    Pixel& PixelMap::operator[](int i)
    {
        return m_pixels[i];
    }
    
    Pixel& PixelMap::operator[](int x, int y)
    {
        return m_pixels[x*y];
    }
    
    Pixel PixelMap::operator[](int i) const
    {
        return m_pixels[i];
    }
    
    Pixel PixelMap::operator[](int x, int y) const
    {
        return m_pixels[x*y];
    }
    
    PixelMap PixelMap::loadFromPNG(const char *pngfile) noexcept(false)
    {
        return {10, 10}; // Todo loadFromPNG
    }
    
    void PixelMap::saveInPng(const code3c::PixelMap &map, FILE *dest)
    {
        // Todo saveInPng
    }
}