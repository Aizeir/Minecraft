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
    const float movement_speed = 4.0f;
    const float sensibility = .1f;

    vec3 pos, front;
    vec3 front_xy, right_xy;
    mat4 view;

    float yaw, pitch;

    Camera() {
        pos = vec3(0.5f,16.0f,0.5f);
        front = vec3(0.f,0.f,-1.f);
        yaw = -90.0f;
    }

    const vec3 size = vec3(1.0f,1.0f,1.0f) * 2.f;
    
    vec3 speed = vec3();
    bool ground = false;
    bool collisions(World* world, vec3 dv) {
        // Hitbox ponctuelle
        ivec3 block = floor(pos);

        if (world->is_solid(block)) {
            if (dv.x < 0) pos.x = block.x;
            if (dv.x > 0) pos.x = block.x + 1;
            if (dv.z < 0) pos.z = block.z;
            if (dv.z > 0) pos.z = block.z + 1;
            if (dv.y < 0) pos.y = block.y;
            if (dv.y > 0) pos.y = block.y + 1;
            return true;
        }
        return false;
    }

    ivec3 block() { return ivec3(floor(pos)); }

    bool selecting = false;
    ivec3 selection = SELECTION_DEFAULT;
    ivec3 selection_face = SELECTION_DEFAULT;

    std::array<ivec3, 2> raycast(World* world) {
        const float MAX_DISTANCE = 10.f;
        selecting = false;

        // Initialisation des variables de direction et de position
        vec3 direction = glm::normalize(front);
        vec3 start_pos = pos;
        ivec3 block = floor(start_pos);
        vec3 offset = start_pos - (vec3)block;

        // Détermination de la direction de déplacement dans chaque axe (X, Y, Z)
        ivec3 step(
            (direction.x > 0.0f) ? 1 : -1,
            (direction.y > 0.0f) ? 1 : -1,
            (direction.z > 0.0f) ? 1 : -1
        );

        // Si la caméra est déjà dans un bloc solide, retourne une valeur par défaut
        if (world->is_solid(block)) return {SELECTION_DEFAULT, SELECTION_DEFAULT};

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
        while (distToX <= MAX_DISTANCE && distToY <= MAX_DISTANCE && distToZ <= MAX_DISTANCE) {
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
            if (world->is_solid(block)) {
                selecting = true;
                return {block, face};
            }
        }

        // Retourne une valeur par défaut si aucun bloc solide n'est touché
        return {SELECTION_DEFAULT, SELECTION_DEFAULT};
    }

    void update(float dt, World* world) {
        // // gravity
        // speed.y -= GRAVITY;

        // // collisions
        // vec3 dv = speed*dt;
        // if (collisions(world, vec3(dv.x, 0.0f, 0.0f))) {
        //     speed.x = 0;
        // }
        // if (collisions(world, vec3(0.0f, dv.y, 0.0f))) {
        //     speed.y = 0;
        //     if (dv.y < 0) {
        //         ground = true;
        //     }
        // }
        // if (collisions(world, vec3(0.0f, 0.0f, dv.z))) {
        //     speed.z = 0;
        // }

        pos += speed * dt;

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