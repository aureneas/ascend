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
    u_16 attrm[6];

    u_16 ftr_num;
    FeatureData* ftr_list;

    ALLEGRO_USTR* name;
    ALLEGRO_USTR* desc;

    ALLEGRO_COLOR tint;

    Equip* gen_item();
};


void init();
EquipData* get_std_clothes();
EquipData* get_op_armor();


}


#endif // AC_ITEM_DATA_H
