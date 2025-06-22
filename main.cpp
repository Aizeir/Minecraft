#include "util.cpp"
#include "camera.cpp"
#include "world.cpp"
#include "player.cpp"

// RAYCAST VERTICAL BUG

float dt = 0.0f;
float last_frame = 0.0f;

bool is_first_mouse_pos = true;
vec2 last_mouse_pos;

Camera camera = Camera();
Player player = Player(&camera);
World world = World(camera.pos);

void input(GLFWwindow *w, Camera& camera) {
    // Wireframe mode
    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Moving
    player.sprint = (glfwGetKey(w, GLFW_KEY_F) == GLFW_PRESS);
    player.speed.x = player.speed.z = 0; ////;
    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) player.speed += camera.front_xy * player.get_speed();
    if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) player.speed -= camera.front_xy * player.get_speed();
    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) player.speed -= camera.right_xy * player.get_speed();
    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) player.speed += camera.right_xy * player.get_speed();

    // Jump
    if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS && player.ground) player.speed.y = JUMP_FORCE;

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
    camera.pitch = clamp(camera.pitch, -89.0f, 89.0f);
}

// Fonction appelée à chaque clic
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && camera.selecting) {
        if (world.get_block(camera.selection) != BEDROCK) {
            world.set_block(camera.selection, AIR);
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && camera.selecting) {
        if (!player.collides(camera.selection+camera.selection_face))
            world.set_block(camera.selection+camera.selection_face, PLANKS);
    }}

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
    glEnable(GL_CULL_FACE);

    // Seeder le random (enfin, je crois)
    srand((unsigned)(time(0)));

    return window;
}

