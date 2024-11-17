#include "camera.h"
#include "float.h"

extern unsigned int __heap_base;

#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"

void *bump_pointer = &__heap_base;
void *bump_malloc(int n) {
    void *r = bump_pointer;
    bump_pointer += n;
    return r;
}

camera cam = {0};

float test_func(camera *cam) { return cam->look_at.z; }

// position, scale, & rotate
// you must have set the position, rotation, and height
// in the object before invoking
void obj_psr(object *obj) {
    vec3 position = obj->position;
    vec3 rotation = obj->rotation;
    float height = obj->height;

    float max_y = -FLT_MAX, min_y = FLT_MAX;
    point3 center_grav = {0};

    vec3 v, n;

    for (size_t i = 0; i < obj->vertex_count; ++i) {
        v = obj->vertices[i];

        max_y = fmaxf(v.y, max_y);
        min_y = fminf(v.y, min_y);

        center_grav = vec3_add(center_grav, v);
    }

    center_grav = vec3_scalar_divide(center_grav, obj->vertex_count);

    float cur_height = max_y - min_y;
    float height_adjust = height / cur_height;

    float theta_x = degrees_to_radians(rotation.x);
    float theta_y = degrees_to_radians(rotation.y);
    float theta_z = degrees_to_radians(rotation.z);

    // clang-format off
    mat3 roll_rotation = {{
        {1, 0             , 0            },
        {0, cosf(theta_x) , sinf(theta_x)},
        {0, -sinf(theta_x), cosf(theta_x)},
    }};

    mat3 pitch_rotation = {{
        {cosf(theta_y), 0, -sinf(theta_y)},
        {0            , 1, 0             },
        {sinf(theta_y), 0, cosf(theta_y) },
    }};

    mat3 yaw_rotation = {{
        {cosf(theta_z) , sinf(theta_z), 0},
        {-sinf(theta_z), cosf(theta_z), 0},
        {0             , 0            , 1},
    }};
    // clang-format on

    for (size_t i = 0; i < obj->vertex_count; ++i) {
        v = obj->vertices[i];

        v = vec3_sub(v, center_grav);            // center
        v = vec3_scalar_mult(v, height_adjust);  // scale
        v = mat3_vec3_mult(roll_rotation, v);    // rotate
        v = mat3_vec3_mult(pitch_rotation, v);   //
        v = mat3_vec3_mult(yaw_rotation, v);     //
        v = vec3_add(v, position);               // position

        obj->vertices[i] = v;

        n = obj->vertex_normals[i];

        n = mat3_vec3_mult(roll_rotation, n);
        n = mat3_vec3_mult(pitch_rotation, n);
        n = mat3_vec3_mult(yaw_rotation, n);

        obj->vertex_normals[i] = n;
    }
}

void obj_shift_by(object *obj, float sx, float sy, float sz) {
    vec3 shift = (vec3){sx, sy, sz};

    for (size_t i = 0; i < obj->vertex_count; ++i) {
        obj->vertices[i] = vec3_add(obj->vertices[i], shift);
    }

    obj->position = vec3_add(obj->position, shift);
}

void obj_rotate_by(object *obj, float rx, float ry, float rz) {
    vec3 position = obj->position;
    vec3 rotation = obj->rotation;

    float theta_x = degrees_to_radians(rx);
    float theta_y = degrees_to_radians(ry);
    float theta_z = degrees_to_radians(rz);

    // clang-format off
    mat3 roll_rotation = {{
        {1, 0             , 0            },
        {0, cosf(theta_x) , sinf(theta_x)},
        {0, -sinf(theta_x), cosf(theta_x)},
    }};

    mat3 pitch_rotation = {{
        {cosf(theta_y), 0, -sinf(theta_y)},
        {0            , 1, 0             },
        {sinf(theta_y), 0, cosf(theta_y) },
    }};

    mat3 yaw_rotation = {{
        {cosf(theta_z) , sinf(theta_z), 0},
        {-sinf(theta_z), cosf(theta_z), 0},
        {0             , 0            , 1},
    }};
    // clang-format on

    vec3 v, n;
    for (size_t i = 0; i < obj->vertex_count; ++i) {
        v = obj->vertices[i];

        v = vec3_sub(v, position);
        v = mat3_vec3_mult(roll_rotation, v);
        v = mat3_vec3_mult(pitch_rotation, v);
        v = mat3_vec3_mult(yaw_rotation, v);
        v = vec3_add(v, position);

        obj->vertices[i] = v;

        n = obj->vertex_normals[i];

        n = mat3_vec3_mult(roll_rotation, n);
        n = mat3_vec3_mult(pitch_rotation, n);
        n = mat3_vec3_mult(yaw_rotation, n);

        obj->vertex_normals[i] = n;
    }

    obj->rotation = vec3_add(rotation, (vec3){rx, ry, rz});
}

void obj_set_position(object *obj, float x, float y, float z) {
    obj->position = (vec3){x, y, z};
}

void obj_set_rotation(object *obj, float x, float y, float z) {
    obj->rotation = (vec3){x, y, z};
}

void obj_set_height(object *obj, float height) { obj->height = height; }
