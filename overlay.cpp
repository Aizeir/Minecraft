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

    void draw(Texture atlas) {
        // Inventory
        vec4 rect = vec4(Wf/2.f - hotbar_size.x * 3, Hf*.1f, hotbar_size.x * 6, hotbar_size.y * 6);
        blit(hotbar, rect);
        
        float slot_w = rect.w / (float)MAX_INV;
        for (int i=0; i<player->inventory.size(); i++) {
            auto [item,amount] = player->inventory[i];
            blit(atlas, vec4(rect.x + i*slot_w, rect.y, slot_w, rect[3]), vec4(0,0,.25,.25));
        }
    }
};