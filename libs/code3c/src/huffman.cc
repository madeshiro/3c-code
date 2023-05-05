#include "code3c/huffman.hh"
#include <cstring>
#include <stdexcept>
#include <functional>

namespace code3c
{
    HuffmanTree::Node::Node(huff_char_t c, uint32_t w):
        m_0(nullptr), m_1(nullptr),
        weight(w), ch(c)
    {
    }
    
    HuffmanTree::Node::Node(const code3c::HuffmanTree::Node & node): /* NOLINT */
        m_0(node.m_0 ? new Node(*node.m_0) : nullptr),
        m_1(node.m_1 ? new Node(*node.m_1) : nullptr),
        weight(node.weight), ch(node.ch)
    {
    }
    
    HuffmanTree::Node::~Node()
    {
        delete m_0;
        delete m_1;
    }

    
    HuffmanTree::HuffmanTree(const HuffmanTable &table):
        m_root(new Node)
    {
        // Recursive function to init Tree
        std::function<void(Node *, huff_char_t, const char* bits, uint32_t bitl)>
                rec_init = [&](Node *node, huff_char_t ch, const char* bits, uint32_t bitl)
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
            rec_init(m_root, {.ch32=pair.first}, pair.second.bits(), pair.second.bitl());
        }
    }

    
    HuffmanTree::HuffmanTree(HuffmanTree::Node *root):
        m_root(root)
    {
    }

    
    HuffmanTree::HuffmanTree(HuffmanTree::Node **leaves, uint32_t len):
        m_root(new Node)
    {
        auto sort_ascending = [&](uint32_t off)
        {
            // Sort by weight (ascending)
            for (uint32_t i(off); i < len - 1; i++)
            {
                HuffmanTree::Node *cpy(leaves[i]);
                HuffmanTree::Node **cur(&leaves[i]);
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
            sort_ascending(i);
            current = new Node;
            current->m_0 = leaves[i];
            current->m_1 = leaves[i+1];
            current->weight = leaves[i]->weight + leaves[i+1]->weight;
            current->ch = {.ch32 = 0}; // ignore

            leaves[i] = nullptr;
            leaves[i+1] = current;
        }

        m_root->m_0 = leaves[i];
        if (i+1 < len)
            m_root->m_1 = leaves[i+1];
    }

    
    HuffmanTree::HuffmanTree(const code3c::HuffmanTree &tree):
        m_root(new Node(*tree.m_root))
    {
    }

    
    HuffmanTree::~HuffmanTree()
    {
        delete m_root;
    }

    
    HuffmanTree& HuffmanTree::truncate(uint32_t floor)
    {
        // TODO truncate
        return *this;
    }

    
    HuffmanTree HuffmanTree::truncateAt(uint32_t floor) const
    {
        return HuffmanTree(*this).truncate(floor);
    }

    
    huff_char_t HuffmanTree::operator[](uint32_t bseq) const noexcept(false)
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

    
    HuffmanTable::Cell::Cell(char *bits, uint32_t bitl) :
            m_bitl(bitl), m_bits(bits)
    {
    }

    
    HuffmanTable::Cell::Cell(const Cell &cell):
            m_bitl(cell.m_bitl),
            m_bits(strncpy(new char[cell.m_bitl], cell.m_bits, cell.m_bitl))
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
        return m_bits[i] == '1';
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

    
    HuffmanTable::HuffmanTable(const std::map<char32_t, Cell> &table):
            m_tree(nullptr), m_table(table)
    {
        m_tree = new HuffmanTree(*this);
    }

    
    HuffmanTable::HuffmanTable(const HuffmanTree &tree):
            m_tree(new HuffmanTree(tree)), m_table()
    {
        std::function<void(const typename HuffmanTree::Node*, const char*, uint32_t)>
            init_rec = [&] (const HuffmanTree::Node * node,
                            const char * seq,
                            uint32_t seql) {
            char *bits = strncpy(new char[seql + 1], seq, seql);
            if (*node)
            {
                m_table.insert(std::pair<char32_t, Cell>(node->ch.ch32, Cell(bits, seql)));
            }
            else
            {
                if (node->m_0)
                {
                    bits[seql] = '0';
                    init_rec(node->m_0, bits, seql + 1);
                }
                if (node->m_1)
                {
                    bits[seql] = '1';
                    init_rec(node->m_1, bits, seql + 1);
                }

                delete[] bits;
            }
        };

        init_rec(m_tree->m_root, "", 0);
    }

    
    const HuffmanTable::Cell& HuffmanTable::operator[](huff_char_t c) const
    {
        return m_table.at(c.ch32);
    }

    
    huff_char_t HuffmanTable::operator[](const char *bits, uint32_t len) const
    {
        for (auto pair : m_table)
        {
            if (pair.second.equal(bits, len))
                return huff_char_t {.ch32=pair.first};
        }

        throw std::runtime_error("Unable to find Cell");
    }

    
    uint32_t HuffmanTable::size() const
    {
        return m_table.size();
    }

    std::ostream& operator<<(std::ostream& os, const HuffmanTable& table)
    {
        for (auto& pair : table.m_table)
        {
            os << "'" << (char)pair.first << "' : (" << pair.second.bitl()
               << " bits) [";
            for (uint32_t i(0); i < pair.second.bitl(); i++)
                os << (pair.second[i] ? '1' : '0');
            os << "]\n";
        }
        return os;
    }
}