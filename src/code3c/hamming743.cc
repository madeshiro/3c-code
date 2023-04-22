#include <stdexcept>
#include "code3c/hamming743.hh"

namespace code3c
{
#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-bool-literals"
    const matbase2& Hamming743::G_()
    {
        static matbase2 _G(7, 4, new bool*[7] { /* NOLINT */
            new bool[4] {1, 1, 0, 1},
            new bool[4] {1, 0, 1, 1},
            new bool[4] {1, 0, 0, 0},
            new bool[4] {0, 1, 1, 1},
            new bool[4] {0, 1, 0, 0},
            new bool[4] {0, 0, 1, 0},
            new bool[4] {0, 0, 0, 1}
        });

        return _G;
    }

    const matbase2& Hamming743::G() const
    {
        return G_();
    }

    const matbase2& Hamming743::H_()
    {
        static matbase2 _H(3, 7, new bool*[3] { /* NOLINT */
            new bool[7] {0, 0, 0, 1, 1, 1, 1},
            new bool[7] {0, 1, 1, 0, 0, 1, 1},
            new bool[7] {1, 0, 1, 0, 1, 0, 1}
        });

        return _H;
    }

    const matbase2& Hamming743::H() const
    {
        return H_();
    }

    const matbase2& Hamming313::G_()
    {
        static matbase2 _G(3, 1, new bool*[3] { /* NOLINT */
            new bool(1),
            new bool(1),
            new bool(1)
        });

        return _G;
    }

    const matbase2& Hamming313::G() const
    {
        return G_();
    }

    const matbase2& Hamming313::H_()
    {
        static matbase2 _H(2, 3, new bool*[2] { /* NOLINT */
            new bool[3] {0,1,1},
            new bool[3] {1,0,1}
        });

        return _H;
    }

    const matbase2& Hamming313::H() const
    {
        return H_();
    }

    matbase2 Hamming::hword::wtom(hword_t w, uint blen)
    {
        matbase2 _wtom(blen, 1);
        for (uint i(0); i < blen; i++)
            _wtom[i, 0] = static_cast<bool>((w>>(blen-1-i)) & 0b1);
        return _wtom;
    }

    char Hamming::hword::mtow(const matbase2 & m)
    {
        char _mtow(0);
        for (int i(0); i < m.n(); i++)
            _mtow |= ((m[i, 0]&0b1) << (m.n()-1-i));
        return _mtow;
    }

    Hamming::hword::hword_t Hamming::hword::xptow(char x, char p, uint n)
    {
        hword_t _hword(0);

        uint pow2(0);
        while (1 << pow2 < n) pow2++;

        for (uint i(0), pos(n); i < n; i++, pos--)
        {
            if (!(pos xor (1 << pow2)))
            {
                _hword |= (p&1) << i;
                p >>= 1;
                pow2--;
            }
            else
            {
                _hword |= (x&1) << i;
                x >>= 1;
            }
        }

        return _hword;
    }

    Hamming::hword::lhword_t Hamming::hword::wtoxp(char w, uint n, char *x, char *p)
    {
        char _x(0), _p(0);

        uint pow2(0), _pi;
        while (1 << pow2 < n) pow2++;
        _pi = pow2;

        for (uint i(0), pos(n), _xi(0); i < n; i++, pos--, w>>=1)
        {
            if (pos == (1 << (pow2-1)))
            {
                _p |= (w&1) << (_pi-pow2);
                pow2--;
            }
            else
            {
                _x |= (w&1) << _xi;
                _xi++;
            }
        }

        if (x) *x = _x;
        if (p) *p = _p;
        return (_x << 8) | _p;
    }

    Hamming::hword::hword(const Hamming &hamm):
            m_hword(0), m_matb(hamm.G().n(), 1), m_g(hamm.G()), m_h(hamm.H()) {}

    Hamming::hword::hword(hword_t x, const Hamming& hamm):
            m_hword(0), m_matb(hamm.G().n(), 1), m_g(hamm.G()), m_h(hamm.H())
    {
        matbase2 xmatb(wtom(x, hamm.G().m()));
        m_matb = m_g*xmatb;
        m_hword = mtow(m_matb);
    }

    Hamming::hword::hword(hword_t x, hword_t p, const Hamming& hamm):
            m_hword(xptow(x,p,hamm.G().n())), m_matb(hamm.G().n(), 1),
            m_g(hamm.G()), m_h(hamm.H())
    {
    }

    Hamming::hword::hword(const matbase2& _vec, const Hamming& hamm):
            m_hword(mtow(_vec)), m_matb(_vec), m_g(hamm.G()), m_h(hamm.H())
    {
    }

    Hamming::hword::hword(const matbase2& _vec, const matbase2& G, const matbase2& H):
            m_hword(mtow(_vec)), m_matb(_vec), m_g(G), m_h(H)
    {
    }

    Hamming::hword::hword(const hword &_hword) = default;

    bool Hamming::hword::parity() const
    {
        uint _sum(0);
        char _p(p());
        for (uint i(0); i < dim_n()-dim_k(); i++, _p>>=1)
            _sum += _p&1;
        return _sum%2 == 0;
    }

    Hamming::hword::hword_t Hamming::hword::x() const
    {
        return (wtoxp(m_hword, dim_n()) >> 8) & 0xff;
    }

