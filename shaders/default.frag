#version 330 core

out vec4 FragColor;

in vec3 frag_pos;
in vec3 normal;

uniform vec3 color;
uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 camera_pos;

void main() {
    float ambient_strength = 0.1;
    vec3 ambiant = light_color * ambient_strength;

    vec3 light_dir = normalize(light_pos - frag_pos); // towards LIGHT

    float diffuse_value = max(dot(light_dir, normal), 0.0);
    vec3 diffuse = light_color * diffuse_value;

    vec3 view_dir = normalize(camera_pos - frag_pos); // towards CAMERA
    vec3 tangent = cross(normal, cross(light_dir, normal));
    //vec3 reflection = dot(light_dir, normal) * normal - dot(light_dir, tangent) * tangent;
    vec3 reflect_dir = reflect(-light_dir, normal);
    
    float specular_strength = 0.5;
    float specular_value = pow(max(dot(view_dir, reflect_dir), 0.0), 32) * specular_strength;
    vec3 specular = light_color * specular_value;
    
    FragColor = vec4((ambiant + diffuse + specular) * color, 1.0);
}