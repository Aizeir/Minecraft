#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "util.cpp"
#include <math.h>
using namespace std; 


GLFWwindow* setup() {
    // Initialiser glfw (librairie ~ pygame)
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Fenêtre
    GLFWwindow* window;
    window = glfwCreateWindow(1280, 720, "Minecraft", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    // Charger glad
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glViewport(0, 0, 1280, 720);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    return window;
}

void input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

int main() {
    // Fenêtre
    GLFWwindow* window = setup();

    // Shader
    Program program = Program("shaders/triangle.vert", "shaders/triangle.frag");

    // Données des "primitives"
    float vertices[] = {
        -0.5f, 0.5f, 0.0f,  1.f,0.f,0.f, // top left 
        0.5f, 0.5f, 0.0f,   0.f,1.f,0.f,  // top right
        -0.5f, -0.5f, 0.0f, 0.f,0.f,1.f, // bottom left
        0.5f, -0.5f, 0.0f,  0.f,0.f,0.f, // bottom right
    };
    unsigned int indices[] = {
        0, 1, 2,   // first triangle
        1, 2, 3    // second triangle
    }; 
    float texCoords[] = {
        0.0f, 0.0f,  // lower-left corner  
        1.0f, 0.0f,  // lower-right corner
        0.5f, 1.0f   // top-center corner
    };

    // Vertex array
    unsigned int VAO = load_vertex_array(sizeof(vertices), vertices, sizeof(indices), indices);
    // Set our vertex attributes pointers (loc, num, type, norm?, stride, offset)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(0);  
    glEnableVertexAttribArray(1);  

    // Textures
    // (paramétrage)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int container = load_texture("assets/container.jpg");
    

    // Mainloop
    while (!glfwWindowShouldClose(window)) {
        input(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // draw
        program.set_float("time", (float)glfwGetTime());
        program.use();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        // des trucs
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Quit
    glfwTerminate();
    return 0;
}
