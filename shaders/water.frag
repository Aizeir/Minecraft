#version 330 core

out vec4 FragColor;

in vec3 frag_pos;
in vec3 normal;
in vec2 uv;
flat in int face;
flat in float lighting;
flat in ivec3 block;

uniform vec3 color;
uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 camera_pos;

uniform sampler2D atlas;
uniform vec2 atlas_size;

void main() {
    vec4 image = texture2D(atlas, uv);
    vec3 frag_color = image.rgb * lighting;
    FragColor = vec4(frag_color, .5);
}