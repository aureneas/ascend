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

    ALLEGRO_USTR* name;
    ALLEGRO_USTR* desc;

    Item(ALLEGRO_BITMAP*, ALLEGRO_COLOR);
    virtual ~Item();
    virtual void interact() {}
    virtual void draw(int, int, int);
};


enum EQUIPMENT_SLOT {
    WEAPON_PRIMARY = 0,
    WEAPON_SECONDARY = 1,
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
    bool equipped;
    u_16 attrm[6];

    Feature* ftr;
    u_16 ftr_num;

    Equip(ALLEGRO_BITMAP*, ALLEGRO_COLOR, EQUIPMENT_SLOT);
    ~Equip();
    void interact();
    void draw(int, int, int);
};


struct Inventory {
    u_16 size;
    Item** item;

    Inventory(u_16);
    virtual ~Inventory();
    bool check(Item**);
    Item** get(u_16);
    void insert(Item*);
    void insert(Item*, u_16);
};


}

#endif // AC_ITEM_H
