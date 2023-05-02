#include "code3c/huffman.hh"
#include <cstring>
#include <stdexcept>
#include <functional>

namespace code3c
{
    template class HuffmanTree<char8_t>;
    template class HuffmanTree<char16_t>;
    template class HuffmanTree<char32_t>;

    template class HuffmanTable<char8_t>;
    template class HuffmanTable<char16_t>;
    template class HuffmanTable<char32_t>;

    template < typename _CharT >
    HuffmanTree<_CharT>::Node::Node(const code3c::HuffmanTree<_CharT>::Node & node): /* NOLINT */
        m_0(node.m_0 ? new Node(*node.m_0) : nullptr),
        m_1(node.m_1 ? new Node(*node.m_1) : nullptr),
        weight(node.weight), ch(node.ch)
    {
    }

    template < typename _CharT >
    HuffmanTree<_CharT>::Node::~Node()
    {
        delete m_0;
        delete m_1;
    }

    template < typename _CharT >
    HuffmanTree<_CharT>::HuffmanTree(const HuffmanTable<_CharT> &table):
        m_root(new Node)
    {
        // Recursive function to init Tree
        std::function<void(Node *, _CharT, const char* bits, uint32_t bitl)>
                rec_init = [&](Node *node, _CharT ch, const char* bits, uint32_t bitl)
        {
            if (!bitl)
            {
                node->ch = ch;
            }
            else
            {
                Node** dest = *bits ? &node->m_1 : &node->m_0;
                if (!*dest) *dest = new Node;
                rec_init(*dest, ch, bits+1, bitl-1);
            }
        };

        // Init tree from each table's cell
        for (const auto &pair : table.m_table)
        {
            rec_init(m_root, pair.first, pair.second.bits(), pair.second.bitl());
        }
    }

    template < typename _CharT >
    HuffmanTree<_CharT>::HuffmanTree(HuffmanTree<_CharT>::Node *root):
        m_root(root)
    {
    }

    template < typename _CharT >
    HuffmanTree<_CharT>::HuffmanTree(HuffmanTree<_CharT>::Node **leaves, uint32_t len):
        m_root(new Node)
    {
        auto sort_ascending = [&]()
        {
            // Sort by weight (ascending)
            for (uint32_t i(0); i < len - 1; i++)
            {
                HuffmanTree<_CharT>::Node *cpy(leaves[i]);
                HuffmanTree<_CharT>::Node **cur(&leaves[i]);
                for (uint32_t j(i + 1); j < len; j++)
                {
                    if ((*cur)->get_weight() > leaves[j]->get_weight())
                        cur = &leaves[j];
                }

                leaves[i] = *cur;
                (*cur) = cpy;
            }
        };

        uint32_t i(0);
        Node* current;
        for (; i < len-2; i++)
        {
            sort_ascending();
            current = new Node;
            current->m_0 = leaves[i];
            current->m_1 = leaves[i+1];
            leaves[i+1] = current;
        }

        m_root->m_0 = leaves[i];
        if (i+1 != len)
            m_root->m_1 = leaves[i+1];
    }

    template < typename _CharT >
    HuffmanTree<_CharT>::HuffmanTree(const code3c::HuffmanTree<_CharT> &tree):
        m_root(new Node(*tree.m_root))
    {
    }

    template < typename _CharT >
    HuffmanTree<_CharT>::~HuffmanTree()
    {
        delete m_root;
    }

    template < typename _CharT >
    HuffmanTree<_CharT>& HuffmanTree<_CharT>::truncate(uint32_t floor)
    {
        // TODO truncate
        return *this;
    }

    template < typename _CharT >
    HuffmanTree<_CharT> HuffmanTree<_CharT>::truncateAt(uint32_t floor) const
    {
        return HuffmanTree<_CharT>(*this).truncate(floor);
    }

    template < typename _CharT >
    _CharT HuffmanTree<_CharT>::operator[](uint32_t bseq) const noexcept(false)
    {
        const Node* cur(m_root);
        for (uint32_t _(0); _<32; _++)
        {
            if (*cur)
                return cur->ch;
            else
            {
                cur = (*cur)[bseq & 1];
                bseq >>= 1;
            }
        }

        throw std::runtime_error("invalid bit sequence to retrieve leaf in the huffman "
                                 "tree");
    }

    template < typename _CharT >
    HuffmanTable<_CharT>::Cell::Cell(char *bits, uint32_t bitl) :
            m_bitl(bitl), m_bits(bits)
    {
    }

    template < typename _CharT >
    HuffmanTable<_CharT>::Cell::Cell(const Cell &cell):
            m_bitl(cell.m_bitl), m_bits(new char[cell.m_bitl])
    {
    }

    template < typename _CharT >
    HuffmanTable<_CharT>::Cell::~Cell()
    {
        delete[] m_bits;
    }

    template < typename _CharT >
    uint32_t HuffmanTable<_CharT>::Cell::bitl() const
    {
        return m_bitl;
    }

    template < typename _CharT >
    char HuffmanTable<_CharT>::Cell::operator[](uint32_t i) const
    {
        return m_bits[i] & 1;
    }

    template < typename _CharT >
    bool HuffmanTable<_CharT>::Cell::equal(const char * bits, uint32_t bitl)
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

    template < typename _CharT >
    HuffmanTable<_CharT>::HuffmanTable(const std::map<_CharT, Cell> &table):
            m_tree(nullptr), m_table(table)
    {
        m_tree = new HuffmanTree<_CharT>(*this);
    }

    template < typename _CharT >
    HuffmanTable<_CharT>::HuffmanTable(const HuffmanTree<_CharT> &tree):
            m_tree(new HuffmanTree<_CharT>(tree)), m_table()
    {
        std::function<void(const typename HuffmanTree<_CharT>::Node*, const char*, uint32_t)>
            init_rec = [&] (const HuffmanTree<_CharT>::Node * node,
                            const char * seq,
                            uint32_t seql) {
            char *bits = strcpy(new char[seql + 1], seq);
            if (*node)
            {
                m_table.insert(std::pair<_CharT, Cell>(node->ch, Cell(bits, seql + 1)));
            }
            else
            {
                if (node->m_0)
                    init_rec(node->m_0, bits, seql + 1);
                if (node->m_1)
                    init_rec(node->m_1, bits, seql + 1);

                delete[] bits;
            }
        };

        init_rec(m_tree->m_root, "", 0);
    }

    template < typename _CharT >
    const HuffmanTable<_CharT>::Cell& HuffmanTable<_CharT>::operator[](_CharT c) const
    {
        return m_table.at(c);
    }

    template < typename _CharT >
    _CharT HuffmanTable<_CharT>::operator[](const char *bits, uint32_t len) const
    {
        for (auto pair : m_table)
        {
            if (pair.second.equal(bits, len))
                return pair.first;
        }

        throw std::runtime_error("Unable to find Cell");
    }

    std::ostream& operator<<(std::ostream& os, const HuffmanTable<char>& table)
    {
        for (auto& pair : table.m_table)
        {
            os << pair.first << " : (" << pair.second.bitl() << " bits) [";
            for (uint32_t i(0); i < pair.second.bitl(); i++)
                os << pair.second[i];
            os << "]\n";
        }
        return os;
    }
}