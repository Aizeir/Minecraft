#include "util.cpp"
#include "camera.cpp"

float dt = 0.0f;
float last_frame = 0.0f;

bool is_first_mouse_pos = true;
vec2 last_mouse_pos;

Camera camera = Camera();

void input(GLFWwindow *w, Camera& camera) {
    // Wireframe mode
    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Moving
    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) camera.pos += camera.front * camera.speed * dt;
    if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) camera.pos -= camera.front * camera.speed * dt;
    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) camera.pos -= camera.right * camera.speed * dt;
    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) camera.pos += camera.right * camera.speed * dt;
}

void mouse_callback(GLFWwindow* window, double mouse_x, double mouse_y) {
    vec2 mouse_pos = vec2((float)mouse_x, (float)mouse_y);
    
    if (is_first_mouse_pos) {
        last_mouse_pos = mouse_pos;
        is_first_mouse_pos = false;
    }
    
    vec2 offset = (mouse_pos - last_mouse_pos) * camera.sensibility;
    last_mouse_pos = mouse_pos;

    camera.yaw += offset.x;
    camera.pitch -= offset.y;
    camera.pitch = clamp(camera.pitch, -89.f, 89.f);
}

GLFWwindow* setup() {
    // Initialiser glfw (librairie ~ pygame)
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Fenêtre
    GLFWwindow* window;
    window = glfwCreateWindow(W, H, "Minecraft", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetCursorPosCallback(window, mouse_callback);

    // Charger glad
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glViewport(0, 0, W, H);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });
    glEnable(GL_DEPTH_TEST);

    return window;
}

int main() {
    // Fenêtre
    GLFWwindow* window = setup();

    // Shader
    Program program = Program("shaders/triangle.vert", "shaders/triangle.frag");

    // Données des "primitives"
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    uint indices[36];
    for (uint i = 0; i < 36; i++) indices[i] = i;
    
    // Vertex array
    unsigned int VAO = load_vertex_array(sizeof(vertices), vertices, sizeof(indices), indices);
    // Set our vertex attributes pointers (loc, num, type, norm?, stride, offset)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(0);  
    glEnableVertexAttribArray(1);  

    // Textures
    // (paramétrage)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    stbi_set_flip_vertically_on_load(true);

    // images
    unsigned int container = load_texture("assets/container.jpg");
    uint container_unit = 0;
    unsigned int brick = load_texture("assets/awesomeface.png", GL_RGBA);
    uint brick_unit = 1;
    program.use();
    program.set_int("image", container_unit);
    program.set_int("image2", brick_unit);

    // Objets
    vec3 cubePositions[] = {
        vec3( 0.0f,  0.0f,  0.0f), 
        vec3( 2.0f,  5.0f, -15.0f), 
        vec3(-1.5f, -2.2f, -2.5f),  
        vec3(-3.8f, -2.0f, -12.3f),  
        vec3( 2.4f, -0.4f, -3.5f),  
        vec3(-1.7f,  3.0f, -7.5f),  
        vec3( 1.3f, -2.0f, -2.5f),  
        vec3( 1.5f,  2.0f, -2.5f), 
        vec3( 1.5f,  0.2f, -1.5f), 
        vec3(-1.3f,  1.0f, -1.5f)  
    };

    glm::mat4 projection = glm::perspective(rad(60.0f), (float)W / (float)H, 0.1f, 100.0f);

    // Mainloop
    while (!glfwWindowShouldClose(window)) {
        // delta time
        float frame = glfwGetTime();
        dt = frame - last_frame;
        last_frame = frame;

        // update
        input(window, camera);
        camera.update();
        
        // draw
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glBindVertexArray(VAO); 
        bind_texture(container, container_unit);
        bind_texture(brick, brick_unit);
        program.use();
        
        for (uint i = 0; i<10; ++i) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            if (i%3==0) angle += (float)glfwGetTime()*5.f;
            model = glm::rotate(model, rad(angle), vec3(1.0f, 0.3f, 0.5f));
            program.set_mat4("transform", projection * camera.view * model);
            
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
        // des trucs
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Quit
    glfwTerminate();
    return 0;
}
