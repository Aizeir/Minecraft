#pragma once
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
#include <fmt/format.h>
using namespace std; 

int success; char info_log[512];

// -1. Definitions
typedef unsigned int uint;
typedef unsigned char uchar;
typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::ivec2 ivec2;
typedef glm::ivec3 ivec3;
typedef glm::vec4 vec4;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;

const int W = 1280;
const int H = 720;

inline float  rad(float  deg) {return glm::radians(deg);}
inline double rad(double deg) {return glm::radians(deg);}

const ivec3 ILEFT  = ivec3(-1, 0, 0);
const ivec3 IRIGHT = ivec3(1, 0, 0);
const ivec3 IUP    = ivec3(0, 1, 0);
const ivec3 IDOWN  = ivec3(0, -1, 0);
const ivec3 IFRONT = ivec3(0, 0, -1);
const ivec3 IBACK  = ivec3(0, 0, 1);
const ivec3 IDIRS[6] = {ILEFT, IRIGHT, IUP, IDOWN, IFRONT, IBACK};

const vec3 LEFT =  vec3(-1.0f, 0.0f, 0.0f); 
const vec3 RIGHT = vec3(1.0f, 0.0f, 0.0f); 
const vec3 UP =    vec3(0.0f, 1.0f, 0.0f); 
const vec3 DOWN =  vec3(0.0f, -1.0f, 0.0f); 
const vec3 FRONT = vec3(0.0f, 0.0f, -1.0f); 
const vec3 BACK =  vec3(0.0f, 0.0f, 1.0f);
const vec3 DIRS[6] = {LEFT, RIGHT, UP, DOWN, FRONT, BACK};

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

ostream& operator<<(ostream& os, const glm::ivec2& v) {
    return os << "ivec2(" << v.x << ", " << v.y << ")";
}
ostream& operator<<(ostream& os, const glm::ivec3& v) {
    return os << "ivec3(" << v.x << ", " << v.y << ", " << v.z << ")";
}
ostream& operator<<(ostream& os, const glm::vec2& v) {
    return os << "vec2(" << v.x << ", " << v.y << ")";
}
ostream& operator<<(ostream& os, const glm::vec3& v) {
    return os << "vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
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
    void set_vec3(const string &name, glm::vec3 vec) const{
        glUniform3f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z);
    }
    void set_vec4(const string &name, glm::vec4 vec) const{
        glUniform4f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z, vec.w);
    }
    void set_vec2(const string &name, glm::vec2 vec) const{
        glUniform2f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y);
    }
    // ------------------------------------------------------------------------
    void set_mat4(const string &name, glm::mat4 mat) const{
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
    void set_mat3(const string &name, glm::mat3 mat) const{
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
};


// II. Cube vertices
struct Vertex {
    vec3 pos;
    vec3 normal;
    vec2 uv;
    int id = 0;

    Vertex() = default;
    Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v, int _id=0) :
        pos(x,y,z), normal(nx,ny,nz), uv(u,v), id(_id) {};
};

ostream& operator<<(ostream& os, const Vertex& v) {
    return os << "Vertex " << v.pos
              << "; " << v.normal
              << "; " << v.uv
              << "; " << v.id;
}

const int VERTEX_PER_FACE = 6;
Vertex cube_faces[6][VERTEX_PER_FACE] = {
    // Left face
    {
        {0.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f},
        {0.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f},
        {0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f},
        {0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f},
        {0.0f,  0.0f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f},
        {0.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f}
    },
    // Right face
    {
        {1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f},
        {1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f},
        {1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f},
        {1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f},
        {1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f},
        {1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f}
    },
    // Up face
    {
        {0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f},
        {1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f},
        {1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f},
        {1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f},
        {0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f},
        {0.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f}
    },
    // Down face
    {
        {0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f},
        {1.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f},
        {1.0f,  0.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f},
        {1.0f,  0.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f},
        {0.0f,  0.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f},
        {0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f}
    },
    // Back face
    {
        {0.0f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f},
        {1.0f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f},
        {1.0f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f},
        {1.0f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f},
        {0.0f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f},
        {0.0f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f}
    },
    // Front face
    {
        {0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f},
        {1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f},
        {1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f},
        {1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f},
        {0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f},
        {0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f}
    },
};


// unsigned int cube_indices[] = {
//     0,1,2, 1,0,3,
//     0,1,2, 2,3,0,
//     0,1,2, 2,3,0,
//     0,1,2, 1,0,3,
//     0,1,2, 1,0,3,
//     0,1,2, 2,3,0,
// };

// III. Textures
unsigned int load_texture(const char* path, uint colormap) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_set_flip_vertically_on_load(true);

    int width, height, num_channels;
    unsigned char *data = stbi_load(path, &width, &height, &num_channels, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, colormap, width, height, 0, colormap, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    stbi_image_free(data);
    return texture;
}

void bind_texture(uint texture, uint unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
}

// ? - MONDE
const int CHUNK_W = 16;
const int CHUNK_H = 16;
const int CHUNK_D = 16;
const int LOAD = 1;

ivec2 get_chunk_pos(int x, int y, int z) {
    return ivec2(floor((float)x / (float)CHUNK_W), floor((float)z / (float)CHUNK_D));
}
ivec2 get_chunk_pos(ivec3 pos) {
    return get_chunk_pos(pos.x, pos.y, pos.z);
}

bool in_chunk_local(int x, int y, int z) {
    return (0<=x && x<CHUNK_W) && (0<=y && y<CHUNK_H) && (0<=z && z<CHUNK_D);
}

const float GRAVITY = 0.5f;
const float JUMP_FORCE = 13.0f;

// DEBUG
GLenum glCheckError(const char *file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            // case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            // case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        cerr << error << " | " << file << " (" << line << ")" << endl;
    }
    return errorCode;
}
#define _check_error() glCheckError(__FILE__, __LINE__) 