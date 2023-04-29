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

#define CODE3C_HUFFMAN_NO       0x0
#define CODE3C_HUFFMAN_ASCII    0x1 /*< corpus US/UK text    */
#define CODE3C_HUFFMAN_LATIN    0x2 /*< corpus latin text    */
#define CODE3C_HUFFMAN_BINARY   0x3 /*< binary compression   */
#define CODE3C_HUFFMAN_JAPANESE 0x4 /*< corpus japanese text */

namespace code3c
{
    class HuffmanTree;

    
    class HuffmanTable
    {
        const HuffmanTree* m_tree;
    public:


        static HuffmanTable* fromFile(const char* htfFile);
    };
    
    class HuffmanTree
    {
    };
}

#endif //HH_LIB_HUFFMAN_3CCODE
