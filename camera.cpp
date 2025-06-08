#include "util.cpp"
#include "world.cpp"

const vec3 RIGHT = vec3(1.0f, 0.0f, 0.0f); 
const vec3 UP = vec3(0.0f, 1.0f, 0.0f); 
const vec3 BACK = vec3(0.0f, 0.0f, 1.0f); 

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
        pos = vec3(8.f,16.0f,8.0f);
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

    bool selecting = false;
    glm::ivec3 selection = {-1, -1, -1};
    glm::ivec3 raycast(World* world) {
        const float MAX_DISTANCE = 10.f;
        const float STEP = 0.1f;
        vec3 direction = glm::normalize(front);

        // for (float d = 0.0f; d < MAX_DISTANCE; d += STEP) {
        //     p += direction * STEP;
        //     glm::ivec3 block = glm::floor(p);

        //     if (world->is_solid(block.x,block.y,block.z))
        //         selecting = true;
        //         return block;
        // }
        glm::ivec3 p = glm::floor(pos + vec3(0.f,-2.f,0.f));
        if (world->is_solid(p.x,p.y,p.z)) {
            selecting = true;
            return p;
        }
        selecting = false;
        return {-1, -1, -1};
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
        selection = raycast(world);

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


// uint minX = clamp(b.x-LOAD,0,MAPW-1); uint maxX = clamp(b.x+LOAD,0,MAPW-1);
// uint minZ = clamp(b.z-LOAD,0,MAPD-1); uint maxZ = clamp(b.z+LOAD,0,MAPD-1);
// uint minY = clamp(b.y-LOAD,0,MAPH-1); uint maxY = clamp(b.y+LOAD,0,MAPH-1);
// for (uint x = minX; x <= maxX; x++) {
// for (uint z = minZ; z <= maxZ; z++) {
// for (uint y = minY; y <= maxY; y++) {