    Hamming::hword::hword_t Hamming::hword::p() const
    {
        return wtoxp(m_hword, dim_n()) & 0xff;
    }

    Hamming::hword::hword_t Hamming::hword::err() const
    {
        return hword::mtow(m_h*m_matb);
    }

    Hamming::hword::hword_t Hamming::hword::m() const
    {
        return m_hword;
    }

    const matbase2& Hamming::hword::getVector() const
    {
        return m_matb;
    }

    Hamming::hword Hamming::hword::invert_bit(uint pos) const
    {
        matbase2 _matb(m_matb);
        _matb[pos-1, 0] = !_matb[pos-1, 0];
        return {_matb, m_g, m_h};
    }

    Hamming::hword& Hamming::hword::operator=(const hword & _hword)
    {
        if (m_g == _hword.m_g)
        {
            m_hword = _hword.m_hword;
            m_matb = _hword.m_matb;
        }
        else throw std::runtime_error("Invalid G matrix");

        return *this;
    }

    bool Hamming::hword::operator==(const code3c::Hamming::hword & _hword) const
    {
        return m_g == _hword.m_g && m_matb == _hword.m_matb && m_hword == _hword.m_hword;
    }

    bool Hamming::hword::operator!=(const code3c::Hamming::hword & _hword) const
    {
        return !operator==(_hword);
    }

    Hamming::Hamming(): m_hwordsl(0), m_hwords(nullptr)
    {
    }

    Hamming::Hamming(const code3c::Hamming &hamm):
        m_hwordsl(hamm.m_hwordsl), m_hwords(new hword*[hamm.m_hwordsl])
    {
        for (size_t i(0); i < m_hwordsl; i++)
        {
            m_hwords[i] = new hword(*hamm.m_hwords[i]);
        }
    }

    Hamming::~Hamming()
    {
        if (m_hwords)
        {
            for (size_t i(0); i < m_hwordsl; i++)
                delete m_hwords[i];
            delete[] m_hwords;
        }
    }

    Hamming::hword** Hamming::begin()
    {
        return m_hwords;
    }

    Hamming::hword** Hamming::end()
    {
        return &m_hwords[m_hwordsl];
    }

    void Hamming::set_buffer(const char *xbuf, size_t xbytel)
    {
        if (m_hwords)
        {
            for (size_t i(0); i < m_hwordsl; i++)
                delete m_hwords[i];
            delete[] m_hwords;
        }

        m_hwords  = new hword*[xbytel*8/dim_k()];

        size_t ihwords(0);
        for (size_t ib(0); ib < 8*xbytel; ihwords++)
        {
            char x(0);
            for (size_t _(0); _ < dim_k(); _++, ib++)
            {
                char c = xbuf[ib/8];
                x <<= 1;
                x |= (c>>(ib%8)) & 1;
            }
            m_hwords[ihwords] = new hword(x, *this);
        }

        m_hwordsl = ihwords;
    }

    void Hamming::set_buffer(const char *xbuf, const char *mbuf, size_t xbitl)
    {
        if (m_hwords)
        {
            for (size_t i(0); i < m_hwordsl; i++)
                delete m_hwords[i];
            delete[] m_hwords;
        }

        m_hwords = new hword*[xbitl/dim_k()];

        size_t ihwords(0);
        for (size_t ib(0), ip(0); ib < xbitl; ihwords++)
        {
            char x(0), p(0);
            for (size_t _(0); _ < dim_k(); _++, ib++)
            {
                char c = xbuf[ib/8];
                x <<= 1;
                x |= (c>>(ib%8)) & 1;
            }
            for (size_t _(0); _ < (dim_n()-dim_k()); _++, ip++)
            {
                char c = mbuf[ip/8];
                p <<= 1;
                p |= (c>>(ip%8)) & 1;
            }
            m_hwords[ihwords] = new hword(x, p, *this);
        }
    }

    char* Hamming::build_xbuffer(char* xbuffer, size_t* size)
    {
        if (m_hwordsl)
        {
            for (size_t i(0), ib(0); i < m_hwordsl; i++)
            {
                hword * word(m_hwords[i]);
                hword::hword_t x(word->x());
                for (size_t _(0); _ < word->dim_k(); _++, ib++)
                {
                    char *c = &xbuffer[ib/8];
                    *c <<= 1;
                    *c |= (x >> (word->dim_k()-_-1))& 1;
                }
            }
        }

        if (size)
                *size = xbitl()/8;
        return xbuffer;
    }

    char* Hamming::build_pbuffer(char* pbuffer, size_t* size)
    {
        if (m_hwordsl)
        {
            for (size_t i(0), ib(0); i < m_hwordsl; i++)
            {
                hword * word(m_hwords[i]);
                hword::hword_t p(word->p());
                for (size_t _(0); _ < word->dim_n()-word->dim_k(); _++, ib++)
                {
                    char *c = &pbuffer[ib / 8];
                    *c <<= 1;
                    *c |= (p >> (word->dim_n()-word->dim_k()-_-1))& 1;
                }
            }
        }

        if (size)
                *size = pbitl()/8+1;
        return pbuffer;
    }

    const Hamming::hword& Hamming::operator[](size_t _i) const
    {
        return *m_hwords[_i];
    }
#pragma clang diagnostic pop // "modernize-use-bool-literals"
}
