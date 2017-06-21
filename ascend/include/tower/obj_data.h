#ifndef AT_OBJ_DATA_H
#define AT_OBJ_DATA_H

#include <allegro5/allegro.h>
#include "../engine/interface/animation.h"

namespace tower {


struct Object;
struct Actor;

struct Floor;
struct Tower;


struct ObjectData {
    u_16 bmp;
    u_16 offset;

    ObjectData(u_16, u_16);
    virtual Object* gen_object(asset::asset_package*, u_16, u_16, u_16);
    virtual Object* gen_object(Tower*, u_16, u_16, u_16);
};

template <typename T>
struct SubObjectData: public ObjectData {
    SubObjectData(u_16, u_16);
    Object* gen_object(asset::asset_package*, u_16, u_16, u_16);
    Object* gen_object(Tower*, u_16, u_16, u_16);
};


enum ACTION {
    MOVE_EAST = 0,
    MOVE_NORTH = 1,
    MOVE_WEST = 2,
    MOVE_SOUTH = 3,
    DO_NOTHING = 4,
    ACT_ATTACK = 5,
    WAIT_FOR_INPUT = 6
};

typedef std::forward_list<engine::Animation*> AnimationList;
typedef std::pair<AnimationList, AnimationList> AnimationListPair;
typedef AnimationListPair (*ActionFunc)(Actor*, Object*, Tower*);

struct Action {
    ActionFunc act;
    Object* target;
    Actor* actor;
};

typedef Action (*Agent)(Actor*, Tower*);


enum Attribute: int {
    MAX_HP = 0,
    MAX_SP = 1,
    ATK = 2,
    DEF = 3,
    SPD = 4,
    MAG = 5
};


struct ActorData: public ObjectData {
    Agent agent;
    ALLEGRO_COLOR tint;
    u_16 def_attr[6];
    ObjectData* corpse_data;

    ActorData(u_16, u_16, Agent, u_16, u_16, u_16, u_16, u_16, u_16, ObjectData*);
    virtual Object* gen_object(Tower*, u_16, u_16, u_16);
};


}

#endif // AT_OBJ_DATA_H
