#include "obj.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

OBJ read_OBJ_file(const char *obj_filepath) {
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
                         face_count * sizeof(OBJ_face_element));

    if (arena == NULL) {
        fclose(obj_file);
        fprintf(stderr, "error malloc obj arena\n");
        exit(1);
    }

    point3 *vertices = (point3 *)arena;

    vec2 *vertex_textures = (vec2 *)(vertices + vertex_count);

    point3 *vertex_normals = (point3 *)(vertex_textures + vertex_texture_count);

    OBJ_face_element *faces =
        (OBJ_face_element *)(vertex_normals + vertex_normal_count);

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
                if (fscanf(obj_file, "%d/%d/%d", &i, &j, &k) != 3) {
                    fclose(obj_file);
                    fprintf(stderr,
                            "error reading face, face must contain "
                            "vertex/vertex_texture/vertex_normal x 3");
                    exit(1);
                };
                faces[f_idx].vertex_idxs[h] = i;
                faces[f_idx].vertex_texture_idxs[h] = j;
                faces[f_idx].vertex_normal_idxs[h] = k;
            }
            ++f_idx;
            continue;
        }
    }

    fclose(obj_file);

    return (OBJ){
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

int main(void) {
    OBJ obj = read_OBJ_file("head.obj");
    printf(
        "vertex count: %d\nvertex normal count: %d\nvertex texture count: "
        "%d\nface count: %d\n",
        obj.vertex_count, obj.vertex_normal_count, obj.vertex_texture_count,
        obj.face_count);
    return 0;
}
