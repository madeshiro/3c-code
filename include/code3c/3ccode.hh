/* 3C-CODE Library
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

#define CODE3C_MODEL_1 0 /*< */
#define CODE3C_MODEL_2 1 /*< */
#define CODE3C_MODEL_3 2 /*< */

#define CODE3C_COLORMODE_WB     1, 1   /*< WHITE AND BLACK: 1bit per pattern  */
#define CODE3C_COLORMODE_WB2C   2, 3   /*< WHITE, BLACK AND TWO COLORS: 2bits */
#define CODE3C_COLORMODE_WB6C   3, 7   /*< WHITE, BLACK AND SIX COLORS: 3bits */

namespace code3c
{
    class Code3C
    {
    protected:
        char* m_data;
        HuffmanTable table;
        HuffmanTree tree;
        
        inline static struct CODE3C_MODEL_DESC {
            int error_margin;
            unsigned bitl, mask;
        } models[3] = {
                {
                    7, CODE3C_COLORMODE_WB
                },
                {
                    11, CODE3C_COLORMODE_WB2C
                },
                {
                    16, CODE3C_COLORMODE_WB6C
                }
        };
    public:
        class Code3CMat
        {
            int m_axis_r, m_axis_t; /*< axis "radius", axis "theta" */
            char** m_mat;
            
            CODE3C_MODEL_DESC m_desc;
        public:
            Code3CMat(int size, CODE3C_MODEL_DESC cModelDesc) noexcept(false);
            Code3CMat(const Code3CMat& mat);
            virtual ~Code3CMat();
            
            
            char operator[](int r, int t) const noexcept(false);
            char& operator[](int r, int t) noexcept(false);
            /**
             * Get the mask to apply
             * @return
             */
            inline unsigned mask() const { return m_desc.mask; }
            /**
             * Get the number of bits coded by each "pixel"
             * @return
             */
            inline unsigned bitl() const { return m_desc.bitl; }
        };
        
        
        Code3C(const char* buffer, size_t bufsize, uint32_t model = 2);
        Code3C(const char* utf8str);
        Code3C(const char32_t* unistr);
        Code3C(const Code3C& c3c);
        virtual ~Code3C();
        
        
        virtual void huffman(uint32_t);
        
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