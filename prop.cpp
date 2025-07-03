#pragma once
#include "util.cpp"

struct P_Vertex {
    vec3 pos;
    vec3 normal;
    vec2 uv;
    P_Vertex() = default;
    P_Vertex(Vertex vertex) : pos(vertex.pos), normal(vertex.normal), uv(vertex.uv) {};
            
    int face = 0;
    float lighting = 1.0f;
    ivec3 block = {0,0,0};
};

void set_prop_vertex_attribs() {
    // - Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(P_Vertex), (void*)offsetof(P_Vertex, pos));
    glEnableVertexAttribArray(0);
    // - Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(P_Vertex), (void*)offsetof(P_Vertex, normal));
    glEnableVertexAttribArray(1);
    // - UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(P_Vertex), (void*)offsetof(P_Vertex, uv));
    glEnableVertexAttribArray(2);
    // - Face ID
    glVertexAttribIPointer(3, 1, GL_INT, sizeof(P_Vertex), (void*)offsetof(P_Vertex, face));
    glEnableVertexAttribArray(3);
    // - Lighting value (per face)
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(P_Vertex), (void*)offsetof(P_Vertex, lighting));
    glEnableVertexAttribArray(4);
    // - Block position
    glVertexAttribIPointer(5, 3, GL_INT, sizeof(P_Vertex), (void*)offsetof(P_Vertex, block));
    glEnableVertexAttribArray(5);
}

Vertex prop_faces[2][VERTEX_PER_FACE] = {
    // First diagonal face (from (0,0,0) to (1,1,1))
    {
        {0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f},
        {1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f},
        {1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f},
        {1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f}
    },
    // Second diagonal face (from (0,0,1) to (1,1,0))
    {
        {0.0f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f},
        {1.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f},
        {1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f},
        {1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f},
        {0.0f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f}
    },
};
