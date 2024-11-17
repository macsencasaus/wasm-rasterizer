import { Uint32, Allocator } from "./utils.js";

export class TextureStruct {
    readonly imageWidth: Uint32;
    readonly imageHeight: Uint32;
    readonly imageChannels: Uint32;

    readonly imagePtr: Uint32;

    readonly ptr: number;

    constructor(view: DataView, malloc: Allocator) {
        this.imageWidth = new Uint32(view, malloc);
        this.imageHeight = new Uint32(view, malloc);
        this.imageChannels = new Uint32(view, malloc);

        this.imagePtr = new Uint32(view, malloc);

        this.ptr = this.imageWidth.ptr;
    }
}

export const loadTexture = async (
    textureURL: string,
    malloc: (n: number) => number,
    memory: ArrayBuffer,
    view: DataView,
): Promise<number> => {
    return new Promise((resolve, reject) => {
        const textureImage = new Image();
        textureImage.crossOrigin = "anonymous";
        textureImage.src = textureURL;

        textureImage.onload = () => {
            const canvas = document.createElement("canvas");
            const context = canvas.getContext("2d");
            if (!context) {
                reject(new Error("Canvas context not supported"));
                return;
            }

            canvas.width = textureImage.width;
            canvas.height = textureImage.height;

            context.drawImage(textureImage, 0, 0);

            const textureData = context.getImageData(
                0,
                0,
                textureImage.width,
                textureImage.height,
            ).data;

            const textureStruct = new TextureStruct(view, malloc);

            const texturePtr = malloc(textureData.length);
            const texture = new Uint8Array(
                memory,
                texturePtr,
                textureData.length,
            );

            texture.set(textureData);

            textureStruct.imageWidth.write(textureImage.width);
            textureStruct.imageHeight.write(textureImage.height);
            textureStruct.imageChannels.write(4 | 0);

            textureStruct.imagePtr.write(texturePtr);

            resolve(textureStruct.ptr);
        };

        textureImage.onerror = (error) => {
            reject(new Error(`Failed to load image: ${error}`));
        };
    });
};
