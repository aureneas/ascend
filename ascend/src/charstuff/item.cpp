#include "../../include/charstuff/item.h"
#include "../../include/graphics.h"
#include "../../include/tower/actor.h"
#include "../../include/_debug.h"

namespace charstuff {


Item::Item(ALLEGRO_BITMAP* b, ALLEGRO_COLOR t) {
    bmp = b;
    tint = t;
    parent = nullptr;
}

Item::~Item() {
    DEBUG_PRINT("Item deconstruction.");
}

void Item::draw(int x, int y, int flags) {
    graphics::draw_tinted(bmp, tint, x, y, flags);
}

Equip::Equip(ALLEGRO_BITMAP* b, ALLEGRO_COLOR t, EQUIPMENT_SLOT s) : Item(b,t), attrm{0} {
    slot = s;
    equipped = false;
}

Equip::~Equip() {
    DEBUG_PRINT("Equip deconstruction.");
}

void Equip::interact() {
    tower::get_player()->toggle_equip(this);
}

void Equip::draw(int x, int y, int flags) {
    Item::draw(x, y, flags);
    for (u_16 f = 0; f < ftr_num; ++f) {
        //DEBUG_PRINT("FEATURE : " << ftr[f].bmp);
        graphics::draw_tinted(ftr[f].bmp, ftr[f].tint, x, y, flags);
    }
}


Inventory::Inventory(u_16 s) {
    size = s;
    item = new Item*[size];
    for (u_16 i = s; i > 0; --i)
        item[i-1] = nullptr;
}

Inventory::~Inventory() {
    for (int i = size-1; i >= 0; --i)
        delete item[i];
    delete[] item;
}

bool Inventory::check(Item** pptr) {
    return ((uintptr_t)pptr >= (uintptr_t)item && (uintptr_t)pptr < (uintptr_t)(item + size));
}

Item** Inventory::get(u_16 index) {
    return ((index >= size) ? nullptr : &item[index]);
}

void Inventory::insert(Item* it) {
    if (it) {
        for (u_16 i = 0; i < size; ++i) {
            if (!item[i]) {
                item[i] = it;
                if (it->parent) *(it->parent) = nullptr;
                it->parent = item + i;
                return;
            }
        }
    }
}

void Inventory::insert(Item* it, u_16 index) {
    if (it) {
        Item* temp = item[index];
        item[index] = it;
        if (it->parent)  *(it->parent) = temp;
        if (temp)        temp->parent = it->parent;
        it->parent = item + index;
    }
}

/**
 *  Transfers all items FROM this TO inv
 */
void Inventory::transfer(Inventory* inv) {
    for (u_16 i = size; i > 0; --i) {
        if (item[i - 1])
            inv->insert(item[i - 1], i - 1);
    }
}

}
