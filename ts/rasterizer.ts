import * as utils from "./utils.js";
import { Allocator, Vec3, Vec3Struct } from "./utils.js";

import { loadOBJ } from "./obj.js";
import { loadTexture } from "./texture.js";

type ObjModifier = (ObjPtr: number, x: number, y: number, z: number) => void;

export class WasmRasterizer {
    private wasmExports!: WebAssembly.Exports;
    private memory!: ArrayBuffer;
    private view!: DataView;

    private cameraPtr!: number;
    private lookFromPtr!: number;
    private lookAtPtr!: number;
    private vupPtr!: number;

    private malloc!: Allocator;

    private cameraInitialize!: (
        camPtr: number,
        imageWidth: number,
        imageHeight: number,
        imageChannels: number,
        vfov: number,
    ) => void;

    private objPSR!: (objPtr: number) => void;

    private objSetPosition!: ObjModifier;

    private objSetRotation!: ObjModifier;

    private objSetHeight!: (objPtr: number, height: number) => void;

    private objShiftBy!: ObjModifier;

    private objRotateBy!: ObjModifier;

    private rasterizeObj!: (
        imageBufferPtr: number,
        cameraPtr: number,
        objPtr: number,
        texturePtr: number,
        zBufferPtr: number,
    ) => void;

    private testFunc!: (objPtr: number) => number;

    private imageBufferPtr!: number;
    private imageBuffer!: Uint8ClampedArray;

    private zBufferPtr!: number;
    private zBuffer!: Float32Array;

    private lookFrom!: Float32Array;
    private lookAt!: Float32Array;
    private vup!: Float32Array;

    private imageWidth!: number;
    private imageHeight!: number;
    private imageChannels!: number;

    private entities!: Entity[];

    constructor() {}

    async initializeWasmImport(wasmFilePath: string): Promise<void> {
        const { instance } = await WebAssembly.instantiateStreaming(
            fetch(wasmFilePath),
        );

        this.wasmExports = instance.exports;

        let wasmMemory = this.wasmExports.memory as WebAssembly.Memory;
        this.memory = wasmMemory.buffer;
        this.view = new DataView(this.memory);

        this.cameraPtr = this.wasmExports.cam.valueOf() as number;
        this.lookFromPtr = this.wasmExports.look_from.valueOf() as number;
        this.lookAtPtr = this.wasmExports.look_at.valueOf() as number;
        this.vupPtr = this.wasmExports.vup.valueOf() as number;

        this.malloc = this.wasmExports.bump_malloc as Allocator;

        this.cameraInitialize = this.wasmExports.camera_initialize as (
            camPtr: number,
            imageWidth: number,
            imageHeight: number,
            imageChannels: number,
            vfov: number,
        ) => void;

        this.objPSR = this.wasmExports.obj_psr as (objPtr: number) => void;

        this.objSetPosition = this.wasmExports.obj_set_position as ObjModifier;
        this.objSetRotation = this.wasmExports.obj_set_rotation as ObjModifier;
        this.objSetHeight = this.wasmExports.obj_set_height as (
            objPtr: number,
            height: number,
        ) => void;

        this.objShiftBy = this.wasmExports.obj_shify_by as ObjModifier;
        this.objRotateBy = this.wasmExports.obj_rotate_by as ObjModifier;

        this.rasterizeObj = this.wasmExports.rasterize_obj as (
            imageBufferPtr: number,
            cameraPtr: number,
            objPtr: number,
            texturePtr: number,
            zBufferPtr: number,
        ) => void;

        this.testFunc = this.wasmExports.test_func as (
            objPtr: number,
        ) => number;

        this.lookFrom = new Float32Array(this.memory, this.lookFromPtr, 3);

        this.lookAt = new Float32Array(this.memory, this.lookAtPtr, 3);

        this.vup = new Float32Array(this.memory, this.vupPtr, 3);
    }

    initializeBuffers(
        imageWidth: number,
        imageHeight: number,
        imageChannels: number,
    ): void {
        this.imageWidth = imageWidth;
        this.imageHeight = imageHeight;
        this.imageChannels = imageChannels;

        this.imageBufferPtr = this.malloc(
            imageWidth * imageHeight * imageChannels * utils.UINT8_SIZE,
        );

        this.imageBuffer = new Uint8ClampedArray(
            this.memory,
            this.imageBufferPtr,
            imageWidth * imageHeight * imageChannels,
        );
        this.imageBuffer.fill(0);

        this.zBufferPtr = this.malloc(
            imageWidth * imageHeight * utils.FLOAT32_SIZE,
        );
        this.zBuffer = new Float32Array(
            this.memory,
            this.zBufferPtr,
            imageWidth * imageHeight,
        );
        this.zBuffer.fill(-Infinity);

        this.entities = [];
    }

    setCamera(vFov: number, lookFrom: Vec3, lookAt: Vec3, vup: Vec3): void {
        this.lookFrom.set(lookFrom);
        this.lookAt.set(lookAt);
        this.vup.set(vup);

        this.cameraInitialize(
            this.cameraPtr,
            this.imageWidth,
            this.imageHeight,
            this.imageChannels,
            vFov,
        );
    }

    async pushEntity(
        objUrl: string,
        textureUrl: string,
        initialPosition: Vec3,
        initialRotation: Vec3,
        initialHeight: number,
    ): Promise<void> {
        const objPtrPromise = loadOBJ(
            objUrl,
            this.malloc,
            this.memory,
            this.view,
        );
        const texturePtrPromise = loadTexture(
            textureUrl,
            this.malloc,
            this.memory,
            this.view,
        );

        const [objPtr, texturePtr] = await Promise.all([
            objPtrPromise,
            texturePtrPromise,
        ]);

        this.objSetPosition(objPtr, ...initialPosition);
        this.objSetRotation(objPtr, ...initialRotation);
        this.objSetHeight(objPtr, initialHeight);

        this.objPSR(objPtr);

        this.entities.push({
            objPtr: objPtr,
            texturePtr: texturePtr,
        });
    }

    shiftEntity(idx: number, shift: Vec3): void {
        const entity = this.entities[idx];
        this.objShiftBy(entity.objPtr, ...shift);
    }

    rotateEntity(idx: number, rotation: Vec3): void {
        const entity = this.entities[idx];
        this.objRotateBy(entity.objPtr, ...rotation);
    }

    render(): void {
        this.imageBuffer.fill(0);
        this.zBuffer.fill(-Infinity);
        for (const entity of this.entities) {
            this.rasterizeObj(
                this.imageBufferPtr,
                this.cameraPtr,
                entity.objPtr,
                entity.texturePtr,
                this.zBufferPtr,
            );
        }
    }

    writeToImageData(imageData: ImageData) {
        imageData.data.set(this.imageBuffer);
    }
}

type Entity = {
    objPtr: number;
    texturePtr: number;
};
