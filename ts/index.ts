import { WasmRasterizer } from "./rasterizer.js";
import { Vec3 } from "./utils.js";

const imageWidth = 800;
const imageHeight = 800;
const imageChannels = 4;
const vFov = 20.0;

const wasmImportUrl = "./wasm/rasterizer.wasm";
const objUrl = "./3d/head.obj";
const textureUrl = "./img/head_texture_smol.png";

const canvasId = "myCanvas";

(async () => {
    const canvas = document.getElementById(canvasId) as HTMLCanvasElement;
    canvas.width = imageWidth;
    canvas.height = imageHeight;

    const ctx = canvas.getContext("2d");
    if (ctx == null) {
        throw new Error("2d canvas not supported");
    }

    const imageData = new ImageData(imageWidth, imageHeight);

    const rasterizer = new WasmRasterizer();

    const lookFrom: Vec3 = [0.0, 0.0, 0.0];
    const lookAt: Vec3 = [0.0, 0.0, -1.0];
    const vup: Vec3 = [0.0, 1.0, 0.0];

    await rasterizer.initializeWasmImport(wasmImportUrl);

    rasterizer.initializeBuffers(imageWidth, imageHeight, imageChannels);

    rasterizer.setCamera(vFov, lookFrom, lookAt, vup);

    const initPosition = [0.0, 0.0, -3.0] as Vec3;
    const initRotation = [0.0, 0.0, 0.0] as Vec3;
    const initHeight = 1.0;

    await rasterizer.pushEntity(
        objUrl,
        textureUrl,
        initPosition,
        initRotation,
        initHeight,
    );

    const rotation = [0.0, 5.0, 0.0] as Vec3;

    const frame = (timestamp: number) => {
        ctx.clearRect(0, 0, imageWidth, imageHeight);
        rasterizer.rotateEntity(0, rotation);
        rasterizer.render();
        rasterizer.writeToImageData(imageData);
        ctx.putImageData(imageData, 0, 0);
        requestAnimationFrame(frame);
    };
    requestAnimationFrame(frame);
})();
