#include "../../include/asset/asset.h"
#include "../../include/tower/actor.h"
#include "../../include/tower/tower.h"
#include "../../include/graphics.h"

namespace tower {


Actor::Actor(u_16 f, u_16 t, u_16 s) : Object(nullptr, 0, f, t, s), attr{30,10,1,1,1,1}, pp{30,10} {
    agent = simple_move_agent;
}

Actor::Actor(ActorData* d, asset::asset_package* p, u_16 f, u_16 t, u_16 s) : Object(d, p, f, t, s), pp{d->def_attr[MAX_HP], d->def_attr[MAX_SP]} {
    agent = d->agent;
    tint = d->tint;
    for (int i = 5; i >= 0; --i)
        attr[i] = d->def_attr[i];
    corpse_data = d->corpse_data;
}

void Actor::draw(int x, int y) {
    if (bmp) graphics::draw_tinted(bmp, tint, x, y, (face >= 2 ? ALLEGRO_FLIP_HORIZONTAL : 0));
}


Human::Human(u_16 f, u_16 t, u_16 s) : Actor(f, t, s) {
    item = new charstuff::Item*[6];
    for (int i = 5; i >= 0; --i)
        item[i] = nullptr;

    bmp = asset::get_tower(1);
    offset = 1;
}

void Human::draw(int x, int y) {
    Actor::draw(x, y);

    if (item[charstuff::HEAD])
        item[charstuff::HEAD]->draw(x+(face >= 2 ? 1 : -1), y-4, (face >= 2 ? ALLEGRO_FLIP_HORIZONTAL : 0));
    else
        graphics::draw(asset::get_tower(21), x+(face >= 2 ? 1 : -1), y-4, (face >= 2 ? ALLEGRO_FLIP_HORIZONTAL : 0));

    if (item[charstuff::BODY])
        item[charstuff::BODY]->draw(x+(face >= 2 ? 2 : 0), y+4, (face >= 2 ? ALLEGRO_FLIP_HORIZONTAL : 0));
    else {
        graphics::draw_tinted(asset::get_tower(10), al_map_rgb(207,196,184), x+(face >= 2 ? 2 : 0), y+2, (face >= 2 ? ALLEGRO_FLIP_HORIZONTAL : 0));
        graphics::draw_tinted(asset::get_tower(22), al_map_rgb(207,196,184), x+(face >= 2 ? 2 : 0), y+4, (face >= 2 ? ALLEGRO_FLIP_HORIZONTAL : 0));
    }
    // TODO draw equipment
}


ActorData::ActorData(u_16 b, u_16 o, Agent a, u_16 d_hp, u_16 d_sp, u_16 d_atk, u_16 d_def, u_16 d_spd, u_16 d_mag, ObjectData* cd) :
                        ObjectData(b, o), def_attr{ d_hp, d_sp, d_atk, d_def, d_spd, d_mag } {
    agent = a;
    corpse_data = cd;
}

Object* ActorData::gen_object(Tower* tw, u_16 f, u_16 t, u_16 s) {
    tw->active.emplace_back(0.0, new Actor(this, tw->assets, f, t, s));
    tw->floor[f]->objects.emplace_back(tw->active.back().second);
    return tw->floor[f]->objects.back().get();
}


}
