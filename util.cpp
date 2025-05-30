#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
using namespace std; 

int success; char info_log[512];


// I. SHADERS
unsigned int load_shader(GLenum type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        if (type == GL_VERTEX_SHADER)
             cout << "ERROR: VERTEX SHADER\n" << info_log << endl;
        else cout << "ERROR: FRAGMENT SHADER\n" << info_log << endl;
    }
    return shader;
}

unsigned int load_program(unsigned int vert, unsigned int frag) {
    unsigned int program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, info_log);
        cout << "ERROR: SHADER PROGRAM\n" << info_log << endl;
    }

    return program;
}

unsigned int load_shader_program(const char* vert_source, const char* frag_source) {
    unsigned int vert = load_shader(GL_VERTEX_SHADER, vert_source);
    unsigned int frag = load_shader(GL_FRAGMENT_SHADER, frag_source);
    unsigned int program = load_program(vert, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);
    return program;
}

// II. BUFFERS
unsigned int load_vertex_array(unsigned int vertices_num, float* vertices, unsigned int indices_num, unsigned int* indices) {
    // Buffers (Vertex, element)
    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Vertex array
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // Copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_num, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_num, indices, GL_STATIC_DRAW);
    return VAO;
}