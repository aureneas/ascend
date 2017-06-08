#include "../../include/charstuff/item.h"
#include "../../include/graphics.h"

namespace charstuff {


Item::Item(ALLEGRO_BITMAP* b, ALLEGRO_COLOR t) {
    bmp = b;
    tint = t;
}

void Item::draw(int x, int y, int flags) {
    graphics::draw_tinted(bmp, tint, x, y, flags);
}

Equip::Equip(ALLEGRO_BITMAP* b, ALLEGRO_COLOR t, EQUIPMENT_SLOT s) : Item(b,t), attr_m() {
    slot = s;
}

void Equip::draw(int x, int y, int flags) {
    Item::draw(x, y, flags);
    for (std::forward_list<Feature>::iterator it = ftr.begin(); it != ftr.end(); ++it)
        graphics::draw_tinted(it->bmp, it->tint, x, y, flags);
}


void Inventory::insert(Item* it) {
    if (it) {
        for (u_16 i = size; i > 0; --i) {
            if (!item[size - i]) {
                item[size - i] = it;
                *(it->parent) = nullptr;
                it->parent = item + size - i;
                return;
            }
        }
    }
}

void Inventory::insert(Item* it, u_16 index) {
    if (it) {
        Item* temp = item[index];
        item[index] = it;
        *(it->parent) = temp;
        if (temp) temp->parent = it->parent;
        it->parent = item + index;
    }
}

void Inventory::expand(u_16 expand_by) {
    Item** temp = (Item**)realloc(item, sizeof(Item*) * (size + expand_by));
    if (!temp)
        throw "Unable to expand Inventory.";

    for (int i = expand_by-1; i >= 0; --i)
        temp[size+i] = nullptr;
    item = temp;
    size += expand_by;
}

}
