#ifndef CAMERA_H
#define CAMERA_H

#include "obj.h"
#include "texture.h"
#include "vec3.h"

// extern float look_from[3], look_at[3], vup[3];

typedef struct {
    uint32_t image_width;
    uint32_t image_height;
    uint32_t image_channels;
    vec3 look_at;
    vec3 look_from;
    vec3 vup;
    float vfov;

    point3 _center;
    vec3 _viewport_upper_left;
    vec3 _pixel00_loc;
    vec3 _pixel_delta_u;
    vec3 _pixel_delta_v;

    vec3 _w;
    vec3 _u;
    vec3 _v;
} camera;

void rasterize_stl(uint8_t *image_buffer, camera *c, float vertices[],
                   uint32_t face_count, float *z_buffer, color color);

void rasterize_obj(uint8_t *image_buffer, camera *c, object *obj,
                   texture_image *texture, float *z_buffer);

void camera_initialize(camera *c, uint32_t image_width, uint32_t image_height,
                       uint32_t image_channels, float vfov);

void position_and_scale_obj(object *obj, vec3 pos, vec3 rot, float height);

#endif  // CAMERA_H
