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

#define CODE3C_MODEL_WB 0 /*< */
#define CODE3C_MODEL_WB2C 1 /*< */
#define CODE3C_MODEL_WB6C 2 /*< */

#define CODE3C_COLORMODE_WB     1, 1   /*< WHITE AND BLACK: 1bit per pattern  */
#define CODE3C_COLORMODE_WB2C   2, 3   /*< WHITE, BLACK AND TWO COLORS: 2bits */
#define CODE3C_COLORMODE_WB6C   3, 7   /*< WHITE, BLACK AND SIX COLORS: 3bits */

#define CODE3C_ERRLVL_A    0 // 14%
#define CODE3C_ERRLVL_B    1 // 33%

#ifdef CODE3C_DEBUG
#define cDebug(x...) printf("[debug] " x)
#else
#define cDebug(x...)
#endif

namespace code3c
{
    static struct CODE3C_MODEL_DESC { /* NOLINT */
            int model_id;
            unsigned bitl, mask;
            const char* default_logo;
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
                    "resources/3ccode-wb.png",
                    {
                        {30,  60, 30, 3}, /** 60 slices, 30 units, 3 units per data
                                           *  dimension 120x120 (pu²)
                                           *  header 20 bits
                                           *  542 bits (67 B)
                                           */
                        {30,  60, 30, 2}, /** 60 slices, 30 units, 2 units per data
                                           *  dimension 120x120 (pu²)
                                           *  header 30 bits
                                           *  827 bits (103 B)
                                           */
                        {90, 100, 60, 3}, /** 180 slices, 60 units, 3 units per data
                                           *  dimension 200x200 (pu²)
                                           *  header 40 bits
                                           *  3,452 bits (431 B)
                                           */
                        {90, 140, 90, 2}  /** 180 slices, 90 units, 2 units per data
                                           *  dimension 280x280 (pu²)
                                           *  header 90 bits
                                           *  7,877 bits (984 B)
                                           */
                    }
            },
            {
                    1, CODE3C_COLORMODE_WB2C,
                    "resources/3ccode-wb2c.png",
                    {
                        {30,  50, 24, 3}, /** 60 slices, 24 units, 3 units per data
                                           *  dimension 100x100 (pu²)
                                           *  header 16 bits
                                           *  856 bits (107 B)
                                           */
                        {30,  60, 30, 2}, /** 60 slices, 20 units, 2 units per data
                                           *  dimension 120x120 (pu²)
                                           *  header 30 bits
                                           *  1,654 bits (206 B)
                                           */
                        {90, 100, 60, 3}, /** 180 slices, 60 units, 3 units per data
                                           *  dimension 200x200 (pu²)
                                           *  header 40 bits
                                           *  6,904 bits (863 B)
                                           */
                        {90, 140, 90, 2}  /** 180 slices, 90 units, 2 units per data
                                           *  dimension 280x280 (pu²)
                                           *  header 90 bits
                                           *  15,754 bits (1,969 B)
                                           */
                    }
            },
            {
                    2, CODE3C_COLORMODE_WB6C,
                    "resources/3ccode-wb6c.png",
                    {
                        {30,  50, 21, 3}, /** 60 slices, 21 units, 3 units per data
                                           *  dimension 100x100 (pu²)
                                           *  header 14 bits
                                           *  1,113 bits (139 B)
                                           */
                        {30,  60, 30, 2}, /** 60 slices, 30 units, 2 units per data
                                           *  dimension 120x120 (pu²)
                                           *  header 30 bits
                                           *  2,481 bits (310 B)
                                           */
                        {90, 100, 60, 3}, /** 180 slices, 60 units, 3 units per data
                                           *  dimension 200x200 (pu²)
                                           *  header 40 bits
                                           *  10,356 bits (1,294 B)
                                           */
                        {90, 140, 90, 2}  /** 180 slices, 90 units, 2 units per data
                                           *  dimension 280x280 (pu²)
                                           *  header 90 bits
                                           *  23,631 bits (2,953 B)
                                           */
                    }
            }
    };

#define CODE3C_MODEL_DESC_1 code3c::code3c_models[CODE3C_MODEL_WB];
#define CODE3C_MODEL_DESC_2 code3c::code3c_models[CODE3C_MODEL_WB2C];
#define CODE3C_MODEL_DESC_3 code3c::code3c_models[CODE3C_MODEL_WB6C];
    
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
            const HuffmanTable* m_huffman;

            char* m_huffdata;
            uint32_t m_hufflen;
        public:
            static const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION&
                getdim(const CODE3C_MODEL_DESC&, uint32_t, int err);
            
            Code3CData(Code3C* parent, const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION&)
                noexcept(false);
            Code3CData(const Code3CData& mat);
            ~Code3CData() override;
            
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
        int m_huffmodel;
        const CODE3C_MODEL_DESC& m_desc;
        const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION& m_dim;

        struct header final
        {
            uint64_t id, err, huff; /*< identifiers */
            size_t   dlen;          /*< data length */

            uint32_t meta_dlen_bitl;     /*< dlen   length (bit) */
            uint32_t meta_head_bitl = 6; /*< header length (bit) */
            uint32_t meta_full_bitl = meta_head_bitl+meta_dlen_bitl;
            char operator[](uint32_t i) const;
        } m_header;

        Code3CData m_dataMat;
    public:
        Code3C(const char* buffer, size_t bufsize, uint32_t model,
               int err = CODE3C_ERRLVL_A, int huff = CODE3C_HUFFMAN_NO);
        Code3C(const char* utf8str, uint32_t model,
               int err = CODE3C_ERRLVL_A, int huff = CODE3C_HUFFMAN_NO);
        Code3C(const char32_t* unistr, uint32_t model,
               int err = CODE3C_ERRLVL_A, int huff = CODE3C_HUFFMAN_NO);
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