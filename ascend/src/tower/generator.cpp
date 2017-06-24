#include <cstdlib>
#include <vector>
#include "../../include/utility.h"
#include "../../include/tower/tower.h"
#include "../../include/tower/object.h"
#include "../../include/tower/actor.h"
#include "../../include/tower/towers_data.h"

// TODO remove when done with debugging
#include "../../include/_debug.h"


namespace tower {


Tower* c_tower = nullptr;


u_16 RoomData_Simple::generate_room(Tower* tw, u_16 f, u_16 t) {
    u_16 sparcity = 0;
    u_16 dx = rsize_min + rand2<u_16>(rsize_rng);
    u_16 dy = rsize_min + rand2<u_16>(rsize_rng);
    if (rect) {
        u_16 s = tw->floor[f]->size;
        if (dx < t%s) {
            t -= dx;
            dx *= 2;
        } else {
            u_16 dt = (t%s) - 1;
            dx += dt;
            t -= dt;
        }
        if (dy < t/s) {
            t -= (s*dy);
            dy *= 2;
        } else {
            dy += (t/s) - 1;
            t = s + (t%s);
        }
        if (dx >= s-(t%s)-1)
            dx = s-(t%s)-1;
        if (dy >= s-(t/s)-1)
            dy = s-(t/s)-1;
        for (int i = dx; i >= 0; --i) {
            for (int j = dy; j >= 0; --j) {
                u_16 t2 = t + i + (s*j);
                if (!tw->floor[f]->tile[t2]) {
                    Tile* tl = new Tile();
                    tl->floor = tw->assets->get(tile->bmp);
                    // place objects
                    u_16 trand = rand2<u_16>(MAX_PROB);
                    for (std::map<u_16, ObjectData*>::iterator it = tile->obj_t.begin(); it != tile->obj_t.end(); ++it) {
                        if (it->first > trand) {
                            tl->occupy = it->second->gen_object(tw, f, t2, tw->floor[f]->size);
                            break;
                        }
                    }
                    tw->floor[f]->tile[t2] = tl;
                    ++sparcity;
                }
            }
        }
    }
    return sparcity;
}

u_16 RoomData_Complex::generate_room(Tower* tw, u_16 f, u_16 t) {
    u_16 sparcity = 0;
    for (std::vector<RoomData_Simple>::iterator it = sub_rooms.begin(); it != sub_rooms.end(); ++it)
        sparcity += it->generate_room(tw, f, t);
    return sparcity;
}


u_16 construct_passage(TowerData* td, u_16 floor, u_16 start, int d, u_16 mult) {
    u_16 scarcity = 0;
    for (int i = 0; (d > 0 ? i < d : i > d); (d > 0 ? i++ : i--)) {
        u_16 t = start + (i*mult);
        if (!c_tower->floor[floor]->tile[t]) {
            Tile* tile = new Tile();
            tile->floor = c_tower->assets->get(td->basic_t);
            c_tower->floor[floor]->tile[t] = tile;
            scarcity++;
        }
    }
    return scarcity;
}


Tower* standard_generator(TowerData* td, bool from_bottom) {
    if (c_tower) { delete c_tower; }
    c_tower = new Tower(td->asset_data.load(), td->num_floors, td->size);

    // construct sparcity array
    u_16* sparcity = new u_16[td->num_floors];
    for (u_16 a = 0; a < td->num_floors; a++)
        sparcity[a] = 0;

    // place entrance
    u_16 entr_index = (td->size/6)+rand2<u_16>(td->size/3, 2);
    c_tower->floor[0]->tile[entr_index] = new Tile();
    Tile* t_entr = c_tower->floor[0]->tile[entr_index];
    t_entr->floor = c_tower->assets->get(td->basic_t);
    t_entr->occupy = new Portal(SOUTH, 0, entr_index, td->size);
    c_tower->floor[0]->objects.emplace_back(t_entr->occupy);
    // place player
    entr_index += td->size;
    c_tower->floor[0]->tile[entr_index] = new Tile();
    Tile* t_entrp = c_tower->floor[0]->tile[entr_index];
    t_entrp->floor = c_tower->assets->get(td->basic_t);
    if (from_bottom) {
        t_entrp->occupy = get_player();
        get_player()->pos.x = (18*(entr_index%td->size)) + 9;
        get_player()->pos.y = 27;
        get_player()->face = SOUTH;
        c_tower->floor[0]->objects.emplace_back(get_player());
        c_tower->active.emplace_back(0.0, get_player());
    }
    c_tower->t_entr = { 0, entr_index };
    sparcity[0] += 2;

    std::vector<u_16> rooms;
    rooms.push_back(entr_index);
    for (u_16 f = 0; f < td->num_floors; f++) {
        while(sparcity[f] < td->sparcity) {
            // place next room
            u_16 tile = rooms[rand()%rooms.size()];
            u_16 nxt = tile;
            int dx = 0, dy = 0;
            if (tile/td->size < td->size/3)
                dy = 1 - (tile/td->size);
            else
                dy = 1 - (td->size/3);
            dy = std::min(dy + rand2<int>((int)(td->size*2/3)), (int)(td->size - (tile/td->size) - 1));
            if (tile%td->size < td->size/3)
                dx = 1 - (tile%td->size);
            else
                dx = 1 - (td->size/3);
            dx = std::min(dx + rand2<int>((int)(td->size*2/3)), (int)(td->size - (tile%td->size) - 1));

            /*rsign()*rand2<int>((int)(td->size/10), 2);
            int dx = rsign()*rand2<int>((int)(td->size/10), 2);
            if (-dx >= tile%td->size)
                dx = 1 - (tile%td->size);
            else if ((tile%td->size) + dx >= td->size)
                dx = td->size - (tile%td->size) - 1;
            if (-dy >= tile/td->size)
                dy = 1 - (tile/td->size);
            else if ((tile/td->size) + dy >= td->size)
                dy = td->size - (tile/td->size) - 1;*/
            nxt += dx + (td->size*dy);

            rooms.push_back(nxt);
            // determine properties of room
            u_16 rt = rand() % MAX_PROB;
            for (std::map<u_16, RoomData*>::iterator it = td->room_t.begin(); it != td->room_t.end(); ++it) {
                if (rt < it->first) {
                    // place tiles and elements of room
                    sparcity[f] += it->second->generate_room(c_tower, f, nxt);
                    break;
                }
            }
            // place passage between previous room and current room
            /*int dy = (nxt/td->size) - (tile/td->size);
            int dx = (nxt%td->size) - (tile%td->size);*/
            if ((dy+dx > 0 && dy-dx > 0) || (dy+dx < 0 && dy-dx < 0)) // abs(dx) < abs(dy)
                sparcity[f] += construct_passage(td, f, tile, dx, 1) + construct_passage(td, f, tile+dx, dy, td->size);
            else
                sparcity[f] += construct_passage(td, f, tile, dy, td->size) + construct_passage(td, f, tile+(td->size*dy), dx, 1);
        }

        // generate portal(s) to next floor OR exit
        if (f < td->num_floors - 1) {
            u_16 stair = rooms[rooms.size() - 1];

            u_16 ctile = stair;
            Direction s_face;
            int dx = 0;
            int dy = 0;
            int d = 0;
            while (true) {
                if (c_tower->floor[f]->tile[ctile] != nullptr) {
                    if (c_tower->floor[f]->tile[ctile]->occupy == nullptr) {
                        if (ctile % td->size > 1 && ctile % td->size < td->size - 1 && c_tower->floor[f]->tile[ctile + 1] != nullptr) {
                            if (c_tower->floor[f]->tile[ctile + 1]->occupy == nullptr) {
                                stair = ctile;
                                s_face = EAST;
                                break;
                            }
                        }
                        if (ctile / td->size > 1 && ctile / td->size < td->size - 1 && c_tower->floor[f]->tile[ctile + td->size] != nullptr) {
                            if (c_tower->floor[f]->tile[ctile + td->size]->occupy == nullptr) {
                                stair = ctile;
                                s_face = SOUTH;
                                break;
                            }
                        }
                    }
                }

                if ((dx == d && dy+1 == 0) || d == 0) {
                    ++d;
                    dx = d;
                    dy = 0;
                } else {
                    if (dx == d && dy != d) {
                        ++dy;
                    } else if (dx != -d && dy == d) {
                        --dx;
                    } else if (dx == -d && dy != -d) {
                        --dy;
                    } else if (dx != d && dy == -d) {
                        ++dx;
                    }
                }
                if ((int)(stair % td->size) > -dx && (int)(td->size - (stair % td->size)) > dx
                        && (int)(stair / td->size) > -dy && (int)(td->size - (stair / td->size)) > dy) {
                    ctile = stair + dx + (td->size * dy);
                }
            }
            u_16 stair_out = stair - (s_face == EAST ? 1 : td->size);
            DEBUG_PRINT("STAIRS (FLOOR " << f << ") located at <" << (stair % td->size) << ", " << (stair / td->size) << ">");
            c_tower->floor[f]->tile[stair]->occupy = new Portal(td->portal_t, c_tower->assets, s_face,
                                                        { f + 1, stair_out }, f, stair, td->size);
            c_tower->floor[f]->objects.emplace_back(c_tower->floor[f]->tile[stair]->occupy);

            // set up for next floor
            rooms.clear();
            rooms.push_back(stair_out);
            c_tower->floor[f + 1]->tile[stair] = new Tile();
            c_tower->floor[f + 1]->tile[stair]->floor = nullptr;//c_tower->assets->get(td->basic_t);
            c_tower->floor[f + 1]->tile[stair]->occupy = new Portal(td->portal_t, c_tower->assets, (Direction)((s_face+2)%4),
                                                            { f, (2 * stair) - stair_out }, f, stair, td->size);
            c_tower->floor[f + 1]->objects.emplace_back(c_tower->floor[f + 1]->tile[stair]->occupy);
            c_tower->floor[f + 1]->tile[stair_out] = new Tile();
            c_tower->floor[f + 1]->tile[stair_out]->floor = c_tower->assets->get(td->basic_t);
        } else {
            // TODO generate exit
            // TODO place player (if !from_bottom)
        }

        // generate walls
        u_16 arrsize = td->size*td->size;
        for (u_16 j = arrsize; j > 0; --j) {
            u_16 i = arrsize - j;
            if (!c_tower->floor[f]->tile[i]) {
                if ((i+1 < arrsize && c_tower->floor[f]->tile[i+1] != nullptr)
                        || (i+td->size < arrsize && c_tower->floor[f]->tile[i+td->size] != nullptr)
                        || (i+td->size+1 < arrsize && c_tower->floor[f]->tile[i+td->size+1] != nullptr)) {
                    c_tower->floor[f]->tile[i] = new Tile();
                    c_tower->floor[f]->tile[i]->floor = c_tower->assets->get(td->basic_t);
                    c_tower->floor[f]->tile[i]->occupy = new Object(c_tower->assets->get(td->wall_t), 9, f, i, td->size);
                    c_tower->floor[f]->objects.emplace_back(c_tower->floor[f]->tile[i]->occupy);
                }
            }
        }
    }
    delete[] sparcity;

    return c_tower;
}


}
