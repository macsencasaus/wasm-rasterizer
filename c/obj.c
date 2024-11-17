#include "obj.h"

#include <assert.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wasm_simd128.h>

object OBJ_read_file(const char *obj_filepath) {
    FILE *obj_file = fopen(obj_filepath, "r");
    if (obj_file == NULL) {
        fprintf(stderr, "error reading obj file: %s\n", obj_filepath);
        exit(1);
    }

    uint32_t vertex_count = 0, vertex_texture_count = 0,
             vertex_normal_count = 0, face_count = 0;

#define BUFFER_SIZE 128
    char buffer[BUFFER_SIZE];

    while (fgets(buffer, BUFFER_SIZE, obj_file)) {
        switch (buffer[0]) {
            case 'v': {
                switch (buffer[1]) {
                    case ' ': {
                        ++vertex_count;
                    } break;
                    case 't': {
                        if (buffer[2] == ' ') {
                            ++vertex_texture_count;
                        }
                    } break;
                    case 'n': {
                        if (buffer[2] == ' ') {
                            ++vertex_normal_count;
                        }
                    } break;
                }
            } break;
            case 'f': {
                if (buffer[1] == ' ') {
                    ++face_count;
                }
            } break;
        }
    }

    if (ferror(obj_file)) {
        fclose(obj_file);
        fprintf(stderr, "Error reading file\n");
        exit(1);
    }

    void *arena = malloc(vertex_count * sizeof(point3) +
                         vertex_texture_count * sizeof(vec2) +
                         vertex_normal_count * sizeof(point3) +
                         face_count * sizeof(object_face));

    if (arena == NULL) {
        fclose(obj_file);
        fprintf(stderr, "error malloc obj arena\n");
        exit(1);
    }

    point3 *vertices = (point3 *)arena;

    vec2 *vertex_textures = (vec2 *)(vertices + vertex_count);

    point3 *vertex_normals = (point3 *)(vertex_textures + vertex_texture_count);

    object_face *faces = (object_face *)(vertex_normals + vertex_normal_count);

    if (fseek(obj_file, 0, SEEK_SET) != 0) {
        fclose(obj_file);
        fprintf(stderr, "error returning to start of file\n");
        exit(1);
    };

    uint32_t v_idx = 0, vt_idx = 0, vn_idx = 0, f_idx = 0;

    float x, y, z, w;
    uint32_t i, j, k;

    while (fscanf(obj_file, "%s", buffer) != EOF) {
        if (ferror(obj_file)) {
            fclose(obj_file);
            fprintf(stderr, "Error reading file\n");
            exit(1);
        }
        if (strcmp(buffer, "v") == 0) {
            if (fscanf(obj_file, "%f %f %f %f", &x, &y, &z, &w) < 3) {
                fclose(obj_file);
                fprintf(stderr, "error reading vertex\n");
                exit(1);
            }
            vertices[v_idx] = (point3){x, y, z};
            ++v_idx;
            continue;
        }
        if (strcmp(buffer, "vt") == 0) {
            if (fscanf(obj_file, "%f %f %f", &x, &y, &w) < 2) {
                fclose(obj_file);
                fprintf(stderr, "error reading vertex texture\n");
                exit(1);
            }
            vertex_textures[vt_idx] = (vec2){x, y};
            ++vt_idx;
            continue;
        }
        if (strcmp(buffer, "vn") == 0) {
            if (fscanf(obj_file, "%f %f %f", &x, &y, &z) < 3) {
                fclose(obj_file);
                fprintf(stderr, "error reading vertex normals\n");
                exit(1);
            }
            vertex_normals[vn_idx] = (point3){x, y, z};
            ++vn_idx;
            continue;
        }
        if (strcmp(buffer, "f") == 0) {
            for (int h = 0; h < 3; ++h) {
                if (fscanf(obj_file, "%u/%u/%u", &i, &j, &k) != 3) {
                    fclose(obj_file);
                    fprintf(stderr,
                            "error reading face, face must contain "
                            "vertex/vertex_texture/vertex_normal x 3");
                    exit(1);
                };
                faces[f_idx].vertex_idxs[h] = i - 1;
                faces[f_idx].vertex_texture_idxs[h] = j - 1;
                faces[f_idx].vertex_normal_idxs[h] = k - 1;
            }
            ++f_idx;
            continue;
        }
    }

