#include <stdexcept>
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
    
    PixelMap PixelMap::resize(float scale) const
    {
        return resize(static_cast<int>(width()*scale),
               static_cast<int>(height()*scale));
    }
    
    PixelMap PixelMap::resize(int width, int height) const
    {
        if (width <= 0)
            width = this->width();
        if (height <= 0)
            height = this->height();
        
        PixelMap pixelMap(width, height);
        for (int x = 0; x < width; x++)
        {
            int srcX = (x*m_width)/width;
            for (int y = 0; y < height; y++)
            {
                int srcY = (y*m_height)/height;
                Pixel pixelSrc = (*this)[srcX, srcY];
                pixelMap[x,y] = pixelSrc;
            }
        }
        
        return pixelMap;
    }
    
    Pixel& PixelMap::operator[](int i)
    {
        return m_pixels[i];
    }
    
    Pixel& PixelMap::operator[](int x, int y)
    {
        return m_pixels[y*width()+x];
    }
    
    Pixel PixelMap::operator[](int i) const
    {
        return m_pixels[i];
    }
    
    Pixel PixelMap::operator[](int x, int y) const
    {
        return m_pixels[y*width()+x];
    }
    
    PixelMap PixelMap::loadFromPNG(const char *pngfile) noexcept(false)
    {
        png_descp desc = open_png(pngfile);
        if (!desc)
            throw std::runtime_error("Unable to load png file");
        
        PixelMap pixelMap(desc->width, desc->height);
        for (int y = 0; y < desc->height; y++)
        {
            for (int x = 0, i = 0; x < desc->width; x++, i+=4)
            {
                int r = 0xff & desc->rows[y][i];
                int g = 0xff & desc->rows[y][i+1];
                int b = 0xff & desc->rows[y][i+2];
                png_byte a = 0xff & desc->rows[y][i+3];
                unsigned long rgb = (r << 16) | (g << 8) | b;
                pixelMap[x,y] = {x, y, rgb, a};
            }
            
        }
        
        free_png_desc(desc);
        return pixelMap;
    }
    
    void PixelMap::saveInPng(const code3c::PixelMap &map, FILE *dest)
    {
        // Todo saveInPng
    }
}