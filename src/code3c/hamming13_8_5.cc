#include "code3c/hamming13_8_5.hh"

namespace code3c
{
#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-bool-literals"
    matbase2 Hamming13_8_5::G()
    {
        static matbase2 _G(13, 8, new bool*[13] { /* NOLINT */
            new bool[8] {1,0,0,0,0,0,0,0}, // 0
            new bool[8] {0,1,0,0,0,0,0,0}, // 1
            new bool[8] {0,0,1,0,0,0,0,0}, // 2
            new bool[8] {0,0,0,1,0,0,0,0}, // 3
            new bool[8] {0,0,0,0,1,0,0,0}, // 4
            new bool[8] {0,0,0,0,0,1,0,0}, // 5
            new bool[8] {0,0,0,0,0,0,1,0}, // 6
            new bool[8] {0,0,0,0,0,0,0,1}, // 7
            new bool[8] {1,1,0,0,1,0,1,0}, // 8
            new bool[8] {0,1,1,1,0,0,1,0}, // 9
            new bool[8] {1,0,1,1,1,0,0,1}, // 10
            new bool[8] {1,0,1,0,1,1,0,0}, // 11
            new bool[8] {1,1,0,0,1,1,0,1}, // 12
        });

        return _G;
    }

    matbase2 Hamming13_8_5::H()
    {
        static matbase2 _H(5, 13, new bool*[5] { /* NOLINT */
            new bool[13] {1,1,0,0,1,0,1,0,1,0,0,0,0},
            new bool[13] {0,1,1,1,0,0,1,0,0,1,0,0,0},
            new bool[13] {1,0,1,1,1,0,0,1,0,0,1,0,0},
            new bool[13] {1,0,1,0,1,1,0,0,0,0,0,1,0},
            new bool[13] {1,1,0,0,1,1,0,1,0,0,0,0,1}
        });

        return _H;
    }
#pragma clang diagnostic pop

    matbase2 Hamming13_8_5::hword::wtom(char16_t w, uint blen)
    {
        matbase2 _wtom(blen, 1);
        for (uint i(0); i < blen; i++)
            _wtom[i, 0] = static_cast<bool>((w>>(blen-1-i)) & 0b1);
        return _wtom;
    }

    char16_t Hamming13_8_5::hword::mtow(const matbase2 & m)
    {
        char16_t _mtow('\0');
        for (int i(0); i < m.n(); i++)
            _mtow |= ((m[i, 0]&0b1) << (m.n()-1-i));
        return _mtow;
    }

    Hamming13_8_5::hword::hword():
        m_word('\0'), m_matb(13, 1)
    {
    }

    Hamming13_8_5::hword::hword(char16_t word, bool is_x):
        m_word(word), m_matb(13, 1)
    {
        if (is_x)
        {
            m_matb = G()*wtom(word, 8u);
            m_word = mtow(m_matb);
            m_word = (m_word<<1) | ((m_word&0b111 % 2) == 0);
        }
        else
        {
            m_matb = wtom(word >> 1, 13u);
        }
    }

    bool Hamming13_8_5::hword::parity() const
    {
        return m_word & 1;
    }

    char Hamming13_8_5::hword::x() const
    {
        return (m_word >> 4) & 0xf;
    }

    char16_t Hamming13_8_5::hword::m() const
    {
        return m_word;
    }

    matbase2 Hamming13_8_5::hword::get_mat() const
    {
        return m_matb;
    }

    int Hamming13_8_5::hword::err() const
    {
        return mtow(H()*m_matb);
    }

    char Hamming13_8_5::hword::ctrl() const
    {
        return m_word & 0xf;
    }

    Hamming13_8_5::Hamming13_8_5(char word8):
        m_hword(word8, true)
    {
    }

    Hamming13_8_5::Hamming13_8_5(char16_t word8ctrl5):
        m_hword(word8ctrl5, false)
    {
    }

    const Hamming13_8_5::hword& Hamming13_8_5::word() const
    {
        return m_hword;
    }
}
