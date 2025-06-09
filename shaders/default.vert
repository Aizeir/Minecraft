#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in int aId;

out vec3 frag_pos;
out vec3 normal;
out vec2 uv;
flat out int block_id;

uniform mat4 transform;
uniform vec2 atlas_size;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);

    frag_pos = aPos;
    normal = aNormal;
    uv = (vec2(aId % 4, 3 - aId / 4) + aTexCoord) / atlas_size;
    block_id = aId;
}
