#ifndef AT_OBJECT_H
#define AT_OBJECT_H

#include <allegro5/allegro.h>
#include "../asset/asset.h"
#include "../point.h"
#include "../types.h"
#include "position.h"
#include "obj_data.h"

namespace tower {


struct Object {
    ALLEGRO_BITMAP* bmp;
    u_16 offset;

    Point pos;
    u_16 floor;
    Direction face;

    Object(ALLEGRO_BITMAP*, int, u_16, u_16, u_16);
    Object(ObjectData*, asset::asset_package*, u_16, u_16, u_16);
    virtual void draw(int, int);
};

struct Portal: public Object {
    Portal(Direction, u_16, u_16, u_16);
};


}

#endif // AT_OBJECT_H
