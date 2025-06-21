#version 330 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D world_texture;

uniform sampler2D crosshair;
uniform vec4 crosshair_rect;

vec2 flip_y(vec2 xy) {return vec2(xy.x, 1.0 - xy.y);}
vec2 rect_pos(vec4 rect) {return flip_y((flip_y(uv) - rect.xy) / rect.zw);}

void main() {
    vec4 crosshair_tex = texture2D(crosshair, rect_pos(crosshair_rect));
    vec4 world_tex = texture2D(world_texture, uv);

    FragColor = world_tex;
    if (crosshair_tex.a == 1.0) {
        FragColor = vec4(vec3(1.0) - world_tex.rgb * 0.5, 1.0);
    }
}
