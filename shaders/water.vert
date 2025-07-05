#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aLighting;

out vec3 frag_pos;
out vec3 normal;
out vec2 uv;
flat out float lighting;

uniform mat4 transform;
uniform vec2 atlas_size;
uniform float time;
uniform int water_face;

void main() {
    vec3 pos = aPos;
    if (aNormal.y == 1.0) {
        pos.y -= .2 + sin(pos.x + time)*.1 + sin(pos.z + time)*.1;
    }
    gl_Position = transform * vec4(pos, 1.0);

    frag_pos = pos;
    normal = aNormal;
    uv = (vec2(water_face % 4, atlas_size.y - 1 - water_face / 4) + aTexCoord) / atlas_size;
    lighting = aLighting;
}
