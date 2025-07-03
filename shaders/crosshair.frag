#version 330 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D world_texture;
uniform sampler2D crosshair;
uniform sampler2D depth_texture;
uniform vec4 rect;
uniform bool underwater;
uniform vec3 sky_color;

// Fog
vec3 waterfog_color = vec3(.1, .2, .5);

void main() {
    vec4 world_tex = texture2D(world_texture, uv);

    // Fog
    if (underwater) {
        float depth = texture2D(depth_texture, uv).r;
        world_tex.rgb = mix(waterfog_color, world_tex.rgb, pow(1-pow(depth,4), 0.5));
    }
    else {
        float depth = pow(texture2D(depth_texture, uv).r, 2);
        // Adjust these values to control fog start and end distances
        float fog_near = 0.9999;
        float fog_far = 1.0;
        float fogFactor = clamp((fog_far - depth) / (fog_far - fog_near), 0.0, 1.0);
        world_tex.rgb = mix(sky_color, world_tex.rgb, fogFactor);
    }

    // Crosshair
    FragColor = world_tex;
    if ((rect.x <= uv.x) && (uv.x <= rect.x+rect.z) && (rect.y <= uv.y) && (uv.y <= rect.y+rect.w)) {
        vec4 crosshair_tex = texture2D(crosshair, (uv-rect.xy) / rect.zw);

        // Inversion des couleurs
        if (crosshair_tex.a == 1.0)
            FragColor = vec4(vec3(1.0) - world_tex.rgb * 0.5, 1.0);
        
        //FragColor = texture2D(crosshair, (uv-rect.xy) / rect.zw);
    }
}
