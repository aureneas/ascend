#ifndef AT_ACTOR_H
#define AT_ACTOR_H

#include <allegro5/allegro.h>
#include "../types.h"
#include "../charstuff/item.h"
#include "object.h"
#include "obj_data.h"

namespace tower {

#define MAXED_ATTRIBUTE UINT_MAX


struct Actor: public Object, public charstuff::Inventory {
    u_16 attr[6];
    u_16 pp[2];

    ALLEGRO_COLOR tint;

    Actor(u_16, u_16, u_16);
    Actor(ActorData*, asset::asset_package*, u_16, u_16, u_16);
    virtual void draw(int, int);
    Agent agent;
};

struct Human: public Actor {
    Human(u_16, u_16, u_16);
    void draw(int, int);
};

struct Player: public Human {
    u_32 ap;
    u_16 upgr[6];

    charstuff::Inventory* invt;

    Player();

    void incr_attr(Attribute);
};


AnimationList action_attack(Actor*, Object*, Tower*);

Action lazy_agent(Actor*, Tower*);
Action simple_move_agent(Actor*, Tower*);


void init_player();
void new_player();
Player* get_player();


}

#endif // AT_ACTOR_H
