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
    vec3 specular;
    float shininess;
}; 
uniform Material material;

struct Light {
    vec3 pos;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 
uniform Light light;

void main() {
    vec3 diffuse = vec3(texture2D(material.diffuse, uvs));
    vec3 ambient_color = light.ambient * diffuse;

    vec3 light_dir = normalize(light.pos - frag_pos); // towards LIGHT

    float diffuse_value = max(dot(light_dir, normal), 0.0);
    vec3 diffuse_color = light.diffuse * (diffuse_value * diffuse);

    vec3 view_dir = normalize(camera_pos - frag_pos); // towards CAMERA
    vec3 tangent = cross(normal, cross(light_dir, normal));
    //vec3 reflect_dir = dot(light_dir, normal) * normal - dot(light_dir, tangent) * tangent;
    vec3 reflect_dir = reflect(-light_dir, normal);
    
    float specular_value = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular_color = light.specular * (specular_value * material.specular);
    
    FragColor = vec4(ambient_color + diffuse_color + specular_color, 1.0);
}