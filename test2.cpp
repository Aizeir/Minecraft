#include "util.cpp"
#include "camera.cpp"
#include "world.cpp"
#include "player.cpp"
#include "overlay.cpp"
#include "block.cpp"


void input(GLFWwindow *w, Camera& camera) {
    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

GLFWwindow* setup() {
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window;
    window = glfwCreateWindow(W, H, "Minecraft", NULL, NULL);   
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glViewport(0, 0, W, H);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    srand((unsigned)(time(0)));
    return window;
}

class Game { public:
    GLFWwindow* window;
    Program ui_program;
    uint VAO_UI, VBO_UI;
    Texture hotbar; vec2 hotbar_size;
    GLenum err;

    Game() {
    window = setup();
    err = glGetError(); if (err != GL_NO_ERROR) cerr << "Begin openGL error: " << hex << err << endl;
    
    hotbar = load_texture("assets/inventory.png", GL_RGBA, &hotbar_size);
    ui_program = Program("shaders/default.vert", "shaders/ui.frag");

    glGenBuffers(1, &VBO_UI);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_UI);
    
    glGenVertexArrays(1, &VAO_UI);
    glBindVertexArray(VAO_UI);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    err = glGetError(); if (err != GL_NO_ERROR) cerr << "End setup openGL error: " << hex << err << endl;
 
    while (!glfwWindowShouldClose(window)) {
       
        glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
        
        glBindVertexArray(VAO_UI);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_UI);

        vec4 rect = vec4(Wf/2.f - hotbar_size.x * 3, Hf*.1f, hotbar_size.x * 6, hotbar_size.y * 6);

        array<float, 24> vertices;
        vertices = quad_rect(rect[0], rect[1], rect[2], rect[3]);
        glBufferData(GL_ARRAY_BUFFER, 24*sizeof(float), vertices.data(), GL_STATIC_DRAW);

        ui_program.use(); 
        ui_program.set_texture("image", hotbar, GL_TEXTURE0);
        ui_program.set_vec4("image_rect", vec4(0,0,1,1));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }}
};

int main() {
    Game game = Game();
    glfwTerminate();
    return 0;
}
