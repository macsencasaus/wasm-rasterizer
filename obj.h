#ifndef OBJ_H
#define OBJ_H

#include "vec3.h"

typedef struct {
    uint32_t vertex_idxs[3];
    uint32_t vertex_texture_idxs[3];
    uint32_t vertex_normal_idxs[3];
} OBJ_face_element;

typedef struct {
    uint32_t vertex_count;
    uint32_t vertex_texture_count;
    uint32_t vertex_normal_count;
    uint32_t face_count;

    void *arena;

    point3 *vertices;
    vec2 *vertex_textures;
    point3 *vertex_normals;
    OBJ_face_element *faces;
} OBJ;

OBJ read_OBJ_file(const char *obj_filepath);

#endif  // OBJ_H
