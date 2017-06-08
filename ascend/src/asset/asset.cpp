#include "../../include/asset/asset.h"
// TODO remove later
#include "../../include/_debug.h"


namespace asset {


/** ASSET PACKAGES **/
asset_package* tower_asset;
asset_package* tower_asset_extra;
ALLEGRO_FONT* pixel_bold_font;

void init() {
    tower_asset = asset_package_data{ "lib/package/tower.png", "data/towers/tower_asset.dat", true }.load();
    pixel_bold_font = al_load_bitmap_font("lib/font/pixel_bold.png");
}

ALLEGRO_BITMAP* get_tower(u_16 index) {
    return tower_asset->get(index);
}

ALLEGRO_FONT* get_pixel_bold() {
    return pixel_bold_font;
}


/**
 *  -----------------------
 *  ASSET_PACKAGE FUNCTIONS
 *  -----------------------
 */
asset_package::asset_package() {
    size = 0;
    capacity = 4;
    assets = (ALLEGRO_BITMAP**)malloc(sizeof(ALLEGRO_BITMAP*) * capacity);
    while (assets == NULL && capacity > 1)
        assets = (ALLEGRO_BITMAP**)malloc(sizeof(ALLEGRO_BITMAP*) * (--capacity));
    if (!assets)
        throw "Not enough memory to hold all assets.";
}

asset_package::~asset_package() {
    for (u_16 i = size; i > 0;)
        al_destroy_bitmap(assets[--i]);
    free(assets);
}

void asset_package::increase_capacity() {
    u_16 ncap = (capacity >= 20 ? 7 : 5)*capacity/4;
    do {
        if (ALLEGRO_BITMAP** temp = (ALLEGRO_BITMAP**)realloc(assets, sizeof(ALLEGRO_BITMAP*) * ncap)) {
            capacity = ncap;
            assets = temp;
            return;
        }
        ncap = (ncap+capacity)/2;
    } while (ncap > capacity);
    throw "Not enough memory to hold all assets.";
}

void asset_package::insert(ALLEGRO_BITMAP* bmp) {
    if (size == capacity)
        increase_capacity();
    assets[size++] = bmp;
}

ALLEGRO_BITMAP* asset_package::get(u_16 index) {
    return ((index < size && index >= 0) ? assets[index] : NULL);
}


/**
 *  ----------------------------
 *  ASSET_PACKAGE_DATA FUNCTIONS
 *  ----------------------------
 */

asset_package* asset_package_data::load() {
    asset_package* ap = nullptr;
    ALLEGRO_FILE* file = nullptr;
    try {
        if (is_tower) {
            if (tower_asset_extra != nullptr && tower_asset_extra != tower_asset)
                delete tower_asset_extra;
            tower_asset_extra = new asset_package();
            ap = tower_asset_extra;
        } else {
            // TODO change this later
            throw "Asset package is not tower.";
        }

        ALLEGRO_BITMAP* bmp = al_load_bitmap(file_bmp);
        if (!bmp)   throw "Error reading file.";
        ap->insert(bmp);
        file = al_fopen(file_data, "rb");
        if (file) {
            int64_t size = al_fsize(file);
            int64_t index = 8;
            while (index <= size) {
                int dat[4] = { (int)al_fread16be(file),     // 1 - x coordinate
                                (int)al_fread16be(file),    // 2 - y coordinate
                                (int)al_fread16be(file),    // 3 - width
                                (int)al_fread16be(file) };  // 4 - height
                ALLEGRO_BITMAP* bmp1 = al_create_sub_bitmap(bmp, dat[0], dat[1], dat[2], dat[3]);

                if (bmp1) {
                    ap->insert(bmp1);
                    index += 8;
                } else {
                    throw "Error creating sprite.";
                }
            }
            al_fclose(file);
        }

        return ap;
    } catch (const char* cstr) {
        if (ap)     ap->~asset_package();
        if (file)   al_fclose(file);
        throw cstr;
    }
}


}
