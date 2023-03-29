#include <iostream>
#include <code3c/bitmat.hh>

using namespace code3c;

int test_mat_multiply();
int test_mat_addition();
int test_mat_substraction();

typedef int (*TestFunction)(void); /* NOLINT */
typedef struct /* NOLINT */
{
    const char* name;
    TestFunction func;
    uint32_t id;
    int exit_code ;
} testFunctionMapEntry;

static testFunctionMapEntry registeredFunctionEntries[] = {
        {
            "mat::addition",
            test_mat_addition,
            0, 0
        },
        {
            "mat::substraction",
            test_mat_substraction,
            1, 0
        },
        {
            "mat::multiply",
            test_mat_multiply,
            2, 0
        }
};

int test_mat_operation(int argc [[maybe_unused]], char** argv [[maybe_unused]])
{
    uint32_t status(0u), pass(0),
             found(sizeof(registeredFunctionEntries)/sizeof(testFunctionMapEntry));
    
    std::cout << "Running Matrix/Vector tests..." << std::endl;
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

template < typename T, int n, int m >
void printMatrix(const mat<T,n,m>& mat1)
{
    for (int i(0); i < 3; i++)
    {
        for (int j(0); j < 3; j++)
            std::cout << mat1[i, j] << '\t';
        std::cout << std::endl;
    }
}

int test_mat_addition()
{
    int nbErrors(0);
    {
        int** d1 = new int*[3]
                {
                    new int[3]{10, 15, 20},
                    new int[3]{20,  5,  6},
                    new int[3]{ 1,  3,  5}
                };
        int** d2 = new int*[3]
                {
                    new int[3]{5, 2, 13},
                    new int[3]{2, 7,  6},
                    new int[3]{3, 1,  1}
                };
        int** dr12 = new int*[3]
                {
                    new int[3]{15,17,33},
                    new int[3]{22,12,12},
                    new int[3]{ 4, 4, 6}
                };
        mat3d m1(d1), m2(d2), r12(dr12);
        nbErrors += m1+m2 != r12;
    }
    
    return nbErrors;
}

int test_mat_substraction()
{
    int nbErrors(0);
    {
        int** d1 = new int*[3]
                {
                    new int[3]{10, 15, 20},
                    new int[3]{20,  5,  6},
                    new int[3]{ 1,  3,  5}
                };
        int** d2 = new int*[3]
                {
                    new int[3]{5, 2, 13},
                    new int[3]{2, 7,  6},
                    new int[3]{3, 1,  1}
                };
        int** dr12 = new int*[3]
                {
                    new int[3]{ 5,13, 7},
                    new int[3]{18,-2, 0},
                    new int[3]{-2, 2, 4}
                };
        mat3d m1(d1), m2(d2), r12(dr12);
        nbErrors += m1-m2 != r12;
    }
    
    return nbErrors;
}

int test_mat_multiply()
{
    int nbErrors(0);
    {
        int** d1 = new int*[3]
                {
                    new int[3]{10, 15, 20},
                    new int[3]{20,  5,  6},
                    new int[3]{ 1,  3,  5}
                };
        int** d2 = new int*[3]
                {
                    new int[3]{5, 2, 13},
                    new int[3]{2, 7,  6},
                    new int[3]{3, 1,  1}
                };
        int** dr12 = new int*[3]
                {
                    new int[3]{140,145,240},
                    new int[3]{128, 81,296},
                    new int[3]{ 26, 28, 36}
                };
        int** dr1d = new int*[3]
                {
                    new int[3]{ 50, 75, 100},
                    new int[3]{100, 25,  30},
                    new int[3]{  5, 15,  25}
                };
        mat3d m1(d1), m2(d2), r12(dr12), r1d(dr1d);
        nbErrors += m1*m2 != r12;
        nbErrors += 5*m1 != r1d;
    }
    
    return nbErrors;
}
