#pragma once
#include "util.cpp"

struct T_Vertex {
    vec3 pos;
    vec3 normal;
    vec2 uv;
    T_Vertex() = default;
    T_Vertex(Vertex vertex) : pos(vertex.pos), normal(vertex.normal), uv(vertex.uv) {};
            
    int face = 0; // texture de la face
    float lighting = 1.0f; // niveau de luminosité
    ivec3 block = {0,0,0}; // position du block (savoir si c'est la selection)
};

void set_terrain_vertex_attribs() {
    // - Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(T_Vertex), (void*)offsetof(T_Vertex, pos));
    glEnableVertexAttribArray(0);
    // - Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(T_Vertex), (void*)offsetof(T_Vertex, normal));
    glEnableVertexAttribArray(1);
    // - UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(T_Vertex), (void*)offsetof(T_Vertex, uv));
    glEnableVertexAttribArray(2);
    // - Face ID
    glVertexAttribIPointer(3, 1, GL_INT, sizeof(T_Vertex), (void*)offsetof(T_Vertex, face));
    glEnableVertexAttribArray(3);
    // - Lighting value (per face)
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(T_Vertex), (void*)offsetof(T_Vertex, lighting));
    glEnableVertexAttribArray(4);
    // - Block position
    glVertexAttribIPointer(5, 3, GL_INT, sizeof(T_Vertex), (void*)offsetof(T_Vertex, block));
    glEnableVertexAttribArray(5);
}

Vertex terrain_faces[6][VERTEX_PER_FACE];

struct T_copy_struct { T_copy_struct() { std::memcpy(terrain_faces, cube_faces, sizeof(terrain_faces)); }};
static T_copy_struct _T_copy_struct;