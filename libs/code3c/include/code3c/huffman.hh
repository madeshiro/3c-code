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
#ifndef HH_LIB_HUFFMAN_3CCODE
#define HH_LIB_HUFFMAN_3CCODE
#include "3ccodelib.hh"
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <map>

#define CODE3C_HUFFMAN_NO       0x0
#define CODE3C_HUFFMAN_ASCII    0x1 /*< corpus US/UK text    */
#define CODE3C_HUFFMAN_LATIN    0x2 /*< corpus latin text    */
#define CODE3C_HUFFMAN_BINARY   0x3 /*< binary compression   */
#define CODE3C_HUFFMAN_CNJP     0x4 /*< corpus CN/JP text    */

namespace code3c
{
    union huff_char_t
    {
        char8_t ch8;
        char16_t ch16;
        char32_t ch32;
    };

    class HuffmanTable;
    class HTFile;

    class HuffmanTree
    {
        friend class HuffmanTable;
    public:
        struct Node final
        {
            friend class HuffmanTable;
            friend class HuffmanTree;
        protected:
            Node* m_0 = nullptr;  // Left
            Node* m_1 = nullptr;  // Right
            uint32_t weight;
            huff_char_t ch;
        public:
            Node() = default;
            Node(huff_char_t, uint32_t);
            Node(const Node&);
            ~Node();

            /**
             *
             * @param _bit
             * @return
             */
            inline const Node* operator[](char _bit) const
            { return static_cast<const Node*>(_bit & 1 ? m_1 : m_0); }

            /**
             * @return True if the Node is a leaf, false otherwise
             */
            explicit inline operator bool() const
            { return m_0 == nullptr && m_1 == nullptr; }

            explicit inline operator huff_char_t() const
            { return ch; }

            explicit inline operator char8_t () const
            { return ch.ch8; }

            explicit inline operator char16_t () const
            { return ch.ch16; }

            explicit inline operator char32_t () const
            { return ch.ch32; }

            inline uint32_t get_weight() const
            { return weight; }
        };
    private:
        Node * m_root;

        explicit HuffmanTree(const HuffmanTable &table);
    public:
        explicit HuffmanTree(HuffmanTree::Node * root);
        HuffmanTree(HuffmanTree::Node ** leaves, uint32_t len);
        HuffmanTree(const HuffmanTree& tree);
        ~HuffmanTree();

        /**
         * A 32bit huffman (invert) sequence
         * @param bseq
         * @return
         */
        huff_char_t operator [](uint32_t bseq) const noexcept(false);
    };

    class HuffmanTable
    {
        friend class HuffmanTree;
        friend class HTFile;

        // 8bit char print
        friend std::ostream& operator<<(std::ostream&, const HuffmanTable&);

        class Cell final
        {
            uint32_t m_bitl;
            char* m_bits;
        public:
            Cell(char* bits, uint32_t bitl);
            Cell(const Cell& cell);
            ~Cell();

            uint32_t bitl() const;
            char operator[](uint32_t) const;

            bool equal(const char*, uint32_t);

            inline char* begin() const
            { return m_bits; }
            inline char* end() const
            { return &m_bits[m_bitl]; }

            inline const char* bits() const
            { return m_bits; }
        };

        HuffmanTree* m_tree;
        // use per default max capacity char
        std::map<char32_t, Cell> m_table;

        /**
         *
         * @param table
         */
        explicit HuffmanTable(const std::map<char32_t, Cell> &table);
    public:
        explicit HuffmanTable(const HuffmanTree& tree);

        const Cell& operator [](huff_char_t c) const;
        huff_char_t operator [](const char* bits, uint32_t len) const;

        /**
         *
         * @return the table's row number
         */
        uint32_t size() const;

        template < typename _CharT >
        uint32_t lengthOf(const _CharT* str,
                          size_t slen,
                          uint32_t *_out_bitl=nullptr) const;

        template < typename _CharT >
        char8_t* encode(const _CharT* buf, uint32_t slen, uint32_t* _out_bitl) const;

        /**
         * Count the number of chars that were coded in the Huffman compressed sequence.
         * @param hbuf the encode data
         * @param bitl the number of bits in the buffer
         * @return The number of encoded chars
         */
        uint32_t countChars(const char8_t * hbuf, size_t bitl) const;

        template < typename _CharT >
        _CharT* decode(const char8_t* hbuf, uint32_t bitl, uint32_t* _out_len) const;
    };

    extern template uint32_t HuffmanTable::lengthOf<char8_t>(const char8_t*, size_t,
            uint32_t*) const;
    extern template uint32_t HuffmanTable::lengthOf<char16_t>(const char16_t*, size_t,
            uint32_t*) const;
    extern template uint32_t HuffmanTable::lengthOf<char32_t>(const char32_t*, size_t,
            uint32_t*) const;

    extern template char8_t* HuffmanTable::encode<char8_t>(const char8_t *, uint32_t,
            uint32_t *) const;
    extern template char8_t* HuffmanTable::encode<char16_t>(const char16_t *, uint32_t,
            uint32_t *)const;
    extern template char8_t* HuffmanTable::encode<char32_t>(const char32_t *, uint32_t,
            uint32_t *) const;