void draw_ui(Program ui_program, Texture texture, vec4 rect) {
    mat4 model = glm::translate(mat4(1.0f), vec3(rect.x, rect.y, 0.0f));
    model = glm::scale(model, vec3(rect.z, rect.w, 1.0f));

    ui_program.use();
    ui_program.set_mat4("model", model);
    ui_program.set_int("image", texture.unit);

    bind_texture(texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main() {
    // Fenêtre
    GLFWwindow* window = setup();

    // Shaders
    Program wd_program("shaders/world.vert", "shaders/world.frag");
    Program ui_program("shaders/ui.vert", "shaders/ui.frag");
    wd_program.use();

    // Textures
    Texture atlas = load_texture("assets/atlas.png", GL_RGBA);
    Texture crosshair = load_texture("assets/crosshair.png", GL_RGBA);

    // WORLD
    unsigned int world_frame;
    glGenFramebuffers(1, &world_frame);
    glBindFramebuffer(GL_FRAMEBUFFER, world_frame);  

    unsigned int world_texture_ptr;
    glGenTextures(1, &world_texture_ptr);
    glBindTexture(GL_TEXTURE_2D, world_texture_ptr);
    uint world_texture_unit = texture_next_unit; texture_next_unit += 1;
    Texture world_texture = {world_texture_ptr, world_texture_unit};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, world_texture_ptr, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);  
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, W, H);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Buffers (Vertex, element)
    unsigned int VBO, EBO; glGenBuffers(2, (&VBO, &EBO));

    // Vertex array
    unsigned int VAO; glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

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
    // - Lighting value (per face)
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, lighting));
    glEnableVertexAttribArray(4);
    // - Block position
    glVertexAttribIPointer(5, 3, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, block));
    glEnableVertexAttribArray(5);

    // Projection
    mat4 projection = glm::perspective(rad(60.0f), (float)W / (float)H, 0.1f, (float)((LOAD-2) * CHUNK_W) * (float)sqrt(2));

    // UI
    unsigned int VBO_UI; glGenBuffers(1, &VBO_UI);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_UI);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    
    unsigned int VAO_UI; glGenVertexArrays(1, &VAO_UI);
    glBindVertexArray(VAO_UI);

    // Position (x, y)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture (u, v)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Projection ortho
    mat4 projection_ui = glm::ortho(0.0f, (float)W, (float)H, 0.0f);

    // Mainloop
    while (!glfwWindowShouldClose(window)) {
        // delta time
        float frame = glfwGetTime();
        dt = frame - last_frame;
        last_frame = frame;

        // update
        input(window, camera);
        // player
        player.update(dt, &world);
        
        // WORLD
        glBindFramebuffer(GL_FRAMEBUFFER, world_frame);
        glClearColor(0.5f, 0.7f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        wd_program.use();
        bind_texture(atlas);

        // Uniforms
        wd_program.set_vec3("camera_pos", camera.pos);

        wd_program.set_int("atlas", atlas.unit);
        wd_program.set_vec2("atlas_size", vec2(4,8));
        wd_program.set_float("material.shininess", 32.0f);

        wd_program.set_vec3("dirlight.direction", light_direction);
        wd_program.set_vec3("dirlight.ambient",  vec3(1.0f,1.0f,1.0f));
        wd_program.set_vec3("dirlight.diffuse",  vec3(0.7f, 0.7f, 0.7f)); // darken diffuse light a bit
        wd_program.set_vec3("dirlight.specular", vec3(0.f, 0.f, 0.f)); 

        wd_program.set_vec3("spotlight.position", camera.pos);
        wd_program.set_vec3("spotlight.direction", camera.front);
        wd_program.set_vec3("spotlight.ambient",  vec3(0.2f, 0.2f, 0.2f));
        wd_program.set_vec3("spotlight.diffuse",  vec3(0.5f, 0.5f, 0.5f)); // darken diffuse light a bit
        wd_program.set_vec3("spotlight.specular", vec3(1.0f, 1.0f, 1.0f)); 
        wd_program.set_float("spotlight.constant",  1.0f);
        wd_program.set_float("spotlight.linear",    0.09f);
        wd_program.set_float("spotlight.quadratic", 0.032f);
        wd_program.set_float("spotlight.cutoff", glm::cos(rad(12.5f)));
        wd_program.set_float("spotlight.outer_cutoff", glm::cos(rad(17.5f)));
                
        wd_program.set_mat4("transform", projection * camera.view);
        wd_program.set_ivec3("selection", camera.selection);
        wd_program.set_ivec3("selection_face", camera.selection_face);

        // Chunks
        ivec2 c = get_chunk_pos(player.get_block());

        // Générer les nouveaux chunks
        for (int cx = c.x-LOAD; cx <= c.x + LOAD; cx++) {
        for (int cz = c.y-LOAD; cz <= c.y + LOAD; cz++) {
            Chunk* chunk = world.get_chunk(cx,cz);
            if (chunk == nullptr) {
                chunk = world.create_chunk(cx,cz);
            }
        }}

        // Dessiner les meshs des chunks
        for (int cx = c.x-LOAD; cx <= c.x + LOAD; cx++) {
        for (int cz = c.y-LOAD; cz <= c.y + LOAD; cz++) {
            // Le chunk
            Chunk* chunk = world.get_chunk(cx,cz);

            // Créér les meshs si le chunk est nouveau
            if (chunk->is_new) {
                chunk->create_mesh();
                
                // Recréation des meshs des chunks adjacents
                for (int i = 0; i < 4; i++) {
                    Chunk* neighbor = world.get_chunk(cx + IDIRS[i].x, cz + IDIRS[i].y);
                    if (neighbor != nullptr && !neighbor->is_new) { // s'il est new, c'est lui même qui create mesh, pour update les voisins... (c compliqué)
                        neighbor->create_mesh();
                    }
                }
            }
            // Vertices
            glBufferData(GL_ARRAY_BUFFER, chunk->vertices.size() * sizeof(Vertex), chunk->vertices.data(), GL_STATIC_DRAW);
            // Draw
            glDrawArrays(GL_TRIANGLES, 0, chunk->vertices.size());
        }}
        // - Indices (-33% sommets) glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
        //glDrawElements(GL_TRIANGLES, vertices.size(), GL_UNSIGNED_INT, nullptr);

        // WORLD (x2)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        glBindVertexArray(VAO_UI);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_UI);
        bind_texture(world_texture);
        
        ui_program.use();
        ui_program.set_mat4("projection", projection_ui);
        ui_program.set_int("world_texture", world_texture.unit);
        bind_texture(crosshair);
        ui_program.set_int("crosshair", crosshair.unit);

        ui_program.set_vec4("crosshair_rect", texcoord_rect(get_rect(Wf/2.0f,Hf/2.0f,40.0f,40.0f)));

        glDrawArrays(GL_TRIANGLES, 0, 6);

        // des trucs
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Quit
    glfwTerminate();
    return 0;
}
