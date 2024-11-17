#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

typedef struct {
    int32_t image_width, image_height, image_channels;

    uint8_t *image;
} texture_image;

texture_image read_texture_image_png(const char *filepath);

uint8_t *get_pixel_from_norm(texture_image *ti, float nx, float ny);

void destroy_texture(texture_image *ti);

#endif  // TEXTURE_H