    fclose(obj_file);

    assert(vertex_count == vertex_normal_count);

    return (object){
        .vertex_count = vertex_count,
        .vertex_normal_count = vertex_normal_count,
        .vertex_texture_count = vertex_texture_count,
        .face_count = face_count,

        .arena = arena,

        .vertices = vertices,
        .vertex_textures = vertex_textures,
        .vertex_normals = vertex_normals,
        .faces = faces,
    };
}

OBJ_soa OBJ_read_file_soa(const char *obj_filepath) {
    FILE *obj_file = fopen(obj_filepath, "r");
    if (obj_file == NULL) {
        fprintf(stderr, "error reading obj file: %s\n", obj_filepath);
        exit(1);
    }

    uint32_t vertex_count = 0, vertex_texture_count = 0,
             vertex_normal_count = 0, face_count = 0;

#define BUFFER_SIZE 128
    char buffer[BUFFER_SIZE];

    while (fgets(buffer, BUFFER_SIZE, obj_file)) {
        switch (buffer[0]) {
            case 'v': {
                switch (buffer[1]) {
                    case ' ': {
                        ++vertex_count;
                    } break;
                    case 't': {
                        if (buffer[2] == ' ') {
                            ++vertex_texture_count;
                        }
                    } break;
                    case 'n': {
                        if (buffer[2] == ' ') {
                            ++vertex_normal_count;
                        }
                    } break;
                }
            } break;
            case 'f': {
                if (buffer[1] == ' ') {
                    ++face_count;
                }
            } break;
        }
    }

    if (ferror(obj_file)) {
        fclose(obj_file);
        fprintf(stderr, "Error reading file\n");
        exit(1);
    }

    void *arena = malloc(vertex_count * sizeof(point3) +
                         vertex_texture_count * sizeof(vec2) +
                         vertex_normal_count * sizeof(point3) +
                         face_count * sizeof(object_face));

    if (arena == NULL) {
        fclose(obj_file);
        fprintf(stderr, "error malloc obj arena\n");
        exit(1);
    }

    float *vertices_start = (float *)arena;
    point3_soa vertices = {
        .x = vertices_start,
        .y = vertices_start + vertex_count,
        .z = vertices_start + 2 * vertex_count,
    };

    float *vertex_texture_start = vertices_start + 3 * vertex_count;
    vec2_soa vertex_textures = {
        .x = vertex_texture_start,
        .y = vertex_texture_start + vertex_texture_count,
    };

    float *vertex_normal_start =
        vertex_texture_start + 3 * vertex_texture_count;
    point3_soa vertex_normals = {
        .x = vertex_normal_start,
        .y = vertex_normal_start + vertex_normal_count,
        .z = vertex_normal_start + 2 * vertex_normal_count,
    };

    uint32_t *faces_start =
        (uint32_t *)(vertex_normal_start + 3 * vertex_normal_count);

    face_element_soa faces = {
        .vertex_idxs =
            (vec3i_soa){
                .x = faces_start,
                .y = faces_start + face_count,
                .z = faces_start + 2 * face_count,
            },
        .vertex_texture_idxs =
            (vec3i_soa){
                .x = faces_start + 3 * face_count,
                .y = faces_start + 4 * face_count,
                .z = faces_start + 5 * face_count,
            },
        .vertex_normal_idxs =
            (vec3i_soa){
                .x = faces_start + 6 * face_count,
                .y = faces_start + 7 * face_count,
                .z = faces_start + 8 * face_count,
            },
    };

    uint32_t v_idx = 0, vt_idx = 0, vn_idx = 0, f_idx = 0;

    float x, y, z, w;
    uint32_t i, j, k;

    while (fscanf(obj_file, "%s", buffer) != EOF) {
        if (ferror(obj_file)) {
            fclose(obj_file);
            fprintf(stderr, "Error reading file\n");
            exit(1);
        }
        if (strcmp(buffer, "v") == 0) {
            if (fscanf(obj_file, "%f %f %f %f", &x, &y, &z, &w) < 3) {
                fclose(obj_file);
                fprintf(stderr, "error reading vertex\n");
                exit(1);
            }
            vertices.x[v_idx] = x;
            vertices.y[v_idx] = y;
            vertices.z[v_idx] = z;
            ++v_idx;
            continue;
        }
        if (strcmp(buffer, "vt") == 0) {
            if (fscanf(obj_file, "%f %f %f", &x, &y, &w) < 2) {
                fclose(obj_file);
                fprintf(stderr, "error reading vertex texture\n");
                exit(1);
            }
            vertex_textures.x[vt_idx] = x;
            vertex_textures.y[vt_idx] = y;
            ++vt_idx;
            continue;
        }
        if (strcmp(buffer, "vn") == 0) {
            if (fscanf(obj_file, "%f %f %f", &x, &y, &z) < 3) {
                fclose(obj_file);
                fprintf(stderr, "error reading vertex normals\n");
                exit(1);
            }
            vertex_normals.x[vn_idx] = x;
            vertex_normals.y[vn_idx] = y;
            vertex_normals.z[vn_idx] = z;
            ++vn_idx;
            continue;
        }
        if (strcmp(buffer, "f") == 0) {
            for (int h = 0; h < 3; ++h) {
                if (fscanf(obj_file, "%u/%u/%u", &i, &j, &k) != 3) {
                    fclose(obj_file);
                    fprintf(stderr,
                            "error reading face, face must contain "
                            "vertex/vertex_texture/vertex_normal x 3");
                    exit(1);
                };

                switch (h) {
                    case 0: {
                        faces.vertex_idxs.x[f_idx] = i - 1;
                        faces.vertex_texture_idxs.x[f_idx] = j - 1;
                        faces.vertex_normal_idxs.x[f_idx] = k - 1;
                    } break;
                    case 1: {
                        faces.vertex_idxs.y[f_idx] = i - 1;
                        faces.vertex_texture_idxs.y[f_idx] = j - 1;
                        faces.vertex_normal_idxs.y[f_idx] = k - 1;
                    } break;
                    case 2: {
                        faces.vertex_idxs.z[f_idx] = i - 1;
                        faces.vertex_texture_idxs.z[f_idx] = j - 1;
                        faces.vertex_normal_idxs.z[f_idx] = k - 1;
                    } break;
                }
            }
            ++f_idx;
            continue;
        }
    }

    fclose(obj_file);

    assert(vertex_count == vertex_normal_count);

    return (OBJ_soa){
        .vertex_count = vertex_count,
        .vertex_texture_count = vertex_texture_count,
        .vertex_normal_count = vertex_normal_count,

        .arena = arena,

        .vertices = vertices,
        .vertex_textures = vertex_textures,
        .vertex_normals = vertex_normals,
        .faces = faces,
    };
}

