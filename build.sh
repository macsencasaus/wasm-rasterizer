#!/bin/bash
mkdir -p wasm/
$WASI_SDK_PATH/bin/clang \
    --target=wasm32-wasi \
    --sysroot=$WASI_SDK_PATH/share/wasi-sysroot \
    -msimd128 \
    -nostartfiles \
    -O3 \
    -flto \
    -Wl,--no-entry \
    -Wl,--export=bump_malloc \
    -Wl,--export=rasterize_obj \
    -Wl,--export=cam \
    -Wl,--export=camera_initialize \
    -Wl,--export=look_from \
    -Wl,--export=look_at \
    -Wl,--export=vup \
    -Wl,--export=obj_psr \
    -Wl,--export=obj_shift_by \
    -Wl,--export=obj_rotate_by \
    -Wl,--export=obj_set_position \
    -Wl,--export=obj_set_rotation \
    -Wl,--export=obj_set_height \
    -Wl,--export=test_func \
    -Wl,--lto-O3 \
    -Wl,--initial-memory=20971520 \
    -o wasm/rasterizer.wasm \
    c/camera.c c/rasterizer.c -lm

wasm2wat wasm/rasterizer.wasm > wasm/rasterizer.wat

