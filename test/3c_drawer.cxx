#include <iostream>
#include <code3c/3ccode.hh>
#include <code3c/drawer.hh>

using code3c::matb;
using code3c::Code3C;
using code3c::Drawer;
using code3c::Code3CDrawer;

/* override classes */
class TestDrawer : public Code3CDrawer
{
    char buf[256];
public:
    TestDrawer(): Code3CDrawer(700, 700, matb(20))
    {
    }
    
    void setup() override
    {
        setTitle("Code3C Test Window");
        
        unsigned long color;
        for (int r=350; r >= 100; r-=5)
        {
            color = (r%2 ? ~0 : 0);
            for (int i(0); i < 2*30; i++)
            {
                set_color(color);
                draw_slice(350, 350, r, 6, 2 * M_PI * i / 60.0);
                color = ~color;
            }
        }
        set_color(0xff0000);
        draw_slice(350, 350, 350, 6, 0);
        draw_slice(350, 350, 350, 6, -M_PI/2.0);
        draw_slice(350, 350, 350, 12, 3.0*M_PI/4.0);
        for (int i = 0; i < 15; i++)
        {
            draw_slice(350, 350, 105, 6, 2*M_PI*i/60.0);
            draw_slice(350, 350, 105, 6, M_PI+ 2*M_PI*i/60.0);
        }
        set_color(0);
        draw_line(350, 350, 700, 350);
        set_color(0xbe55ab);
        fill_circle(350, 350, 100);
        set_color(0);
    }
    
    void draw() override
    {
    }
    
    void onMouseWheel() override
    {
    }
};

/* test variables */
Drawer * drawer;

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

int test_create_data()
{
    char buf[256];
    scanf("%s", buf);
    Code3C code3C(buf, CODE3C_MODEL_1);
    drawer = code3C.draw();
    drawer->run();
    
    return 0;
}

int test_create_window()
{
    TestDrawer testDrawer;
    testDrawer.run();
    return 0;
}