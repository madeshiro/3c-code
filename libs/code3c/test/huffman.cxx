#include <iostream>
#include <code3c/huffman.hh>
#include <cstring>

using code3c::HuffmanTree;
using code3c::HuffmanTable;
using code3c::HTFile;

int test_build_table();
int test_huff_encode();
int test_huff_file();
int test_htf_full_generation();

typedef int (*TestFunction)(void); /* NOLINT */
typedef struct /* NOLINT */
{
    const char* name;
    TestFunction func;
    uint32_t id;
    int exit_code;
} testFunctionMapEntry;

static testFunctionMapEntry registeredFunctionEntries[] = {
        {
            "build_table",
            test_build_table,
            0, 0
        },
        {
            "huffman_encode/decode",
            test_huff_encode,
            1, 0
        },
        {
            "huffman_file_generation",
            test_huff_file,
            2, 0
        },
        {
            "huffman_full_generation",
            test_htf_full_generation,
            3, 0
        }
};

int test_huffman(int argc [[maybe_unused]], char** argv [[maybe_unused]])
{
    uint32_t status(0u), pass(0),
             found(sizeof(registeredFunctionEntries)/sizeof(testFunctionMapEntry));

    std::cout << "Running Huffman code tests..." << std::endl;
    std::cout << "Found " << found << " test(s) to run" << std::endl;

    for (testFunctionMapEntry &entry : registeredFunctionEntries)
    {
        std::cout << "test " << entry.name << "... ";
        entry.exit_code = entry.func();
        if (entry.exit_code != 0)
        {
            std::cout << "FAIL with return code " << entry.exit_code << std::endl;
            status |= (0x1 << entry.id);
        }
        else
        {
            pass++;
            std::cout << "OK" << std::endl;
        }
    }

    std::cout << pass << "/" << found << " test(s) passed" << std::endl;
    return (int) status;
}

void print_hbuf(const char8_t * hbuf, uint32_t bitl)
{
    for (uint32_t ibit(0); ibit<bitl;)
    {
        char8_t _bit = hbuf[ibit/8];
        for (uint32_t off(0); off < 8 && ibit < bitl; ibit++, off++)
            std::cout << (char) (((_bit >> (7-off))&1)+'0');
    }
}

HuffmanTree::Node** build_nodes(const char* txt, uint32_t* _out_len)
{
    uint32_t len(0), slen(strlen(txt));
    std::map<char, uint32_t> map;

    uint32_t i;
    for (i = 0; i < slen; i++)
    {
        if (map.contains(txt[i]))
            map[txt[i]]++;
        else {
            map.insert(std::pair<char, uint32_t>(txt[i], 1));
            len++;
        }
    }

    HuffmanTree::Node** nodes = new HuffmanTree::Node *[len];

    i = 0;
    for (auto p : map)
        nodes[i++] = new HuffmanTree::Node(code3c::huff_char_t{(char8_t)p.first},
                                           p.second);

    if (_out_len)
        *_out_len = len;
    return nodes;
}

int test_build_table()
{
    const char sample[] = "My sample text";
    uint32_t nlen;
    HuffmanTree::Node** nodes = build_nodes(sample, &nlen);

    HuffmanTree tree8(nodes, nlen);
    HuffmanTable table8(tree8);
    delete[] nodes;
#ifdef CODE3C_DEBUG
    std::cout << std::endl << table8 << std::endl;
#endif

    // Wrong table size
    if (table8.size() != nlen)
        return -1;
    return 0;
}

int test_huff_encode()
{
    const char* samples[] = {
            "My sample text",
            "a8zd974531het8/.;:;afekf^^^\\983=(",
            "012345678989756423120",
            "d"
    };

    for (auto sample : samples)
    {
        uint32_t nlen, sblen, hblen;

        // Huffman classes
        HuffmanTree::Node** leaves = build_nodes(sample, &nlen);
        HuffmanTree tree(leaves, nlen);
        HuffmanTable table(tree);

        // Encode/Decode buffers
        char8_t *hbuf;
        char* sbuf;

        hbuf = table.encode(sample, strlen(sample), &hblen);
        sbuf = table.decode<char>(hbuf, hblen, &sblen);

#ifdef CODE3C_DEBUG
        printf("\nsample:\n\twitness: \n\t\tstr: %s\n\t\tlength: %lu\n",
               sample, strlen(sample));
        printf("\tencode:\n\t\tbits: ");
        print_hbuf(hbuf, hblen);
        printf("\n\t\tlength: %d\n", hblen);
        printf("\tdecode:\n\t\tstr: %s\n\t\tlength: %d\n", sbuf, sblen);
        std::cout << "Table: \n" << table;
#endif

        if (sblen != strlen(sample))
            return 1;
        if (strcmp(sbuf, sample))
            return 2;

        // Release memory
        delete[] leaves;
        delete[] sbuf;
        delete[] hbuf;
    }

    return 0;
}

int test_huff_file()
{
    const char* htf_file = "test_huff_file.htf";
    const char* samples[] = {
            "My sample text",
            "a8zd974531het8/.;:;afekf^^^\\983=(",
            "012345678989756423120",
            "d"
    };

    for (auto sample : samples)
    {
        char8_t *hbuf;
        uint32_t hblen, slen;

        /* encode and save table */
        {
            uint32_t nlen;

            // Huffman classes
            HuffmanTree::Node **leaves = build_nodes(sample, &nlen);
            HuffmanTree  tree(leaves, nlen);
            HuffmanTable table(tree);

            // Encode buffers
            hbuf = table.encode(sample, strlen(sample), &hblen);

            // Save file
            if (!HTFile::toFile(htf_file, table))
                return 10;
        }

        /* load table and decode */
        {
            HuffmanTable * table = HTFile::fromFile(htf_file);
            if (table)
            {
#ifdef CODE3C_DEBUG
                std::cout << std::endl << *table << std::endl;
#endif
                char *sbuf = table->decode<char>(hbuf, hblen, &slen);
                if (slen != strlen(sample))
                    return 1;
                if (strcmp(sbuf, sample))
                    return 2;
            }
            else return 11;
        }
    }

    std::remove(htf_file);
    return 0;
}

int test_htf_full_generation()
{
    FILE* en_EN = fopen("resources/training_set.en_EN.txt", "rb");
    if (en_EN)
    {
        fseek(en_EN, 0L, SEEK_END);
        size_t fsize = ftell(en_EN);
        fseek(en_EN, 0L, SEEK_SET);
        char* fbuf = new char[fsize];
        if (fread(fbuf, sizeof(char), fsize, en_EN) == fsize)
        {
            uint32_t nlen, hlen;

            // Huffman classes
            HuffmanTree::Node **leaves = build_nodes(fbuf, &nlen);
            HuffmanTree tree(leaves, nlen);
            HuffmanTable table(tree);
#ifdef CODE3C_DEBUG
            std::cout << std::endl << "'en_EN' Huffman Table" <<
            std::endl << table << std::endl;
#endif

            if (!HTFile::toFile("en_EN.htf", table))
                return 1;


            delete[] table.encode(fbuf, fsize, &hlen);
            printf("[encode vs raw]: %d / %lu\n", hlen/8, fsize);
        }
        else return -2;
    }
    else return -1;

    // EXIT_SUCCESS
    return 0;
}