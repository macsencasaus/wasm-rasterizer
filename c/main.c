#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "obj.h"
#include "texture.h"
#include "vec3.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void rasterize(uint8_t *image_buffer, uint32_t image_width,
               uint32_t image_height, uint32_t image_channels) {
    OBJ head_obj = OBJ_read_file("diablo3_pose.obj");
    OBJ_position_and_scale(&head_obj, (point3){0, 0, -3}, (vec3){0, 45, 0},
                           1.0);

    camera cam = {0};
    /* cam.image_width = image_width; */
    /* cam._image_height = image_height; */
    /**/
    /* cam.vfov = 80; */
    cam.look_from = (point3){0, 0, 0};
    cam.look_at = (point3){0, 0, -1.0};
    cam.vup = (vec3){0, 1, 0};

    camera_initialize(&cam, image_width, image_height, image_channels, 20);

    float *z_buffer =
        (float *)malloc(image_width * image_height * sizeof(float));
    for (int i = 0; i < image_height * image_width; ++i) {
        z_buffer[i] = -INFINITY;
    }

    texture_image ti = read_texture_image_png("diablo3_pose_diffuse.png");

    rasterize_obj(image_buffer, &cam, &head_obj, &ti, z_buffer);

    free(z_buffer);
    OBJ_destroy(&head_obj);
    destroy_texture(&ti);
}

int main() {
    const uint32_t image_width = 400;
    const uint32_t image_height = 400;
    const uint32_t image_channels = 4;

    uint8_t *image_buffer = (uint8_t *)malloc(image_width * image_height * 4);

    rasterize(image_buffer, image_width, image_height, image_channels);

    if (stbi_write_png("img.png", image_width, image_height, image_channels,
                       image_buffer, image_width * image_channels)) {
        printf("Image saved\n");
    } else {
        printf("Failed to save image\n");
    }

    free(image_buffer);

    return 0;
}
