#ifndef PPM_H
#define PPM_H

#include <stdint.h>

#include "vec3.h"

typedef struct {
    uint32_t width;
    uint32_t height;

    color *image;
} PPM_image;

PPM_image read_PPM_file(const char *filepath);

#endif  // PPM_H
