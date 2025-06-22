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
    FastNoiseLite tree_noise;
    unordered_map<pair<int, int>, Chunk, pair_hash> chunks;

    World(vec3 spawn_pos);

    Chunk* create_chunk(int cx, int cz);

    Chunk* get_chunk(int cx, int cz);
    Chunk* get_chunk_of_block(int x, int y, int z);

    bool is_solid(int x, int y, int z);
    int get_block(int x, int y, int z);
    void set_block(int x, int y, int z, int id);

    vector<array<int, 6>> structure_queue;
    void set_block_struct(int x, int y, int z, int id);

    // Définitions en ivec
    Chunk* get_chunk(ivec2 c) {return get_chunk(c.x,c.y);}
    Chunk* get_chunk_of_block(ivec3 pos) {return get_chunk_of_block(pos.x,pos.y,pos.z);}

    bool is_solid(ivec3 pos) {return is_solid(pos.x,pos.y,pos.z);}
    int get_block(ivec3 pos) {return get_block(pos.x,pos.y,pos.z);}
    void set_block(ivec3 pos, int id) {return set_block(pos.x,pos.y,pos.z, id);}
};


const int ESTIMATED_NUM_CUBES = CHUNK_W*CHUNK_H*CHUNK_D - (CHUNK_W-2)*(CHUNK_H-2)*(CHUNK_D-2);
const int ESTIMATED_NUM_VERTICES = ESTIMATED_NUM_CUBES * 2 * VERTEX_PER_FACE;

class Chunk {public:
    World* world;
    int chunk_x, chunk_z;
    int map[CHUNK_W][CHUNK_D][CHUNK_H];
    //vector<array<array<int, CHUNK_W>, CHUNK_D>> map;
    bool is_new;

    int compute_ground(int x, int z) {
        float value = (world->noise.GetNoise((float)(chunk_x*CHUNK_W + x), (float)(chunk_z*CHUNK_D + z)) + 1.0f)/2.0f;
        int ground = BED_LEVEL + (int)(value * (CHUNK_H - BED_LEVEL - 5)); // entre 0 et CHUNK_H-1
        return ground;
    }

    Chunk() = default;
    Chunk(World* world, int cx, int cz): world(world), chunk_x(cx), chunk_z(cz), is_new(true) {
        // Génération procédurale
        for (int x=0;x<CHUNK_W;x++) {
        for (int z=0;z<CHUNK_D;z++) {
        
        // Valeur du noise
        int ground = compute_ground(x,z);

        // Choix des blocs
        for (int y=0;y<CHUNK_H;y++) {
            if (y >= ground+1) {
                map[x][z][y] = AIR;
            }
            else if (y >= ground-2) {
                if (ground > SEA_LEVEL) {
                    if (y == ground) {map[x][z][y] = GRASS;}
                    else {map[x][z][y] = DIRT;}
                }
                else {
                    map[x][z][y] = SAND;
                }
            }
            else if (y >= 1) {
                int r = randint(1,100);
                if (r <= 1) {
                    map[x][z][y] = GOLD;
                }
                else if (r <= 4) {
                    map[x][z][y] = COPPER;
                }
                else if (r <= 10) {
                    map[x][z][y] = IRON;
                }
                else {
                    map[x][z][y] = STONE;
                }
            }
            else if (y == 0) {
                map[x][z][y] = BEDROCK;
            }
        }}}

        // Planter les arbres
        for (int x=0;x<CHUNK_W;x++) {
        for (int z=0;z<CHUNK_D;z++) {

        // Valeur du noise
        int ground = compute_ground(x,z);

        // Valeur du tree noise (-1/1)
        float tree_val = (world->tree_noise.GetNoise((float)(cx*CHUNK_W + x), (float)(cz*CHUNK_D + z)) + 1.0f)/2.0f;
        tree_val = (tree_val / 20.f);
        if ((ground >= SEA_LEVEL+1) && (tree_val >= (float)rand() / (float)RAND_MAX)) {
            plant_tree(x,ground+1,z);
        }
        }}

        // Structures en attente
        for (int i=0; i<world->structure_queue.size(); i++) {
            auto [x,y,z, block, cx2, cz2] = world->structure_queue[i];
            if (cx == cx2 && cz == cz2) {
                map[x][z][y] = block;
                world->structure_queue.erase(world->structure_queue.begin() + i);
            }
        }
    }

    void plant_tree(int base_x,int base_y,int base_z) {
        for (int i=0;i<tree_structure.size(); i++) {
            auto [dx, dy, dz, block] = tree_structure[i];
            int x = base_x + dx, y = base_y + dy, z = base_z + dz;
            if (in_chunk_local(x,y,z))
                map[x][z][y] = block;
            else {
                world->set_block_struct(chunk_x*CHUNK_W+x,y,chunk_z*CHUNK_D+z,block);
            }
        }
    }

