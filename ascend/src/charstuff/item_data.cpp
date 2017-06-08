#include "../../include/charstuff/item_data.h"
#include "../../include/asset/asset.h"

namespace charstuff {


EquipData std_clothes_dt = { /* base bitmap */          5,
                                /* equip slot */        BODY,
                                /* attribute mods */    { 0,0,0,1,0,0 },
                                /* no. features */      1
                            };

EquipData* get_std_clothes() {
    std_clothes_dt.tint = al_map_rgb(207,184,159);

    std_clothes_dt.ftr_list = new FeatureData[1];
    std_clothes_dt.ftr_list[0] = { 23, al_map_rgb(56,71,48) };

    return &std_clothes_dt;
}


std::forward_list<Equip> gen_equips;

Equip* EquipData::gen_item() {
    gen_equips.emplace_front(asset::get_tower(bmp), tint, slot);
    Equip* ptr = &gen_equips.front();
    for (int f = ftr_num-1; f >= 0; --f)
        ptr->ftr.push_front({ asset::get_tower(ftr_list[f].bmp), ftr_list[f].tint });
    // TODO attr modification
    return ptr;
}


}
