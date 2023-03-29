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
#ifndef HH_LIB_3CCODE
#define HH_LIB_3CCODE
#include <cstddef>
#include <cstdint>
#include "displaymanager.hh"
#include "huffman.hh"
#include "bitmat.hh"

#define CODE3C_MODEL_1 0 /*< */
#define CODE3C_MODEL_2 1 /*< */
#define CODE3C_MODEL_3 2 /*< */

#define CODE3C_COLORMODE_WB     1, 1   /*< WHITE AND BLACK: 1bit per pattern  */
#define CODE3C_COLORMODE_WB2C   2, 3   /*< WHITE, BLACK AND TWO COLORS: 2bits */
#define CODE3C_COLORMODE_WB6C   3, 7   /*< WHITE, BLACK AND SIX COLORS: 3bits */

namespace code3c
{
    static struct CODE3C_MODEL_DESC {
            int error_margin;
            unsigned bitl, mask;
            struct CODE3C_MODEL_DIMENSION {
                int rev, absRad, effRad, deltaRad;
                int axis_t = rev*2, axis_r = effRad/deltaRad;
            } dimensions[3];
    } code3c_models[3] = {
            {
                    7,  CODE3C_COLORMODE_WB,
                    {
                        {90, 15, 10, 1},
                        {180, 40, 26, 2},
                        {360, 90, 60, 4}
                    }
            },
            {
                    11, CODE3C_COLORMODE_WB2C,
                    {
                        {90, 15, 10, 1},
                        {180, 40, 26, 2},
                        {360, 90, 60, 4}
                    }
            },
            {
                    16, CODE3C_COLORMODE_WB6C,
                    {
                        {90, 15, 10, 1},
                        {180, 40, 26, 2},
                        {360, 90, 60, 4}
                    }
            }
    };

#define CODE3C_MODEL_DESC_1 code3c::code3c_models[CODE3C_MODEL_1];
#define CODE3C_MODEL_DESC_2 code3c::code3c_models[CODE3C_MODEL_2];
#define CODE3C_MODEL_DESC_3 code3c::code3c_models[CODE3C_MODEL_3];
    
    template < CODE3C_MODEL_DESC &cModelDesc >
    /**
     *
     * @tparam cModelDesc
     */
    class Code3C
    {
        // TODO Code3CReader x Code3C (not prior)
        // friend class Code3CReader;
        // template < CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION model_dimension >
        // Code3C(matb<model_dimension.axis_t, model_dimension.axis_r>);
    public:
        template < int axis_t, int axis_r >
        /**
         *
         * @tparam axis_t
         * @tparam axis_r
         */
        class Code3CData : public matb<axis_t, axis_r>
        {
        public:
            Code3CData(char* data) noexcept(false);
            Code3CData(const Code3CData& mat);
            ~Code3CData() = default;
            
            /**
             * Get the byte at the specified cursor pos.
             * @param cursor the position of the byte to get. The cursor's value must
             * be included in the interval [ 0, Code3CDATA::size() [
             * @return a byte at the required position
             */
            char getByte(uint32_t cursor) const;
            
            /**
             * Get the size of all the data's segment, including error bytes segment.
             * The return value has 'byte' as unit (<code>8 * size()</code> to get
             * size in bit).
             * @return the complete size of the 3c-code buffer.
             */
            uint32_t size() const;
            
            /**
             * Get the size of the data-exclusive segment. The return value has 'byte'
             * as unit (<code>8 * dataSegSize()</code> to get size in bit).
             * @return the size of the data segment.
             */
            uint32_t dataSegSize() const;
            
            /**
             * Get the size of the error bytes segment. The return value has 'byte' as
             * unit (<code>8 * errSegSize()</code> to get size in bit).
             * @return the size of the error bytes segment
             */
            uint32_t errSegSize() const;
            
            /**
             * Get the mask to apply
             * @return
             */
            inline unsigned mask() const { return cModelDesc.mask; }
            /**
             * Get the number of bits coded by each "pixel"
             * @return
             */
            inline unsigned bitl() const { return cModelDesc.bitl; }
        };
    protected:
        char* m_data;
        
        /**
         * Code3CData&lt;axis_t, axis_r&gt;
         */
        void *m_dataMat;
        
        HuffmanTable m_huftable;
        HuffmanTree m_huftree;
    public:
        Code3C(const char* buffer, size_t bufsize, uint32_t model = 2);
        Code3C(const char* utf8str);
        Code3C(const char32_t* unistr);
        Code3C(const Code3C& c3c);
        virtual ~Code3C();
        
        /**
         *
         */
        virtual void huffman(uint32_t);
        
        /**
         *
         * @return
         */
        DisplayManager* draw();
        
        /**
         * Get the raw data.
         * @warning data may be different that the original due to utf8 transformation
         * @return raw data
         */
        char const * data() const;
    };
}

#endif // HH_LIB_3CCODE