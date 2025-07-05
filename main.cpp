#include "util.cpp"
#include "camera.cpp"
#include "world.cpp"
#include "player.cpp"
#include "overlay.cpp"
#include "block.cpp"
#include "water.cpp"
#include "terrain.cpp"
#include "prop.cpp"

// TEXTURE BUG PIXELS RESIDUELS
// COLLISION MOVEMENT BUG: JUMP 1BLOCK AU DESSUS DE SOI XRAY
// COLLISION MOVEMENT BUG: GRANDE VITESSE FONCAGE TRAVERSE MAP
// BUG TREE GEN CHUNKS (VERIFIER LA RIGUEUR DU TRUC)
// RAYCAST BUG VISER 1BLOCK EN DESSOUS DE SOI / DANS L'EAU / TERRE FERME
bool is_first_mouse_pos = true;
vec2 last_mouse_pos;

Camera camera = Camera();
Player player = Player(&camera);

World world = World(camera.pos);

void input(GLFWwindow *w, Camera& camera) {
    // Moving
    player.sprint = (glfwGetKey(w, GLFW_KEY_F) == GLFW_PRESS);
    player.speed.x = player.speed.z = 0;

    vec3 front = player.underwater ? camera.front : camera.front_xy;
    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) player.speed += front * player.get_speed();
    if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) player.speed -= front * player.get_speed();
    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) player.speed -= camera.right_xy * player.get_speed();
    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) player.speed += camera.right_xy * player.get_speed();

    // Jump
    if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS && !player.flying) {
        if (player.underwater) {
            player.speed.y = WATER_JUMP_FORCE;
        }
        else if (!player.underwater && player.ground) {
            player.speed.y = JUMP_FORCE;
        }
    }
    // Fly down
    if (player.flying) {
        player.speed.y = FLY_FORCE * ((glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS) - (glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS));
    }
}

int polygon_mode = GL_FILL;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Wireframe mode
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        polygon_mode = (polygon_mode==GL_FILL) ? GL_LINE : GL_FILL;
        glPolygonMode(GL_FRONT_AND_BACK, polygon_mode);
    // Fly
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        player.speed = vec3();
        player.flying = !player.flying;
    }
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
    camera.pitch = glm::clamp(camera.pitch, -89.0f, 89.0f);
}

// Fonction appelée à chaque clic
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    // Basic
    if (button == GLFW_MOUSE_BUTTON_LEFT && action==GLFW_PRESS) player.mouse_press.x = true;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action==GLFW_RELEASE) player.mouse_press.x = false;
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action==GLFW_PRESS) player.mouse_press.y = true;
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action==GLFW_RELEASE) player.mouse_press.y = false;

    // Break block
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        player.breaking = BLOCK_DEFAULT;
    }
    // Place block
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && camera.selecting) {
        if (!player.collides(camera.selection+camera.selection_face) && (player.inventory.size() > 0)) {
            pair<int,int> its = player.inventory.at(player.selection);
            if (its.second > 0) {
                its.second -= 1;
                world.set_block(camera.selection+camera.selection_face, its.first);
                
                if (its.second <= 0)
                    player.inventory.erase(player.inventory.begin() + player.selection);
            }
        }
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
    glfwSetKeyCallback(window, key_callback);
    // Charger glad
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glViewport(0, 0, W, H);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Des trucs
    srand((unsigned)(time(0))); // Seeder le random (enfin, je crois)
    return window;
}

class Game { public:
    // Fenêtre
    GLFWwindow* window;
    // Shaders
    Program wd_program, water_program, prop_program;
    Program crosshair_program, ui_program;
    // Textures
    Texture atlas, crosshair, breaking;
    vec2 atlas_size = vec2(4,8);
    vec2 atlas_img_size;
    // Framebuffer
    Texture world_texture, depth_texture;
    uint world_fbo;
    // Vertex arrays
    uint VAO, VBO, EBO;
    uint VAO_WATER, VBO_WATER, EBO_WATER;
    uint VAO_PROP, VBO_PROP, EBO_PROP;
    uint VAO_UI, VBO_UI, EBO_UI;
    // Projection matrices
    mat4 projection;
    // Overlay
    Overlay overlay;
    GLenum err;