void OBJ_position_and_scale(object *obj, vec3 *posp, vec3 *rotationp,
                            float height) {
    vec3 pos = *posp;
    vec3 rotation = *rotationp;

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
        {1, 0            , 0           },
        {0, cosf(theta_x) , sinf(theta_x)},
        {0, -sinf(theta_x), cosf(theta_x)},
    }};

    mat3 pitch_rotation = {{
        {cosf(theta_y), 0, -sinf(theta_y)},
        {0           , 1, 0            },
        {sinf(theta_y), 0, cosf(theta_y) },
    }};

    mat3 yaw_rotation = {{
        {cosf(theta_z) , sinf(theta_z), 0},
        {-sinf(theta_z), cosf(theta_z), 0},
        {0            , 0           , 1},
    }};
    // clang-format on

    for (uint32_t i = 0; i < obj->vertex_count; ++i) {
        v = obj->vertices[i];

        v = vec3_sub(v, center_grav);            // center
        v = vec3_scalar_mult(v, height_adjust);  // scale
        v = mat3_vec3_mult(roll_rotation, v);    // rotate
        v = mat3_vec3_mult(pitch_rotation, v);   //
        v = mat3_vec3_mult(yaw_rotation, v);     //
        v = vec3_add(v, pos);                    // position

        obj->vertices[i] = v;

        n = obj->vertex_normals[i];

        n = mat3_vec3_mult(roll_rotation, n);
        n = mat3_vec3_mult(pitch_rotation, n);
        n = mat3_vec3_mult(yaw_rotation, n);

        obj->vertex_normals[i] = n;
    }
}

