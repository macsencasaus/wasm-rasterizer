export const loadSTL = async (stl_url, malloc, memoryBuffer) => {
    return fetch(stl_url)
        .then((response) => response.arrayBuffer())
        .then((arrayBuffer) => {
        return parseBinarySTL(arrayBuffer, malloc, memoryBuffer);
    })
        .catch((error) => {
        console.error("error fetching stl", error);
        return null;
    });
};
export const parseBinarySTL = (stlByteArray, malloc, memoryBuffer) => {
    const stlDataView = new DataView(stlByteArray);
    const faceCount = stlDataView.getUint32(80, true);
    const verticesPtr = malloc(faceCount * 12 * Float32Array.BYTES_PER_ELEMENT);
    let offset = 84;
    let vertices = new Float32Array(memoryBuffer, verticesPtr, faceCount * 12); // each face has 12 floats: n, v1, v2, v3
    for (let i = 0; i < faceCount; i++) {
        for (let j = 0; j < 4; j++) {
            vertices[i * 12 + j * 3 + 0] = -stlDataView.getFloat32(offset + 0, true);
            vertices[i * 12 + j * 3 + 2] = stlDataView.getFloat32(offset + 4, true);
            vertices[i * 12 + j * 3 + 1] = stlDataView.getFloat32(offset + 8, true);
            offset += 12;
        }
        offset += 2;
    }
    return {
        faceCount: faceCount,
        verticesPtr: verticesPtr,
        vertices: vertices,
    };
};
