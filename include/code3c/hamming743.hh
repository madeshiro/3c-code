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
#ifndef HH_LIB_HAMMING743
#define HH_LIB_HAMMING743
#include "bitmat.hh"

namespace code3c
{
    typedef mat<bool> matbase2;
    typedef vec<bool> vecbase2;

    class Hamming743
    {
        class hword final
        {
            char m_word;
            matbase2 m_matb;

            static matbase2 wtom(char w, uint blen);
            static char mtow(const matbase2&);
        public:
            hword();
            hword(char word, bool is_x);
            ~hword() = default;

            /**
             * Get the parity of the corrector code
             * @return
             */
            bool parity() const;

            /**
             * Get initial message (4bits)
             * @return
             */
            char x() const;

            /**
             * Get encode message (8bits)
             * @return
             */
            char m() const;

            /**
             * Get control bits sequence (4bits)
             * @return
             */
            char ctrl() const;
        } m_hword[2];
    public:
        static matbase2 G();
        static matbase2 H();

        explicit Hamming743(char word2x4);
        explicit Hamming743(char word4ctrl4[2]);

        /**
         * Get the lowest 4bit of the 8bit word
         * @return
         */
        const hword& Lword() const;

        /**
         * Get the higher 4bit of the 8bit word
         * @return
         */
        const hword& Hword() const;
    };
}

#endif //HH_LIB_HAMMING743
