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
#ifndef HH_LIB_PIXELMAP
#define HH_LIB_PIXELMAP
#include "3ccodelib.hh"
#include <png.h>

#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#endif // __cplusplus
#define PNG_DESC_MODE_READ  0
#define PNG_DESC_MODE_WRITE 1
    
    typedef struct PNG_LIGHT_DESC png_desc, *png_descp;
    struct PNG_LIGHT_DESC
    {
        int mode;
        int width, height;
        png_byte color_type;
        png_byte bit_depth;
        
        png_structp png;
        png_infop info;
        png_bytepp rows;
        
        FILE* _file;
    };
    
    png_descp open_png(FILE* pngfile);
    png_descp read_png(FILE* fin);
    png_descp create_png(FILE* fio, int width, int height);
    void free_png_desc(png_descp desc);
#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef __cplusplus
namespace code3c
{
    struct Pixel
    {
        int x, y;
        unsigned long color;
        unsigned char alpha;
    };
    
    class PixelMap
    {
        int m_width, m_height;
        
        uint32_t m_size;
        Pixel* m_pixels;
    public:
        PixelMap(int width, int height);
        PixelMap(const PixelMap& map);
        virtual ~PixelMap();
        
        virtual int height() const final;
        virtual int width() const final;
        virtual uint32_t size() const final;
        
        /**
         * Resize the Pixelmap according to the specified scale.
         * @param scale the resizing scale
         */
        virtual PixelMap resize(float scale) const;
        /**
         * Resize the Pixelmap to another width x height.
         *
         * @param width New width to apply (put 0 to remain unchanged)
         * @param height New height to apply (put 0 to remain unchanged)
         */
        virtual PixelMap resize(int width, int height) const;
        
        PixelMap& operator =(const PixelMap& map);
        
        Pixel& operator[](int i);
        Pixel& operator[](int x, int y);
        Pixel operator[](int i) const;
        Pixel operator[](int x, int y) const;
        
        static PixelMap loadFromPNG(const char* pngfile) noexcept(false);
        static void saveInPng(const PixelMap& map, FILE* dest);
    };
}
#endif
#endif // HH_LIB_PIXELMAP
