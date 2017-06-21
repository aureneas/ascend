#include "../../include/asset/asset.h"
#include "../../include/tower/actor.h"
#include "../../include/tower/tower.h"
#include "../../include/graphics.h"

namespace tower {


Actor::Actor(u_16 f, u_16 t, u_16 s) : Object(nullptr, 0, f, t, s), charstuff::Inventory(18), attrp{30,10,1,1,1,1}, pp{30,10} {
    agent = simple_move_agent;
    evaluate_mods();
}

Actor::Actor(ActorData* d, asset::asset_package* p, u_16 f, u_16 t, u_16 s) : Object(d, p, f, t, s), charstuff::Inventory(18), pp{d->def_attr[MAX_HP], d->def_attr[MAX_SP]} {
    agent = d->agent;
    tint = d->tint;
    for (int i = 5; i >= 0; --i)
        attrp[i] = d->def_attr[i];
    corpse_data = d->corpse_data;
    evaluate_mods();
}

void Actor::evaluate_mods() {
    for (int i = 5; i >= 0; --i) {
        attr[i] = attrp[i];
        mods[i] = 0;
    }
}

void Actor::draw(int x, int y) {
    if (bmp) graphics::draw_tinted(bmp, tint, x, y, (face >= 2 ? ALLEGRO_FLIP_HORIZONTAL : 0));
}


Human::Human(u_16 f, u_16 t, u_16 s) : Actor(f, t, s), equips{nullptr} {
    bmp = asset::get_tower(1);
    offset = 1;
    for (int i = 3; i >= 0; --i)
        attrp[2 + i] = 10;
    evaluate_mods();
}

void Human::abs_equip(charstuff::Equip* e, charstuff::EQUIPMENT_SLOT slot) {
    if (equips[slot]) equips[slot]->equipped = false;
    equips[slot] = e;
    e->equipped = true;
}

void Human::evaluate_mods() {
    Actor::evaluate_mods();
    for (int i = 5; i >= 0; --i) {
        if (equips[i]) {
            for (int j = 5; j >= 0; --j)
                mods[j] += equips[i]->attrm[j];
        }
    }
    for (int i = 5; i >= 0; --i) {
        if (mods[i] + (int)attrp[i] > 0)
            attr[i] = attrp[i] + mods[i];
        else
            attr[i] = 1;
    }
}

void Human::equip(charstuff::Equip* e) {
    if (e->slot == charstuff::WEAPON_PRIMARY) {
        abs_equip(e, charstuff::WEAPON_PRIMARY);
        if (equips[charstuff::WEAPON_SECONDARY]) {
            equips[charstuff::WEAPON_SECONDARY]->equipped = false;
            equips[charstuff::WEAPON_SECONDARY] = nullptr;
        }
    } else if (e->slot == charstuff::WEAPON_SECONDARY) {
        if (!equips[charstuff::WEAPON_PRIMARY])
            abs_equip(e, charstuff::WEAPON_PRIMARY);
        else {
            if (equips[charstuff::WEAPON_PRIMARY]->slot == charstuff::WEAPON_PRIMARY)
                abs_equip(e, charstuff::WEAPON_PRIMARY);
            else if (!equips[charstuff::WEAPON_SECONDARY]) {
                if (equips[charstuff::WEAPON_PRIMARY]->attrm[ATK] > e->attrm[ATK])
                    abs_equip(e, charstuff::WEAPON_SECONDARY);
                else {
                    equips[charstuff::WEAPON_SECONDARY] = equips[charstuff::WEAPON_PRIMARY];
                    equips[charstuff::WEAPON_PRIMARY] = e;
                    e->equipped = true;
                }
            } else {
                abs_equip(e, ((equips[charstuff::WEAPON_SECONDARY]->attrm[ATK] > e->attrm[ATK]) ?
                              charstuff::WEAPON_SECONDARY : charstuff::WEAPON_PRIMARY));
            }
        }
    } else {
        abs_equip(e, e->slot);
    }
    evaluate_mods();
}

void Human::dequip(charstuff::EQUIPMENT_SLOT slot) {
    equips[slot]->equipped = false;
    equips[slot] = nullptr;
    evaluate_mods();
}

void Human::toggle_equip(charstuff::Equip* e) {
    if (e->slot == charstuff::WEAPON_SECONDARY) {
        if (equips[charstuff::WEAPON_PRIMARY] == e)
            dequip(charstuff::WEAPON_PRIMARY);
        else if (equips[charstuff::WEAPON_SECONDARY] == e)
            dequip(charstuff::WEAPON_SECONDARY);
        else
            equip(e);
    } else {
        if (equips[e->slot] == e)
            dequip(e->slot);
        else
            equip(e);
    }
}

int* Human::get_replace(charstuff::Equip* e) {
    int* m = new int[6];
    for (int i = 5; i >= 0; --i)
        m[i] = 0;

    if (e->slot == charstuff::WEAPON_PRIMARY) {
        if (equips[charstuff::WEAPON_PRIMARY]) {
            for (int i = 5; i >= 0; --i)
                m[i] += equips[charstuff::WEAPON_PRIMARY]->attrm[i];
        }
        if (equips[charstuff::WEAPON_SECONDARY]) {
            for (int i = 5; i >= 0; --i)
                m[i] += equips[charstuff::WEAPON_SECONDARY]->attrm[i];
        }
    } else if (e->slot == charstuff::WEAPON_SECONDARY) {
        if (!equips[charstuff::WEAPON_SECONDARY]) {
            if (equips[charstuff::WEAPON_PRIMARY]) {
                if (equips[charstuff::WEAPON_PRIMARY]->slot == charstuff::WEAPON_PRIMARY) {
                    for (int i = 5; i >= 0; --i)
                        m[i] += equips[charstuff::WEAPON_PRIMARY]->attrm[i];
                }
            }
        } else {
            if (equips[charstuff::WEAPON_SECONDARY]->attrm[ATK] < e->attrm[ATK]) {
                for (int i = 5; i >= 0; --i)
                    m[i] += equips[charstuff::WEAPON_PRIMARY]->attrm[i];
            } else {
                for (int i = 5; i >= 0; --i)
                    m[i] += equips[charstuff::WEAPON_SECONDARY]->attrm[i];
            }
        }
    } else {
        if (equips[e->slot]) {
            for (int i = 5; i >= 0; --i)
                m[i] += equips[e->slot]->attrm[i];
        }
    }

    return m;
}

void Human::draw(int x, int y) {
    Actor::draw(x, y);

    if (equips[charstuff::HEAD])
        equips[charstuff::HEAD]->draw(x+(face >= 2 ? 1 : -1), y-4, (face >= 2 ? ALLEGRO_FLIP_HORIZONTAL : 0));
    else
        graphics::draw(asset::get_tower(21), x+(face >= 2 ? 1 : -1), y-4, (face >= 2 ? ALLEGRO_FLIP_HORIZONTAL : 0));

    if (equips[charstuff::BODY])
        equips[charstuff::BODY]->draw(x+(face >= 2 ? 2 : 0), y+4, (face >= 2 ? ALLEGRO_FLIP_HORIZONTAL : 0));
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
