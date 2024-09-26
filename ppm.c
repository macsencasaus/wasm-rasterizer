#include "ppm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PPM_image read_PPM_file(const char *filepath) {
    FILE *ppm_file = fopen(filepath, "r");
    if (ppm_file == NULL) {
        fprintf(stderr, "error opening file %s\n", filepath);
        exit(1);
    }

    char ppm_format[3] = {0};

    if (fscanf(ppm_file, "%s", ppm_format) != 1) {
        fclose(ppm_file);
        fprintf(stderr, "Error reading ppm format from file\n");
        exit(1);
    }

    if (!strcmp(ppm_format, "P3")) {
        fclose(ppm_file);
        fprintf(stderr, "unsupported ppm file format: %s, only supports P3",
                ppm_format);
        exit(1);
    }

    uint32_t width, height, max_value;

    fscanf(ppm_file, "%d%d%d", &width, &height, &max_value);

    color *image = (color *)malloc(width * height * sizeof(color));

    uint32_t r, g, b;

    for (uint32_t i = 0; i < width * height; ++i) {
        if (fscanf(ppm_file, "%d%d%d", &r, &g, &b) != 3) {
            fclose(ppm_file);
            fprintf(stderr, "error reading values\n");
            exit(1);
        }
        image[i] = (color){
            .x = r / (float)max_value,
            .y = g / (float)max_value,
            .z = b / (float)max_value,
        };
    }

    fclose(ppm_file);

    return (PPM_image){
        .width = width,
        .height = height,
        .image = image,
    };
}

color PPM_get_pixel(PPM_image ppm_image, uint32_t x, uint32_t y) {
    if (x < 0 || x >= ppm_image.width || y < 0 || y >= ppm_image.height) {
        fprintf(
            stderr,
            "coordinates (%d, %d) out of bounds reading ppm image of dimension "
            "(%d, %d)\n",
            x, y, ppm_image.width, ppm_image.height);
        exit(1);
    }
    uint32_t img_idx = x * ppm_image.height + y;
    return ppm_image.image[img_idx];
}

color PPM_get_pixel_from_normal_coords(PPM_image ppm_image, float nx,
                                       float ny) {
    if (nx < 0 || nx > 1 || ny < 0 || ny > 1) {
        fprintf(stderr, "normal coordinates (%f, %f) out of bounds\n", nx, ny);
        exit(1);
    }
    uint32_t x = (ppm_image.width - 1) * nx;
    uint32_t y = (ppm_image.height - 1) * ny;

    return PPM_get_pixel(ppm_image, x, y);
}
