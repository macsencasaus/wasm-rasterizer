#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#define INF INFINITY
#define PI 3.1415926535897932385f

typedef struct vec3 {
    float x, y, z;
} vec3;

static inline vec3 vec3_neg(vec3 v) { return (vec3){-v.x, -v.y, -v.z}; }

static inline vec3 vec3_add(vec3 v1, vec3 v2) {
    return (vec3){
        .x = v1.x + v2.x,
        .y = v1.y + v2.y,
        .z = v1.z + v2.z,
    };
}

static inline vec3 vec3_sub(vec3 v1, vec3 v2) {
    return (vec3){
        .x = v1.x - v2.x,
        .y = v1.y - v2.y,
        .z = v1.z - v2.z,
    };
}

static inline vec3 vec3_mult(vec3 v1, vec3 v2) {
    return (vec3){
        .x = v1.x * v2.x,
        .y = v1.y * v2.y,
        .z = v1.z * v2.z,
    };
}

static inline vec3 vec3_scalar_add(vec3 v, float t) {
    return (vec3){v.x + t, v.y + t, v.z + t};
}

static inline vec3 vec3_scalar_mult(vec3 v, float t) {
    return (vec3){v.x * t, v.y * t, v.z * t};
}

static inline vec3 vec3_scalar_divide(vec3 v, float t) {
    return vec3_scalar_mult(v, 1 / t);
}

static inline float vec3_length_squared(vec3 this) {
    return this.x * this.x + this.y * this.y + this.z * this.z;
}

static inline float vec3_length(vec3 v) {
    return sqrtf(vec3_length_squared(v));
}

typedef struct {
    uint32_t m_z;
    uint32_t m_w;
} random_state;

// source:
// https://stackoverflow.com/questions/612860/what-can-make-a-program-run-slower-when-using-more-threads
static inline uint32_t getRandom(uint32_t *m_z, uint32_t *m_w) {
    *m_z = 36969 * (*m_z & 65535) + (*m_z >> 16);
    *m_w = 18000 * (*m_w & 65535) + (*m_w >> 16);
    return (*m_z << 16) + *m_w;
}

static inline float random_float(random_state *rs) {
    return (getRandom(&rs->m_z, &rs->m_w) + 1) * 2.328306435454494e-10;
}

static inline float random_float_range(float min, float max, random_state *rs) {
    return min + (max - min) * random_float(rs);
}

static inline vec3 vec3_random(random_state *rs) {
    return (vec3){random_float(rs), random_float(rs), random_float(rs)};
}

static inline vec3 vec3_random_range(float min, float max, random_state *rs) {
    return (vec3){random_float_range(min, max, rs),
                  random_float_range(min, max, rs),
                  random_float_range(min, max, rs)};
}

typedef vec3 point3, color;

static inline color linear_to_gamma(color c) {
    c.x = sqrtf(c.x / 255) * 255;
    c.y = sqrtf(c.y / 255) * 255;
    c.z = sqrtf(c.z / 255) * 255;
    return c;
}

