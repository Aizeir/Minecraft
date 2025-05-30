#include "util.cpp"

const vec3 up = vec3(0.0f, 1.0f, 0.0f); 

mat4 look_at(vec3 pos, vec3 target, vec3 fixed) {
    // base de la caméra
    vec3 D = pos-target;
    mat4 camera_basis = mat4(
        glm::normalize(vec4(glm::cross(fixed, D), 0.0f)), // droite
        glm::normalize(vec4(fixed, 0.0f)), // haut
        glm::normalize(vec4(D, 0.0f)), // direction (opposé, pour sens direct)
        vec4(0.f, 0.f, 0.f, 1.f)
    );

    return glm::transpose(camera_basis) * glm::translate(mat4(1.f), -pos);
}

class Camera { public:
    const float speed = 1.0f;
    const float sensibility = .1f;

    vec3 pos, front, right;
    mat4 view;

    float yaw, pitch;

    Camera() {
        pos = vec3(0.f,0.f,3.f);
        front = vec3(0.f,0.f,-1.f);
        yaw = -90.0f;

    }

    void update() {
        right = glm::normalize(glm::cross(front, up));
        
        vec3 direction;
        direction.x = cos(rad(yaw)) * cos(rad(pitch));
        direction.y = sin(rad(pitch));
        direction.z = sin(rad(yaw)) * cos(rad(pitch));
        front = glm::normalize(direction);

        view = look_at(pos, pos+front, up); 
    }
};