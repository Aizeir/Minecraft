#version 330 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D image;
uniform vec4 image_rect;

uniform bool selection;

void main() {
    vec4 image_tex = texture2D(image, image_rect.xy + uv * image_rect.zw);
    if (selection) image_tex.rgb += vec3(.1,.1,.1);
    FragColor = vec4(image_tex.rgb, 1.0);
}
