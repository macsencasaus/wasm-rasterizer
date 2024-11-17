export type Allocator = (n: number) => number;
export type Vec3 = [number, number, number];

export const UINT8_SIZE = 1;
export const UINT16_SIZE = 2;
export const UINT32_SIZE = 4;
export const FLOAT32_SIZE = 4;

export class Uint8 {
    static readonly size = UINT8_SIZE;
    private readonly view: DataView;
    readonly ptr: number;

    constructor(view: DataView, malloc: Allocator) {
        this.ptr = malloc(Uint8.size);
        this.view = view;
    }

    read(): number {
        return this.view.getUint8(this.ptr);
    }

    write(value: number): void {
        this.view.setUint8(this.ptr, value | 0);
    }
}

export class Uint32 {
    static readonly size = UINT32_SIZE;
    private readonly view: DataView;
    readonly ptr: number;

    constructor(view: DataView, malloc: Allocator) {
        this.ptr = malloc(Uint32.size);
        this.view = view;
    }

    read(): number {
        return this.view.getUint32(this.ptr, true);
    }

    write(value: number): void {
        this.view.setUint32(this.ptr, value | 0, true);
    }
}

export class Float32 {
    static readonly size = FLOAT32_SIZE;
    private readonly view: DataView;
    readonly ptr: number;

    constructor(view: DataView, malloc: Allocator) {
        this.ptr = malloc(Float32.size);
        this.view = view;
    }

    read(): number {
        return this.view.getFloat32(this.ptr, true);
    }

    write(value: number): void {
        this.view.setFloat32(this.ptr, value, true);
    }
}

export class Vec3Struct {
    static readonly size = 3 * FLOAT32_SIZE;

    readonly x: Float32;
    readonly y: Float32;
    readonly z: Float32;

    readonly ptr: number;

    constructor(view: DataView, malloc: Allocator, v?: Vec3) {
        this.x = new Float32(view, malloc);
        this.y = new Float32(view, malloc);
        this.z = new Float32(view, malloc);

        if (v) {
            this.set(v);
        }

        this.ptr = this.x.ptr;
    }

    set(v: Vec3) {
        this.x.write(v[0]);
        this.y.write(v[1]);
        this.z.write(v[2]);
    }
}
