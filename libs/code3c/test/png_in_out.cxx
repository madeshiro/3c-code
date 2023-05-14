#include <iostream>
#include <code3c/drawer.hh>
#include <code3c/pixelmap.hh>

using namespace code3c;

int test_png_read();
int test_png_resize();
int test_png_save();

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
        },
        {
            "png_save",
            test_png_save,
            2, 0
        }
#endif
};

int test_png_in_out(int argc [[maybe_unused]], char** argv [[maybe_unused]])
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
    PixelMap map(PixelMap::loadFromPNG("resources/3ccode-wb2c.png"));
    class PNGDrawer : public SimpleDrawer
    {
        PixelMap &map;
    public:
        explicit PNGDrawer(PixelMap &map):
                SimpleDrawer(map.width(), map.height(), mat<char8_t>(20)),
                map(map)
        {
        }
        
        void setup() override
        {
            setTitle("PNG Test Window");
        }
        
        void draw() override
        {
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
    } pngDrawer(map);
    
    pngDrawer.run();
    return 0;
}

int test_png_resize()
{
    PixelMap map(PixelMap::loadFromPNG("resources/3ccode-wb2c.png"));
    PixelMap scaled(map.resize(169*2,0));
    class PNGDrawer : public SimpleDrawer
    {
        PixelMap &map;
    public:
        explicit PNGDrawer(PixelMap &map):
                SimpleDrawer(map.width(), map.height(), mat<char8_t>(20)),
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
    } pngDrawer(scaled);
    
    pngDrawer.run();
    return 0;
}

int test_png_save()
{
    FILE* dest = fopen("isima_resize.png", "wb");
    if (dest)
    {
        PixelMap map(PixelMap::loadFromPNG("resources/3ccode-wb2c.png"));
        PixelMap::saveInPng(map.resize(2.0f), dest);
        fclose(dest);
    } else return 1;
    
    return 0;
}