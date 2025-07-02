#version 330 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D world_texture;
uniform sampler2D crosshair;
uniform vec4 rect;

void main() {
    vec4 world_tex = texture2D(world_texture, uv);
    FragColor = world_tex;

    // Crosshair
    if ((rect.x <= uv.x) && (uv.x <= rect.x+rect.z) && (rect.y <= uv.y) && (uv.y <= rect.y+rect.w)) {
        vec4 crosshair_tex = texture2D(crosshair, (uv-rect.xy) / rect.zw);

        // Inversion des couleurs
        if (crosshair_tex.a == 1.0)
            FragColor = vec4(vec3(1.0) - world_tex.rgb * 0.5, 1.0);
        
        FragColor = texture2D(crosshair, (uv-rect.xy) / rect.zw);
    }
}
