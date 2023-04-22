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
#include "drawer.hh"
#include "huffman.hh"
#include "bitmat.hh"
#include "hamming743.hh"

#define CODE3C_MODEL_1 0 /*< */
#define CODE3C_MODEL_2 1 /*< */
#define CODE3C_MODEL_3 2 /*< */

#define CODE3C_COLORMODE_WB     1, 1   /*< WHITE AND BLACK: 1bit per pattern  */
#define CODE3C_COLORMODE_WB2C   2, 3   /*< WHITE, BLACK AND TWO COLORS: 2bits */
#define CODE3C_COLORMODE_WB6C   3, 7   /*< WHITE, BLACK AND SIX COLORS: 3bits */

#define CODE3C_ERRLVL_A    0 // 14%
#define CODE3C_ERRLVL_B    1 // 33%

#ifdef CODE3C_DEBUG
#define cDebug(x...) printf(x);
#else
#define cDebug(x...)
#endif

namespace code3c
{
    static struct CODE3C_MODEL_DESC { /* NOLINT */
            int model_id;
            unsigned bitl, mask;
            struct CODE3C_MODEL_DIMENSION { /* NOLINT */
                const int rev, absRad, effRad, deltaRad;
                const int axis_t = rev*2, axis_r = effRad/deltaRad;
                const uint32_t capacity =
                        axis_t*axis_r - (3*axis_r) - rev + 2;
            } dimensions[4];
            Hamming* hamming[2] = {
                    new Hamming743(),
                    new Hamming313()
            };
    } code3c_models[3] = {
            {
                    0,  CODE3C_COLORMODE_WB,
                    {
                        {30,  50, 20, 4}, //  30 slices, 20 units, 4 units per data
                        {30,  60, 30, 2}, //  60 slices, 30 units, 2 units per data
                        {60, 100, 60, 2}, // 120 slices, 60 units, 2 units per data
                        {90, 140, 80, 2}  // 180 slices, 80 units, 2 units per data
                    }
            },
            {
                    1, CODE3C_COLORMODE_WB2C,
                    {
                        {30,  50, 20, 4}, //  30 slices, 20 units, 4 units per data
                        {30,  60, 30, 2}, //  60 slices, 30 units, 2 units per data
                        {60, 100, 60, 2}, // 120 slices, 60 units, 2 units per data
                        {90, 120, 80, 2}  // 180 slices, 80 units, 2 units per data
                    }
            },
            {
                    2, CODE3C_COLORMODE_WB6C,
                    {
                        {30,  50, 20, 4}, //  30 slices, 20 units, 4 units per data
                        {30,  60, 30, 2}, //  60 slices, 30 units, 2 units per data
                        {60, 100, 60, 2}, // 120 slices, 60 units, 2 units per data
                        {90, 120, 80, 2}  // 180 slices, 80 units, 2 units per data
                    }
            }
    };

#define CODE3C_MODEL_DESC_1 code3c::code3c_models[CODE3C_MODEL_1];
#define CODE3C_MODEL_DESC_2 code3c::code3c_models[CODE3C_MODEL_2];
#define CODE3C_MODEL_DESC_3 code3c::code3c_models[CODE3C_MODEL_3];
    
    class Code3C
    {
        // TODO Code3CReader x Code3C (not prior)
        // friend class Code3CReader;
    public:
        class Code3CData : public matb
        {
            const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION& m_dimension;
            Code3C* m_parent;

            Hamming* m_hamming;
        public:
            static const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION&
                getdim(const CODE3C_MODEL_DESC&, uint32_t, int err);
            
            Code3CData(Code3C* parent, const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION&)
                noexcept(false);
            Code3CData(const Code3CData& mat);
            ~Code3CData() override = default;
            
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
            inline unsigned mask() const { return m_parent->m_desc.mask; }
            /**
             * Get the number of bits coded by each "pixel"
             * @return
             */
            inline unsigned bitl() const { return m_parent->m_desc.bitl; }
            
            inline const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION&
            getDimension() const {
                return m_dimension;
            }
        };
    protected:
        // Buffers
        char* m_data;
        size_t m_datalen;

        int m_errmodel;
        CODE3C_MODEL_DESC& m_desc;
        
        Code3CData m_dataMat;
        
        // HuffmanTable m_huftable;
        // HuffmanTree m_huftree;
    public:
        Code3C(const char* buffer, size_t bufsize, uint32_t model,
               int err = CODE3C_ERRLVL_A);
        Code3C(const char* utf8str, uint32_t model, int err = CODE3C_ERRLVL_A);
        Code3C(const char32_t* unistr, uint32_t model, int err = CODE3C_ERRLVL_A);
        Code3C(const Code3C& c3c);
        virtual ~Code3C();
        
        /**
         *
         */
        // virtual void huffman(uint32_t);
        
        /**
         *
         * @return
         */
        Drawer* draw() const;
        
        /**
         * Get the raw data.
         * @warning data may be different that the original due to utf8 transformation
         * @return raw data
         */
        char const * data() const;
    };
}

#endif // HH_LIB_3CCODE