#pragma once
#include "util.cpp"
#include "player.cpp"


class Overlay { public:
    Texture items, hotbar;
    Program* ui_program;
    vec2 hotbar_size;
    uint VAO_UI, VBO_UI;
    Player* player;

    Overlay() {}
    Overlay(Program* program, uint VAO_UI, uint VBO_UI, Player* player) :
        VAO_UI(VAO_UI), VBO_UI(VBO_UI), ui_program(program), player(player) {}

    void blit(Texture texture, vec4 rect, vec4 crop=vec4(0,0,1,1)) {
        // Mesh
        array<float, 24> vertices;
        vertices = quad_rect(rect[0], rect[1], rect[2], rect[3]);
        glBufferData(GL_ARRAY_BUFFER, 24*sizeof(float), vertices.data(), GL_STATIC_DRAW);

        ui_program->set_texture("image", texture, GL_TEXTURE0);
        ui_program->set_vec4("image_rect", crop);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void draw(Texture atlas, vec2 atlas_size, vec2 atlas_img_size) {
        // Inventory
        vec4 rect = vec4(Wf/2.f - hotbar_size.x * 3, Hf*.1f, hotbar_size.x * 6, hotbar_size.y * 6);
        blit(hotbar, rect);
        
        float pixel = rect[3] / 12.f;
        float slot_w = pixel * 8.f;
        for (int i=0; i<player->inventory.size(); i++) {
            auto [item,amount] = player->inventory[i];
            int face = BlockData[item].faces[0];
            vec4 crop = vec4(face % (int)atlas_size.x, atlas_size.y-1-floor(face/atlas_size.x), 1, 1); /// pixel size
            
            ui_program->set_bool("selection", i==player->selection);
            blit(atlas, vec4(rect.x + pixel*2.f + i*(slot_w+pixel), rect.y + pixel*2.f, slot_w, slot_w), vec4(crop.x*8.f/atlas_img_size.x, crop.y*8.f/atlas_img_size.y, crop[2]*8.f/atlas_img_size.x, crop[3]*8.f/atlas_img_size.y));
        }
    }
};