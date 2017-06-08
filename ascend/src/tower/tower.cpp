#include <algorithm>
#include "../../include/tower/tower.h"
// TODO remove later
#include "../../include/_debug.h"

namespace tower {


Floor::Floor(u_16 s) {
    size = s;
    tile = (Tile**)calloc(s*s, sizeof(Tile*));
}

Floor::~Floor() {
    delete[] tile;
}

bool obj_sort(std::shared_ptr<Object> o1, std::shared_ptr<Object> o2) {
    return ((o1->pos.y/18 != o2->pos.y/18) ? (o1->pos.y/18 > o2->pos.y/18) : (o1->pos.x/18 > o2->pos.x/18));
}

void Floor::refresh_objects() {
    //DEBUG_PRINT("Floor::refresh_objects() called.");
    std::sort(objects.begin(), objects.end(), obj_sort);

    std::vector<std::shared_ptr<Object> >::iterator it = objects.begin();
    u_16 MAX_TILE = size*size;
    u_16 n_tile = (it == objects.end() ? MAX_TILE : ((*it)->pos.y/18)*size + (*it)->pos.x/18);
    //DEBUG_PRINT("\t" << n_tile << "\t" << (it == objects.end() ? 0 : (*it)->bmp));
    //DEBUG_PRINT("n_tile = " << n_tile);
    for (u_16 i = MAX_TILE; i > 0; --i) {
        if (i-1 == n_tile) {
            if (tile[i-1]) {
                tile[i-1]->occupy = (it++)->get();
                n_tile = (it == objects.end() ? MAX_TILE : ((*it)->pos.y/18)*size + (*it)->pos.x/18);
                //DEBUG_PRINT("\t" << n_tile << "\t" << (it == objects.end() ? 0 : (*it)->bmp));
                //DEBUG_PRINT("n_tile = " << n_tile);
            } else {
                // TODO error handlin
            }
        } else if (tile[i-1]) {
            tile[i-1]->occupy = nullptr;
        }
    }
}


Tower::Tower(asset::asset_package* ap, u_16 nf, u_16 s) {
    assets = ap;
    floor = new Floor*[nf];
    for (u_16 i = 0; i < nf; i++)
        floor[i] = new Floor(s);
}

Tower::~Tower() {
    delete[] floor;
}

bool actor_comp(std::pair<double, Actor*> p1, std::pair<double, Actor*> p2) {
    return (p1.first > p2.first);
}

Action Tower::get_next_action() {
    while (active.front().first < ACTION_THRESHOLD) {
        for (std::list<std::pair<double, Actor*> >::iterator it = active.begin(); it != active.end(); ++it) {
            it->first += 1.0 + (log(it->second->attr[SPD]+1)/2.07944);
        }
        active.sort(actor_comp);
    }

    Actor* a = active.front().second;
    Action nxt = { nullptr, nullptr, a };
    if (a->agent)
        nxt = a->agent(a, this);
    active.pop_front();
    active.emplace_back(0.0, a);
    return nxt;
}


}
