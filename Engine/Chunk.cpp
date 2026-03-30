#include "Chunk.h"
#include <cmath>

Chunk::Chunk(int cx, int cz) : chunkX(cx), chunkZ(cz), mesh(nullptr) {
    for (int x = 0; x < SIZE; x++)
    for (int y = 0; y < SIZE; y++)
    for (int z = 0; z < SIZE; z++)
        blocks[x][y][z] = (y <= 4) ? 1 : 0;
}

// Helper: is this position solid?
bool Chunk::IsSolid(int x, int y, int z) {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || z < 0 || z >= SIZE)
        return false; // treat out-of-bounds as air (neighbor chunk)
    return blocks[x][y][z] == 1;
}

void Chunk::EmitFace(std::vector<float>& verts,
                     std::vector<unsigned int>& indices,
                     int x, int y, int z,
                     float nx, float ny, float nz,
                     // 4 corners passed as flat array [x0,y0,z0, x1,y1,z1, ...]
                     const float corners[12])
{
    unsigned int offset = verts.size() / 8;

    float fx = (float)(chunkX * SIZE + x);
    float fy = (float)y;
    float fz = (float)(chunkZ * SIZE + z);

    const float uvs[8] = { 0,0, 1,0, 1,1, 0,1 };

    for (int i = 0; i < 4; i++) {
        verts.push_back(fx + corners[i*3+0]);
        verts.push_back(fy + corners[i*3+1]);
        verts.push_back(fz + corners[i*3+2]);
        verts.push_back(uvs[i*2+0]);
        verts.push_back(uvs[i*2+1]);
        verts.push_back(nx);
        verts.push_back(ny);
        verts.push_back(nz);
    }

    indices.insert(indices.end(), {
        offset+0, offset+1, offset+2,
        offset+2, offset+3, offset+0,
    });
}


void Chunk::BuildMesh() {
    std::vector<float>        verts;
    std::vector<unsigned int> indices;

    // Face definitions: normal + 4 corners (local offsets)
    struct FaceDef {
        int   nx, ny, nz;       // neighbor offset to check
        float fnx, fny, fnz;    // normal
        float corners[12];      // 4 verts * 3 floats
    };

    const FaceDef faces[6] = {
        // Top (+Y)
        { 0, 1, 0,   0,1,0,  { 0,1,0, 1,1,0, 1,1,1, 0,1,1 } },
        // Bottom (-Y)
        { 0,-1, 0,   0,-1,0, { 0,0,1, 1,0,1, 1,0,0, 0,0,0 } },
        // Right (+X)
        { 1, 0, 0,   1,0,0,  { 1,0,0, 1,0,1, 1,1,1, 1,1,0 } },
        // Left (-X)
        {-1, 0, 0,  -1,0,0,  { 0,0,1, 0,0,0, 0,1,0, 0,1,1 } },
        // Front (+Z)
        { 0, 0, 1,   0,0,1,  { 0,0,1, 1,0,1, 1,1,1, 0,1,1 } },  // ← fix winding
        // Back (-Z)
        { 0, 0,-1,   0,0,-1, { 1,0,0, 0,0,0, 0,1,0, 1,1,0 } },
    };

    for (int x = 0; x < SIZE; x++)
        for (int y = 0; y < SIZE; y++)
            for (int z = 0; z < SIZE; z++) {
                if (blocks[x][y][z] == 0) continue; // skip air

                for (auto& f : faces) {
                     // only emit face if neighbor is air
                     if (!IsSolid(x + f.nx, y + f.ny, z + f.nz))
                          EmitFace(verts, indices, x, y, z,
                                     f.fnx, f.fny, f.fnz, f.corners);
        }
    }

    delete mesh;
    mesh = new Mesh;
    mesh->Initc(verts, indices);
}