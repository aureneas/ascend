#include <cmath>
#include "../../include/utility.h"
#include "../../include/tower/actor.h"
#include "../../include/tower/obj_data.h"
#include "../../include/tower/tower.h"
#include "../../include/engine/state.h"
#include "../../include/engine/interface/animation.h"
// TODO remove later
#include "../../include/_debug.h"

namespace engine {
State* get_state();
}

namespace tower {


Object o_hold(nullptr, 0, 0, 0, 1);

AnimationListPair action_wait(Actor* actor, Object* target, Tower* tow) {
    return AnimationListPair();
}

template <Direction D>
AnimationListPair action_move(Actor* actor, Object* target, Tower* tow) {
    AnimationListPair anim;
    Floor* f = tow->floor[actor->floor];
    Point p = translate(actor->pos, D);
    //u_16 t1 = (actor->pos.x/18) + f->size*(actor->pos.y/18);
    u_16 t = (p.x/18) + f->size*(p.y/18);
    if (!f->tile[t]->occupy) {
        f->tile[t]->occupy = &o_hold;
        int dx = (p.x/18) - (get_player()->pos.x/18);
        int dy = (p.y/18) - (get_player()->pos.y/18);
        bool full_animation = ((actor->floor == get_player()->floor) && (dx+dy >= -16) && (dx+dy <= 22) && (abs(dx-dy) <= 12));
        anim.first.push_front(new engine::MoveAnimation(engine::animation_linmove, (full_animation ? 9 : 1),
                                                     { p.x - actor->pos.x, p.y - actor->pos.y },
                                                     &actor->pos));
        if (full_animation) {
            // TODO SpriteAnimation
        }
    }
    return anim;
}

AnimationListPair action_attack(Actor* actor, Object* target, Tower* tow) {
    AnimationListPair anim;
    if (engine::TowerState* ts = dynamic_cast<engine::TowerState*>(engine::get_state())) {
        if (Actor* trg = dynamic_cast<Actor*>(target)) {
            int dx = (trg->pos.x/18) - (ts->camera.x/18);
            int dy = (trg->pos.y/18) - (ts->camera.y/18);
            double hit = ((actor->attr[SPD] * (1 - (1.0/pow(5.1, pow(actor->attr[SPD], 0.25))))) + (trg->attr[SPD] / pow(1.26, pow(trg->attr[SPD], 0.55))))/(double)(actor->attr[SPD] + trg->attr[SPD]);
            engine::Widget* t = nullptr;
            int dmg = 0;
            while (rand2<double>(1.0) <= hit) {
                ++dmg;
                hit = pow(hit, 3);
            }
            if (dmg > 0) {
                DEBUG_PRINT("HITS: " << dmg << ", ATK: " << actor->attr[ATK] << ", DEF: " << trg->attr[DEF]);
                dmg = (int)round(/*0.3869 */log2(2 * dmg) * log2(((double)actor->attr[ATK]/trg->attr[DEF]) + 1));
                DEBUG_PRINT("DAMAGE: " << dmg);
                anim.first.push_front(new engine::ArithmeticAnimation(engine::animation_addition, 4, &trg->pp[MAX_HP], -dmg));
                anim.first.push_front(new engine::FlashAnimation(&trg->tint, 1, 3));
                if (dmg >= (int)trg->pp[MAX_HP]) {
                    //anim.push_front(new engine::FunctionAnimation<engine::TowerState>(ts->reset_twidget, 5, ts));
                    //anim.push_front(new engine::FunctionAnimation<Floor>(ts->cfloor->refresh_objects, 5, ts->cfloor));
                    anim.first.push_front(new engine::FunctionAnimation2<Floor, Object>(ts->cfloor->remove, 5, ts->cfloor, trg));
                    anim.first.push_front(new engine::FunctionAnimation2<Tower, Actor>(ts->ctower->remove, 5, ts->ctower, trg));
                    Object* crps = trg->corpse_data->gen_object(ts->ctower->assets, trg->floor, (trg->pos.x/18) + ts->cfloor->size*(trg->pos.y/18), ts->cfloor->size);
                    anim.first.push_front(new engine::FunctionAnimation2<Floor, Object>(ts->cfloor->insert, 5, ts->cfloor, crps));
                    // TODO transfer objects
                    anim.first.push_front(new engine::FunctionAnimation<engine::TowerState>(ts->reset_twidget, 5, ts));
                }
                t = ts->insert(new engine::TextWidget(dmg,
                           {
                               200 - al_get_bitmap_width(trg->bmp)/2 + (trg->pos.x%18 - trg->pos.y%18) + 18*(dx-dy) - (ts->camera.x%18 - ts->camera.y%18),
                               133 - al_get_bitmap_height(trg->bmp)*3/2 + trg->offset + (trg->pos.x%18 + trg->pos.y%18)/2 + 9*(dx+dy) - (ts->camera.x%18 + ts->camera.y%18)/2
                           }));
                double angel = ((rand()%2 == 0) ? 0.5236 : 1.5708) + rand2<double>(0.5236, 2);
                u_16 sped = rand2<u_16>(4, 2) + 4;
                anim.second.push_front(new engine::VelocityAnimation(engine::animation_fallvel, 54,
                                                              Point{ (int)(sped*cos(angel)), -(int)(sped*sin(angel)) },
                                                              &t->crd));
                //anim.push_front(new engine::FadeAnimation(&t->tint, al_map_rgba(0,0,0,0), 18));
                anim.second.push_front(new engine::FunctionAnimation2<engine::TowerState, engine::Widget>(ts->remove, 54, ts, t));
            } else {
                // TODO miss message
                t = ts->insert(new engine::TextWidget(al_ustr_new("MISS"),
                           {
                               192 - al_get_bitmap_width(trg->bmp)/2 + (trg->pos.x%18 - trg->pos.y%18) + 18*(dx-dy) - (ts->camera.x%18 - ts->camera.y%18),
                               133 - al_get_bitmap_height(trg->bmp)*3/2 + trg->offset + (trg->pos.x%18 + trg->pos.y%18)/2 + 9*(dx+dy) - (ts->camera.x%18 + ts->camera.y%18)/2
                           }, asset::get_pixel_bold()));
                anim.second.push_front(new engine::MoveAnimation(engine::animation_linmove, 18, Point{ 0, -18 }, &t->crd));
                anim.second.push_front(new engine::FadeAnimation(&t->tint, al_map_rgba(0, 0, 0, 0), 36));
                anim.second.push_front(new engine::FunctionAnimation2<engine::TowerState, engine::Widget>(ts->remove, 36, ts, t));
                DEBUG_PRINT("MISS");
            }
        }
    }
    return anim;
}

/*
AnimationList action_move_north(Actor* actor, Object* target, Tower* tow) {
    return action_move(NORTH, actor, tow);
}

AnimationList action_move_east(Actor* actor, Object* target, Tower* tow) {
    return action_move(EAST, actor, tow);
}

AnimationList action_move_south(Actor* actor, Object* target, Tower* tow) {
    return action_move(SOUTH, actor, tow);
}

AnimationList action_move_west(Actor* actor, Object* target, Tower* tow) {
    return action_move(WEST, actor, tow);
}
*/



ActionFunc afunc[7] =   {
                            &action_move<NORTH>,
                            &action_move<EAST>,
                            &action_move<SOUTH>,
                            &action_move<WEST>,
                            &action_wait,
                            &action_attack,
                            nullptr
                        };

Action player_agent(Actor* a, Tower* t) {
    return { afunc[WAIT_FOR_INPUT], nullptr, a };
}

Action lazy_agent(Actor* a, Tower* t) {
    return { afunc[DO_NOTHING], nullptr, a };
}

Action simple_move_agent(Actor* a, Tower* t) {
    Floor* f = t->floor[a->floor];
    u_16 moves[8] = { NORTH, EAST, SOUTH, WEST, rand2<u_16>(4,1), rand2<u_16>(4,1), rand2<u_16>(4,1), rand2<u_16>(4,1) };
    for (int i = 7; i >= 0; --i) {
        Point n = translate(a->pos, (Direction)moves[i]);
        if (n.x >= 0 && n.y >= 0 && n.x < (int)(18*f->size) && n.y < (int)(18*f->size)) {
            u_16 t = (f->size * (n.y/18)) + (n.x/18);
            if (f->tile[t] != nullptr && f->tile[t]->occupy == nullptr)
                return { afunc[moves[i]], nullptr, a };
        }
    }
    return { afunc[DO_NOTHING], nullptr, a };
}

Action simple_aggressive_agent(Actor* a, Tower* t) {
    if (a->floor != get_player()->floor || (abs(a->pos.x-get_player()->pos.x) > 54 && abs(a->pos.y-get_player()->pos.y) > 54))
        return simple_move_agent(a, t);

    if ((get_player()->pos.x-a->pos.x == 0 && abs(get_player()->pos.y-a->pos.y) <= 18) || (get_player()->pos.y-a->pos.y == 0 && abs(get_player()->pos.x-a->pos.x) <= 18))
        return { afunc[ACT_ATTACK], get_player(), a };
    Floor* f = t->floor[a->floor];
    //u_16 t = (a->pos.x/18) + f->size*(a->pos.y/18);
    for (int i = 3; i >= 0; --i) {
        Point n = translate(a->pos, (Direction)i);
        if (n.x >= 0 && n.y >= 0 && n.x < (int)(18*f->size) && n.y < (int)(18*f->size)
                && abs(n.x-get_player()->pos.x) <= abs(a->pos.x-get_player()->pos.x)
                && abs(n.y-get_player()->pos.y) <= abs(a->pos.y-get_player()->pos.y)) {
            u_16 t = (f->size * (n.y/18)) + (n.x/18);
            if (f->tile[t] != nullptr && f->tile[t]->occupy == nullptr) {
                return { afunc[i], nullptr, a };
            }
        }
    }
    return { afunc[DO_NOTHING], nullptr, a };
}


}
