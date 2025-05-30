#version 330 core

in vec3 color;
out vec4 FragColor;
uniform float time;

void main() {
    FragColor = vec4(color * (sin(time*5)+1.)/2., 1.0);
}