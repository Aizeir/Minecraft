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
uniform Material material;

struct Light {
    vec4 vector;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
}; 
uniform Light light;

void main() {
    // Light properties
    vec3 light_dir; float attenuation;
    if (light.vector.w == 0.0) {
        light_dir = normalize(-light.vector.xyz);
    }
    else {
        vec3 light_pos = light.vector.xyz;
        light_dir = normalize(light_pos - frag_pos); // towards LIGHT
        
        float light_dist = length(light_pos - frag_pos);
        attenuation = 1.0 / (light.constant + light.linear * light_dist + light.quadratic * pow(light_dist,2));   
    }

    // Ambient
    vec3 diffuse_map = texture2D(material.diffuse, uvs).rgb;
    vec3 ambient_color = light.ambient * (diffuse_map * attenuation);

    // Diffuse
    float diffuse_value = max(dot(light_dir, normal), 0.0);
    vec3 diffuse_color = light.diffuse * (diffuse_value * diffuse_map * attenuation);
    
    // Specular
    vec3 view_dir = normalize(camera_pos - frag_pos); // towards CAMERA
    vec3 reflect_dir = reflect(-light_dir, normal);

    vec3 specular_map = texture2D(material.specular, uvs).rgb;
    float specular_value = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular_color = light.specular * (specular_value * specular_map * attenuation);
    
    //vec3 emission = texture2D(material.emission, uvs).rgb;

    FragColor = vec4(ambient_color + diffuse_color + specular_color, 1.0);
}