    vector<Vertex> vertices;
    void create_mesh() {
        is_new = false;
        vertices.clear();
        vertices.reserve(ESTIMATED_NUM_VERTICES);
        // Parcourir tous les blocs
        for (int x=0;x<CHUNK_W;x++) {
        for (int y=0;y<CHUNK_H;y++) {
        for (int z=0;z<CHUNK_D;z++) {

        // Block ID
        ivec3 block(chunk_x*CHUNK_W+x, y, chunk_z*CHUNK_D+z);
        int id = get_local_block(x,y,z);
        if (id == AIR) continue;

        // Matrices (utile uniquement si +commpliqué que des translations)
        //mat4 model = glm::translate(mat4(1.0f), (vec3)block);
        //mat3 normal_mat = glm::mat3(glm::transpose(glm::inverse(model)));
        //normal = normalize(vec3(normal_mat * vec4(normal, 1.0f)));
        //vertex.normal = normalize(vec3(normal_mat * vec4(vertex.normal, 1.0f)));

        // Blocs voisins
        for (int face_idx=0; face_idx<6; face_idx++) {
            // Bloc voisin
            ivec3 n(x + IDIRS[face_idx].x, y + IDIRS[face_idx].y, z + IDIRS[face_idx].z);

            // La face est elle visible ?
            bool hidden;
            if (in_chunk_local(n.x, n.y, n.z))
                hidden = is_solid_local(n.x, n.y, n.z);
            else
                hidden = world->is_solid(chunk_x*CHUNK_W+n.x, n.y, chunk_z*CHUNK_D+n.z);

            if (!hidden) {
                // Lighting
                vec3 normal = cube_faces[face_idx][0].normal;
                float value = max(0.f, (dot(light_direction, -normal)+1.0f)/2.0f);
                float lighting = pow(value, 1.0f / 2.2f) * 2; // gamma

                // Copier tous les vertices
                for (int vert_idx=0;vert_idx<6;vert_idx++) {
                    Vertex vertex = cube_faces[face_idx][vert_idx];
                    vertex.pos += block;
                    vertex.id = BlockData[id].faces[face_idx];
                    vertex.lighting = lighting;
                    vertex.block = block;
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
        create_mesh();
        update_neighbor_meshes(x,y,z);
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

    void update_neighbor_meshes(int local_x, int y, int local_z) {
        int x = chunk_x*CHUNK_W + local_x, z = chunk_z*CHUNK_D + local_z;

        for (int i=0; i<size(IDIRS); i++) {
            ivec3 dir = IDIRS[i];
            Chunk* n = world->get_chunk_of_block(x + dir.x, y + dir.y, z + dir.z);
            if (n != nullptr && !(n->chunk_x==chunk_x && n->chunk_z==chunk_z)){
                n->create_mesh();
            }
        }
    }
};

ostream& operator<<(ostream& os, const Chunk& chunk) {
    return os << "Chunk (" << chunk.chunk_x << " " << chunk.chunk_z << ")";
}

// WORLD

World::World(vec3 spawn_pos) {
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    tree_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetSeed(rand());
    tree_noise.SetSeed(rand());
    // Chunks
    ivec2 c = get_chunk_pos(spawn_pos);
    for (int cx = c.x-LOAD; cx <= c.x + LOAD; cx++) {
    for (int cz = c.y-LOAD; cz <= c.y + LOAD; cz++) {
        chunks[{cx, cz}] = Chunk(this, cx, cz);
    }}
}

Chunk* World::create_chunk(int cx, int cz) {
    chunks[{cx, cz}] = Chunk(this, cx, cz);         // Création dans la map
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
    if (!(0 <= y && y < CHUNK_H)) return;
    Chunk* chunk = get_chunk_of_block(x,y,z); if (chunk == nullptr) return;
    return chunk->set_block(x,y,z, id);
}

void World::set_block_struct(int x, int y, int z, int id) {
    if (!(0 <= y && y < CHUNK_H)) return;
    Chunk* chunk = get_chunk_of_block(x,y,z);
    if (chunk == nullptr) {
        ivec2 chunk_pos = get_chunk_pos(x,y,z);
        structure_queue.push_back({
            x - chunk_pos.x*CHUNK_W,  y,  z - chunk_pos.y*CHUNK_D,
            id, chunk_pos.x, chunk_pos.y
        });
    }
    else {
        return chunk->set_block(x,y,z, id);
    }
}
