#pragma once
#include "util.cpp"
#include "world.cpp"


mat4 look_at(vec3 pos, vec3 target, vec3 fixed) {
    // base de la caméra
    vec3 D = pos-target;
    mat4 camera_basis = mat4(
        glm::normalize(vec4(glm::cross(fixed, D), 0.0f)), // droite
        glm::normalize(vec4(glm::cross(D, glm::cross(fixed, D)), 0.0f)), // haut
        glm::normalize(vec4(D, 0.0f)), // direction (opposé, pour sens direct)
        vec4(0.f, 0.f, 0.f, 1.f)
    );

    return glm::transpose(camera_basis) * glm::translate(mat4(1.f), -pos);
}


class Camera { public:
    const float sensibility = .1f;

    vec3 pos, front;
    vec3 front_xy, right_xy;
    mat4 view;

    float yaw, pitch;

    Camera() {
        pos = vec3(0.5f,CHUNK_H*2,0.5f);
        front = vec3(0.f,0.f,-1.f);
        yaw = -90.0f;
    }

    bool selecting = false;
    ivec3 selection = BLOCK_DEFAULT;
    ivec3 selection_face = BLOCK_DEFAULT;

    std::array<ivec3, 2> raycast(World* world) {
        const float MAX_DISTANCE = 10.f;
        selecting = false;

        // Initialisation des variables de direction et de position
        vec3 direction = glm::normalize(front);
        vec3 start_pos = pos;
        ivec3 player_block = ivec3(floor(start_pos));
        ivec3 block = player_block;
        vec3 offset = start_pos - (vec3)block;

        // Détermination de la direction de déplacement dans chaque axe (X, Y, Z)
        ivec3 step(
            (direction.x > 0.0f) ? 1 : -1,
            (direction.y > 0.0f) ? 1 : -1,
            (direction.z > 0.0f) ? 1 : -1
        );

        // Si la caméra est déjà dans un bloc solide, retourne une valeur par défaut
        // if (world->is_solid(block)) return {BLOCK_DEFAULT, BLOCK_DEFAULT};

        // Calcul des distances unitaires de déplacement dans chaque axe
        float distUnitX = direction.x != 0.0f ? abs(1.0f / direction.x) : numeric_limits<float>::infinity();
        float distUnitY = direction.y != 0.0f ? abs(1.0f / direction.y) : numeric_limits<float>::infinity();
        float distUnitZ = direction.z != 0.0f ? abs(1.0f / direction.z) : numeric_limits<float>::infinity();

        // Initialisation des distances initiales pour chaque axe
        float distToX = (step.x == 1 ? 1.0f - offset.x : offset.x) * distUnitX;
        float distToY = (step.y == 1 ? 1.0f - offset.y : offset.y) * distUnitY;
        float distToZ = (step.z == 1 ? 1.0f - offset.z : offset.z) * distUnitZ;

        // Tant que la distance totale n'a pas dépassé la distance maximale
        ivec3 face;
        while (glm::distance((vec3)block + vec3(.5,.5,.5), pos) <= MAX_DISTANCE) {
            // On avance dans l'axe ayant la distance la plus courte
            if (distToX <= distToY && distToX <= distToZ) {
                block.x += step.x;
                distToX += distUnitX;
                face = ivec3(-step.x, 0, 0); // on entre dans le bloc par la face opposée à step
            }
            else if (distToY <= distToZ) {
                block.y += step.y;
                distToY += distUnitY;
                face = ivec3(0, -step.y, 0);
            }
            else {
                block.z += step.z;
                distToZ += distUnitZ;
                face = ivec3(0, 0, -step.z);
            }

            // Bloc solide
            int id = world->get_block(block);
            bool solid = id != AIR && BlockData[id].state != BlockState::LIQUID;
            if (solid && block != player_block) {
                selecting = true;
                return {block, face};
            }
        }

        // Retourne une valeur par défaut si aucun bloc solide n'est touché
        return {BLOCK_DEFAULT, BLOCK_DEFAULT};
    }

    void reflect() {
        pos.y = 2*SEA_LEVEL - pos.y;
        pitch = - pitch;
    }

    void update(float dt, World* world) {
        // selected block
        auto [sel, sel_face] = raycast(world);
        selection = sel; selection_face = sel_face;

        // camera
        vec3 direction;
        direction.x = cos(rad(yaw)) * cos(rad(pitch));
        direction.y = sin(rad(pitch));
        direction.z = sin(rad(yaw)) * cos(rad(pitch));
        front = glm::normalize(direction);

        view = look_at(pos, pos+front, UP); 

        front_xy = glm::normalize(vec3(front.x,0.0f,front.z));
        right_xy = glm::cross(front_xy, UP);
    }
};