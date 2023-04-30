#include "code3c/huffman.hh"
#include <cstring>
#include <stdexcept>

namespace code3c
{
    HuffmanTree::HuffmanTree(const HuffmanTable &table):
        m_root(new Node)
    {
        // TODO HuffmanTree from Table
    }

    HuffmanTree::HuffmanTree(HuffmanTree::Node *root):
        m_root(root)
    {
    }

    HuffmanTree::HuffmanTree(HuffmanTree::Node *leaves, uint32_t len)
    {

    }

    HuffmanTree::Node::~Node()
    {
        delete m_0;
        delete m_1;
    }

    HuffmanTable::Cell::Cell(char *bits, uint32_t bitl) :
            m_bitl(bitl), m_bits(bits)
    {
    }

    HuffmanTable::Cell::Cell(const Cell &cell):
            m_bitl(cell.m_bitl), m_bits(new char[cell.m_bitl])
    {
    }

    HuffmanTable::Cell::~Cell()
    {
        delete[] m_bits;
    }

    uint32_t HuffmanTable::Cell::bitl() const
    {
        return m_bitl;
    }

    char HuffmanTable::Cell::operator[](uint32_t i) const
    {
        return m_bits[i] & 1;
    }

    bool HuffmanTable::Cell::equal(const char * bits, uint32_t bitl)
    {
        if (bitl == m_bitl)
        {
            for (uint32_t i(0); i < bitl; i++)
                if (bits[i] != m_bits[i])
                    return false;
            return true;
        }
        return false;
    }

    HuffmanTable::HuffmanTable(const std::map<char, Cell> &table):
            m_tree(nullptr), m_table(table)
    {
        m_tree = new HuffmanTree(*this);
    }

    HuffmanTable::HuffmanTable(const HuffmanTree &tree):
            m_tree(new HuffmanTree(tree)), m_table()
    {
        _rec_init_from_tree(m_tree->m_root, "", 0);
    }

    void HuffmanTable::_rec_init_from_tree( /* NOLINT */
                                           const HuffmanTree::Node * node,
                                           const char * seq,
                                           uint32_t seql)
    {
        char* bits = strcpy(new char[seql+1], seq);
        if (*node)
        {
            m_table.insert(std::pair<char, Cell>(node->ch, Cell(bits, seql+1)));
        }
        else
        {
            if (node->m_0)
                _rec_init_from_tree(node->m_0, bits, seql + 1);
            if (node->m_1)
                _rec_init_from_tree(node->m_1, bits, seql + 1);

            delete[] bits;
        }
    }

    const HuffmanTable::Cell& HuffmanTable::operator[](char c) const
    {
        return m_table.at(c);
    }

    char HuffmanTable::operator[](const char *bits, uint32_t len) const
    {
        for (auto pair : m_table)
        {
            if (pair.second.equal(bits, len))
                return pair.first;
        }

        throw std::runtime_error("Unable to find Cell");
    }
}