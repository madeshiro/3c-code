#include <iostream>
#include <code3c/drawer.hh>

using code3c::matb;
using code3c::Code3CDrawer;

/* override classes */
class TestDrawer : public Code3CDrawer
{
    char buf[256];
public:
    TestDrawer(): Code3CDrawer(500, 300, matb(20))
    {
    }
    
    void setup() override
    {
        setTitle("Code3C Test Window");
        setFrameRate(60);
    }
    
    void draw() override
    {
        char fps[16];
        sprintf(fps, "%lu fps", this->frameRate());
        this->draw_text(fps, 0, 12);
    }
};

/* test variables */


/* test functions */
int test_create_window();


typedef int (*TestFunction)(void); /* NOLINT */
typedef struct /* NOLINT */
{
    const char* name;
    TestFunction func;
    uint32_t id;
    int exit_code;
} testFunctionMapEntry;

static testFunctionMapEntry registeredFunctionEntries[] = {
#ifndef CMAKE_CTEST_ENV_NOGUI
        {
            "create_window",
            test_create_window,
            0, 0
        }
#endif
};

int test_3c_drawer(int argc [[maybe_unused]], char** argv [[maybe_unused]])
{
    uint32_t status(0u), pass(0),
             found(sizeof(registeredFunctionEntries)/sizeof(testFunctionMapEntry));
    
    std::cout << "Running X11/Win32 display tests..." << std::endl;
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

int test_create_window()
{
    TestDrawer drawer;
    drawer.loop();
    return 0;
}