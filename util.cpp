#ifndef FILE_UTIL
#define FILE_UTIL

#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
using namespace std; 

int success; char info_log[512];

// -1. Definitions
typedef unsigned int uint;
typedef unsigned char uchar;
typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4 mat4;

#define W 1280
#define H 720

inline float  rad(float  deg) {return glm::radians(deg);}
inline double rad(double deg) {return glm::radians(deg);}

// 0. UTIL
string read_file(string path) {
    string content;
    ifstream file;
    stringstream stream;
    // ensure ifstream objects can throw exceptions
    file.exceptions(ifstream::failbit | ifstream::badbit);
    try {
        // open file
        file.open(path);
        // read file's buffer contents into streams
        stream << file.rdbuf();
        // close file handlers
        file.close();
        // convert stream into string
        content = stream.str();
    }
    catch (ifstream::failure& e) {
        cout << "ERROR SHADER FILE READ (" << path << "): " << e.what() << endl;
    }
    return content;
}

template <typename TYPENAME>
TYPENAME clamp(TYPENAME value, TYPENAME minVal, TYPENAME maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}


// I. SHADERS
unsigned int load_shader(GLenum type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        cout << source << endl;
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

class Program { public:
    unsigned int ID;

    Program(string vert_path, string frag_path) {
        string vert_code_str = read_file(vert_path);
        string frag_code_str = read_file(frag_path);
        const char* vert_code = vert_code_str.c_str();
        const char* frag_code = frag_code_str.c_str();
        ID = load_shader_program(vert_code, frag_code);
    }

    void use() {
        glUseProgram(ID); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void set_bool(const string &name, bool value) const{        
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void set_int(const string &name, int value) const{
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void set_float(const string &name, float value) const{
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void set_mat4(const string &name, glm::mat4 mat) const{
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
};

// II. BUFFERS
unsigned int load_vertex_array(unsigned int vertices_num, float* vertices, unsigned int indices_num, unsigned int* indices) {
    // Buffers (Vertex, element)
    unsigned int VBO, EBO;
    glGenBuffers(2, (&VBO, &EBO));

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


// III. Textures
unsigned int load_texture(const char* path, uint colormap=GL_RGB) {
    int width, height, num_channels;
    unsigned char *data = stbi_load(path, &width, &height, &num_channels, 0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, colormap, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    stbi_image_free(data);
    return texture;
}

void bind_texture(uint texture, uint unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
}

#endif