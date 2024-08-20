#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"

typedef struct {
    uint32_t image_width;
    uint32_t _image_height;
    vec3 look_at;
    vec3 look_from;
    int samples_per_pixel;
    vec3 vup;
    float vfov;

    point3 _center;
    vec3 _viewport_upper_left;
    vec3 _pixel00_loc;
    vec3 _pixel_delta_u;
    vec3 _pixel_delta_v;
    float _pixel_samples_scale;

    vec3 _w;
    vec3 _u;
    vec3 _v;

} camera;

void rasterize(uint8_t *image_buffer, camera *c, float vertices[],
               uint32_t face_count, float *z_buffer, color color);
void camera_initialize(camera *c, float image_width, float image_height,
                       float vfov);
#endif  // CAMERA_H