void OBJ_position_and_scale_soa(OBJ_soa *obj, point3 pos, vec3 rotation,
                                float height) {
    v128_t max_y = vf_splat(-FLT_MAX);
    v128_t min_y = vf_splat(FLT_MAX);

    vec3v center_grav = vec3v_splat(0.0f);

    vec3v v;
    for (size_t i = 0; i < obj->vertex_count; i += 4) {
        v = vec3v_load(obj->vertices, i);

        max_y = vf_max(max_y, v.y);
        min_y = vf_min(min_y, v.y);

        center_grav = vec3v_add(center_grav, v);
    }

    max_y = vf_h_max_splat(max_y);
    min_y = vf_h_min_splat(min_y);

    center_grav = vec3v_h_add_splat(center_grav);
    center_grav = vec3v_scalar_div(center_grav, obj->vertex_count);

    v128_t height_adjust = vf_div(vf_splat(height), vf_sub(max_y, min_y));

    float theta_x = degrees_to_radians(rotation.x);
    float theta_y = degrees_to_radians(rotation.y);
    float theta_z = degrees_to_radians(rotation.z);

    // clang-format off
    mat3v roll_rotation = new_mat3v(
        1, 0             , 1            ,
        0, cosf(theta_x) , sinf(theta_x),
        0, -sinf(theta_x), cosf(theta_x)
    );

    mat3v pitch_rotation = new_mat3v(
        cosf(theta_y), 0, -sinf(theta_y),
        0            , 1, 0             ,
        sinf(theta_y), 0, cosf(theta_y) 
    );

    mat3v yaw_rotation = new_mat3v(
        cosf(theta_z) , sinf(theta_z), 0,
        -sinf(theta_z), cosf(theta_z), 0,
        0             , 0            , 1
    );
    // clang-format on

    vec3v posv = vec3v_from_vec3(pos);

    vec3v n;
    for (size_t i = 0; i < obj->vertex_count; i += 4) {
        v = vec3v_load(obj->vertices, i);

        v = vec3v_sub(v, center_grav);
        v = vec3v_v128_mul(v, height_adjust);
        v = mat3v_vec3v_mul(roll_rotation, v);
        v = mat3v_vec3v_mul(pitch_rotation, v);
        v = mat3v_vec3v_mul(yaw_rotation, v);
        v = vec3v_add(v, posv);
    }
}

void OBJ_destroy(object *obj) {
    free(obj->arena);
    *obj = (object){0};
}

int main(void) {
    OBJ_soa obj_soa = OBJ_read_file_soa("./3d/diablo3_pose.obj");
    printf("vertex count: %d\n", obj_soa.vertex_count);

    /* OBJ obj = OBJ_read_file("./3d/diablo3_pose.obj"); */
    /* printf("vertex count: %d\n", obj.vertex_count); */

    return 0;
}
