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

#define CODE3C_MODEL_WB 0   /*< White and Black model: 1bit per pattern      */
#define CODE3C_MODEL_WB2C 1 /*< White, Black and 2 colours: 2bit per pattern */
#define CODE3C_MODEL_WB6C 2 /*< White, Black and 6 colours: 6bit per pattern */

#define CODE3C_COLORMODE_WB     1, 1   /*< WHITE AND BLACK: 1bit per pattern  */
#define CODE3C_COLORMODE_WB2C   2, 3   /*< WHITE, BLACK AND TWO COLORS: 2bits */
#define CODE3C_COLORMODE_WB6C   3, 7   /*< WHITE, BLACK AND SIX COLORS: 3bits */

#define CODE3C_ERRLVL_A    0 // 14%
#define CODE3C_ERRLVL_B    1 // 33%

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
                    C3CRC("3ccode-wb.png"),
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
                    C3CRC("3ccode-wb2c.png"),
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
                    C3CRC("3ccode-wb6c.png"),
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

    class Code3C
    {
    protected:
        class data : public mat8_t
        {
            friend class Code3C;

            Code3C*  m_parent;
            Hamming* m_hamming;

            explicit data(Code3C* parent);
            data(Code3C* parent, const mat8_t& in_data);
            explicit data(const data& obj);
            ~data() override;
        public:
            /**
             * Get byte at the current index (rawdata only)
             * @param index
             * @return
             */
            char8_t getByte(size_t index) const;

            /**
             * Get byte at the current index (rawdata only)
             * @param index
             * @return
             */
            inline char8_t operator[](size_t index) const
            { return getByte(index); }

            /**
             * Get the mask to apply
             * @return
             */
            inline unsigned mask() const
            { return code3c_models[m_parent->m_desc].mask; }

            /**
             * Get the number of bits coded by each "pixel"
             * @return
             */
            inline unsigned bitl() const
            { return code3c_models[m_parent->m_desc].bitl; }

            /**
             * Get the size of all the data's segment, including error bytes segment.
             * The return value has 'byte' as unit (<code>8 * size()</code> to get
             * size in bit).
             * @return the complete size of the 3c-code buffer.
             */
            size_t size() const;

            /**
             * Get the size of the data-exclusive segment. The return value has 'byte'
             * as unit (<code>8 * dataSegSize()</code> to get size in bit).
             * @return the size of the data segment.
             */
            size_t dataSegSize() const;

            /**
             * Get the size of the error bytes segment. The return value has 'byte' as
             * unit (<code>8 * errSegSize()</code> to get size in bit).
             * @return the size of the error bytes segment
             */
            size_t errSegSize() const;
        } *m_data = nullptr;

        uint8_t m_errmodel  = CODE3C_ERRLVL_A;   // default value
        uint8_t m_huffmodel = CODE3C_HUFFMAN_NO; // default value

        uint8_t m_desc = CODE3C_MODEL_WB2C;
        uint8_t m_dim  = 0;

        char8_t* m_rawdata;
        size_t m_datalen;
        
        const char* m_logo;

        Drawer* m_drawer;

        struct header final
        {
            uint8_t desc, err, huff;
            size_t dlen;

            uint32_t meta_dlen_bitl;     /*< dlen   length (bit) */
            uint32_t meta_head_bitl = 6; /*< header length (bit) */
            uint32_t meta_full_bitl = meta_head_bitl+meta_dlen_bitl;
            char8_t operator[](size_t i) const;
            void from_buffer(char8_t * buf, size_t len);
        } m_header;
    public:
        /**
         *
         */
        explicit Code3C(const char* utf8buf);
        /**
         *
         * @param utf8buf
         * @param buflen
         */
        Code3C(const char* utf8buf, size_t buflen);
        /**
         *
         * @param in_data
         */
        Code3C(const mat8_t& in_data);
        /**
         * Copy constructor.
         * @param code3C the object to copy
         */
        Code3C(const Code3C& code3C);
        virtual ~Code3C() noexcept;

        // #### Step by Step (in order) #### //

        /**
         * Defines the 3C-Code model to apply. Differents values are available:
         * <ul>
         *  <li>CODE3C_MODEL_WB (0): 1bit per pattern</li>
         *  <li>CODE3C_MODEL_WB2C (1): 2bit per pattern</li>
         *  <li>CODE3C_MODEL_WB6C (2): 3bit per pattern</li>
         * </ul>
         *
         * @version 1.0.0-RC
         * @warning Mandatory function (even if the default value is WB2C, it's
         * prefearable to explicitly specfied the 3C-Code model)
         * @param model
         */
        void setModel(uint8_t model);

        /**
         * Defines the error model to apply to the 3C-Code. Differents values are
         * available. For the current version 1.0.0-rc, two levels exists:
         * <ul>
         *  <li>CODE3C_ERRLVL_A (0) with 14% error coverage</li>
         *  <li>CODE3C_ERRLVL_B (1) with 33% error coverage</li>
         * </ul>
         *
         * @version 1.0.0-RC
         * @note Per default, the model is ERRLVL_A (with 14% error coverage)
         * @param model the Hamming model identifier
         */
        void setErrorModel(uint8_t model);

        /**
         * Defines the compression model to apply to the 3C-Code. Differents values
         * are available. For the current version 1.0.0-rc, two pre-built tables exists:
         * <ul>
         *  <li>CODE3C_HUFFMAN_ASCII (0x1)</li>
         *  <li>CODE3C_HUFFMAN_LATIN (0x2)</li>
         * </ul>
         * To remove the current Huffman Table to apply to the 3C-Code, use
         * <code>CODE3C_HUFFMAN_NO (0x0)</code> macro.
         *
         * @version 1.0.0-RC
         * @note Per default, no huffman table is set, meaning no data compression.
         * @param model the Huffman model identifier
         */
        void setHuffmanTable(uint8_t model);

        /**
         * 
         * @param fname 
         */
        void setLogo(const char* fname);
        
        /**
         *
         */
        bool generate();

        // #### Display / Save #### //

        /**
         *
         * @return
         */
        void display() const;

        /**
         * Gets the drawer of the 3C-Code
         * @return the 3C-Code drawer
         */
        Drawer* drawer() const;

        /**
         *
         */
        bool save(const char* dest) const;

        // #### Util functions #### //

        virtual const char* begin() const;
        virtual const char* end() const;

        /**
         * Gets the raw data of the 3C-Code.
         * @return the raw data (UTF-8 encoding per default)
         */
        inline char8_t const* rawdata() const
        { return m_rawdata; }

        /**
         * Gets a char at the specified index from rawdata.
         * @param index The index to get the char from.
         * @return a char from the specified index in rawdata.
         */
        inline char operator[] (size_t index) const
        { return (char) m_rawdata[index]; }

        /**
         * Get the rawdata's length.
         * @return rawdata's length.
         */
        size_t rawSize() const;

        /**
         * Get the size of all the data's segment, including error bytes segment.
         * The return value has 'byte' as unit (<code>8 * size()</code> to get
         * size in bit).
         *
         * @warning method available only if 3C-Code has been generated through
         * <code>Code3C::generate()</code> or using <code>Code3C::Code3C(const mat8_t&)
         * </code> constructor.
         * @return the complete size of the 3c-code buffer.
         */
        inline size_t size() const
        { return dataSegSize() + errSegSize(); }

        /**
         * Get the size of the data-exclusive segment. The return value has 'byte'
         * as unit (<code>8 * dataSegSize()</code> to get size in bit).
         *
         * @warning method available only if 3C-Code has been generated through
         * <code>Code3C::generate()</code> or using <code>Code3C::Code3C(const mat8_t&)
         * </code> constructor.
         * @return the size of the data segment.
         */
        size_t dataSegSize() const;

        /**
         * Get the size of the error bytes segment. The return value has 'byte' as
         * unit (<code>8 * errSegSize()</code> to get size in bit).
         *
         * @warning method available only if 3C-Code has been generated through
         * <code>Code3C::generate()</code> or using <code>Code3C::Code3C(const mat8_t&)
         * </code> constructor.
         * @return the size of the error bytes segment
         */
        size_t errSegSize() const;

        /**
         * Get the 3C-Code dimension.
         *
         * @warning method available only if 3C-Code has been generated through
         * <code>Code3C::generate()</code> or using <code>Code3C::Code3C(const mat8_t&)
         * </code> constructor.
         * @return the 3C-Code dimension descriptor.
         */
        const CODE3C_MODEL_DESC::CODE3C_MODEL_DIMENSION& dimension() const;

        /**
         * Get the 3C-Code model descriptor.
         * @return the 3C-Code model descriptor.
         */
        const CODE3C_MODEL_DESC& model() const;
    };
}

#endif // HH_LIB_3CCODE