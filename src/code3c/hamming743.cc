#include "code3c/hamming743.hh"

namespace code3c
{
#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-bool-literals"
    matbase2 Hamming743::G()
    {
        static matbase2 _G(7, 4, new bool*[7] { /* NOLINT */
            new bool[4] {1, 0, 0, 0},
            new bool[4] {0, 1, 0, 0},
            new bool[4] {0, 0, 1, 0},
            new bool[4] {0, 0, 0, 1},
            new bool[4] {1, 1, 0, 1},
            new bool[4] {0, 1, 1, 1},
            new bool[4] {1, 0, 1, 1}
        });

        return _G;
    }

    matbase2 Hamming743::H()
    {
        static matbase2 _H(3, 7, new bool*[3] { /* NOLINT */
            new bool[7] {1, 1, 0, 1, 1, 0, 0},
            new bool[7] {0, 1, 1, 1, 0, 1, 0},
            new bool[7] {1, 0, 1, 1, 0, 0, 1}
        });

        return _H;
    }
#pragma clang diagnostic pop

    matbase2 Hamming743::hword::wtom(char w, uint blen)
    {
        matbase2 _wtom(blen, 1);
        for (uint i(0); i < blen; i++)
            _wtom[i, 0] = static_cast<bool>((w>>(blen-1-i)) & 0b1);
        return _wtom;
    }

    char Hamming743::hword::mtow(const matbase2 & m)
    {
        char _mtow('\0');
        for (int i(0); i < m.n(); i++)
            _mtow |= ((m[i, 0]&0b1) << (m.n()-1-i));
        return _mtow;
    }

    Hamming743::hword::hword():
        m_word('\0'), m_matb(7, 1)
    {
    }

    Hamming743::hword::hword(char word, bool is_x):
        m_word(word), m_matb(7, 1)
    {
        if (is_x)
        {
            m_matb = G()*wtom(word, 4u);
            m_word = mtow(m_matb);
            m_word = (m_word<<1) | ((m_word&0b111 % 2) == 0);
        }
        else
        {
            m_matb = wtom(word >> 1, 7u);
        }
    }

    bool Hamming743::hword::parity() const
    {
        return m_word & 1;
    }

    char Hamming743::hword::x() const
    {
        return (m_word >> 4) & 0xf;
    }

    char Hamming743::hword::m() const
    {
        return m_word;
    }

    char Hamming743::hword::ctrl() const
    {
        return m_word & 0xf;
    }

    Hamming743::Hamming743(char word2x4)
    {
        m_hword[0] = hword(word2x4>>4, true);
        m_hword[1] = hword(word2x4>>0, true);
    }

    Hamming743::Hamming743(char *word4ctrl4)
    {
        m_hword[0] = hword(word4ctrl4[0], false);
        m_hword[1] = hword(word4ctrl4[1], false);
    }

    const Hamming743::hword& Hamming743::Hword() const
    {
        return m_hword[0];
    }

    const Hamming743::hword& Hamming743::Lword() const
    {
        return m_hword[1];
    }
}
