#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 uvs;
out vec3 frag_pos;
out vec3 normal;

uniform mat4 model;
uniform mat4 transform;
uniform mat3 normal_mat;

void main() {
    vec4 frag_pos4 = model * vec4(aPos, 1.0);
    gl_Position = transform * frag_pos4;

    uvs = aTexCoord;
    frag_pos = vec3(frag_pos4);
    normal = normalize(normal_mat * aNormal);
}
