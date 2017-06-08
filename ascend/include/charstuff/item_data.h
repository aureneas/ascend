#ifndef AC_ITEM_DATA_H
#define AC_ITEM_DATA_H

#include "item.h"

namespace charstuff {


struct FeatureData {
    u_16 bmp;
    ALLEGRO_COLOR tint;
};

struct EquipData {
    u_16 bmp;
    EQUIPMENT_SLOT slot;
    int attr_m[6];

    u_16 ftr_num;

    ALLEGRO_COLOR tint;
    FeatureData* ftr_list;

    Equip* gen_item();
};


EquipData* get_std_clothes();


}


#endif // AC_ITEM_DATA_H
