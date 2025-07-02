#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <filesystem>
#include <array>
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
typedef glm::ivec4 ivec4;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;

const int W = 1280;
const int H = 720;
const float Wf = (float)W;
const float Hf = (float)H;

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

const int MAX_INV = 8;

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

string expand_shader_includes(const string& shader_code, const filesystem::path& base_directory, unordered_set<string>& included_files) {
    // Create string streams for input and output
    stringstream input(shader_code);
    ostringstream output;
    string line;

    // Process each line of the shader code
    while (std::getline(input, line)) {
        // Check for #include directive at the start of the line
        if (line.find("#include") == 0) {
            // Extract the filename between quotes
            size_t start = line.find('"') + 1;
            size_t end = line.find('"', start);
            if (start == string::npos || end == string::npos || end <= start) {
                cerr << "Erreur de syntaxe include: " << line << endl;
                continue;
            }

            string include_file = line.substr(start, end - start);
            filesystem::path full_path = base_directory / include_file;
            string full_path_str = full_path.lexically_normal().string();

            // Skip duplicate includes
            if (included_files.count(full_path_str)) {
                output << "// (skip duplicate include: " << include_file << ")\n";
                continue;
            }

            // Try to open the include file
            ifstream file(full_path);
            if (!file.is_open()) {
                cerr << "Erreur : impossible d'ouvrir " << full_path_str << endl;
                continue;
            }

            // Mark this file as included
            included_files.insert(full_path_str);
            stringstream included_code;
            included_code << file.rdbuf();

            // Recursively expand includes in the included file
            output << "// Begin include: " << include_file << "\n";
            output << expand_shader_includes(included_code.str(), full_path.parent_path(), included_files);
            output << "// End include: " << include_file << "\n";
        }
        else {
            // Output normal lines directly
            output << line << "\n";
        }
    }

    // Return the expanded shader code
    return output.str();
}

template <typename TYPENAME>
bool set_contains(unordered_set<TYPENAME>* map, TYPENAME value) {
    return map->find(value) != map->end();
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
ostream& operator<<(ostream& os, const glm::vec4& v) {
    return os << "vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
}


vec4 get_rect(float x, float y, float w, float h) {
    return vec4(x-w/2.0f, y-h/2.0f, w, h);
}
vec4 get_rect(vec4 rect) {
    return get_rect(rect.x,rect.y,rect.z,rect.w);
}
vec4 texcoord_rect(float x, float y, float w, float h) {
    return vec4(x/Wf,y/Hf,w/Wf,h/Hf);
}
vec4 texcoord_rect(vec4 rect) {
    return texcoord_rect(rect.x,rect.y,rect.z,rect.w);
}

int randint(int a, int b) {
    return a + rand() % (b-a+1);
}

bool proba(int x) {
    return randint(1,x) == 1;
}

// III. Textures
struct Texture { GLuint value; }; // sert juste a pas confondre avec les i/GL_TEXTURE_i
Texture create_texture(int width, int height, int colormap, unsigned char* data=NULL, int src_format=GL_UNSIGNED_BYTE) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, colormap, width, height, 0, colormap, src_format, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return {texture};
}

void bind_texture(GLenum unit, Texture texture) {
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, texture.value);
}

Texture load_texture(const char* path, int colormap, vec2 *size=nullptr) {
    stbi_set_flip_vertically_on_load(true);

    // Load image data
    int width, height, num_channels;
    unsigned char *data = stbi_load(path, &width, &height, &num_channels, 0);
    stbi_image_free(data);
    if (size != nullptr) *size = vec2(width, height);

    // Get texture
    return create_texture(width, height, colormap, data);
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

    Program() {ID = 1000;};
    Program(string vert_path, string frag_path) {
        // files
        string vert_code_str = read_file(vert_path);
        string frag_code_str = read_file(frag_path);
        // includes
        unordered_set<string> already_included;
        vert_code_str = expand_shader_includes(vert_code_str, "assets/shaders", already_included);
        already_included.clear();
        frag_code_str = expand_shader_includes(frag_code_str, "assets/shaders", already_included);
        
        // code
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
    void set_texture(const string &name, Texture texture, GLenum unit) const{
        bind_texture(unit, texture);
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)(unit-GL_TEXTURE0)); 
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
    void set_ivec3(const string &name, glm::ivec3 vec) const{
        glUniform3i(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z);
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
    int face = 0;
    float lighting = 1.0f;
    ivec3 block = {0,0,0};

    Vertex() = default;
    Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v) :
        pos(x,y,z), normal(nx,ny,nz), uv(u,v) {};
};



ostream& operator<<(ostream& os, const Vertex& v) {
    return os << "Vertex " << v.pos
              << "; " << v.normal
              << "; " << v.uv
              << "; " << v.face;
}

const int VERTEX_PER_FACE = 6;
Vertex cube_faces[6][VERTEX_PER_FACE] = {
    // Left face
    {
        {0.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f},
        {0.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f},
        {0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f},
        {0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f},
        {0.0f,  0.0f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f},
        {0.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f}
    },
    // Right face
    {
        {1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f},
        {1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f},
        {1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f},
        {1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f},
        {1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f},
        {1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f}
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

float full_quad_vertices[] = {
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
};

array<float, 24> quad_rect(float x, float y, float w, float h) {
    float min_nx = x / Wf *2-1, min_ny = y / Hf *2-1;
    float max_nx = (x+w) / Wf *2-1, max_ny = (y+h) / Hf *2-1;

    return {
        min_nx,  max_ny,  0.0f, 1.0f,
        min_nx,  min_ny,  0.0f, 0.0f,
        max_nx,  min_ny,  1.0f, 0.0f,

        min_nx,  max_ny,  0.0f, 1.0f,
        max_nx,  min_ny,  1.0f, 0.0f,
        max_nx,  max_ny,  1.0f, 1.0f
    };
}

// unsigned int cube_indices[] = {
//     0,1,2, 1,0,3,
//     0,1,2, 2,3,0,
//     0,1,2, 2,3,0,
//     0,1,2, 1,0,3,
//     0,1,2, 1,0,3,
//     0,1,2, 2,3,0,
// };


// ? - MONDE
const int CHUNK_W = 16;
const int CHUNK_H = 30;
const int CHUNK_D = 16;
const int LOAD = 6;
const int SEA_LEVEL = 10;
const int BED_LEVEL = 4;

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
vec3 light_direction = normalize(vec3(-0.2f, -1.0f, -0.3f));
const ivec3 SELECTION_DEFAULT = {0, -1, 0};

// - FRAMEBUFFERS
int renderbuffer(int w, int h, int component) {
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);  
    glRenderbufferStorage(GL_RENDERBUFFER, component, w, h);
    return rbo;
}

pair<uint,Texture> create_framebuffer(int w, int h) {
    // Framebuffer object
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    // Color buffer
    Texture color = create_texture(w,h, GL_RGB);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color.value, 0);

    // Depth buffer
    //     *depth = create_texture(w,h, GL_DEPTH_COMPONENT);
    //     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth->texture, 0);
    int rbo = renderbuffer(w,h, GL_DEPTH_COMPONENT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Rebind default, return
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return {fbo, color};
}

void bind_framebuffer(int fbo=0, int w=W, int h=H) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0,0, w,h);
}

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