#ifndef PPM_H
#define PPM_H

#include <stdint.h>

#include "vec3.h"

typedef struct {
    uint32_t width;
    uint32_t height;

    color *image;
} PPM_image;

PPM_image PPM_read_file(const char *filepath);

void PPM_destroy(PPM_image *ppm);

#endif  // PPM_H
