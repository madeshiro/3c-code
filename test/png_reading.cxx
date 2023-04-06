#include <iostream>
#include <code3c/drawer.hh>
#include <code3c/pixelmap.hh>

using namespace code3c;

int test_png_read();
int test_png_resize();

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
                "png_read",
                test_png_read,
                0, 0
        },
        {
                "png_resize",
                test_png_resize,
                1, 0
        }
#endif
};

int test_png_reading(int argc [[maybe_unused]], char** argv [[maybe_unused]])
{
    uint32_t status(0u), pass(0),
            found(sizeof(registeredFunctionEntries)/sizeof(testFunctionMapEntry));
    
    std::cout << "Running PNG files I/O tests..." << std::endl;
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

int test_png_read()
{
    PixelMap map(PixelMap::loadFromPNG("gitlab_isima.png"));
    class PNGDrawer : public Code3CDrawer
    {
        PixelMap &map;
    public:
        explicit PNGDrawer(PixelMap &map):
            Code3CDrawer(map.width(), map.height(), matb(20)),
            map(map)
        {
        }
        
        void setup() override
        {
            setTitle("PNG Test Window");
            background(0xffffff);
            
            for (int x = 0; x < map.width(); x++)
            {
                for (int y = 0; y < map.height(); y++)
                {
                    if (map[x,y].alpha != 0)
                        draw_pixel(map[x,y].color, x, y);
                }
            }
        }
        
        void draw() override
        {
        }
    } pngDrawer(map);
    
    pngDrawer.run();
    return 0;
}

int test_png_resize()
{
    return 0;
}