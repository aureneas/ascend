#include "../../../include/engine/interface/widget.h"
#include "../../../include/graphics.h"
// TODO remove later
#include "../../../include/_debug.h"

using namespace engine;

//#define NO_SUCH_TILE 65535

Widget::Widget() {
    tint = al_map_rgb(255,255,255);
}

void Widget::draw() {
    if (bmp) graphics::draw(bmp, crd.x, crd.y, 0);
}

bool Widget::in_bounds(int x, int y) {
    return (x >= crd.x && x <= crd.x + al_get_bitmap_width(bmp) && y >= crd.y && y <= crd.y + al_get_bitmap_height(bmp));
}



TextWidget::TextWidget(int s, Point p) {
    crd = p;
    text = al_ustr_newf("%d", s);

    bmp = nullptr;
    mouse_on = false;
    mouse_on_func = nullptr;
    mouse_off_func = nullptr;
    click_func = nullptr;

}

void TextWidget::draw() {
    // TODO variable fonts/tints?
    graphics::draw_ustr(asset::get_pixel_bold(), tint, text, crd.x, crd.y, 0);
}

bool TextWidget::in_bounds(int x, int y) {
    return (x >= crd.x && x <= crd.x + al_get_ustr_width(asset::get_pixel_bold(), text) && y >= crd.y && y <= crd.y + al_get_font_line_height(asset::get_pixel_bold()));
}


TileWidget::TileWidget(tower::Floor* cf, u_16 ti, Point* camera, /*std::forward_list<TileWidget>* twidget,*/ Widget* o) : enws{nullptr, nullptr, nullptr, nullptr} {
    cfloor = cf;
    t_index = ti;
    obj = o;

    bmp = cf->tile[ti]->floor;

    int dx = (t_index%cfloor->size) - (camera->x/18);
    int dy = (t_index/cfloor->size) - (camera->y/18);
    crd.x = 182                                     // centers the tile
                + 18*(dx-dy)                        // offsets by x-offset of tile
                - (camera->x%18 - camera->y%18);    // offsets by NEGATIVE x-offset of camera
    crd.y = 132                                     // centers the tile
                + 9*(dx+dy)                         // offsets by y-offset of tile
                - (camera->x%18 + camera->y%18)/2;  // offsets by NEGATIVE y-offset of camera

    /*u_16 enws_i[4] = { (t_index%cfloor->size > 0 ? t_index-1 : NO_SUCH_TILE),
                        (t_index >= cfloor->size ? t_index-cfloor->size : NO_SUCH_TILE),
                        (t_index%cfloor->size < cfloor->size-1 ? t_index+1 : NO_SUCH_TILE),
                        (t_index < cfloor->size*(cfloor->size-1) ? t_index+cfloor->size : NO_SUCH_TILE) };
    DEBUG_PRINT(t_index << " (" << t_index%cfloor->size << ", " << t_index/cfloor->size << ")");
    for (int i = 3; i >= 0; --i)
        DEBUG_PRINT("\t" << i << " " << enws_i[i] << " (" << enws_i[i]%cfloor->size << ", " << enws_i[i]/cfloor->size << ")");
    for (std::forward_list<TileWidget>::iterator it = twidget->begin(); it != twidget->end(); ++it) {
        for (int i = 3; i >= 0; --i) {
            if (it->t_index == enws_i[i] && enws_i[i] != NO_SUCH_TILE) {
                enws[i] = &(*it);
                it->enws[(i+2)%4] = this;
                DEBUG_PRINT("\t" << this << " enws[" << i << "] = " << enws[i]);
                DEBUG_PRINT("\t" << &*it << " enws[" << (i+2)%4 << "] = " << it->enws[(i+2)%4]);
                break;
            }
        }
    }*/
}

TileWidget::~TileWidget() {
    for (int i = 3; i >= 0; --i) {
        if (enws[i])
            enws[i]->enws[(i+2)%4] = nullptr;
    }
}



ObjectWidget::ObjectWidget(tower::Object* o, tower::Floor* cfloor, Point* camera) {
    obj = o;
    bmp = obj->bmp;
    reset_crd(camera);
}

void ObjectWidget::reset_crd(Point* camera) {
    int dx = (obj->pos.x/18) - (camera->x/18);
    int dy = (obj->pos.y/18) - (camera->y/18);
    crd.x = 200 - (al_get_bitmap_width(bmp)/2)      // centers the sprite
                + (obj->pos.x%18 - obj->pos.y%18)   // offsets by x-offset of object
                + 18*(dx-dy)                        // offsets by x-offset of tile
                - (camera->x%18 - camera->y%18);    // offsets by NEGATIVE x-offset of camera
    crd.y = 133 - al_get_bitmap_height(bmp)         // centers the bottom of the sprite at the top of the tile
                + obj->offset                       // offsets by inherent y-offset of object
                + (obj->pos.x%18 + obj->pos.y%18)/2 // offsets by y-offset of object
                + 9*(dx+dy)                         // offsets by y-offset of tile
                - (camera->x%18 + camera->y%18)/2;  // offsets by NEGATIVE y-offset of camera
}

void ObjectWidget::draw() {
    obj->draw(crd.x, crd.y);
}
