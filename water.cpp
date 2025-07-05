#pragma once
#include "util.cpp"

struct W_Vertex {
    vec3 pos;
    vec3 normal;
    vec2 uv;
    W_Vertex() = default;
    W_Vertex(Vertex vertex) : pos(vertex.pos), normal(vertex.normal), uv(vertex.uv) {};
    float lighting;
};

void set_water_vertex_attribs() {
    // - Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(W_Vertex), (void*)offsetof(W_Vertex, pos));
    glEnableVertexAttribArray(0);
    // - Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(W_Vertex), (void*)offsetof(W_Vertex, normal));
    glEnableVertexAttribArray(1);
    // - UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(W_Vertex), (void*)offsetof(W_Vertex, uv));
    glEnableVertexAttribArray(2);
    // - Lighting value (per face)
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(W_Vertex), (void*)offsetof(W_Vertex, lighting));
    glEnableVertexAttribArray(3);
}


Vertex water_faces[6][VERTEX_PER_FACE];

struct W_copy_struct { W_copy_struct() { std::memcpy(water_faces, cube_faces, sizeof(water_faces)); }};
static W_copy_struct _W_copy_struct;