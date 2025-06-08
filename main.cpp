#include "util.cpp"
#include "camera.cpp"
#include "world.cpp"

float dt = 0.0f;
float last_frame = 0.0f;

bool is_first_mouse_pos = true;
vec2 last_mouse_pos;

Camera camera = Camera();
World world = World();

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
        world.break_block(camera.selection);
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
    glEnable(GL_CULL_FACE);

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

    // Vertex array
    unsigned int VBO, EBO = cube_buffers();
    unsigned int VAO = cube_vao_with_attribs(VBO, EBO);

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

        // Cube
        glBindVertexArray(VAO);
        program.use();
        program.set_vec3("camera_pos", camera.pos);

        program.set_int("atlas", atlas_unit);
        program.set_vec2("atlas_size", vec2(4,4));
        program.set_float("material.shininess", 32.0f);

        program.set_vec3("dirlight.direction", vec3(-0.2f, -1.0f, -0.3f));
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

        for (uint x=0;x<MAPW;x++) {
        for (uint z=0;z<MAPD;z++) {
        for (uint y=0;y<MAPH;y++) {
            // block id
            int id = world.map[x][z][y];
            if (id == -1) continue;
            program.set_int("block.id", id);
            program.set_int("tex_coord_x", id%4);
            program.set_int("tex_coord_y", 3 - id/4);

            // hover
            program.set_int("hover", (x==camera.selection.x && y == camera.selection.y && z == camera.selection.z));

            // compute object's transform
            vec3 position(x,y,z);
            mat4 model = glm::translate(mat4(1.0f), position);
            // draw
            program.set_mat4("model", model);
            program.set_mat3("normal_mat", glm::mat3(glm::transpose(glm::inverse(model))));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }}}

        // des trucs
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Quit
    glfwTerminate();
    return 0;
}
