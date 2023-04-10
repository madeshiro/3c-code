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
                foreground(color);
                draw_slice(350, 350, r, 6, 360*i / 60.0);
                color = ~color;
            }
        }
        foreground(0xff0000);
        draw_slice(350, 350, 350, 6, 0);
        draw_slice(350, 350, 350, 6, -90);
        draw_slice(350, 350, 350, 12, 3*360/8.0);
        for (int i = 0; i < 15; i++)
        {
            draw_slice(350, 350, 105, 6, 180*i/60);
            draw_slice(350, 350, 105, 6, 180*(1+i)/60);
        }
        foreground(0);
        draw_line(350, 350, 700, 350);
        foreground(0xbe55ab);
        fill_circle(350, 350, 100);
        foreground(0);
    }
    
    void draw() override
    {
    }
};

/* test variables */
Drawer * drawer;

/* test functions */
int test_create_window();
int test_create_data();
int test_draw_pixel();
int test_key_binding();


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
        },
        {
            "create_data",
            test_create_data,
            1, 0
        },
        {
            "draw_pixel",
            test_draw_pixel,
            2, 0
        },
        {
            "key_binding",
            test_key_binding,
            3, 0
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
    Code3C code3C("https://gitlab.isima.fr/rinbaudelet/uca-l3_graphicalprot",
                  CODE3C_MODEL_WB2C);
    drawer = code3C.draw();
    drawer->run();
    delete drawer;
    return 0;
}

int test_create_window()
{
    TestDrawer testDrawer;
    testDrawer.run();
    return 0;
}

int test_draw_pixel()
{
    class PixelTestDrawer : public Code3CDrawer
    {
    public:
        PixelTestDrawer() : Code3CDrawer(400, 400, matb(20))
        {
        }
        
        void setup() override
        {
            setTitle("Code3C Pixel drawing - Test Window");
            background(0xffffff);
        }
        
        void draw() override
        {
            background(0xffffff);
            unsigned long colors[4] = {
                    0xff0000,
                    0x00ff00,
                    0x0000ff,
                    0x00ffff
            };
            
            for (int y = 0; y < height(); y++)
            {
                unsigned long color = colors[(y/10)%4];
                for (int x = 0; x < width(); x++)
                {
                    draw_pixel(color, x, y);
                }
            }
        }

        void onKeyPressed() override
        {
            if (key == 'a')
            {
                printf("Save screen\n");
                fflush(stdout);
                savePNG("test_drawing.png");
            }
        }
    } testDrawer;
    testDrawer.run();
    
    return 0;
}

int test_key_binding()
{
    class KeyBindingDrawer : public Code3CDrawer
    {
        void onCtrlAPressed()
        {
            printf("Touch ALT_L+A pressed !\n");
            fflush(stdout);
        }
    public:
        KeyBindingDrawer(): Code3CDrawer(400, 400, matb(10))
        {
            bindKey((DRAWER_KEY_CTRLL | 'a'),
                    reinterpret_cast<delegate>(&KeyBindingDrawer::onCtrlAPressed)
                    );
        }
        
        void setup() override
        {
            setTitle("Key Binding Test");
            background(0xffffff);
        }
        
        void draw() override
        {
        
        }
    } keyBindingDrawer;
    keyBindingDrawer.run();
    return 0;
}