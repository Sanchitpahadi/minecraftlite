#include "World.h"

// World.cpp
void World::Update(float playerX, float playerZ) {
    // which chunk is the player in?
    int pcx = (int)std::floor(playerX / Chunk::SIZE);
    int pcz = (int)std::floor(playerZ / Chunk::SIZE);

    // load chunks within render distance
    for (int dx = -renderDistance; dx <= renderDistance; dx++)
        for (int dz = -renderDistance; dz <= renderDistance; dz++) {
              int cx = pcx + dx;
              int cz = pcz + dz;
              std::string key = Key(cx, cz);

              if (chunks.find(key) == chunks.end()) {
                  Chunk* c = new Chunk(cx, cz);
                  c->BuildMesh();
                  chunks[key] = c;
              }
    }

    // unload chunks outside render distance + margin
    int unloadDist = renderDistance + 2;
    for (auto it = chunks.begin(); it != chunks.end(); ) {
        // parse key back to cx,cz
        auto comma = it->first.find(',');
        int cx = std::stoi(it->first.substr(0, comma));
        int cz = std::stoi(it->first.substr(comma+1));

        if (std::abs(cx - pcx) > unloadDist || std::abs(cz - pcz) > unloadDist) {
            delete it->second;
            it = chunks.erase(it);
        } else {
            ++it;
        }
    }
}

void World::Render() {
    for (auto& [key, chunk] : chunks)
        chunk->mesh->Draw(); // assumes Chunk::Render() draws its mesh
}