#include "code3c/huffman.hh"
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
                Node** dest = (*bits == '1') ? &(node->m_1) : &(node->m_0);
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

        int32_t i(0);
        Node* current;
        for (; i < ((int32_t)len)-2; i++)
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

    uint32_t HuffmanTable::countChars(const char8_t * hbuf, size_t bitl) const
    {
        uint32_t count(0), ibit(0);
        char c, b;

        HuffmanTree::Node* node = m_tree->m_root;
        while (ibit < bitl)
        {
            c = hbuf[ibit / 8];
            b = (c >> (7-(ibit%8)))&1;
            node = (1 == b ? node->m_1 : node->m_0);
            ibit++;

            if (*node)
            {
                node = m_tree->m_root;
                count++;
            }
        }

        return count;
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

    template uint32_t HuffmanTable::lengthOf<char8_t>(const char8_t*, size_t,
            uint32_t*) const;
    template uint32_t HuffmanTable::lengthOf<char16_t>(const char16_t*, size_t,
            uint32_t*) const;
    template uint32_t HuffmanTable::lengthOf<char32_t>(const char32_t*, size_t,
            uint32_t*) const;

    template char8_t* HuffmanTable::encode<char8_t>(const char8_t *, uint32_t,
            uint32_t *) const;
    template char8_t* HuffmanTable::encode<char16_t>(const char16_t *, uint32_t,
            uint32_t *)const;
    template char8_t* HuffmanTable::encode<char32_t>(const char32_t *, uint32_t,
            uint32_t *) const;

    template char * HuffmanTable::decode<char>(const char8_t* hbuf,
            uint32_t bitl, uint32_t* _out_len) const;
    template char8_t * HuffmanTable::decode<char8_t>(const char8_t* hbuf,
            uint32_t bitl, uint32_t* _out_len) const;
    template char16_t * HuffmanTable::decode<char16_t>(const char8_t* hbuf,
            uint32_t bitl, uint32_t* _out_len) const;
    template char32_t * HuffmanTable::decode<char32_t>(const char8_t* hbuf,
            uint32_t bitl, uint32_t* _out_len) const;

    template <>
    uint32_t HuffmanTable::lengthOf<char>(const char* str,
                                    size_t slen,
                                    uint32_t *_out_bitl) const
    {
        uint32_t bitl(0);
        for (size_t i(0); i < slen; i++)
            bitl += m_table.at((char32_t)static_cast<unsigned char>(str[i])).bitl();

        if (_out_bitl) *_out_bitl = bitl;
        return bitl/8 + (bitl%8 ? 1 : 0);
    }

    template <>
    char8_t* HuffmanTable::encode<char>(const char * buf,
                                  uint32_t slen,
                                  uint32_t *_out_bitl) const
    {
        // Huffman Buffer length (byte), length in bit
        uint32_t bufl, bitl;
        bufl = lengthOf(buf, slen, &bitl);
        char8_t * hbuf = new char8_t[bufl+1];
        std::memset(hbuf, 0, bufl+1);


        for (uint32_t i(0), ibit(0); i < slen; i++)
        {
            auto cell = m_table.at(static_cast<unsigned char>(buf[i]));
            for (auto& ch : cell)
            {
                char8_t* c = &hbuf[ibit/8];
                ch = (ch=='1')&1;
                ch <<= (7-ibit%8);
                *c |= ch;
                ibit++;
            }
        }

        if (_out_bitl) *_out_bitl = bitl;
        return hbuf;
    }

    uint8_t HTFile::htf_info::to_byte() const
    {
        uint8_t _byte(0);
        _byte |= (char_type&0b11)  << 6;
        _byte |= (entry_bit&0b1)   << 5;
        _byte |= (length_max&0x1f);

        return _byte;
    }

    HTFile::htf_info HTFile::htf_info::from_byte(uint8_t _byte)
    {
        return htf_info {
            .char_type  = static_cast<uint8_t>((_byte >> 6) & 0b11),
            .entry_bit  = static_cast<uint8_t>((_byte >> 5) & 0b1),
            .length_max = static_cast<uint8_t>(_byte & 0x1f)
        };
    }

    char* HTFile::segment::toBits() const
    {
        char* bits = new char[len];
        for (uint8_t ibit(0); ibit < len; )
        {
            char _byte = seq[ibit/8];
            for (uint8_t i(0); i < 8 && ibit < len; ibit++, i++)
            {
                bits[ibit] = '0' + ((_byte >> (7 - i)) & 1);
            }
        }

        return bits;
    }

    void HTFile::init_from_buffer(const char *buffer, size_t len)
    {
        if (std::strncmp(buffer, magic_number, 4) == 0)
        {
            uint32_t nseq = *((uint32_t*) &buffer[4]);
            m_header = {
                    nseq,
                    htf_info::from_byte(buffer[8])
            };

            m_segCount = nseq;
            m_segments = new segment[nseq];
            uint32_t iseq, ibuf;
            for (iseq=0, ibuf=9; ibuf < len && iseq < nseq; iseq++)
            {
                // Useful variables
                const char* buf = &buffer[ibuf];
                segment seg {};

                // Init char
                switch (m_header.info.char_type)
                {
                    case 1: // char8_t
                        seg.ch.ch8 = (char8_t) *buf;
                        break;
                    case 2: // char16_t
                        seg.ch.ch16 = *((char16_t*)buf);
                        break;
                    case 4: // char32_t
                        seg.ch.ch32 = *((char32_t*)buf);
                        break;
                    default:
                        throw std::runtime_error("corrupted header: invalid char type");
                }
                ibuf += m_header.info.char_type;
                buf += m_header.info.char_type;

                // Set bits' length
                seg.len = static_cast<uint8_t>(*buf);
                ibuf++;
                buf++;

                // Set sequence bits
                for (uint8_t i(0); i*8 < seg.len; i++, ibuf++, buf++)
                {
                    seg.seq[i] = *buf;
                }

                m_segments[iseq] = seg;
            }

            if (nseq != iseq)
                throw std::runtime_error("corrupted buffer");
        } else
            throw std::runtime_error("magin number not found");
    }

    HTFile::HTFile(FILE *infile):
            m_segments(nullptr), m_segCount(0),
            m_buf(nullptr), m_lbuf(0)
    {
        fseek(infile, 0L, SEEK_END);
        size_t fsize = ftell(infile);
        fseek(infile, 0L, SEEK_SET);

        char* fbuffer = new char[fsize];
        fread(fbuffer, sizeof(char), fsize/sizeof(char), infile);

        init_from_buffer(fbuffer, fsize);
    }

    HTFile::HTFile(const char *buffer, size_t len):
            m_segments(nullptr), m_segCount(0),
            m_buf(nullptr), m_lbuf(0)
    {
        init_from_buffer(buffer, len);
    }

    HTFile::HTFile(const HuffmanTable &table):
            m_segments(new segment[table.size()]), m_segCount(table.size()),
            m_buf(nullptr), m_lbuf(9)
    {
        m_header = {0, { sizeof(char), 0b1, 3 }};

        for (auto cell : table.m_table)
        {
            m_segments[m_header.seql] = segment {
                    .ch  = {.ch32 = cell.first},
                    .len = static_cast<uint8_t>(cell.second.bitl()),
                    .seq = {0,0,0,0}
            };

            m_header.info.length_max = std::max<uint8_t>(cell.second.bitl(),
                                                         m_header.info.length_max);
            m_header.info.char_type = [cell]() -> uint8_t {
                if (cell.first < 0x100)
                    return sizeof(char);
                if (cell.first < 0x10000)
                    return sizeof(char16_t);

                return sizeof(char32_t);
            }();

            for (uint32_t ibit(0), ibyte(0); ibit < cell.second.bitl(); ibyte++)
            {
                char *c = &m_segments[m_header.seql].seq[ibyte];
                for (uint32_t j(0); j < 8 && ibit < cell.second.bitl(); j++, ibit++)
                {
                    *c |= (cell.second[ibit] << (7-j));
                }
            }

            m_header.seql++;
        }

        // Determine buffer size
        for (auto& seg: *this)
        {
            m_lbuf += m_header.info.char_type + 1 /* bitl byte */;
            m_lbuf += seg.bitl() / 8 + (seg.bitl() % 8 > 0);
        }

        // Initialize buffer
        m_buf = new char[m_lbuf+1];
        m_buf[m_lbuf] = '\0';

        // Write header
        strncpy(m_buf, magic_number, 4);
        strncpy(&m_buf[4], ((char*)&m_header.seql), 4);
        m_buf[8] = (char) m_header.info.to_byte();

        // Write sequences
        char* buf = &m_buf[9];
        for (auto& seg : *this)
        {
            strncpy(buf, ((char*)&seg.ch.ch32), m_header.info.char_type);
            buf += m_header.info.char_type;
            *buf = (char) seg.len;
            buf++;
            for (uint8_t i(0); i*8 < seg.len; i++, buf++)
                *buf = seg.seq[i];
        }
    }

    HTFile::~HTFile()
    {
        delete[] m_buf;
    }

    char* HTFile::write(size_t* _out_len) const
    {
        if (_out_len)
            *_out_len = m_lbuf;
        return strncpy(new char[m_lbuf+1], m_buf, m_lbuf+1);
    }

    bool HTFile::write(FILE* outfile) const
    {
        return std::fwrite(m_buf, sizeof(char), m_lbuf/sizeof(char), outfile)
            == m_lbuf/sizeof(char);
    }

    HuffmanTable* HTFile::read() const
    {
        std::map<char32_t, HuffmanTable::Cell> cells;
        for (auto &seg : *this)
        {
            cells.insert(std::pair<char32_t, HuffmanTable::Cell>{
                seg.ch.ch32,
                { seg.toBits(), seg.bitl() }
            });
        }
        return new HuffmanTable(cells);
    }

    uint8_t HTFile::charSize() const
    {
        return m_header.info.char_type;
    }

    uint8_t HTFile::entryBit() const
    {
        return m_header.info.entry_bit;
    }

    uint8_t HTFile::seqMaxLength() const
    {
        return m_header.info.length_max;
    }

    uint32_t HTFile::countSegments() const
    {
        return m_segCount;
    }

    const HTFile::segment& HTFile::operator[](size_t index) const noexcept(false)
    {
        if (m_segments)
            return m_segments[index];
        else
            throw std::runtime_error("invalid use of operator[] in HTFile");
    }

    const HTFile::segment* HTFile::begin() const
    {
        return m_segments;
    }

    const HTFile::segment* HTFile::end() const
    {
        return &m_segments[m_segCount];
    }

    HuffmanTable* HTFile::fromFile(const char *fname)
    {
        std::FILE* file = std::fopen(fname, "rb");
        if (file)
        {
            HuffmanTable * table = HTFile(file).read();
            std::fclose(file);
            return table;
        }
        return nullptr;
    }

    HuffmanTable* HTFile::fromBuffer(const char *buf, size_t buflen)
    {
        return HTFile(buf, buflen).read();
    }

    bool HTFile::toFile(const char *dest, const HuffmanTable &table)
    {
        std::FILE* file = fopen(dest, "wb");
        if (file)
        {
            bool status(HTFile(table).write(file));
            std::fclose(file);
            return status;
        }
        return false;
    }

    char* HTFile::toBuffer(const HuffmanTable &table, size_t * _out_len)
    {
        return HTFile(table).write(_out_len);
    }
}