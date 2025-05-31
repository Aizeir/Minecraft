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

    // Paramétrage de toutes les textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    stbi_set_flip_vertically_on_load(true);

    return window;
}

int main() {
    // Fenêtre
    GLFWwindow* window = setup();

    // Shaders
    Program program("shaders/default.vert", "shaders/default.frag");
    Program light_shader("shaders/default.vert", "shaders/light.frag");

    // Textures
    unsigned int container = load_texture("assets/container2.png", GL_RGBA); uint container_unit = 0;
    unsigned int container_spec = load_texture("assets/container2_specular.png", GL_RGBA); uint cont_spec_unit = 1;
    unsigned int matrix = load_texture("assets/matrix.jpg", GL_RGB); uint mat_unit = 2;
    program.use();

    // Objects
    unsigned int VBO, EBO = cube_buffers();
    unsigned int VAO = cube_vao_with_attribs(VBO, EBO);
    unsigned int lightVAO = cube_vao_with_attribs(VBO, EBO);

    // Cubes
    vec3 cube_positions[] = {
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
    vec3 light_pos(1.2f, 1.0f, 2.0f);

    mat4 projection = glm::perspective(rad(60.0f), (float)W / (float)H, 0.1f, 100.0f);

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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Cube
        glBindVertexArray(VAO);
        program.use();
        program.set_vec3("camera_pos", camera.pos);

        bind_texture(container, container_unit);
        bind_texture(container_spec, cont_spec_unit);
        bind_texture(matrix, mat_unit);
        program.set_int("material.diffuse", container_unit);
        program.set_int("material.specular", cont_spec_unit);
        program.set_float("material.shininess", 32.0f);

        //program.set_vec4("light.vector", vec4(-0.2f, -1.0f, -0.3f, 0.0f));
        program.set_vec4("light.vector", vec4(light_pos, 1.0f));
        program.set_vec3("light.ambient",  vec3(0.2f, 0.2f, 0.2f));
        program.set_vec3("light.diffuse",  vec3(0.5f, 0.5f, 0.5f)); // darken diffuse light a bit
        program.set_vec3("light.specular", vec3(1.0f, 1.0f, 1.0f)); 
        program.set_float("light.constant",  1.0f);
        program.set_float("light.linear",    0.09f);
        program.set_float("light.quadratic", 0.032f);	
        
        program.set_mat4("transform", projection * camera.view);

        mat4 model;
        for (unsigned int i = 0; i < 10; i++) {
            model = mat4(1.0f);
            model = glm::translate(model, cube_positions[i]);
            model = glm::rotate(model, rad(20.0f*i), glm::vec3(1.0f, 0.3f, 0.5f));
                
            program.set_mat4("model", model);
            program.set_mat3("normal_mat", glm::mat3(glm::transpose(glm::inverse(model))));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
  
        // Light
        glBindVertexArray(lightVAO);
        light_shader.use();
        light_shader.set_vec3("light_color", vec3(1.0f, 1.0f, 1.0f));
        
        model = mat4(1.0f);
        model = glm::translate(model, light_pos);
        model = glm::scale(model, vec3(0.2f));
        light_shader.set_mat4("model", model);
        light_shader.set_mat4("transform", projection * camera.view);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // des trucs
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Quit
    glfwTerminate();
    return 0;
}
