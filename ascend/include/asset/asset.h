#ifndef AA_ASSET_H
#define AA_ASSET_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "../types.h"

namespace asset {


class asset_package {
    private:
        u_16 capacity;
        u_16 size;
        ALLEGRO_BITMAP* base;
        ALLEGRO_BITMAP** assets;

        void increase_capacity();
    public:
        asset_package();
        ~asset_package();
        void insert(ALLEGRO_BITMAP*);
        ALLEGRO_BITMAP* get(u_16);
};

struct asset_package_data {
    const char* file_bmp;
    const char* file_data;
    bool is_tower;

    asset_package* load();
};


void init();

ALLEGRO_BITMAP* get_tower(u_16);

ALLEGRO_FONT* get_pixel_small();
ALLEGRO_FONT* get_pixel_large();
ALLEGRO_FONT* get_pixel_bold();


}

#endif // AA_ASSET_H
