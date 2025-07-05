#version 330 core

out vec4 FragColor;

in vec3 frag_pos;
in vec3 normal;
in vec2 uv;
in vec2 uv01;
flat in int face;
flat in float lighting;
flat in ivec3 block;

uniform vec3 color;
uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 camera_pos;
uniform ivec3 selection;
uniform ivec3 selection_face;

uniform sampler2D atlas;
uniform vec2 atlas_size;

uniform sampler2D breaking_image;
uniform int is_breaking;


void main() {
    vec4 image = texture2D(atlas, uv);

    vec3 frag_color = image.rgb * lighting;

    // Selection
    if (block == selection) {
        frag_color = mix(vec3(0), frag_color, 0.5);
        // Breaking
        if (is_breaking >= 0) {
            vec4 brk_color = texture2D(breaking_image, vec2((is_breaking + uv01.x) / 3.0, uv01.y));
            if (brk_color.a == 1)
                frag_color = brk_color.rgb;
        }
    }

    // Water
    if (image.a == 0) discard;
    FragColor = vec4(frag_color, 1.0);
}