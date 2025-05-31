#version 330 core

out vec4 FragColor;

in vec2 uvs;
in vec3 frag_pos;
in vec3 normal;

uniform vec3 color;
uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 camera_pos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
}; 

struct DirLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 direction;
};

struct PointLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;
    vec3 direction;
    
    float constant;
    float linear;
    float quadratic;

    float cutoff;
    float outer_cutoff;
};

uniform Material material;
uniform DirLight dirlight;
uniform PointLight pointlights[4];
uniform SpotLight spotlight;


// PHONG
vec3 calc_ambient(vec3 light_ambient, vec3 diffuse_map, float attenuation=0.0) {
    return light_ambient * (diffuse_map * attenuation);
}

vec3 calc_diffuse(vec3 light_dir, vec3 light_diffuse, vec3 diffuse_map, float attenuation=0.0) {
    float diffuse_value = max(dot(light_dir, normal), 0.0);
    return light_diffuse * (diffuse_value * diffuse_map * attenuation);
}

vec3 calc_specular(vec3 light_dir, vec3 light_specular, float attenuation=0.0) {
    vec3 view_dir = normalize(camera_pos - frag_pos); // towards CAMERA
    vec3 reflect_dir = reflect(-light_dir, normal);

    vec3 specular_map = texture2D(material.specular, uvs).rgb;
    float specular_value = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    return light_specular * (specular_value * specular_map * attenuation);
}

// LIGHTS
vec3 calc_dirlight(DirLight light) {
    // Light properties
    vec3 light_dir = normalize(-light.direction);

    // Phong
    vec3 diffuse_map = texture2D(material.diffuse, uvs).rgb;
    vec3 ambient_color = calc_ambient(light.ambient, diffuse_map);
    vec3 diffuse_color = calc_diffuse(light_dir, light.diffuse, diffuse_map);
    vec3 specular_color = calc_specular(light_dir, light.specular);

    return ambient_color + diffuse_color + specular_color;
}

vec3 calc_pointlight(PointLight light) {
    // Light properties
    vec3 light_dir = normalize(light.position - frag_pos); // towards LIGHT
    float light_dist = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * light_dist + light.quadratic * pow(light_dist,2));   

    // Phong
    vec3 diffuse_map = texture2D(material.diffuse, uvs).rgb;
    vec3 ambient_color = calc_ambient(light.ambient, diffuse_map, attenuation);
    vec3 diffuse_color = calc_diffuse(light_dir, light.diffuse, diffuse_map, attenuation);
    vec3 specular_color = calc_specular(light_dir, light.specular, attenuation);

    return ambient_color + diffuse_color + specular_color;
}

vec3 calc_spotlight(SpotLight light) {
    // Light properties
    vec3 light_dir = normalize(light.position - frag_pos); // towards LIGHT
    float light_dist = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * light_dist + light.quadratic * pow(light_dist,2));   

    // Circle edge
    float theta = dot(light_dir, normalize(-light.direction));
    float percent = clamp((theta - light.outer_cutoff) / (light.cutoff - light.outer_cutoff), 0.0, 1.0); 
    attenuation *= percent;

    // Ambient
    vec3 diffuse_map = texture2D(material.diffuse, uvs).rgb;
    vec3 ambient_color = calc_ambient(light.ambient, diffuse_map, attenuation);

    // Visibility cone
    if (theta < light.outer_cutoff) return ambient_color;

    // Phong
    vec3 diffuse_color = calc_diffuse(light_dir, light.diffuse, diffuse_map, attenuation);
    vec3 specular_color = calc_specular(light_dir, light.specular, attenuation);

    return ambient_color + diffuse_color + specular_color;
}


void main() {
    vec3 color;
    color = calc_dirlight(dirlight);
    for (int i=0;i<4;i++) {
        color += calc_pointlight(pointlights[i]);
    }
    color += calc_spotlight(spotlight);

    FragColor = vec4(color, 1.0);
}