#version 330 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D image;
uniform vec4 image_rect;

void main() {
    vec4 image_tex = texture2D(image, image_rect.xy + uv * image_rect.zw);
    if (image_tex.a == 1.0) FragColor = image_tex;
}
