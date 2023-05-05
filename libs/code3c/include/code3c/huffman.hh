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

        HuffmanTree& truncate(uint32_t floor);
        HuffmanTree  truncateAt(uint32_t floor) const;

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

            inline char* begin()
            { return m_bits; }
            inline char* end()
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
    };

    // 8 bit char table print
    std::ostream& operator<<(std::ostream& os, const HuffmanTable& table);

    /* TODO HTFile
    class HTFile
    {
    protected:
        // Structs
        struct htf_header
        {
            char* title;
            uint32_t seql, titlel;
            uint8_t info;
        } m_header;

        // Fields
        FILE* m_fin;

        // Constructors
        HTFile(const char* fname, bool do_write) noexcept(false);
        ~HTFile();

        // Methods

    public:
        // Input methods
        static HuffmanTable* fromFile();
        static HuffmanTable* fromBuffer(const char* buf);

        // Output methods
        static bool toFile(const char* dest, const HuffmanTable& table);
        static char* toBuffer(const HuffmanTable& table);
    };
    */
}

#endif //HH_LIB_HUFFMAN_3CCODE
