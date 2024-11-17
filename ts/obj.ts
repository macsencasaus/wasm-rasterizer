import * as utils from "./utils.js";
import { Vec3Struct, Float32, Uint32, Allocator } from "./utils.js";

export class ObjStruct {
    static readonly VERTEX_BYTE_SIZE = 3 * utils.FLOAT32_SIZE;
    static readonly VERTEX_TEXTURE_BYTE_SIZE = 2 * utils.FLOAT32_SIZE;
    static readonly VERTEX_NORMAL_BYTE_SIZE = 3 * utils.FLOAT32_SIZE;
    static readonly FACE_ELEMENT_BYTE_SIZE = 9 * utils.UINT32_SIZE;

    readonly vertexCount: Uint32;
    readonly vertexTextureCount: Uint32;
    readonly vertexNormalCount: Uint32;
    readonly faceCount: Uint32;

    readonly position: Vec3Struct;
    readonly rotation: Vec3Struct;
    readonly height: Float32;

    readonly arenaPtr: Uint32;

    readonly verticesPtr: Uint32;
    readonly vertexTexturesPtr: Uint32;
    readonly vertexNormalsPtr: Uint32;
    readonly faceElementsPtr: Uint32;

    readonly ptr: number;

    constructor(view: DataView, malloc: Allocator) {
        this.vertexCount = new Uint32(view, malloc);
        this.vertexTextureCount = new Uint32(view, malloc);
        this.vertexNormalCount = new Uint32(view, malloc);
        this.faceCount = new Uint32(view, malloc);

        this.position = new Vec3Struct(view, malloc);
        this.rotation = new Vec3Struct(view, malloc);
        this.height = new Float32(view, malloc);

        this.arenaPtr = new Uint32(view, malloc);

        this.verticesPtr = new Uint32(view, malloc);
        this.vertexTexturesPtr = new Uint32(view, malloc);
        this.vertexNormalsPtr = new Uint32(view, malloc);
        this.faceElementsPtr = new Uint32(view, malloc);

        this.ptr = this.vertexCount.ptr;
    }
}

export const loadOBJ = async (
    objURL: string,
    malloc: (n: number) => number,
    memory: ArrayBuffer,
    view: DataView,
): Promise<number> => {
    return fetch(objURL)
        .then((response) => response.text())
        .then((body) => parseOBJ(body, malloc, memory, view));
};

const parseOBJ = (
    objFile: string,
    malloc: (n: number) => number,
    memory: ArrayBuffer,
    view: DataView,
): number => {
    let obj = new ObjStruct(view, malloc);

    let vertexCount = 0,
        vertexTextureCount = 0,
        vertexNormalCount = 0,
        faceCount = 0;

    let lines = objFile.split("\n");

    for (const line of lines) {
        if (line.startsWith("v ")) {
            vertexCount++;
        } else if (line.startsWith("vt ")) {
            vertexTextureCount++;
        } else if (line.startsWith("vn ")) {
            vertexNormalCount++;
        } else if (line.startsWith("f ")) {
            faceCount++;
        }
    }

    obj.vertexCount.write(vertexCount);
    obj.vertexTextureCount.write(vertexTextureCount);
    obj.vertexNormalCount.write(vertexNormalCount);
    obj.faceCount.write(faceCount);

    const verticesArrayByteSize = vertexCount * ObjStruct.VERTEX_BYTE_SIZE;

    const vertexTexturesArrayByteSize =
        vertexTextureCount * ObjStruct.VERTEX_TEXTURE_BYTE_SIZE;

    const vertexNormalArrayByteSize =
        vertexNormalCount * ObjStruct.VERTEX_NORMAL_BYTE_SIZE;

    const facesArrayByteSize = faceCount * ObjStruct.FACE_ELEMENT_BYTE_SIZE;

    const arenaPtr = malloc(
        verticesArrayByteSize +
            vertexTexturesArrayByteSize +
            vertexNormalArrayByteSize +
            facesArrayByteSize,
    );

    obj.arenaPtr.write(arenaPtr);

    const verticesPtr = arenaPtr;
    const vertexTexturesPtr = verticesPtr + verticesArrayByteSize;
    const vertexNormalsPtr = vertexTexturesPtr + vertexTexturesArrayByteSize;
    const faceElementsPtr = vertexNormalsPtr + vertexNormalArrayByteSize;

    obj.verticesPtr.write(verticesPtr);
    obj.vertexTexturesPtr.write(vertexTexturesPtr);
    obj.vertexNormalsPtr.write(vertexNormalsPtr);
    obj.faceElementsPtr.write(faceElementsPtr);

    const vertices = new Float32Array(memory, verticesPtr, 3 * vertexCount);
    const vertexTextures = new Float32Array(
        memory,
        vertexTexturesPtr,
        2 * vertexTextureCount,
    );
    const vertexNormals = new Float32Array(
        memory,
        vertexNormalsPtr,
        3 * vertexNormalCount,
    );
    const faces = new Uint32Array(memory, faceElementsPtr, 9 * faceCount);

    let verticesIdx = 0,
        vertexTexturesIdx = 0,
        vertexNormalsIdx = 0,
        facesIdx = 0;

    for (const line of lines) {
        if (line.startsWith("v ")) {
            const [_, v1, v2, v3] = line.split(/\s+/);
            vertices[verticesIdx + 0] = parseFloat(v1);
            vertices[verticesIdx + 1] = parseFloat(v2);
            vertices[verticesIdx + 2] = parseFloat(v3);
            verticesIdx += 3;
        } else if (line.startsWith("vt ")) {
            const [_, vt1, vt2, __] = line.split(/\s+/);
            vertexTextures[vertexTexturesIdx + 0] = parseFloat(vt1);
            vertexTextures[vertexTexturesIdx + 1] = parseFloat(vt2);
            vertexTexturesIdx += 2;
        } else if (line.startsWith("vn ")) {
            const [_, vn1, vn2, vn3] = line.split(/\s+/);
            vertexNormals[vertexNormalsIdx + 0] = parseFloat(vn1);
            vertexNormals[vertexNormalsIdx + 1] = parseFloat(vn2);
            vertexNormals[vertexNormalsIdx + 2] = parseFloat(vn3);
            vertexNormalsIdx += 3;
        } else if (line.startsWith("f ")) {
            const fs = line.split(" ").slice(1);
            for (let i = 0; i < 3; i++) {
                const [f1, f2, f3] = fs[i].split("/");
                faces[facesIdx + i + 0] = parseInt(f1) - 1;
                faces[facesIdx + i + 3] = parseInt(f2) - 1;
                faces[facesIdx + i + 6] = parseInt(f3) - 1;
            }
            facesIdx += 9;
        }
    }

    return obj.ptr;
};