    float near_plane = 0.1f;
    float far_plane = (float)((LOAD-2) * CHUNK_W) * (float)sqrt(2);
    void update_projection() {
        float FOV = player.underwater ? 40.0f : 60.0f;
        projection = glm::perspective(rad(FOV), (float)W / (float)H, near_plane, far_plane);
    }

    // UTILISE GL_TEXTURE_0
    void set_default_uniforms(Program* program) {
        program->set_texture("atlas", atlas, GL_TEXTURE0);
        program->set_vec2("atlas_size", atlas_size);

        program->set_vec3("camera_pos", camera.pos);
        program->set_mat4("transform", projection * camera.view);

        program->set_float("time", glfwGetTime());
    }

    Game() {
    // Setup
    window = setup();
    err = glGetError(); if (err != GL_NO_ERROR) cerr << "Begin openGL error: " << hex << err << endl;
    
    // Overlay
    overlay = Overlay(&ui_program, VAO_UI, VBO_UI, &player);
    
    // Framebuffers
    auto fb = create_framebuffer(W, H);
    world_fbo = fb.first; world_texture = fb.second.first, depth_texture = fb.second.second;

    // Textures
    atlas = load_texture("assets/atlas.png", GL_RGBA, &atlas_img_size);
    crosshair = load_texture("assets/crosshair.png", GL_RGBA);
    breaking = load_texture("assets/breaking.png", GL_RGBA);
    
    overlay.items = load_texture("assets/items.png", GL_RGBA);
    overlay.hotbar = load_texture("assets/inventory.png", GL_RGBA, &overlay.hotbar_size);
    
    // Shaders
    wd_program = Program("shaders/world.vert", "shaders/world.frag");
    water_program = Program("shaders/water.vert", "shaders/water.frag");
    prop_program = Program("shaders/world.vert", "shaders/world.frag");
    crosshair_program = Program("shaders/default.vert", "shaders/crosshair.frag");
    ui_program = Program("shaders/default.vert", "shaders/ui.frag");
    
    // Projection
    update_projection();

    // WORLD
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    set_terrain_vertex_attribs();

    // WATER
    glGenBuffers(1, &VBO_WATER);
    glGenVertexArrays(1, &VAO_WATER);
    glBindVertexArray(VAO_WATER);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_WATER);
    set_water_vertex_attribs();

