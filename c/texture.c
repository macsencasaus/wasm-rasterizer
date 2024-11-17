#include "texture.h"

#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

texture_image read_texture_image_png(const char *filepath) {
    int32_t image_width, image_height, image_channels;

    uint8_t *image =
        stbi_load(filepath, &image_width, &image_height, &image_channels, 0);

    return (texture_image){
        .image_width = image_width,
        .image_height = image_height,
        .image_channels = image_channels,
        .image = image,
    };
}

static uint8_t *get_pixel(texture_image *ti, uint32_t x, uint32_t y) {
    size_t ti_idx =
        ti->image_channels * x * ti->image_width + ti->image_channels * y;
    return ti->image + ti_idx;
}

uint8_t *get_pixel_from_norm(texture_image *ti, float nx, float ny) {
    if (nx < 0 || nx > 1 || ny < 0 || ny > 1) {
        fprintf(stderr, "normal coordinates (%f, %f) out of bounds\n", nx, ny);
        exit(1);
    }
    uint32_t x = (ti->image_width - 1) * (1 - ny);
    uint32_t y = (ti->image_height - 1) * nx;

    return get_pixel(ti, x, y);
}

void destroy_texture(texture_image *ti) {
    stbi_image_free(ti->image);
    *ti = (texture_image){0};
}
