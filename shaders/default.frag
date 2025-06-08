#version 330 core

out vec4 FragColor;

in vec2 uvs;
in vec3 frag_pos;
in vec3 normal;

uniform vec3 color;
uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 camera_pos;

struct Block {
    int id;
    float shininess;
}; 

struct DirLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 direction;
};

uniform Block block;
uniform DirLight dirlight;
uniform sampler2D atlas;
uniform vec2 atlas_size;
uniform int hover;

// PHONG
vec3 calc_ambient(vec3 light_ambient, vec3 image, float attenuation=1.0) {
    return light_ambient * (image * attenuation);
}

vec3 calc_diffuse(vec3 light_dir, vec3 light_diffuse, vec3 image, float attenuation=1.0) {
    float diffuse_value = max(dot(light_dir, normal), 0.0);
    return light_diffuse * (diffuse_value * image * attenuation);
}

vec3 calc_specular(vec3 light_dir, vec3 light_specular, vec3 image, float attenuation=1.0) {
    vec3 view_dir = normalize(camera_pos - frag_pos); // towards CAMERA
    vec3 halfway_dir = normalize(light_dir + view_dir);

    vec3 specular_map = image;
    float specular_value = max(pow(dot(normal, halfway_dir), block.shininess), 0.0);
    return light_specular * (specular_value * image * attenuation);
}

// LIGHTS
vec3 calc_dirlight(DirLight light, vec3 image) {
    // Light properties
    vec3 light_dir = normalize(-light.direction);

    // Phong
    vec3 ambient_color = calc_ambient(light.ambient, image);
    vec3 diffuse_color = calc_diffuse(light_dir, light.diffuse, image);
    vec3 specular_color = calc_specular(light_dir, light.specular, image);

    return ambient_color + diffuse_color + specular_color;
}

void main() {
    vec4 image = texture2D(atlas, uvs);

    vec3 frag_color;
    frag_color = calc_dirlight(dirlight, image.rgb);

    if (hover == 1) {
        frag_color = vec3(0,0,0);
    }

    FragColor = vec4(frag_color, 1.0);
}