    extern template char * HuffmanTable::decode<char>(const char8_t* hbuf,
            uint32_t bitl, uint32_t* _out_len) const;
    extern template char8_t * HuffmanTable::decode<char8_t>(const char8_t* hbuf,
            uint32_t bitl, uint32_t* _out_len) const;
    extern template char16_t * HuffmanTable::decode<char16_t>(const char8_t* hbuf,
            uint32_t bitl, uint32_t* _out_len) const;
    extern template char32_t * HuffmanTable::decode<char32_t>(const char8_t* hbuf,
            uint32_t bitl, uint32_t* _out_len) const;

    // 8 bit char table print
    std::ostream& operator<<(std::ostream& os, const HuffmanTable& table);

    class HTFile
    {
    protected:
        // Structs
        struct htf_info
        {
            uint8_t char_type;  // 2 bits
            uint8_t entry_bit;  // 1 bit/experimental
            uint8_t length_max; // 5 bits

            uint8_t to_byte() const;
            static htf_info from_byte(uint8_t);
        };

        struct htf_header
        {
            uint32_t seql;
            htf_info info;
        } m_header;

        struct segment
        {
            huff_char_t ch;
            uint8_t len;
            char seq[4];

            /**
             * Get amount of bits used to encode the char using Huffman Code
             * @return
             */
            inline uint8_t bitl() const
            { return len; }

            /**
             *
             * @return a sequence of bits represented by '0' and '1' chars
             */
            char* toBits() const;
        } *m_segments;

        // Fields
        uint32_t m_segCount;
        char*    m_buf;
        size_t   m_lbuf;

        /**
         *
         * @param buffer
         * @param len
         */
        void init_from_buffer(const char* buffer, size_t len);

        // Constructors
        explicit HTFile(FILE* infile);               // In
        HTFile(const char* buffer, size_t len);      // In
        explicit HTFile(const HuffmanTable& table);  // Out
        ~HTFile();

        // Methods
        char* write(size_t *_out_len)  const;
        bool  write(FILE* outfile) const;
        HuffmanTable* read() const;

        uint8_t charSize() const;
        uint8_t entryBit() const;
        uint8_t seqMaxLength() const;
        uint32_t countSegments() const;

        const segment& operator[](size_t) const noexcept(false);

        const segment* begin() const;
        const segment* end() const;
    public:
        /**
         * magic_number: identifier of a HT file 0x7f + "HTF"
         */
        static constexpr const char magic_number[4] = {0x7f, 'H', 'T', 'F'};

        // Input methods
        static HuffmanTable* fromFile(const char* fname);
        static HuffmanTable* fromBuffer(const char* buf, size_t buflen);

        // Output methods
        static bool toFile(const char* dest, const HuffmanTable& table);
        static char* toBuffer(const HuffmanTable& table, size_t* _out_len = nullptr);
    };

    template < typename _CharT >
    uint32_t HuffmanTable::lengthOf(const _CharT* str,
                                    size_t slen,
                                    uint32_t *_out_bitl) const
    {
        uint32_t bitl(0);
        for (size_t i(0); i < slen; i++)
            bitl += m_table.at((char32_t)str[i]).bitl();

        if (_out_bitl) *_out_bitl = bitl;
        return bitl/8 + (bitl%8 ? 1 : 0);
    }

    template < typename _CharT >
    char8_t* HuffmanTable::encode(const _CharT * buf,
                                  uint32_t slen,
                                  uint32_t *_out_bitl) const
    {
        // Huffman Buffer length (byte), length in bit
        uint32_t bufl, bitl;
        bufl = lengthOf(buf, slen, &bitl);
        char8_t * hbuf = new char8_t[bufl+1];
        std::memset(hbuf, 0, bufl+1);


        for (uint32_t i(0), ibit(0); i < slen; i++)
        {
            auto cell = m_table.at(static_cast<char32_t>(buf[i]));
            for (auto& ch : cell)
            {
                char8_t* c = &hbuf[ibit/8];
                ch = (ch=='1')&1;
                ch <<= (7-ibit%8);
                *c |= ch;
                ibit++;
            }
        }

        if (_out_bitl) *_out_bitl = bitl;
        return hbuf;
    }

    template < typename _CharT >
    _CharT* HuffmanTable::decode(const char8_t *hbuf, uint32_t bitl,
                                 uint32_t *_out_len) const
    {
        uint32_t bufl = countChars(hbuf, bitl);
        _CharT* buf = new _CharT[bufl+1];
        buf[bufl] = '\0';

        uint32_t i(0), ibit(0);
        char c, b;

        HuffmanTree::Node* node = m_tree->m_root;
        while (ibit < bitl)
        {
            c = hbuf[ibit / 8];
            b = (c >> (7-(ibit%8)))&1;
            node = (1 == b ? node->m_1 : node->m_0);
            ibit++;
            if (*node)
            {
                buf[i] = (_CharT) node->ch.ch32;
                node = m_tree->m_root;
                i++;
            }
        }

        if (_out_len) *_out_len = i;
        return buf;
    }
}

#endif //HH_LIB_HUFFMAN_3CCODE
