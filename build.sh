$WASI_SDK_PATH/bin/clang \
    --target=wasm32-unkown-wasi \
    --sysroot=$WASI_SDK_PATH/share/wasi-sysroot \
    -nostartfiles \
    -O3 \
    -flto \
    -Wl,--no-entry \
    -Wl,--export=malloc \
    -Wl,--export=rasterize_cars \
    -Wl,--export=cam \
    -Wl,--export=camera_initialize \
    -Wl,--export=look_from \
    -Wl,--export=look_at \
    -Wl,--export=vup \
    -Wl,--export=STL_position_and_normalize \
    -Wl,--export=new_vec3 \
    -Wl,--lto-O3 \
    -Wl,--initial-memory=20971520 \
    -o add.wasm \
    camera.c add.c -lm

wasm2wat add.wasm > add.wat

