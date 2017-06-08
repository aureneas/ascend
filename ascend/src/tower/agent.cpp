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

AnimationList action_wait(Actor* actor, Object* target, Tower* tow) {
    return AnimationList();
}

template <Direction D>
AnimationList action_move(Actor* actor, Object* target, Tower* tow) {
    AnimationList anim;
    Floor* f = tow->floor[actor->floor];
    Point p = translate(actor->pos, D);
    //u_16 t1 = (actor->pos.x/18) + f->size*(actor->pos.y/18);
    u_16 t = (p.x/18) + f->size*(p.y/18);
    if (!f->tile[t]->occupy) {
        f->tile[t]->occupy = &o_hold;
        int dx = (p.x/18) - (get_player()->pos.x/18);
        int dy = (p.y/18) - (get_player()->pos.y/18);
        bool full_animation = ((actor->floor == get_player()->floor) && (dx+dy >= -16) && (dx+dy <= 22) && (abs(dx-dy) <= 12));
        anim.push_front(new engine::MoveAnimation(engine::animation_linmove, (full_animation ? 9 : 1),
                                                     { p.x - actor->pos.x, p.y - actor->pos.y },
                                                     &actor->pos));
        if (full_animation) {
            // TODO SpriteAnimation
        }
    }
    return anim;
}

AnimationList action_attack(Actor* actor, Object* target, Tower* tow) {
    AnimationList anim;
    if (engine::TowerState* ts = dynamic_cast<engine::TowerState*>(engine::get_state())) {
        if (Actor* trg = dynamic_cast<Actor*>(target)) {
            double hit = ((actor->attr[SPD] * (1 - (1.0/pow(5.1, pow(actor->attr[SPD], 0.25))))) + (trg->attr[SPD] / pow(1.26, pow(trg->attr[SPD], 0.55))))/(double)(actor->attr[SPD] + trg->attr[SPD]);
            engine::Widget* t = nullptr;
            int dmg = 0;
            while (rand2<double>(1.0) <= hit) {
                ++dmg;
                hit = pow(hit, 3);
            }
            if (dmg > 0) {
                dmg = std::max((int)(((log2(dmg) + 4)*(0.5*log2(actor->attr[ATK]) + 1)) - trg->attr[DEF]), 0);
                DEBUG_PRINT("DAMAGE : " << dmg);
                anim.push_front(new engine::ArithmeticAnimation(engine::animation_addition, 6, &trg->pp[MAX_HP], -dmg));
                anim.push_front(new engine::FlashAnimation(&trg->tint, 1, 3));
                int dx = (trg->pos.x/18) - (ts->camera.x/18);
                int dy = (trg->pos.y/18) - (ts->camera.y/18);
                t = ts->insert(new engine::TextWidget(dmg,
                           {
                               200 - al_get_bitmap_width(trg->bmp)/2 + (trg->pos.x%18 - trg->pos.y%18) + 18*(dx-dy) - (ts->camera.x%18 - ts->camera.y%18),
                               133 - al_get_bitmap_height(trg->bmp)*3/2 + trg->offset + (trg->pos.x%18 + trg->pos.y%18)/2 + 9*(dx+dy) - (ts->camera.x%18 + ts->camera.y%18)/2
                           }));
                double angel = ((rand()%2 == 0) ? 0.5236 : 1.5708) + rand2<double>(0.5236, 2);
                u_16 sped = rand2<u_16>(4, 2) + 4;
                anim.push_front(new engine::VelocityAnimation(engine::animation_fallvel, 54,
                                                              Point{ (int)(sped*cos(angel)), -(int)(sped*sin(angel)) },
                                                              &t->crd));
                //anim.push_front(new engine::FadeAnimation(&t->tint, al_map_rgba(0,0,0,0), 18));
                anim.push_front(new engine::DeletionAnimation<engine::TowerState, engine::Widget>(ts->remove, 54, ts, t));
            } else {
                // TODO miss message
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



ActionFunc afunc[6] =   {
                            &action_move<NORTH>,//&action_move_north,
                            &action_move<EAST>,//_east,
                            &action_move<SOUTH>,//_south,
                            &action_move<WEST>,//_west,
                            &action_wait,
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
        if (n.x >= 0 && n.y >= 0 && n.x < (int)18*f->size && n.y < (int)18*f->size) {
            u_16 t = (f->size * (n.y/18)) + (n.x/18);
            if (f->tile[t] != nullptr && f->tile[t]->occupy == nullptr) {
                DEBUG_PRINT("\t(" << a->floor << ") Chose " << moves[i] << " : " << a->pos.x/18 << ", " << a->pos.y/18);
                return { afunc[moves[i]], nullptr, a };
            }
        }
    }
    DEBUG_PRINT("\t(" << a->floor << ") No available moves : " << a->pos.x/18 << ", " << a->pos.y/18);
    return { afunc[DO_NOTHING], nullptr, a };
}


}
