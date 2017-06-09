#include "../../include/tower/towers_data.h"
#include "../../include/tower/actor.h"
#include "../../include/tower/generator.h"

namespace tower {


/** OBJECTS **/
ObjectData orb_corpse = {
                            /* default bmp */ 4,
                            /* offset */ 2
                        };

/** ENEMIES **/
ActorData orb( /* default bmp */ 3,
                /* offset */ 1,
                /* agent */ simple_aggressive_agent,
                /* default attributes */ 10, 0, 1, 1, 1, 1,
                /* corpse */ &orb_corpse
                );

/** TILES **/
TileData tile_debug = { /* default tile bmp */ 1 };

/** ROOMS **/
RoomData_Simple r_debug = { /* default tile data */ &tile_debug,
                                /* minimum room size */ 2,
                                /* range of room sizes */ 4,
                                /* is a rectangle? */ true };

/** TOWERS **/
TowerData t_debug = { /* asset package */ { "lib/package/debug.png", "data/towers/debug_asset.dat", true },
                        /* number of floors */ 5,
                        /* width/height of floors */ 50,
                        /* minimum number of tiles per floor */ 1000,
                        /* tower generation function */ standard_generator,
                        /* basic tile bmp */ 1,
                        /* basic wall bmp */ 2 };


/** VISIBLE FUNCTIONS **/
void init() {
    /* ENEMIES */
    orb.tint = al_map_rgb(255, 255, 255);

    /* DEBUG TOWER */
    tile_debug.obj_t.emplace(1, &orb);
    t_debug.room_t.emplace(MAX_PROB, &r_debug);
}

TowerData* get_t_debug() { return &t_debug; }


}
