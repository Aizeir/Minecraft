#include "util.cpp"


float quad_vertices[] = {
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
};


int main() {
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window;
    window = glfwCreateWindow(W, H, "Minecraft", NULL, NULL);   
    glfwMakeContextCurrent(window);
    
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glViewport(0, 0, W, H);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    // SHADER
    string vert_code_str = read_file("shaders/default.vert");
    string frag_code_str = read_file("shaders/default.frag");
    const char* vert_code = vert_code_str.c_str();
    const char* frag_code = frag_code_str.c_str();
    
    unsigned int vert = load_shader(GL_VERTEX_SHADER, vert_code);
    unsigned int frag = load_shader(GL_FRAGMENT_SHADER, frag_code);
    unsigned int program = load_program(vert, frag);

    glDeleteShader(vert);
    glDeleteShader(frag);

    // TEXTURE
    unsigned int image;
    glGenTextures(1, &image);
    glBindTexture(GL_TEXTURE_2D, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    stbi_set_flip_vertically_on_load(true);

    // Load image data
    int width, height, num_channels;
    unsigned char *data = stbi_load("assets/atlas.png", &width, &height, &num_channels, 0);

    // Get texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // Bind texture to GL_TEXTURE0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image);
    
    // BUFFERS
    unsigned int VAO_UI, VBO_UI;
    glGenBuffers(1, &VBO_UI);
    glGenVertexArrays(1, &VAO_UI);
    glBindVertexArray(VAO_UI);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_UI);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBufferData(GL_ARRAY_BUFFER, size(quad_vertices)*sizeof(float), quad_vertices, GL_STATIC_DRAW);
    
    // MAINLOOP
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
        
        glBindVertexArray(VAO_UI);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_UI);

        glUseProgram(program);
        glUniform1i(glGetUniformLocation(program, "image"), 0); 
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}
