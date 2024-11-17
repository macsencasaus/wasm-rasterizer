import * as utils from "./utils.js";
import { loadOBJ } from "./obj.js";
import { loadTexture } from "./texture.js";
export class WasmRasterizer {
    wasmExports;
    memory;
    view;
    cameraPtr;
    lookFromPtr;
    lookAtPtr;
    vupPtr;
    malloc;
    cameraInitialize;
    objPSR;
    objSetPosition;
    objSetRotation;
    objSetHeight;
    objShiftBy;
    objRotateBy;
    rasterizeObj;
    testFunc;
    imageBufferPtr;
    imageBuffer;
    zBufferPtr;
    zBuffer;
    lookFrom;
    lookAt;
    vup;
    imageWidth;
    imageHeight;
    imageChannels;
    entities;
    constructor() { }
    async initializeWasmImport(wasmFilePath) {
        const { instance } = await WebAssembly.instantiateStreaming(fetch(wasmFilePath));
        this.wasmExports = instance.exports;
        let wasmMemory = this.wasmExports.memory;
        this.memory = wasmMemory.buffer;
        this.view = new DataView(this.memory);
        this.cameraPtr = this.wasmExports.cam.valueOf();
        this.lookFromPtr = this.wasmExports.look_from.valueOf();
        this.lookAtPtr = this.wasmExports.look_at.valueOf();
        this.vupPtr = this.wasmExports.vup.valueOf();
        this.malloc = this.wasmExports.bump_malloc;
        this.cameraInitialize = this.wasmExports.camera_initialize;
        this.objPSR = this.wasmExports.obj_psr;
        this.objSetPosition = this.wasmExports.obj_set_position;
        this.objSetRotation = this.wasmExports.obj_set_rotation;
        this.objSetHeight = this.wasmExports.obj_set_height;
        this.objShiftBy = this.wasmExports.obj_shify_by;
        this.objRotateBy = this.wasmExports.obj_rotate_by;
        this.rasterizeObj = this.wasmExports.rasterize_obj;
        this.testFunc = this.wasmExports.test_func;
        this.lookFrom = new Float32Array(this.memory, this.lookFromPtr, 3);
        this.lookAt = new Float32Array(this.memory, this.lookAtPtr, 3);
        this.vup = new Float32Array(this.memory, this.vupPtr, 3);
    }
    initializeBuffers(imageWidth, imageHeight, imageChannels) {
        this.imageWidth = imageWidth;
        this.imageHeight = imageHeight;
        this.imageChannels = imageChannels;
        this.imageBufferPtr = this.malloc(imageWidth * imageHeight * imageChannels * utils.UINT8_SIZE);
        this.imageBuffer = new Uint8ClampedArray(this.memory, this.imageBufferPtr, imageWidth * imageHeight * imageChannels);
        this.imageBuffer.fill(0);
        this.zBufferPtr = this.malloc(imageWidth * imageHeight * utils.FLOAT32_SIZE);
        this.zBuffer = new Float32Array(this.memory, this.zBufferPtr, imageWidth * imageHeight);
        this.zBuffer.fill(-Infinity);
        this.entities = [];
    }
    setCamera(vFov, lookFrom, lookAt, vup) {
        this.lookFrom.set(lookFrom);
        this.lookAt.set(lookAt);
        this.vup.set(vup);
        this.cameraInitialize(this.cameraPtr, this.imageWidth, this.imageHeight, this.imageChannels, vFov);
    }
    async pushEntity(objUrl, textureUrl, initialPosition, initialRotation, initialHeight) {
        const objPtrPromise = loadOBJ(objUrl, this.malloc, this.memory, this.view);
        const texturePtrPromise = loadTexture(textureUrl, this.malloc, this.memory, this.view);
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
    shiftEntity(idx, shift) {
        const entity = this.entities[idx];
        this.objShiftBy(entity.objPtr, ...shift);
    }
    rotateEntity(idx, rotation) {
        const entity = this.entities[idx];
        this.objRotateBy(entity.objPtr, ...rotation);
    }
    render() {
        this.imageBuffer.fill(0);
        this.zBuffer.fill(-Infinity);
        for (const entity of this.entities) {
            this.rasterizeObj(this.imageBufferPtr, this.cameraPtr, entity.objPtr, entity.texturePtr, this.zBufferPtr);
        }
    }
    writeToImageData(imageData) {
        imageData.data.set(this.imageBuffer);
    }
}
