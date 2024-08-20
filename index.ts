import { loadSTL, verticesInformation } from "./stl.js";

const imageWidth = 400;
const imageHeight = 300;
const vFOV = 30;

(async () => {
    const { instance } = await WebAssembly.instantiateStreaming(
        fetch("./add.wasm"),
    );

    const wasmExports = instance.exports;

    const memory = wasmExports.memory as WebAssembly.Memory;

    // retrieve stack pointers (global variables)
    const camPtr = wasmExports.cam.valueOf();
    const lookFromPtr = wasmExports.look_from.valueOf() as number;
    const lookAtPtr = wasmExports.look_at.valueOf() as number;
    const vupPtr = wasmExports.vup.valueOf() as number;

    // retrieve functions
    const malloc = wasmExports.malloc as Function;
    const cameraInitialize = wasmExports.camera_initialize as Function;
    const newVec3 = wasmExports.new_vec3 as Function;
    const stlPositionAndNormalize =
        wasmExports.STL_position_and_normalize as Function;
    const rasterizeCars = wasmExports.rasterize_cars as Function;

    const imageBufferPtr = malloc(
        imageWidth * imageHeight * 4 * Uint8Array.BYTES_PER_ELEMENT,
    );
    const imageBuffer = new Uint8Array(
        memory.buffer,
        imageBufferPtr,
        imageWidth * imageHeight * 4,
    );
    imageBuffer.fill(0.0);

    const zBufferPtr = malloc(
        imageWidth * imageHeight * Float32Array.BYTES_PER_ELEMENT,
    );
    const zBuffer = new Float32Array(
        memory.buffer,
        zBufferPtr,
        imageWidth * imageHeight,
    );
    zBuffer.fill(-Infinity);

    const lookFrom = new Float32Array(memory.buffer, lookFromPtr, 3);
    lookFrom.set([0.0, 0.0, 0.0]);

    const lookAt = new Float32Array(memory.buffer, lookAtPtr, 3);
    lookAt.set([0.0, 0.0, -1.0]);

    const vup = new Float32Array(memory.buffer, vupPtr, 3);
    vup.set([0.0, 1.0, 0.0]);

    cameraInitialize(camPtr, imageWidth, imageHeight, vFOV);

    const bunnyInformation = (await loadSTL(
        "./bunny.stl",
        malloc,
        memory.buffer,
    )) as verticesInformation;

    const bunnyVerticesPtr = bunnyInformation.verticesPtr;
    // const bunnyVertices = bunnyInformation.vertices;
    const bunnyFaceCount = bunnyInformation.faceCount;

    let positionPtr = newVec3(0.0, 0.0, -4.0);
    let rotationPtr = newVec3(-90.0, 90.0, 0.0);

    stlPositionAndNormalize(
        bunnyVerticesPtr,
        bunnyFaceCount,
        positionPtr,
        rotationPtr,
        1.0,
    );

    rasterizeCars(imageBufferPtr, zBufferPtr, bunnyVerticesPtr, bunnyFaceCount);

    const imageData = new ImageData(
        new Uint8ClampedArray(imageBuffer),
        imageHeight,
        imageWidth,
    );

    const canvas = document.getElementById("myCanvas") as HTMLCanvasElement;
    canvas.width = imageWidth;
    canvas.height = imageHeight;
    const ctx = canvas.getContext("2d");
    if (ctx === null) {
        console.error("bruh");
    } else {
        ctx.putImageData(imageData, 0, 0);
    }
})();
