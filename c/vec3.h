#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <wasm_simd128.h>

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

static inline vec3 vec3_add3(vec3 v1, vec3 v2, vec3 v3) {
    return (vec3){
        .x = v1.x + v2.x + v3.x,
        .y = v1.y + v2.y + v3.y,
        .z = v1.z + v2.z + v3.z,
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

typedef vec3 point3, color;

static inline color linear_to_gamma(color c) {
    c.x = sqrtf(c.x / 255.f) * 255.f;
    c.y = sqrtf(c.y / 255.f) * 255.f;
    c.z = sqrtf(c.z / 255.f) * 255.f;
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

static inline bool vec3_eq(vec3 v, vec3 u) {
    return v.x == u.x && v.y == u.y && v.z == u.z;
}

static inline bool vec3_is_pos(vec3 v) {
    return v.x > 0.f && v.y > 0.f && v.z > 0.f;
}

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

static inline vec2 vec2_add3(vec2 v1, vec2 v2, vec2 v3) {
    return (vec2){
        .x = v1.x + v2.x + v3.x,
        .y = v1.y + v2.y + v3.y,
    };
}

static inline vec2 vec2_scalar_mult(vec2 v, float t) {
    return (vec2){
        .x = v.x * t,
        .y = v.y * t,
    };
}

typedef struct {
    vec3 n1, n2, n3, v1, v2, v3;
    vec2 vt1, vt2, vt3;
} obj_triangle, obj_face;

typedef struct {
    float *x, *y, *z;
} vec3_soa;

typedef vec3_soa point3_soa, color_soa;

typedef struct {
    float *x, *y;
} vec2_soa;

typedef struct {
    uint32_t *x, *y, *z;
} vec3i_soa;

#define v_load wasm_v128_load
#define v_store wasm_v128_store
#define vf_splat wasm_f32x4_splat
#define vf_add wasm_f32x4_add
#define vf_mul wasm_f32x4_mul
#define vf_sub wasm_f32x4_sub
#define vf_div wasm_f32x4_div
#define vf_max wasm_f32x4_max
#define vf_min wasm_f32x4_min
#define vf_shuffle wasm_v32x4_shuffle
#define vf_ex_lane wasm_f32x4_extract_lane

static inline v128_t vf_add3(v128_t v1, v128_t v2, v128_t v3) {
    return vf_add(v1, vf_add(v2, v3));
}

static inline float vf_h_add(v128_t vec) {
    v128_t sum1 = vf_add(vec, vf_shuffle(vec, vec, 2, 3, 0, 1));
    v128_t sum2 = vf_add(sum1, vf_shuffle(sum1, sum1, 1, 0, 3, 2));
    return vf_ex_lane(sum2, 0);
}

static inline v128_t vf_h_add_splat(v128_t vec) {
    v128_t sum1 = vf_add(vec, vf_shuffle(vec, vec, 2, 3, 0, 1));
    return vf_add(sum1, vf_shuffle(sum1, sum1, 1, 0, 3, 2));
}

static inline float vf_h_max(v128_t vec) {
    v128_t sum1 = vf_max(vec, vf_shuffle(vec, vec, 2, 3, 0, 1));
    v128_t sum2 = vf_max(sum1, vf_shuffle(sum1, sum1, 1, 0, 3, 2));
    return vf_ex_lane(sum2, 0);
}

static inline v128_t vf_h_max_splat(v128_t vec) {
    v128_t max1 = vf_max(vec, vf_shuffle(vec, vec, 2, 3, 0, 1));
    return vf_max(max1, vf_shuffle(max1, max1, 1, 0, 3, 2));
}

static inline v128_t vf_h_min_splat(v128_t vec) {
    v128_t min1 = vf_min(vec, vf_shuffle(vec, vec, 2, 3, 0, 1));
    return vf_min(min1, vf_shuffle(min1, min1, 1, 0, 3, 2));
}

typedef struct {
    v128_t x, y, z;
} vec3v;

static inline vec3v vec3v_from_vec3(vec3 v) {
    return (vec3v){
        .x = vf_splat(v.x),
        .y = vf_splat(v.y),
        .z = vf_splat(v.z),
    };
}

static inline vec3v vec3v_load(vec3_soa v, size_t i) {
    return (vec3v){
        .x = v_load(v.x + i),
        .y = v_load(v.y + i),
        .z = v_load(v.z + i),
    };
}

static inline void vec3_store(vec3_soa v, size_t i, vec3v vec) {
    v_store(v.x + i, vec.x);
    v_store(v.y + i, vec.y);
    v_store(v.z + i, vec.z);
}

static inline vec3v vec3v_splat(float a) {
    return (vec3v){
        .x = vf_splat(a),
        .y = vf_splat(a),
        .z = vf_splat(a),
    };
}

static inline vec3v vec3v_add(vec3v v1, vec3v v2) {
    return (vec3v){
        .x = vf_add(v1.x, v2.x),
        .y = vf_add(v1.y, v2.y),
        .z = vf_add(v1.z, v2.z),
    };
}

static inline vec3v vec3v_sub(vec3v v1, vec3v v2) {
    return (vec3v){
        .x = vf_sub(v1.x, v2.x),
        .y = vf_sub(v1.y, v2.y),
        .z = vf_sub(v1.z, v2.z),
    };
}

static inline vec3v vec3v_div(vec3v v1, vec3v v2) {
    return (vec3v){
        .x = vf_div(v1.x, v2.x),
        .y = vf_div(v1.y, v2.y),
        .z = vf_div(v1.z, v2.z),
    };
}

static inline vec3v vec3v_v128_mul(vec3v v, v128_t t) {
    return (vec3v){
        .x = vf_mul(v.x, t),
        .y = vf_mul(v.y, t),
        .z = vf_mul(v.z, t),
    };
}

static inline vec3v vec3v_scalar_div(vec3v v, float t) {
    v128_t t_v = vf_splat(t);
    return (vec3v){
        .x = vf_div(v.x, t_v),
        .y = vf_div(v.y, t_v),
        .z = vf_div(v.z, t_v),
    };
}

static inline vec3v vec3v_h_add_splat(vec3v v) {
    return (vec3v){
        .x = vf_h_add_splat(v.x),
        .y = vf_h_add_splat(v.y),
        .z = vf_h_add_splat(v.z),
    };
}

typedef struct {
    v128_t e[3][3];
} mat3v;

// clang-format off
static inline mat3v new_mat3v(float a11, float a12, float a13,
                              float a21, float a22, float a23,
                              float a31, float a32, float a33) {
    return (mat3v){{
        {vf_splat(a11), vf_splat(a12), vf_splat(a13)},
        {vf_splat(a21), vf_splat(a22), vf_splat(a23)},
        {vf_splat(a31), vf_splat(a32), vf_splat(a33)},
    }};
};
// clang-format on

static inline vec3v mat3v_vec3v_mul(mat3v mat, vec3v vec) {
    v128_t a11 = vf_mul(mat.e[0][0], vec.x);
    v128_t a21 = vf_mul(mat.e[1][0], vec.x);
    v128_t a31 = vf_mul(mat.e[2][0], vec.x);

    v128_t a12 = vf_mul(mat.e[0][1], vec.y);
    v128_t a22 = vf_mul(mat.e[1][1], vec.y);
    v128_t a32 = vf_mul(mat.e[2][1], vec.y);

    v128_t a13 = vf_mul(mat.e[0][2], vec.z);
    v128_t a23 = vf_mul(mat.e[1][2], vec.z);
    v128_t a33 = vf_mul(mat.e[2][2], vec.z);

    return (vec3v){
        .x = vf_add3(a11, a12, a13),
        .y = vf_add3(a21, a22, a23),
        .z = vf_add3(a31, a32, a33),
    };
}

#endif  // VEC3_h
