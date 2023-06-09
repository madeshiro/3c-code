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

    class Hamming
    {
    public:
        virtual const matbase2& G() const = 0;
        virtual const matbase2& H() const = 0;

        class hword final
        {
        public:
            typedef char        hword_t;
            typedef char16_t    lhword_t;
        private:
            hword_t m_hword;
            matbase2 m_matb;
            matbase2 const & m_g;
            matbase2 const & m_h;
        public: // static functions
            static matbase2 wtom(hword_t w, uint32_t blen);
            static hword_t mtow(const matbase2&);
            static hword_t xptow(char x, char p, uint32_t n);
            static lhword_t wtoxp(char w, uint32_t n,
                                  char*x = nullptr, char*p = nullptr);
        public: // class functions
            explicit hword(const Hamming& hamm);
            /**
             *
             * @param x the initial message
             */
            hword(hword_t x, const Hamming& hamm);
            /**
             *
             * @param x the initial message
             * @param p the parity control bits
             */
            hword(hword_t x, hword_t p, const Hamming& hamm);
            /**
             *
             * @param _vec the encoded message 'm'
             */
            hword(const matbase2& _vec, const Hamming& hamm);

            /**
             *
             * @param _vec the encoded message 'm'
             */
            hword(const matbase2& _vec, const matbase2& G, const matbase2& H);
            /**
             * Copy constructor
             * @param _hword the hword to copy
             */
            hword(const hword& _hword);
            ~hword() = default;

            /**
             * Get the parity of the corrector code
             * @return
             */
            bool parity() const;

            /**
             * Get the initial message (k bits)
             * @return
             */
            hword_t x() const;

            /**
             * Get full encode message (n bits)
             * @return
             */
            hword_t m() const;

            /**
             * Get parity bits (n-k bits)
             * @return
             */
            hword_t p() const;

            /**
             * Get the position of the bit in error. If there is no error,
             * the method return 0.
             * @return the position of the bit in error, or 0 if unexisting.
             */
            hword_t err() const;

            /**
             * Get the encoded vector
             * @return
             */
            const matbase2& getVector() const;

            /**
             *
             * @param pos (pos+1)
             * @return
             */
            hword invert_bit(uint32_t pos) const;

            inline uint32_t dim_n() const { return m_g.n(); }
            inline uint32_t dim_k() const { return m_g.m(); }

            /**
             * Operator calling invert_bit(uint32_t)
             * @return
             */
            inline hword operator <=>(uint32_t pos) const
            { return invert_bit(pos);}

            explicit inline operator const matbase2&() const
            { return m_matb; }
            explicit inline operator hword_t() const
            { return m_hword; }

            hword& operator =(const hword&);
            bool operator ==(const hword&) const;
            bool operator !=(const hword&) const;
        };
    private:
        size_t m_hwordsl;
        hword** m_hwords;
    public:
        Hamming();
        Hamming(const Hamming& hamm);
        virtual ~Hamming();

        virtual inline uint32_t dim_n() const final { return G().n(); }
        virtual inline uint32_t dim_k() const final { return G().m(); }

        inline size_t bitl() const
        { return m_hwordsl * dim_n(); }

        inline size_t xbitl() const
        { return m_hwordsl * dim_k(); }

        inline size_t pbitl() const
        { return m_hwordsl * (dim_n()-dim_k()); }

        hword** begin();
        hword** end();

        virtual Hamming* copy() const = 0;

        /**
         *
         * @param xbuf
         * @param xbitl
         */
        virtual void set_buffer(const char* xbuf, size_t xbytel);

        /**
         *
         * @param xbuf
         * @param mbuf
         * @param xbitl
         */
        virtual void set_buffer(const char* xbuf, const char* mbuf, size_t xbitl);

        char* build_xbuffer(char* xbuffer, size_t* size);
        char* build_pbuffer(char* pbuffer, size_t* size);

        const hword& operator [](size_t _i) const;

        inline size_t length() const
        { return m_hwordsl; }
    };

    /**
     * 14% redundancy
     */
    class Hamming743 : public Hamming
    {
    public:
        static const matbase2& G_();
        static const matbase2& H_();

        const matbase2& G() const override;
        const matbase2& H() const override;

        Hamming743() = default;
        Hamming743(const Hamming743& hamm) = default;

        inline Hamming* copy() const override
        { return new Hamming743(*this); }
    };

    /**
     * 33% redundancy
     */
    class Hamming313 : public Hamming
    {
    public:
        static const matbase2& G_();
        static const matbase2& H_();

        const matbase2& G() const override;
        const matbase2& H() const override;

        Hamming313() = default;
        Hamming313(const Hamming313& hamm) = default;

        inline Hamming* copy() const override
        { return new Hamming313(*this); }
    };
}

#endif //HH_LIB_HAMMING743
