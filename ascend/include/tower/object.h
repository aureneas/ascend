#ifndef AT_OBJECT_H
#define AT_OBJECT_H

#include <allegro5/allegro.h>
#include "../asset/asset.h"
#include "../point.h"
#include "../types.h"
#include "position.h"
#include "obj_data.h"
#include "../charstuff/item.h"

namespace tower {

struct Actor;

struct Object {
    ALLEGRO_BITMAP* bmp;
    int offset;

    Point pos;
    u_16 floor;
    Direction face;

    Object(ALLEGRO_BITMAP*, int, u_16, u_16, u_16);
    Object(ObjectData*, asset::asset_package*, u_16, u_16, u_16);
    virtual AnimationListPair interact(Actor*, Tower*);
    virtual void draw(int, int);
};

struct Wall: public Object {
    Wall(ObjectData*, asset::asset_package*, u_16, u_16, u_16);
    void draw(int, int);
};

struct Portal: public Object {
    Position out;

    Portal(Direction, u_16, u_16, u_16);
    Portal(ObjectData*, asset::asset_package*, Direction, Position, u_16, u_16, u_16);
    AnimationListPair interact(Actor*, Tower*);
    void draw(int, int);
};

struct Container: public Object, public charstuff::Inventory {
    Container(ObjectData*, asset::asset_package*, u_16, u_16, u_16);
    AnimationListPair interact(Actor*, Tower*);
};


}

#endif // AT_OBJECT_H
