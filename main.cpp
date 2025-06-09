#include "util.cpp"
#include "camera.cpp"
#include "world.cpp"

float dt = 0.0f;
float last_frame = 0.0f;

bool is_first_mouse_pos = true;
vec2 last_mouse_pos;

Camera camera = Camera();
World world = World(camera.pos);

void input(GLFWwindow *w, Camera& camera) {
    // Wireframe mode
    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Moving
    camera.speed.x = camera.speed.z = camera.speed.y = 0; ////;
    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) camera.speed += camera.front_xy * camera.movement_speed;
    if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) camera.speed -= camera.front_xy * camera.movement_speed;
    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) camera.speed -= camera.right_xy * camera.movement_speed;
    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) camera.speed += camera.right_xy * camera.movement_speed;
    if (glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.speed -= UP * camera.movement_speed;
    if (glfwGetKey(w, GLFW_KEY_E) == GLFW_PRESS) camera.speed += UP * camera.movement_speed;

    // Jump
    if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS && camera.ground) camera.speed.y = JUMP_FORCE;

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

// Fonction appelée à chaque clic
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && camera.selecting) {
        world.set_block(camera.selection, AIR);
    }
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
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    // Charger glad
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glViewport(0, 0, W, H);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    return window;
}

int main() {
    // Fenêtre
    GLFWwindow* window = setup();

    // Shaders
    Program program("shaders/default.vert", "shaders/default.frag");
    program.use();

    // Textures
    unsigned int atlas = load_texture("assets/atlas.png", GL_RGBA); uint atlas_unit = 0;
    bind_texture(atlas, atlas_unit);

    // Buffers (Vertex, element)
    unsigned int VBO, EBO; glGenBuffers(2, (&VBO, &EBO));
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // Vertex array
    unsigned int VAO; glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // - Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);
    // - Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    // - UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(2);
    // - Block ID
    glVertexAttribIPointer(3, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, id));
    glEnableVertexAttribArray(3);
    // - Lighting
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, lighting));
    glEnableVertexAttribArray(4);


    // Projection
    mat4 projection = glm::perspective(rad(60.0f), (float)W / (float)H, 0.1f, 100.0f);

    // Mainloop
    while (!glfwWindowShouldClose(window)) {
        // delta time
        float frame = glfwGetTime();
        dt = frame - last_frame;
        last_frame = frame;

        // update
        input(window, camera);

        // camera
        camera.update(dt, &world);
        
        // draw
        glClearColor(0.5f, 0.7f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Uniforms
        program.use();
        program.set_vec3("camera_pos", camera.pos);

        program.set_int("atlas", atlas_unit);
        program.set_vec2("atlas_size", vec2(4,4));
        program.set_float("material.shininess", 32.0f);

        program.set_vec3("dirlight.direction", light_direction);
        program.set_vec3("dirlight.ambient",  vec3(1.0f,1.0f,1.0f));
        program.set_vec3("dirlight.diffuse",  vec3(0.7f, 0.7f, 0.7f)); // darken diffuse light a bit
        program.set_vec3("dirlight.specular", vec3(0.f, 0.f, 0.f)); 

        program.set_vec3("spotlight.position", camera.pos);
        program.set_vec3("spotlight.direction", camera.front);
        program.set_vec3("spotlight.ambient",  vec3(0.2f, 0.2f, 0.2f));
        program.set_vec3("spotlight.diffuse",  vec3(0.5f, 0.5f, 0.5f)); // darken diffuse light a bit
        program.set_vec3("spotlight.specular", vec3(1.0f, 1.0f, 1.0f)); 
        program.set_float("spotlight.constant",  1.0f);
        program.set_float("spotlight.linear",    0.09f);
        program.set_float("spotlight.quadratic", 0.032f);
        program.set_float("spotlight.cutoff", glm::cos(rad(12.5f)));
        program.set_float("spotlight.outer_cutoff", glm::cos(rad(17.5f)));
                
        program.set_mat4("transform", projection * camera.view);

        // Chunks
        ivec2 c = get_chunk_pos(camera.pos);
        for (int cx = c.x-LOAD; cx <= c.x + LOAD; cx++) {
        for (int cz = c.y-LOAD; cz <= c.y + LOAD; cz++) {
            // Loading chunks
            Chunk* chunk = world.get_chunk(cx,cz);
            if (chunk == nullptr) {
                chunk = world.load_chunk(cx,cz);
            }

            glBindVertexArray(VAO);
            // - Vertices
            glBufferData(GL_ARRAY_BUFFER, chunk->vertices.size() * sizeof(Vertex), chunk->vertices.data(), GL_STATIC_DRAW);
            // - Indices (-33% sommets)
            //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
            // Draw final
            glDrawArrays(GL_TRIANGLES, 0, chunk->vertices.size());
            //glDrawElements(GL_TRIANGLES, vertices.size(), GL_UNSIGNED_INT, nullptr);
        }}

        // des trucs
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Quit
    glfwTerminate();
    return 0;
}
