#ifndef AT_TOWER_H
#define AT_TOWER_H

#include <map>
#include <memory>
#include <list>
#include <vector>
#include <allegro5/allegro.h>
#include "../types.h"
#include "../asset/asset.h"
#include "object.h"
#include "actor.h"
#include "position.h"

namespace tower {

#define MAX_PROB 256
#define ACTION_THRESHOLD 100


struct Tile {
    ALLEGRO_BITMAP* floor;
    Object* occupy;
};

struct Floor {
    Tile** tile;
    std::list<std::shared_ptr<Object> > objects;
    u_16 size;

    Floor(u_16);
    ~Floor();

    void refresh_objects();
    std::shared_ptr<Object>* get_object(Object*);
    void insert(Object*);
    void insert_ptr(std::shared_ptr<Object>*);
    void remove(Object*);
};

struct Tower {
    asset::asset_package* assets;

    Floor** floor;
    u_16 size;      // number of floors

    Position t_entr; // bottom level portal of entry
    Position t_exit; // top level portal of entry

    Tower(asset::asset_package*, u_16, u_16);
    ~Tower();

    std::list<std::pair<double, Actor*> > active;
    Action get_next_action();
    void remove(Actor*);
};


struct TileData {
    u_16 bmp;
    std::map<u_16, ObjectData*> obj_t;

    TileData(u_16 b) : bmp(b) {}
};

struct RoomData {
    virtual u_16 generate_room(Tower*, u_16, u_16) = 0;
};

struct RoomData_Simple: public RoomData {
    TileData* tile;

    u_16 rsize_min;     // minimum dimension for room
    u_16 rsize_rng;     // range of room dimensions
    bool rect;          // rectangle (TRUE) or irregular (FALSE)?

    RoomData_Simple(TileData* td, u_16 a, u_16 b, bool c) : tile(td), rsize_min(a), rsize_rng(b), rect(c) {}
    u_16 generate_room(Tower*, u_16, u_16);
};

struct RoomData_Complex: public RoomData {
    std::vector<RoomData_Simple> sub_rooms;

    u_16 generate_room(Tower*, u_16, u_16);
};

struct TowerData {
    asset::asset_package_data asset_data;

    u_16 num_floors;
    u_16 size;
        // note to self:  width and height must be at least 24, no more than 255
    u_16 sparcity; // minimum number of 'filled' tiles per floor

    Tower* (*tower_generator)(TowerData*, bool);

    u_16 basic_t;
    u_16 wall_t;
    ObjectData* portal_t;

    std::map<u_16, RoomData*> room_t;
};


template <typename T>
SubObjectData<T>::SubObjectData(u_16 b, u_16 o) : ObjectData(b,o) {
    // TODO stuff???
}

template <typename T>
Object* SubObjectData<T>::gen_object(asset::asset_package* p, u_16 f, u_16 t, u_16 s) {
    return new T(this, p, f, t, s);
}

template <typename T>
Object* SubObjectData<T>::gen_object(Tower* tw, u_16 f, u_16 t, u_16 s) {
    tw->floor[f]->objects.emplace_back(new T(this, tw->assets, f, t, s));
    return tw->floor[f]->objects.back().get();
}


}

#endif // AT_TOWER_H
