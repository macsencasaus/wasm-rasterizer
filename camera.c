#include "camera.h"

#include <math.h>
#include <pthread.h>

#include "float.h"
#include "vec3.h"

static void draw_triangle(vec2i t0, vec2i t1, vec2i t2, triangle t,
                          uint8_t *image_buffer, float *z_buffer,
                          uint32_t image_height, uint32_t image_width,
                          color color);

static vec3 light_dir = {0, 0, -1};

void rasterize(uint8_t *image_buffer, camera *c, float vertices[],
               uint32_t face_count, float *z_buffer, color color) {
    uint32_t image_height = c->_image_height;
    uint32_t image_width = c->image_width;

    vec3 dir = vec3_sub(c->look_at, c->look_from);
    float focal_length = vec3_length(dir);

    triangle t;
    for (uint32_t k = 0; k < face_count; ++k) {
        t = *(triangle *)&vertices[k * 12];
        vec3 v1 = vec3_sub(t.v1, c->look_from);
        vec3 v2 = vec3_sub(t.v2, c->look_from);
        vec3 v3 = vec3_sub(t.v3, c->look_from);

        vec3 v1t = vec3_sub(vec3_scalar_mult(v1, (focal_length * focal_length) /
                                                     vec3_dot(dir, v1)),
                            c->_viewport_upper_left);

        vec2i v1_pixel = {
            vec3_dot(c->_pixel_delta_u, v1t) /
                vec3_length_squared(c->_pixel_delta_u),
            vec3_dot(c->_pixel_delta_v, v1t) /
                vec3_length_squared(c->_pixel_delta_v),
        };

        vec3 v2t = vec3_sub(vec3_scalar_mult(v2, (focal_length * focal_length) /
                                                     vec3_dot(dir, v2)),
                            c->_viewport_upper_left);

        vec2i v2_pixel = {
            vec3_dot(c->_pixel_delta_u, v2t) /
                vec3_length_squared(c->_pixel_delta_u),
            vec3_dot(c->_pixel_delta_v, v2t) /
                vec3_length_squared(c->_pixel_delta_v),
        };

        vec3 v3t = vec3_sub(vec3_scalar_mult(v3, (focal_length * focal_length) /
                                                     vec3_dot(dir, v3)),
                            c->_viewport_upper_left);
        vec2i v3_pixel = {
            vec3_dot(c->_pixel_delta_u, v3t) /
                vec3_length_squared(c->_pixel_delta_u),
            vec3_dot(c->_pixel_delta_v, v3t) /
                vec3_length_squared(c->_pixel_delta_v),
        };

        float intensity = -vec3_dot(light_dir, t.n);
        if (intensity > 0) {
            draw_triangle(v1_pixel, v2_pixel, v3_pixel, t, image_buffer,
                          z_buffer, image_height, image_width,
                          vec3_scalar_mult(color, intensity));
        }
    }
}
float look_from[3], look_at[3], vup[3];
void camera_initialize(camera *c, float image_width, float image_height,
                       float vfov) {
    c->image_width = image_width;
    c->_image_height = image_height;

    c->vfov = vfov;
    c->look_from = *(vec3 *)look_from;
    c->look_at = *(vec3 *)look_at;
    c->vup = *(vec3 *)vup;

    c->_pixel_samples_scale = 1.0 / c->samples_per_pixel;

    c->_center = c->look_from;

    float theta = degrees_to_radians(c->vfov);
    float h = tanf(theta / 2.0f);

    point3 viewport_mid = vec3_sub(c->look_at, c->_center);
    float viewport_height = 2 * h * vec3_length(viewport_mid);
    float viewport_width =
        viewport_height * ((float)c->image_width / c->_image_height);

    c->_w = vec3_normalize(vec3_sub(c->look_from, c->look_at));
    c->_u = vec3_normalize(vec3_cross(c->vup, c->_w));
    c->_v = vec3_cross(c->_w, c->_u);

    vec3 viewport_u = vec3_scalar_mult(c->_u, viewport_width);
    vec3 viewport_v = vec3_scalar_mult(c->_v, -viewport_height);

    // pixel delta vectors
    c->_pixel_delta_u = vec3_scalar_divide(viewport_u, c->image_width);
    c->_pixel_delta_v = vec3_scalar_divide(viewport_v, c->_image_height);

    point3 viewport_upper_left =
        vec3_sub(viewport_mid, vec3_add(vec3_scalar_divide(viewport_u, 2.0),
                                        vec3_scalar_divide(viewport_v, 2.0)));

    c->_viewport_upper_left = viewport_upper_left;
    c->_pixel00_loc = vec3_add(
        viewport_upper_left,
        vec3_scalar_mult(vec3_add(c->_pixel_delta_u, c->_pixel_delta_v), 0.5));
}

static vec3 barycentric(vec2i t0, vec2i t1, vec2i t2, vec3 P) {
    vec3 u = vec3_cross((vec3){t2.x - t0.x, t1.x - t0.x, t0.x - P.x},
                        (vec3){t2.y - t0.y, t1.y - t0.y, t0.y - P.y});

    if (fabsf(u.z) < 1) {
        return (vec3){-1, 1, 1};
    }
    return (vec3){1 - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z};
}

static void draw_triangle(vec2i t0, vec2i t1, vec2i t2, triangle t,
                          uint8_t *image_buffer, float *z_buffer,
                          uint32_t image_height, uint32_t image_width,
                          color color) {
    vec2 bbox_min = {FLT_MAX, FLT_MAX};
    vec2 bbox_max = {-FLT_MAX, -FLT_MAX};
    vec2 clamp = {image_width - 1, image_height - 1};

    bbox_min.x = fmaxf(0, fminf(t0.x, fminf(t1.x, t2.x)));
    bbox_min.y = fmaxf(0, fminf(t0.y, fminf(t1.y, t2.y)));

    bbox_max.x = fminf(clamp.x, fmaxf(t0.x, fmaxf(t1.x, t2.x)));
    bbox_max.y = fminf(clamp.y, fmaxf(t0.y, fmaxf(t1.y, t2.y)));

    vec3 P;
    for (P.x = bbox_min.x; P.x < bbox_max.x; ++P.x) {
        for (P.y = bbox_min.y; P.y < bbox_max.y; ++P.y) {
            vec3 bc_screen = barycentric(t0, t1, t2, P);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) {
                continue;
            }
            P.z = t.v1.z * bc_screen.x + t.v2.z * bc_screen.y +
                  t.v3.z * bc_screen.z;

            int image_idx = P.x * image_height * 4 + P.y * 4;
            if (image_idx >= image_height * image_width * 4 || image_idx < 0) {
                continue;
            }
            int z_buffer_idx = P.x * image_height + P.y;
            if (z_buffer[z_buffer_idx] > P.z) {
                continue;
            }
            z_buffer[z_buffer_idx] = P.z;
            // color = linear_to_gamma(color);
            image_buffer[image_idx + 0] = color.x;  // r
            image_buffer[image_idx + 1] = color.y;  // g
            image_buffer[image_idx + 2] = color.z;  // b
            image_buffer[image_idx + 3] = 255;      // a
        }
    }
}