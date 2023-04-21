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
#ifndef HH_LIB_HAMMING13_8_5
#define HH_LIB_HAMMING13_8_5
#include "hamming743.hh"

namespace code3c
{

    class Hamming13_8_5
    {
    public:
        class hword final
        {
            char16_t m_word;
            matbase2 m_matb;

        public:
            static matbase2 wtom(char16_t w, uint blen);
            static char16_t mtow(const matbase2&);

            hword();
            hword(char16_t word, bool is_x);
            ~hword() = default;

            /**
             * Get the parity of the corrector code
             * @return
             */
            bool parity() const;

            /**
             * Get initial message (8bits)
             * @return
             */
            char x() const;

            /**
             * Get encode message (8bits)
             * @return
             */
            char16_t m() const;

            /**
             * Get the vector representing the encode word (13bits)
             * @return
             */
            matbase2 get_mat() const;

            /**
             * Get the position of the error if it exists, 0 otherwise.
             * @return
             */
            int err() const;

            /**
             * Get control bits sequence (5bits)
             * @return
             */
            char ctrl() const;
        };
    private:
        hword m_hword;
    public:
        static matbase2 G();
        static matbase2 H();

        explicit Hamming13_8_5(char word8);
        explicit Hamming13_8_5(char16_t word8ctrl5);

        /**
         * Get the hword
         * @return
         */
        const hword& word() const;
    };
}

#endif //HH_LIB_HAMMING13_8_5