static inline float vec3_dot(vec3 v1, vec3 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

static inline vec3 vec3_cross(vec3 v1, vec3 v2) {
    return (vec3){
        .x = v1.y * v2.z - v1.z * v2.y,
        .y = v1.z * v2.x - v1.x * v2.z,
        .z = v1.x * v2.y - v1.y * v2.x,
    };
}

static inline vec3 vec3_normalize(vec3 v) {
    return vec3_scalar_divide(v, vec3_length(v));
}

static inline vec3 vec3_random_in_unit_disk(random_state *rs) {
    while (true) {
        vec3 p = (vec3){random_float_range(-1, 1, rs),
                        random_float_range(-1, 1, rs), 0};
        if (vec3_length_squared(p) < 1) return p;
    }
}

static inline vec3 vec3_random_in_unit_sphere(random_state *rs) {
    while (true) {
        vec3 p = vec3_random_range(-1, 1, rs);
        if (vec3_length_squared(p) < 1) return p;
    }
}

static inline vec3 vec3_random_unit_vector(random_state *rs) {
    return vec3_normalize(vec3_random_in_unit_sphere(rs));
}

static inline vec3 vec3_random_on_hemisphere(vec3 normal, random_state *rs) {
    vec3 on_unit_sphere = vec3_random_unit_vector(rs);
    if (vec3_dot(on_unit_sphere, normal) > 0.0) {
        return on_unit_sphere;
    } else {
        return vec3_neg(on_unit_sphere);
    }
}

static inline bool vec3_near_zero(vec3 v) {
    return (fabsf(v.x) < 1e-8) && (fabsf(v.y) < 1e-8) && (fabsf(v.z) < 1e-8);
}

static inline vec3 vec3_reflect(vec3 v, vec3 n) {
    return vec3_sub(v, vec3_scalar_mult(n, 2 * vec3_dot(v, n)));
}

static inline vec3 vec3_refract(vec3 uv, vec3 n, float etai_over_etat) {
    float cos_theta = fminf(vec3_dot(vec3_neg(uv), n), 1.0);

    vec3 r_out_perp = vec3_scalar_mult(
        vec3_add(uv, vec3_scalar_mult(n, cos_theta)), etai_over_etat);
    vec3 r_out_parallel = vec3_scalar_mult(
        n, -sqrtf(fabsf(1.0f - vec3_length_squared(r_out_perp))));
    return vec3_add(r_out_perp, r_out_parallel);
}

static inline bool vec3_eq(vec3 v, vec3 u) {
    return v.x == u.x && v.y == u.y && v.z == u.z;
}

static inline bool vec3_is_pos(vec3 v) { return v.x > 0 && v.y > 0 && v.z > 0; }

typedef struct ray {
    point3 orig;
    vec3 dir;
} ray;

static inline point3 ray_at(ray r, float t) {
    return vec3_add(r.orig, vec3_scalar_mult(r.dir, t));
}

static inline float degrees_to_radians(float degrees) {
    return degrees * PI / 180.0f;
}

typedef struct interval {
    float min, max;
} interval;

static inline float interval_size(interval i) { return i.max - i.min; }

static inline bool interval_contains(interval i, float x) {
    return i.min <= x && x <= i.max;
}

static inline bool interval_surrounds(interval i, float x) {
    return i.min < x && x < i.max;
}

static inline float clamp(interval i, float x) {
    return x < i.min ? i.min : x > i.max ? i.max : x;
}

typedef struct {
    vec3 n, v1, v2, v3;
} triangle, face;

typedef struct {
    int x, y;
} vec2i;

static inline vec2i vec2i_add(vec2i v1, vec2i v2) {
    return (vec2i){v1.x + v2.x, v1.y + v2.y};
}

static inline vec2i vec2i_sub(vec2i v1, vec2i v2) {
    return (vec2i){v1.x - v2.x, v1.y - v2.y};
}

static inline vec2i vec2i_scalarMultf(vec2i v1, float t) {
    return (vec2i){v1.x * t, v1.y * t};
}

static inline bool vec2i_eq(vec2i a, vec2i b) {
    return a.x == b.x && a.y == b.y;
}

static inline void vec2i_swap(vec2i *a, vec2i *b) {
    vec2i temp = *b;
    *b = *a;
    *a = temp;
}

typedef struct {
    float e[3][3];
} mat3;

static inline vec3 mat3_vec3_mult(mat3 A, vec3 b) {
    vec3 v = {0};
    v.x = A.e[0][0] * b.x + A.e[0][1] * b.y + A.e[0][2] * b.z;
    v.y = A.e[1][0] * b.x + A.e[1][1] * b.y + A.e[1][2] * b.z;
    v.z = A.e[2][0] * b.x + A.e[2][1] * b.y + A.e[2][2] * b.z;
    return v;
}

typedef struct {
    float x, y;
} vec2;

#endif  // VEC3_h
