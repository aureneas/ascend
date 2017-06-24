#include "../../include/graphics.h"
#include "../../include/engine/state.h"

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

AnimationListPair Object::interact(Actor* a, Tower* t) {
    return AnimationListPair();
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

Portal::Portal(ObjectData* od, asset::asset_package* ap, Direction fc, Position o, u_16 f, u_16 t, u_16 s) : Object(od, ap, f, t, s) {
    face = fc;
    out = o;
}

AnimationListPair Portal::interact(Actor* a, Tower* t) {
    AnimationListPair anim;

    if (a == get_player()) {
        Player* p = get_player();
        engine::TowerState* ts = dynamic_cast<engine::TowerState*>(engine::get_state());
        Point d = Point{ 18 * (int)(out.tile % t->floor[out.floor]->size) + 9, 18 * (int)(out.tile / t->floor[out.floor]->size) + 9 } - p->pos;

        if (t->floor[out.floor]->tile[out.tile]->occupy != nullptr)
            t->floor[out.floor]->remove(t->floor[out.floor]->tile[out.tile]->occupy);
        // TODO replace above with relocation???

        anim.first.push_front(new engine::CameraAnimation(engine::animation_camlinmove, 72, d, ts));
        anim.first.push_front(new engine::MoveAnimation(engine::animation_linmove, 14, Point{ d.x / 5, d.y / 5 }, &p->pos));
        anim.first.push_front(new engine::DelayMoveAnimation(engine::animation_linmove, 36, 37, Point{ d.x - 2*(d.x / 5), d.y - 2*(d.y / 5) }, &p->pos));
        anim.first.push_front(new engine::DelayMoveAnimation(engine::animation_linmove, 58, 72, Point{ d.x / 5, d.y / 5 }, &p->pos));
        anim.second.push_front(new engine::FadeAnimation(graphics::get_screen_tint(), al_map_rgba(0, 0, 0, 255), 14));
        anim.second.push_front(new engine::DelayFadeAnimation(graphics::get_screen_tint(), al_map_rgba(0, 0, 0, 0), 58, 72));
        anim.second.push_front(new engine::ArithmeticAnimation(engine::animation_addition, 36, &p->floor, (int)out.floor - (int)p->floor));
        anim.second.push_front(new engine::FunctionAnimation2<Floor, std::shared_ptr<Object> >(Floor::insert_ptr, 36, t->floor[out.floor], t->floor[p->floor]->get_object(p)));
        anim.second.push_front(new engine::FunctionAnimation2<Floor, Object>(Floor::remove, 36, t->floor[p->floor], p));
        anim.second.push_front(new engine::FunctionAnimation<engine::TowerState>(engine::TowerState::reset_twidget, 36, ts));
    } else {
        // TODO
    }
    return anim;
}

void Portal::draw(int x, int y) {
    if (face == NORTH || face == WEST) {
        if (bmp) {
            graphics::draw(bmp, x, y + 23, ((face % 2 == 0) ? ALLEGRO_FLIP_HORIZONTAL : 0));
            graphics::draw(asset::get_tower(35), x, y + 23, ((face % 2 == 0) ? ALLEGRO_FLIP_HORIZONTAL : 0));
        }
    } else {
        if (bmp) graphics::draw(bmp, x, y, ((face % 2 == 0) ? ALLEGRO_FLIP_HORIZONTAL : 0));
    }
}


Container::Container(ObjectData* d, asset::asset_package* p, u_16 f, u_16 t, u_16 s) : Object(d, p, f, t, s), charstuff::Inventory(18) {}

AnimationListPair Container::interact(Actor* a, Tower* t) {
    if (engine::TowerState* ts = dynamic_cast<engine::TowerState*>(engine::get_state()))
        ts->open_container(this);
    return AnimationListPair();
}


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
