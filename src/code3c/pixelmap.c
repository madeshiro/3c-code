#include <malloc.h>
//#include "code3c/pixelmap.hh"

png_descp open_png(const char* fname)
{
    png_descp desc = NULL;
    FILE* png_file = fopen(fname, "rb");
    if (png_file)
    {
        desc = read_png(png_file);
        fclose(png_file);
    }
    return desc;
}

png_descp read_png(FILE* fin)
{
    png_desc* desc = (png_descp) malloc(sizeof(png_desc));
    desc->png = png_create_read_struct(
            PNG_LIBPNG_VER_STRING, NULL, NULL, NULL
            );
    
    if (!desc->png) {
        free(desc);
        return NULL;
    }
    
    if (setjmp(png_jmpbuf(desc->png))) {
        free(desc);
        return NULL;
    }
    
    png_init_io(desc->png, fin);
    png_read_info(desc->png, desc->info);
    
    desc->width      = png_get_image_width(desc->png, desc->info);
    desc->height     = png_get_image_height(desc->png, desc->info);
    desc->color_type = png_get_color_type(desc->png, desc->info);
    desc->bit_depth  = png_get_bit_depth(desc->png, desc->info);
    
    // Setup reading PNG to RGB
    if (desc->bit_depth == 16)
        png_set_strip_16(desc->png);
    if (desc->color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(desc->png);
    if (desc->color_type == PNG_COLOR_TYPE_GRAY && desc->bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(desc->png);
    if (png_get_valid(desc->png, desc->info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(desc->png);
    
    if (desc->color_type == PNG_COLOR_TYPE_RGB ||
        desc->color_type == PNG_COLOR_TYPE_GRAY ||
        desc->color_type == PNG_COLOR_TYPE_PALETTE) {
        // Fill unused alpha channel
        png_set_filler(desc->png, 0xff, PNG_FILLER_AFTER);
    }
    
    if (desc->color_type == PNG_COLOR_TYPE_GRAY ||
        desc->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(desc->png);
    
    png_read_update_info(desc->png, desc->info);
    
    // Read image
    desc->rows = (png_bytepp) malloc(sizeof(png_bytep)*desc->height);
    if (!desc->rows) {
        free(desc);
        return NULL;
    }
    for (int y = 0; y < desc->height; y++)
        desc->rows[y] = (png_bytep) malloc(png_get_rowbytes(desc->png, desc->info));
    png_read_image(desc->png, desc->rows);
    
    return desc;
}

png_descp create_png(__attribute__((unused)) FILE* fio)
{
    // TODO create_png
    return NULL;
}

void free_png_desc(png_descp desc)
{
    if (desc->png && desc->info)
        png_destroy_read_struct(&desc->png, &desc->info, NULL);
}