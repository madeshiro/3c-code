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
    template < typename _CharT >
    class HuffmanTable;
    class HTFile;


    template < typename _CharT >
    class HuffmanTree
    {
        friend class HuffmanTable<_CharT>;
    public:
        struct Node final
        {
            friend class HuffmanTable<_CharT>;
            friend class HuffmanTree<_CharT>;
        protected:
            Node* m_0 = nullptr;  // Left
            Node* m_1 = nullptr;  // Right
            uint32_t weight;
            _CharT ch;
        public:
            Node() = default;
            Node(_CharT, uint32_t);
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

            explicit inline operator _CharT() const
            { return ch; }

            inline uint32_t get_weight() const
            { return weight; }
        };
    private:
        Node * m_root;

        explicit HuffmanTree(const HuffmanTable<_CharT> &table);
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
        _CharT operator [](uint32_t bseq) const noexcept(false);
    };

    template < typename _CharT >
    class HuffmanTable
    {
        friend class HuffmanTree<_CharT>;
        friend class HTFile;

        friend std::ostream& operator<<(std::ostream&, const HuffmanTable<char8_t>&);

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

        HuffmanTree<_CharT>* m_tree;
        std::map<_CharT, Cell> m_table;

        /**
         *
         * @param table
         */
        explicit HuffmanTable(const std::map<_CharT, Cell> &table);
    public:
        explicit HuffmanTable(const HuffmanTree<_CharT>& tree);

        const Cell& operator [](_CharT c) const;
        _CharT operator [](const char* bits, uint32_t len) const;

        /**
         *
         * @return the table's row number
         */
        uint32_t size() const;
    };
    
    std::ostream& operator<<(std::ostream& os, const HuffmanTable<char8_t>& table);

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

    extern template class HuffmanTree<char8_t>;
    extern template class HuffmanTree<char16_t>;
    extern template class HuffmanTree<char32_t>;

    extern template class HuffmanTable<char8_t>;
    extern template class HuffmanTable<char16_t>;
    extern template class HuffmanTable<char32_t>;
    
    typedef HuffmanTable<char8_t> HuffmanTable8;
    typedef HuffmanTable<char16_t> HuffmanTable16;
    typedef HuffmanTable<char32_t> HuffmanTable32;
    
    typedef HuffmanTree<char8_t> HuffmanTree8;
    typedef HuffmanTree<char16_t> HuffmanTree16;
    typedef HuffmanTree<char32_t> HuffmanTree32;
}

#endif //HH_LIB_HUFFMAN_3CCODE
