#include "../../include/tower/object.h"
#include "../../include/asset/asset.h"
#include "../../include/graphics.h"
#include "../../include/tower/tower.h"

namespace tower {


Object::Object(ALLEGRO_BITMAP* b, int o, u_16 f, u_16 t, u_16 s) {
    bmp = b;
    offset = o;
    pos.x = (int)18*(t%s) + 9;
    pos.y = (int)18*(t/s) + 9;
    face = EAST;
    floor = f;
}

Object::Object(ObjectData* d, asset::asset_package* p, u_16 f, u_16 t, u_16 s) {
    bmp = p->get(d->bmp);
    offset = d->offset;
    pos.x = (int)18*(t%s) + 9;
    pos.y = (int)18*(t/s) + 9;
    face = EAST;
    floor = f;
}

void Object::draw(int x, int y) {
    if (bmp) graphics::draw(bmp, x, y, (face >= 2 ? ALLEGRO_FLIP_HORIZONTAL : 0));
}


Wall::Wall(ObjectData* d, asset::asset_package* p, u_16 f, u_16 t, u_16 s) : Object(d, p, f, t, s) {}

void Wall::draw(int x, int y) {
    Object::draw(x, y);
}


Portal::Portal(Direction fc, u_16 f, u_16 t, u_16 s) : Object(asset::get_tower(4), 9, f, t, s) {
    face = fc;
}


Container::Container(ObjectData* d, asset::asset_package* p, u_16 f, u_16 t, u_16 s) : Object(d, p, f, t, s), charstuff::Inventory(18) {}


ObjectData::ObjectData(u_16 b, u_16 o) {
    bmp = b;
    offset = o;
}

Object* ObjectData::gen_object(asset::asset_package* p, u_16 f, u_16 t, u_16 s) {
    return new Object(this, p, f, t, s);
}

Object* ObjectData::gen_object(Tower* tw, u_16 f, u_16 t, u_16 s) {
    tw->floor[f]->objects.emplace_back(new Object(this, tw->assets, f, t, s));
    return tw->floor[f]->objects.back().get();
}


}
