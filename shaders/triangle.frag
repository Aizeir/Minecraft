#version 330 core

in vec2 tex_coord;
out vec4 FragColor;
uniform sampler2D image;
uniform sampler2D image2;

void main() {
    FragColor = mix(texture2D(image, tex_coord), texture2D(image2, tex_coord), .2);
}