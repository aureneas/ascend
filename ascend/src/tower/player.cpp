#include <cmath>
#include <allegro5/allegro.h>
#include "../../include/tower/actor.h"
#include "../../include/asset/asset.h"
#include "../../include/charstuff/item_data.h"


namespace tower {

Agent player_agent;


Player* player_character = nullptr;

void init_player() {
    player_character = new Player();
}

void new_player() {
    //player_character->item[charstuff::HEAD] = new charstuff::Equip();
    charstuff::Equip* std_clothes = charstuff::get_std_clothes()->gen_item();
    player_character->insert(std_clothes);
    player_character->equip(std_clothes);
    player_character->insert(charstuff::get_op_armor()->gen_item());
}

Player* get_player() {
    return player_character;
}


Player::Player() : Human(0,0,1), upgr{10,18,13,12,11,12} {
    ap = 0;
    tint = al_map_rgb(142,90,62);
    agent = player_agent;

    // TODO let player expand inventory???
    delete[] item;
    size = 42;
    item = new charstuff::Item*[size];
    for (u_16 i = size; i > 0; --i)
        item[i-1] = nullptr;

    //attr[ATK] = 2;
    //attr[SPD] = 1000;
}

Player::~Player() {
    DEBUG_PRINT("deconstruct Player called.");

}

void Player::incr_attr(Attribute a) {
    if (upgr[a] <= ap) {
        ap -= upgr[a];
        switch (a) {
            case MAX_HP:
                if (attrp[a] < 9990) {
                    attrp[a] += 10;
                    upgr[a] = ceil(upgr[a]*(1+exp(-0.001*upgr[a]))); //floorl((2-(0.176656553*log10l(upgr[a])))*upgr[a]);
                } else {
                    attrp[a] = 9999;
                    upgr[a] = MAXED_ATTRIBUTE;
                }
                break;
            case MAX_SP:
                if (attrp[a] < 9995) {
                    attrp[a] += 5;
                    upgr[a] = ceil(upgr[a]*(1+exp(-0.001*upgr[a])));
                } else {
                    attrp[a] = 9999;
                    upgr[a] = MAXED_ATTRIBUTE;
                }
                break;
            default:
                if (attrp[a] < 998) {
                    attrp[a]++;
                    upgr[a] = ceil(upgr[a]*(1+exp(-0.001*upgr[a])));
                } else {
                    attrp[a] = 999;
                    upgr[a] = MAXED_ATTRIBUTE;
                }
        }
    }
}


}
