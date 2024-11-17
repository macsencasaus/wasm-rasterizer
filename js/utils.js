export const UINT8_SIZE = 1;
export const UINT16_SIZE = 2;
export const UINT32_SIZE = 4;
export const FLOAT32_SIZE = 4;
export class Uint8 {
    static size = UINT8_SIZE;
    view;
    ptr;
    constructor(view, malloc) {
        this.ptr = malloc(Uint8.size);
        this.view = view;
    }
    read() {
        return this.view.getUint8(this.ptr);
    }
    write(value) {
        this.view.setUint8(this.ptr, value | 0);
    }
}
export class Uint32 {
    static size = UINT32_SIZE;
    view;
    ptr;
    constructor(view, malloc) {
        this.ptr = malloc(Uint32.size);
        this.view = view;
    }
    read() {
        return this.view.getUint32(this.ptr, true);
    }
    write(value) {
        this.view.setUint32(this.ptr, value | 0, true);
    }
}
export class Float32 {
    static size = FLOAT32_SIZE;
    view;
    ptr;
    constructor(view, malloc) {
        this.ptr = malloc(Float32.size);
        this.view = view;
    }
    read() {
        return this.view.getFloat32(this.ptr, true);
    }
    write(value) {
        this.view.setFloat32(this.ptr, value, true);
    }
}
export class Vec3Struct {
    static size = 3 * FLOAT32_SIZE;
    x;
    y;
    z;
    ptr;
    constructor(view, malloc, v) {
        this.x = new Float32(view, malloc);
        this.y = new Float32(view, malloc);
        this.z = new Float32(view, malloc);
        if (v) {
            this.set(v);
        }
        this.ptr = this.x.ptr;
    }
    set(v) {
        this.x.write(v[0]);
        this.y.write(v[1]);
        this.z.write(v[2]);
    }
}
