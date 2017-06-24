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
    protected:
        virtual void evaluate_mods();
    public:
        u_16 attr[6];
        u_16 attrp[6];
        int mods[6];

        u_16 pp[2];

        ALLEGRO_COLOR tint;

        ObjectData* corpse_data;

        Actor(u_16, u_16, u_16);
        Actor(ActorData*, asset::asset_package*, u_16, u_16, u_16);
        virtual AnimationListPair interact(Actor*, Tower*);
        virtual void draw(int, int);
        Agent agent;
};

struct Human: public Actor {
    protected:
        void abs_equip(charstuff::Equip*, charstuff::EQUIPMENT_SLOT);
        void evaluate_mods();
    public:
        charstuff::Equip* equips[6];

        Human(u_16, u_16, u_16);
        AnimationListPair interact(Actor*, Tower*);
        void equip(charstuff::Equip*);
        void dequip(charstuff::EQUIPMENT_SLOT);
        void toggle_equip(charstuff::Equip*);
        int* get_replace(charstuff::Equip*);
        void draw(int, int);
};

struct Player: public Human {
    u_32 ap;
    u_16 upgr[6];

    Player();
    ~Player();

    void incr_attr(Attribute);
};


AnimationListPair action_attack(Actor*, Object*, Tower*);

Action lazy_agent(Actor*, Tower*);
Action simple_move_agent(Actor*, Tower*);
Action simple_aggressive_agent(Actor*, Tower*);


void init_player();
void new_player();
Player* get_player();


}

#endif // AT_ACTOR_H