    // PROPS
    glGenBuffers(1, &VBO_PROP);
    glGenVertexArrays(1, &VAO_PROP);
    glBindVertexArray(VAO_PROP);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PROP);
    set_prop_vertex_attribs();

    // UI
    glGenBuffers(1, &VBO_UI);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_UI);
    glGenVertexArrays(1, &VAO_UI);
    glBindVertexArray(VAO_UI);

    // Position (x, y)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture (u, v)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    err = glGetError(); if (err != GL_NO_ERROR) cerr << "End setup openGL error: " << hex << err << endl;
 
    // -------- MAINLOOP --------
    float frame, dt, last_frame;
    while (!glfwWindowShouldClose(window)) {
        // Delta time
        frame = glfwGetTime();
        dt = frame - last_frame;
        last_frame = frame;

        // Update
        input(window, camera);
        // Player
        player.update(dt, &world, [&]() {update_projection();});

        // Chunks (générer les nouveaux chunks)
        ivec2 c = get_chunk_pos(player.get_block());

        for (int cx = c.x-LOAD; cx <= c.x + LOAD; cx++) {
        for (int cz = c.y-LOAD; cz <= c.y + LOAD; cz++) {
            Chunk* chunk = world.get_chunk(cx,cz);
            if (chunk == nullptr) {
                chunk = world.create_chunk(cx,cz);
            }
        }}
  
        // -------- Draw --------
        // -------- World render --------
        glBindFramebuffer(GL_FRAMEBUFFER, world_fbo);
        glClearColor(SKY_COLOR.x, SKY_COLOR.y, SKY_COLOR.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // WORLD
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        wd_program.use();
        set_default_uniforms(&wd_program);
        wd_program.set_ivec3("selection", camera.selection);
        wd_program.set_ivec3("selection_face", camera.selection_face);
        wd_program.set_texture("breaking_image", breaking, GL_TEXTURE1);
        wd_program.set_int("is_breaking", (player.breaking == BLOCK_DEFAULT ? -1 : floor(2.4 * (glfwGetTime()-player.break_time) / BREAK_DURATION)));
        
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
                    if (neighbor != nullptr && !neighbor->is_new) // s'il est new, c'est lui même qui create mesh, pour update les voisins... (c compliqué)
                        neighbor->create_mesh();
                }
            }
            
            // World display
            glBufferData(GL_ARRAY_BUFFER, chunk->vertices.size() * sizeof(T_Vertex), chunk->vertices.data(), GL_STATIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, chunk->vertices.size());
            // (Indices) (-33% sommets)
            //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
            //glDrawElements(GL_TRIANGLES, vertices.size(), GL_UNSIGNED_INT, nullptr);
        }}


        // WATER
        glBindVertexArray(VAO_WATER); glBindBuffer(GL_ARRAY_BUFFER, VBO_WATER);
        
        water_program.use();
        set_default_uniforms(&water_program);
        water_program.set_int("water_face", BlockData[WATER].faces[0]);

        for (int cx = c.x-LOAD; cx <= c.x + LOAD; cx++) {
        for (int cz = c.y-LOAD; cz <= c.y + LOAD; cz++) {
            // Le chunk
            Chunk* chunk = world.get_chunk(cx,cz);

            // Water display
            glBufferData(GL_ARRAY_BUFFER, chunk->water_vertices.size() * sizeof(W_Vertex), chunk->water_vertices.data(), GL_STATIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, chunk->water_vertices.size());
        }}

        // PROPS
        glDisable(GL_CULL_FACE);
        glBindVertexArray(VAO_PROP); glBindBuffer(GL_ARRAY_BUFFER, VBO_PROP);
         
        prop_program.use();
        set_default_uniforms(&prop_program);
        prop_program.set_ivec3("selection", camera.selection);
        prop_program.set_ivec3("selection_face", camera.selection_face);
        prop_program.set_texture("breaking_image", breaking, GL_TEXTURE1);
        prop_program.set_int("is_breaking", (player.breaking == BLOCK_DEFAULT ? -1 : floor(2.4 * (glfwGetTime()-player.break_time) / BREAK_DURATION)));
        
        for (int cx = c.x-LOAD; cx <= c.x + LOAD; cx++) {
        for (int cz = c.y-LOAD; cz <= c.y + LOAD; cz++) {
            // Le chunk
            Chunk* chunk = world.get_chunk(cx,cz);

            // Prop display
            glBufferData(GL_ARRAY_BUFFER, chunk->prop_vertices.size() * sizeof(P_Vertex), chunk->prop_vertices.data(), GL_STATIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, chunk->prop_vertices.size());
        }}
        glEnable(GL_CULL_FACE);

        // -------- Non-world renders --------
        glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Crosshair (+world redraw)
        glBindVertexArray(VAO_UI);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_UI);
        glBufferData(GL_ARRAY_BUFFER, sizeof(full_quad_vertices), full_quad_vertices, GL_STATIC_DRAW);
    
        // uniforms
        crosshair_program.use();
        crosshair_program.set_texture("world_texture", world_texture, GL_TEXTURE0);
        crosshair_program.set_texture("crosshair", crosshair, GL_TEXTURE1);
        crosshair_program.set_texture("depth_texture", depth_texture, GL_TEXTURE2);
        crosshair_program.set_vec4("rect", vec4(.5-20.0f/Wf, .5-20.0f/Hf, 40.0f/Wf, 40.0f/Hf));
        crosshair_program.set_bool("underwater", player.underwater);
        crosshair_program.set_vec3("sky_color", SKY_COLOR);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Overlay
        glBindVertexArray(VAO_UI);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_UI);
        ui_program.use(); 
        overlay.draw(atlas, atlas_size, atlas_img_size);

        glEnable(GL_DEPTH_TEST);

        // -------- Des trucs --------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }}
};

int main() {
    Game game = Game();
    glfwTerminate();
    return 0;
}