#ifndef AC_ITEM_H
#define AC_ITEM_H

#include <forward_list>
#include <allegro5/allegro.h>
#include "../types.h"

namespace charstuff {


struct Item {
    Item** parent;

    ALLEGRO_BITMAP* bmp;
    ALLEGRO_COLOR tint;
    // stuff

    Item(ALLEGRO_BITMAP*, ALLEGRO_COLOR);
    virtual void draw(int, int, int);
};


enum EQUIPMENT_SLOT {
    WEAPON_MELEE = 0,
    WEAPON_RANGED = 1,
    HEAD = 2,
    BODY = 3,
    ACCESSORY = 4,
    TAROT = 5
};

struct Feature {
    ALLEGRO_BITMAP* bmp;
    ALLEGRO_COLOR tint;
};

struct Equip: public Item {
    EQUIPMENT_SLOT slot;
    int attr_m[6];
    std::forward_list<Feature> ftr;

    Equip(ALLEGRO_BITMAP*, ALLEGRO_COLOR, EQUIPMENT_SLOT);
    void draw(int, int, int);
};


struct Inventory {
    u_16 size;
    Item** item;

    void insert(Item*);
    void insert(Item*, u_16);
    void expand(u_16);
};


}

#endif // AC_ITEM_H
