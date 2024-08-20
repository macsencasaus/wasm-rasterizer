#include "camera.h"
#include "float.h"

extern unsigned int __heap_base;

#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"

void *bump_pointer = &__heap_base;
void *malloc(int n) {
    void *r = bump_pointer;
    bump_pointer += n;
    return r;
}

void *new_vec3(float x, float y, float z) {
    float *v3 = (float *)malloc(3 * sizeof(float));
    v3[0] = x;
    v3[1] = y;
    v3[2] = z;
    return (void *)v3;
}

camera cam = {0};

void rasterize_cars(uint8_t image_buffer[], float z_buffer[], float vertices[],
                    uint32_t face_count) {
    rasterize(image_buffer, &cam, vertices, face_count, z_buffer,
              (color){255, 255, 255});
}

void STL_position_and_normalize(float vertices[], uint32_t face_count,
                                float pos[], float rot[], float height) {
    vec3 position = *(vec3 *)pos;
    vec3 rotation = *(vec3 *)rot;

    float max_y = -FLT_MAX, min_y = FLT_MAX;
    point3 center_grav = {0};

    vec3 v1, v2, v3, n;

    for (uint32_t i = 0; i < face_count; ++i) {
        v1 = *(vec3 *)&vertices[i * 12 + 3];
        v2 = *(vec3 *)&vertices[i * 12 + 6];
        v3 = *(vec3 *)&vertices[i * 12 + 9];

        max_y = fmaxf(v1.y, max_y);
        max_y = fmaxf(v2.y, max_y);
        max_y = fmaxf(v3.y, max_y);

        min_y = fminf(v1.y, min_y);
        min_y = fminf(v2.y, min_y);
        min_y = fminf(v3.y, min_y);

        center_grav = vec3_add(center_grav, vec3_add(v1, vec3_add(v2, v3)));
    }

    center_grav = vec3_scalar_divide(center_grav, face_count * 3);

    float cur_height = max_y - min_y;
    float height_adjust = height / cur_height;

    float theta_x = degrees_to_radians(rotation.x);
    float theta_y = degrees_to_radians(rotation.y);
    float theta_z = degrees_to_radians(rotation.z);

    mat3 roll_rotation = {{
        {1, 0, 0},
        {0, cos(theta_x), sin(theta_x)},
        {0, -sin(theta_x), cos(theta_x)},
    }};

    mat3 pitch_rotation = {{
        {cos(theta_y), 0, -sin(theta_y)},
        {0, 1, 0},
        {sin(theta_y), 0, cos(theta_y)},
    }};

    mat3 yaw_rotation = {{
        {cos(theta_z), sin(theta_z), 0},
        {-sin(theta_z), cos(theta_z), 0},
        {0, 0, 1},
    }};

    for (uint32_t i = 0; i < face_count; ++i) {
        n = *(vec3 *)&vertices[i * 12 + 0];
        v1 = *(vec3 *)&vertices[i * 12 + 3];
        v2 = *(vec3 *)&vertices[i * 12 + 6];
        v3 = *(vec3 *)&vertices[i * 12 + 9];

        v1 = vec3_add(v1, vec3_neg(center_grav));
        v1 = vec3_scalar_mult(v1, height_adjust);
        v1 = mat3_vec3_mult(roll_rotation, v1);
        v1 = mat3_vec3_mult(pitch_rotation, v1);
        v1 = mat3_vec3_mult(yaw_rotation, v1);
        v1 = vec3_add(v1, position);

        v2 = vec3_add(v2, vec3_neg(center_grav));
        v2 = vec3_scalar_mult(v2, height_adjust);
        v2 = mat3_vec3_mult(roll_rotation, v2);
        v2 = mat3_vec3_mult(pitch_rotation, v2);
        v2 = mat3_vec3_mult(yaw_rotation, v2);
        v2 = vec3_add(v2, position);

        v3 = vec3_add(v3, vec3_neg(center_grav));
        v3 = vec3_scalar_mult(v3, height_adjust);
        v3 = mat3_vec3_mult(roll_rotation, v3);
        v3 = mat3_vec3_mult(pitch_rotation, v3);
        v3 = mat3_vec3_mult(yaw_rotation, v3);
        v3 = vec3_add(v3, position);

        n = mat3_vec3_mult(roll_rotation, n);
        n = mat3_vec3_mult(pitch_rotation, n);
        n = mat3_vec3_mult(yaw_rotation, n);

        *(vec3 *)&vertices[i * 12 + 0] = n;
        *(vec3 *)&vertices[i * 12 + 3] = v1;
        *(vec3 *)&vertices[i * 12 + 6] = v2;
        *(vec3 *)&vertices[i * 12 + 9] = v3;
    }
}
