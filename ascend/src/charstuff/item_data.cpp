#include <memory>
#include "../../include/charstuff/item_data.h"
#include "../../include/asset/asset.h"
#include "../../include/_debug.h"

namespace charstuff {


EquipData std_clothes_dt = { /* base bitmap */          5,
                                /* equip slot */        BODY,
                                /* attribute mods */    { 0,0,0,1,0,0 },
                                /* no. features */      1,
                                /* feature list */      nullptr
                            };

void init() {
    // STANDARD CLOTHES
    std_clothes_dt.tint = al_map_rgb(207,184,159);
    std_clothes_dt.ftr_list = new FeatureData[1];
    std_clothes_dt.ftr_list[0] = { 23, al_map_rgb(56,71,48) };
    std_clothes_dt.name = al_ustr_new("CLOTHES");
    std_clothes_dt.desc = al_ustr_new("Some normal clothes. Good for when you want to not be naked.");
}

EquipData* get_std_clothes() {
    return &std_clothes_dt;
}


std::forward_list<std::unique_ptr<Item> > gen_equips;

Equip* EquipData::gen_item() {
    gen_equips.emplace_front(new Equip(asset::get_tower(bmp), tint, slot));
    Equip* ptr = dynamic_cast<Equip*>(gen_equips.front().get());

    ptr->name = name;
    ptr->desc = desc;

    ptr->ftr_num = ftr_num;
    ptr->ftr = new Feature[ftr_num];
    for (int f = ftr_num-1; f >= 0; --f)
        ptr->ftr[f] = { asset::get_tower(ftr_list[f].bmp), ftr_list[f].tint };

    for (int i = 5; i >= 0; --i)
        ptr->attrm[i] = attrm[i];

    return ptr;
}


}
