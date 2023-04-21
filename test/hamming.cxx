#include <iostream>
#include <code3c/hamming13_8_5.hh>

using code3c::matbase2;
using code3c::vecbase2;
using code3c::Hamming743;
using code3c::Hamming13_8_5;

// Global variables
static matbase2 matgen(7, 4, new bool*[7] {
    new bool[4] {1, 1, 0, 1}, /* NOLINT */
    new bool[4] {1, 0, 1, 1}, /* NOLINT */
    new bool[4] {1, 0, 0, 0}, /* NOLINT */
    new bool[4] {0, 1, 1, 1}, /* NOLINT */
    new bool[4] {0, 1, 0, 0}, /* NOLINT */
    new bool[4] {0, 0, 1, 0}, /* NOLINT */
    new bool[4] {0, 0, 0, 1}  /* NOLINT */
});

static matbase2 matctrl(3, 7, new bool*[3] {
   new bool[7] {0, 0, 0, 1, 1, 1, 1}, /* NOLINT */
   new bool[7] {0, 1, 1, 0, 0, 1, 1}, /* NOLINT */
   new bool[7] {1, 0, 1, 0, 1, 0, 1}  /* NOLINT */
});

// Test functions
int hamm743_test_HG();
int hamm_detect_err();

// Utils functions
uint32_t hdiff(char w1, char w2)
{
    char d(w1 & w2);
    uint32_t _hdiff(0);
    while (d != 0)
    {
        if (d & 0b1)
            _hdiff++;
        d >>= 1;
    }
    return _hdiff;
}

matbase2 xtom(char x)
{
    vecbase2 vx(4);
    for (char i(0); i < 4; i++)
        vx[i] = ((x >> (3-i)) & 0b1);

    return matgen * vx;
}

char mtox(const matbase2& m)
{
    return ((m[2,0] << 3) | (m[4,0] << 2) | (m[5,0] << 1) | (m[6,0])) & 0b1111;
}

char getherr(const matbase2& hx)
{
    return ((hx[0,0] << 2) | (hx[1,0] << 1) | hx[2,0]) & 0b111;
}

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
            "hamming_check_setup",
            hamm743_test_HG,
            0, 0
        },
        {
            "hamming_error_detection",
            hamm_detect_err,
            1, 0
        }
};

int test_hamming(int argc [[maybe_unused]], char** argv [[maybe_unused]])
{
    uint32_t status(0u), pass(0),
             found(sizeof(registeredFunctionEntries)/sizeof(testFunctionMapEntry));

    std::cout << "Running 3C Generation tests..." << std::endl;
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

int hamm_detect_err()
{
    for (int i(0); i < 7; i++)
    {
        for (char x(0); x < 16; x++)
        {
            matbase2 m(xtom(x));
            m[i, 0] = !m[i, 0]; // Generate error on the ith bit
            if (getherr(matctrl * m) != (i+1))
                return (i+1);
        }
    }
    return 0;
}

int hamm743_test_HG()
{
    matbase2 _check743(3, 4, new bool*[3] {
        new bool[4] {0, 0, 0, 0}, /* NOLINT */
        new bool[4] {0, 0, 0, 0}, /* NOLINT */
        new bool[4] {0, 0, 0, 0}  /* NOLINT */
    });

    matbase2 _check13_8_5(5, 8, new bool*[5] {
        new bool[8] {0, 0, 0, 0, 0, 0, 0, 0}, /* NOLINT */
        new bool[8] {0, 0, 0, 0, 0, 0, 0, 0}, /* NOLINT */
        new bool[8] {0, 0, 0, 0, 0, 0, 0, 0}, /* NOLINT */
        new bool[8] {0, 0, 0, 0, 0, 0, 0, 0}, /* NOLINT */
        new bool[8] {0, 0, 0, 0, 0, 0, 0, 0}  /* NOLINT */
    });

    if ((Hamming743::H()*Hamming743::G()) != _check743) return 7;
    if (Hamming13_8_5::H()*Hamming13_8_5::G() != _check13_8_5) return 13;
    int mindiff(13);
    for (int i(0); i < 5; i++)
    {
        for (int j(i+1); j < 5; j++)
        {
            int diff(0);
            for (int k(0); k < 13; k++)
            {
                if (Hamming13_8_5::H()[i, k] != Hamming13_8_5::H()[j, k])
                    diff++;
            }
            if (diff < mindiff)
                mindiff = diff;
        }
    }
    // printf("Minimum difference: %d\n", mindiff);
    if (mindiff < 5)
        return mindiff;

    return 0;
}

int hamm_detect_err_13_8_5()
{
    for (int i(0); i < 7; i++)
    {
        for (unsigned char x(1); x != 0; x++)
        {
            Hamming13_8_5::hword hword(static_cast<char16_t>(x), true);
            matbase2 m(hword.get_mat());
            m[i, 0] = m[i, 0]; // Generate error on the ith bit
            if (hword.err() != (i+1))
                return (i+1);
        }
    }
    return 0;
}