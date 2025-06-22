#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in int aId;
layout (location = 4) in float aLighting;
layout (location = 5) in ivec3 aBlock;

out vec3 frag_pos;
out vec3 normal;
out vec2 uv;
flat out int block_id;
flat out float lighting;
flat out ivec3 block;

uniform mat4 transform;
uniform vec2 atlas_size;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);

    frag_pos = aPos;
    normal = aNormal;
    uv = (vec2(aId % 4, atlas_size.y - 1 - aId / 4) + aTexCoord) / atlas_size;
    block_id = aId;
    lighting = aLighting;
    block = aBlock;
}
