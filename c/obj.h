#ifndef OBJ_H
#define OBJ_H

#include "vec3.h"

typedef struct {
    uint32_t vertex_idxs[3];
    uint32_t vertex_texture_idxs[3];
    uint32_t vertex_normal_idxs[3];
} object_face;

typedef struct {
    uint32_t vertex_count;
    uint32_t vertex_texture_count;
    uint32_t vertex_normal_count;
    uint32_t face_count;

    vec3 position, rotation;
    float height;

    void *arena;

    point3 *vertices;
    vec2 *vertex_textures;
    point3 *vertex_normals;
    object_face *faces;
} object;

typedef struct {
    vec3i_soa vertex_idxs;
    vec3i_soa vertex_texture_idxs;
    vec3i_soa vertex_normal_idxs;
} face_element_soa;

typedef struct {
    uint32_t vertex_count;
    uint32_t vertex_texture_count;
    uint32_t vertex_normal_count;
    uint32_t face_count;

    void *arena;

    point3_soa vertices;
    vec2_soa vertex_textures;
    point3_soa vertex_normals;
    face_element_soa faces;
} OBJ_soa;

object OBJ_read_file(const char *obj_filepath);

OBJ_soa OBJ_read_file_soa(const char *obj_filepath);

// void OBJ_position_and_scale(OBJ *obj, vec3 *posp, vec3 *rotationp,
//                             float height);

void OBJ_destroy(object *obj);

#endif  // OBJ_H
