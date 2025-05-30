#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "util.cpp"
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

    // Shaders
    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main() {\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";
    const char *fragmentShaderSource2 = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "   FragColor = vec4(1.0f, 1.0f, 0.2f, 1.0f);\n"
    "}\n\0";
    
    unsigned int program = load_shader_program(vertexShaderSource, fragmentShaderSource);
    unsigned int program2 = load_shader_program(vertexShaderSource, fragmentShaderSource2);

    // Données des "primitives"
    float vertices[] = {
        -0.5f,  0.5f, 0.0f, // top left 
        0.5f,  0.5f, 0.0f,  // top right
        -0.5f, -0.5f, 0.0f, // bottom left

        0.6f,  0.5f, 0.0f,  // top right
        -0.4f, -0.5f, 0.0f, // bottom left
        0.6f, -0.5f, 0.0f,  // bottom right
    };
    unsigned int indices[] = {
        0, 1, 2,   // first triangle
    }; 
    unsigned int indices2[] = {
        3, 4, 5    // second triangle
    }; 

    unsigned int VAO = load_vertex_array(sizeof(vertices), vertices, sizeof(indices), indices);
    // Set our vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    unsigned int VAO2 = load_vertex_array(sizeof(vertices), vertices, sizeof(indices2), indices2);
    // Set our vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    // Mainloop
    while (!glfwWindowShouldClose(window)) {
        input(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(program);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glUseProgram(program2);
        glBindVertexArray(VAO2);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        
        // des trucs
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Quit
    glfwTerminate();
    return 0;
}
