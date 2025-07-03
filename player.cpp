#pragma once
#include "util.cpp"
#include "world.cpp"
#include "camera.cpp"
#include "block.cpp"
#include <functional>

bool rect3d_collision(vec3 a1, vec3 a2, vec3 b1, vec3 b2) {
    // Check for overlap in all three axes
    return (a1.x <= b2.x && a2.x >= b1.x) &&
           (a1.y <= b2.y && a2.y >= b1.y) &&
           (a1.z <= b2.z && a2.z >= b1.z);
}

class Player { public:
    Camera* camera;
    vec3* pos;

    const float movement_speed = 4.0f;
    const float sprint_speed = 8.0f;
    const float water_sprint_speed = 10.0f;
    const vec3 size = vec3(0.5f,1.5f,0.5f);
    const vec3 cam_off = vec3(0.0f, 0.75f, 0.0f);

    vec3 speed = vec3();
    bool sprint = false;
    bool ground = false;
    bool underwater = false;

    ivec3 breaking = BLOCK_DEFAULT;
    float break_time;

    vec2 mouse_press;

    Player(Camera* camera): camera(camera) {
        pos = &(camera->pos);
        inventory.push_back({GRASS_BLOCK, 33});
        inventory.push_back({PLANKS, 10});
    }

    float get_speed() { return sprint ? (underwater ? water_sprint_speed : sprint_speed) : movement_speed; }
    
    ivec3 get_block() { return ivec3(floor(*pos)); }

    bool collides(ivec3 block) {
        // NE VERIFIE PAS LA SOLIDITE !!!
        vec3 center = *pos - cam_off;
        return rect3d_collision(center-size/2.0f, center+size/2.0f, (vec3)block, (vec3)block+vec3(1,1,1));
    }

    bool collisions(World* world, vec3 dv) {
        // Hitbox rectangle
        vec3 side_plus = size / 2.0f + cam_off;
        vec3 side_minus = size / 2.0f - cam_off;
        int min_x = floor(pos->x + dv.x - side_plus.x);
        int max_x = floor(pos->x + dv.x + side_minus.x);
        int min_y = floor(pos->y + dv.y - side_plus.y);
        int max_y = floor(pos->y + dv.y + side_minus.y);
        int min_z = floor(pos->z + dv.z - side_plus.z);
        int max_z = floor(pos->z + dv.z + side_minus.z);

        // Collisions
        for (int x = min_x; x <= max_x; x++) {
        for (int y = min_y; y <= max_y; y++) {
        for (int z = min_z; z <= max_z; z++) {
        if (world->is_solid(x, y, z)) {

            // if (dv.x < 0) { pos->x = x + 1 + side_plus.x; } // left = block.right <=> center = block.right + side
            // if (dv.x > 0) { pos->x = x - side_minus.x; }
            if (dv.y < 0) { pos->y = y + 1 + side_plus.y; }
            if (dv.y > 0) { pos->y = y - side_minus.y; }
            // if (dv.z < 0) { pos->z = z + 1 + side_plus.z; }
            // if (dv.z > 0) { pos->z = z - side_minus.z; }

            return true;
        }
        }}}
        return false;
    }

    vector<pair<int, int>> inventory;
    bool add_item(int item) {
        // Item déjà dans l'inventaire
        int size = inventory.size();
        for (int i=0; i<size; i++) {
            if (inventory[i].first == item) {
                inventory[item].second += 1;
                return true;
            }
        }
        // Nouveau slot
        if (inventory.size() < MAX_INV) {
            inventory.push_back({item, 1});
            return true;
        }
        // Plus de place
        else return false;
    }

    void update(float dt, World* world, function<void()> update_projection) {
        // camera
        camera->update(dt, world);
        
        // gravity
        if (underwater) {
            speed.y = max(speed.y - WATER_GRAVITY, -WATER_GRAVITY*10);
        }
        else {
            speed.y = max(speed.y - GRAVITY, -GRAVITY*60);
        }

        // Collisions
        ground = false;
        vec3 dv = speed*dt; 
        if (collisions(world, vec3(dv.x, 0.0f, 0.0f))) {
            speed.x = 0;
        }
        else {
            pos->x += dv.x;
        }
        if (collisions(world, vec3(0.0f, dv.y, 0.0f))) {
            speed.y = 0;
            if (dv.y < 0) ground = true;
        }
        else {
            pos->y += dv.y;
        }
        if (collisions(world, vec3(0.0f, 0.0f, dv.z))) {
            speed.z = 0;
        }
        else {
            pos->z += dv.z;
        }

        
        // underwater
        ivec3 block_pos = get_block();
        bool _underwater = (world->get_block(block_pos) == WATER);
        if (underwater != _underwater) {
            speed.y = max(speed.y, 0.0f);
            if (speed.y > 0) speed.y = WATER_JUMP_FORCE * 3;
            underwater = _underwater;
            update_projection();
        };

        // Break block
        // break end
        if (breaking != BLOCK_DEFAULT) {
            if (camera->selection != breaking) {
                breaking = BLOCK_DEFAULT;
            }
            else if ((glfwGetTime() - break_time) >= BREAK_DURATION) {
                world->set_block(breaking, AIR);
                breaking = BLOCK_DEFAULT;
            }
        }
        // break begin
        if (mouse_press.x && camera->selecting && (breaking != camera->selection) && (world->get_block(camera->selection) != BEDROCK)) {
            breaking = camera->selection;
            break_time = glfwGetTime();
        }
    }
};