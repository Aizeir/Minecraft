#pragma once
#include "util.cpp"
#include <FastNoiseLite.h>
#include <vector>
#include <unordered_map>
#include "block.cpp"

struct pair_hash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

class Chunk;

class World {public:
    FastNoiseLite noise;
    unordered_map<pair<int, int>, Chunk, pair_hash> chunks;

    World(vec3 spawn_pos);

    Chunk* load_chunk(int cx, int cz);

    Chunk* get_chunk(int cx, int cz);
    Chunk* get_chunk_of_block(int x, int y, int z);

    bool is_solid(int x, int y, int z);
    int get_block(int x, int y, int z);
    void set_block(int x, int y, int z, int id);

    // Définitions en ivec
    Chunk* get_chunk(ivec2 c) {return get_chunk(c.x,c.y);}
    Chunk* get_chunk_of_block(ivec3 pos) {return get_chunk_of_block(pos.x,pos.y,pos.z);}

    bool is_solid(ivec3 pos) {return is_solid(pos.x,pos.y,pos.z);}
    int get_block(ivec3 pos) {return get_block(pos.x,pos.y,pos.z);}
    void set_block(ivec3 pos, int id) {return set_block(pos.x,pos.y,pos.z, id);}
};


class Chunk {public:
    World* world;
    int chunk_x, chunk_z;
    int map[CHUNK_W][CHUNK_D][CHUNK_H];
    //vector<array<array<int, CHUNK_W>, CHUNK_D>> map;

    Chunk() = default;
    Chunk(World* world, int cx, int cz): world(world), chunk_x(cx), chunk_z(cz) {
        // Génération procédurale
        for (int x=0;x<CHUNK_W;x++) {
        for (int z=0;z<CHUNK_D;z++) {
        float value = (world->noise.GetNoise((float)(cx*CHUNK_W + x), (float)(cz*CHUNK_D + z)) + 1.0f)/2.0f;
        int ground = (int)(value * CHUNK_H); // entre 0 et CHUNK_H-1
        
        // Choix des blocs
        for (int y=0;y<CHUNK_H;y++) {
            if      (y >= ground) map[x][z][y] = AIR;
            else if (y >= ground-1) map[x][z][y] = GRASS;
            else if (y >= ground-3) map[x][z][y] = DIRT;
            else if (y >=  0) map[x][z][y] = STONE;
        }}}

        create_mesh();
    }

    vector<Vertex> vertices;
    void create_mesh() {
        vertices.clear();
        // Parcourir tous les blocs
        for (int x=0;x<CHUNK_W;x++) {
        for (int y=0;y<CHUNK_H;y++) {
        for (int z=0;z<CHUNK_D;z++) {

        // Block ID
        ivec3 block(chunk_x*CHUNK_W+x, y, chunk_z*CHUNK_D+z);
        int id = get_local_block(x,y,z);
        if (id == AIR) continue;

        // Matrices
        mat4 model = glm::translate(mat4(1.0f), (vec3)block);
        mat3 normal_mat = glm::mat3(glm::transpose(glm::inverse(model)));

        // Blocs voisins
        for (int face_idx=0; face_idx<6; face_idx++) {
            // Face visible
            if (!is_solid_local(x + IDIRS[face_idx].x, y + IDIRS[face_idx].y, z + IDIRS[face_idx].z)) {

                // Lighting
                vec3 normal = cube_faces[face_idx][0].normal;
                normal = normalize(vec3(normal_mat * vec4(normal, 1.0f)));

                float value = max(0.f, (dot(light_direction, -normal)+1.0f)/2.0f);
                float lighting = pow(value, 1.0f / 2.2f) * 2; // gamma

                // Copier tous les vertices
                for (int vert_idx=0;vert_idx<6;vert_idx++) {
                    Vertex vertex = cube_faces[face_idx][vert_idx];
                    vertex.pos = vec3(model * vec4(vertex.pos, 1.0f));
                    vertex.normal = normalize(vec3(normal_mat * vec4(vertex.normal, 1.0f)));
                    vertex.id = BlockData[id].faces[face_idx];
                    vertex.lighting = lighting;
                    vertices.push_back(vertex);
                }
            }
        }

        }}}
    }

    int get_local_block(int x, int y, int z) {
        return map[x][z][y];
    }

    void set_local_block(int x, int y, int z, int id) {
        map[x][z][y] = id;
    }

    bool in_chunk(int x, int y, int z) {
        x -= chunk_x * CHUNK_W; z -= chunk_z * CHUNK_D;
        return in_chunk_local(x,y,z);
    }

    void _check_in_chunk(int x, int y, int z, const char* func) {
        if (!in_chunk(x,y,z)) {
            cerr << "CHUNK "<< func <<": block "<<x<<", "<<y<<", "<<z<<" is not in chunk "<<chunk_x<<" "<<chunk_z<<" \n";
            exit(1); // idéalement, replacer exit(1) par une fonction de sauvegarde/quit
        }
    }

    int get_block(int x, int y, int z) {
        _check_in_chunk(x,y,z,__func__);
        x -= chunk_x * CHUNK_W; z -= chunk_z * CHUNK_D;
        return get_local_block(x,y,z);
    }

    void set_block(int x, int y, int z, int id) {
        _check_in_chunk(x,y,z,__func__);
        x -= chunk_x * CHUNK_W; z -= chunk_z * CHUNK_D;
        return set_local_block(x,y,z, id);
    }

    bool is_solid_local(int x, int y, int z) {
        if (!in_chunk_local(x,y,z)) return false;
        return get_local_block(x,y,z) != AIR;
    }

    bool is_solid(int x, int y, int z) {
        if (y < 0 || y >= CHUNK_H) {return false;}
        _check_in_chunk(x,y,z,__func__);
        x -= chunk_x * CHUNK_W; z -= chunk_z * CHUNK_D;
        return is_solid_local(x,y,z);
    }
};

// WORLD

World::World(vec3 spawn_pos) {
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    // Chunks
    ivec2 c = get_chunk_pos(spawn_pos);
    for (int cx = c.x-LOAD; cx <= c.x + LOAD; cx++) {
    for (int cz = c.y-LOAD; cz <= c.y + LOAD; cz++) {
        chunks[{cx, cz}] = Chunk(this, cx, cz);
    }}
}

Chunk* World::load_chunk(int cx, int cz) {
    Chunk chunk = Chunk(this, cx, cz);
    chunks[{cx, cz}] = chunk;
    return &chunks[{cx, cz}];
}

Chunk* World::get_chunk(int cx, int cz) {
    auto iterator = chunks.find({cx, cz});
    return (iterator != chunks.end()) ? &iterator->second : nullptr;
}

Chunk* World::get_chunk_of_block(int x, int y, int z) {
    return get_chunk(get_chunk_pos(x,y,z));
}

bool World::is_solid(int x, int y, int z) {
    Chunk* chunk = get_chunk_of_block(x,y,z); if (chunk==nullptr) return false;
    return chunk->is_solid(x,y,z);
}

int World::get_block(int x, int y, int z) {
    Chunk* chunk = get_chunk_of_block(x,y,z); if (chunk==nullptr) return AIR;
    return chunk->get_block(x,y,z);
}

void World::set_block(int x, int y, int z, int id) {
    Chunk* chunk = get_chunk_of_block(x,y,z); if (chunk == nullptr) return;
    return chunk->set_block(x,y,z, id);
}

// Autre
ostream& operator<<(ostream& os, const Chunk& chunk) {
    return os << "Chunk (" << chunk.chunk_x << " " << chunk.chunk_z << ")";
}