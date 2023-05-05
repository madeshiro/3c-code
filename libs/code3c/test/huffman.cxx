#include <iostream>
#include <code3c/huffman.hh>
#include <cstring>

using code3c::HuffmanTree8;
using code3c::HuffmanTable8;
using code3c::HTFile;

int test_build_table();

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

HuffmanTree8::Node** build_nodes(const char* txt, uint32_t* _out_len)
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

    HuffmanTree8::Node** nodes = new HuffmanTree8::Node *[len];

    i = 0;
    for (auto p : map)
        nodes[i++] = new HuffmanTree8::Node(p.first, p.second);

    if (_out_len)
        *_out_len = len;
    return nodes;
}

int test_build_table()
{
    const char sample[] = "My sample text";
    uint32_t nlen;
    HuffmanTree8::Node** nodes = build_nodes(sample, &nlen);

    HuffmanTree8 tree8(nodes, nlen);
    HuffmanTable8 table8(tree8);
    std::cout << table8 << std::endl;

    delete[] nodes;
    return 0;
}
