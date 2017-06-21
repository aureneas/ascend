#include "../../../include/engine/state.h"
#include "../../../include/graphics.h"
// TODO remove later
#include "../../../include/_debug.h"

using namespace engine;

//#define NO_SUCH_TILE 65535

Widget::Widget() {
    tint = al_map_rgb(255,255,255);
}

Widget::Widget(ALLEGRO_BITMAP* b, Point p) {
    tint = al_map_rgb(255,255,255);
    bmp = b;
    crd = p;
}

void Widget::draw(int x, int y) {
    if (bmp) graphics::draw(bmp, x + crd.x, y + crd.y, 0);
}

bool Widget::in_bounds(int x, int y) {
    return (x >= crd.x && x <= crd.x + al_get_bitmap_width(bmp) && y >= crd.y && y <= crd.y + al_get_bitmap_height(bmp));
}


FWidget::FWidget() {
    mouse_on_func = nullptr;
    mouse_off_func = nullptr;
    click_func = nullptr;
}

int FWidget::update_event(ALLEGRO_EVENT* e, int x, int y) {
    switch (e->type) {
        case ALLEGRO_EVENT_MOUSE_AXES:
            if (in_bounds(x + e->mouse.x, y + e->mouse.y) && !mouse_on) {
                mouse_on = true;
                if (mouse_on_func) return mouse_on_func(this);
            } else if (!in_bounds(x + e->mouse.x, y + e->mouse.y) && mouse_on) {
                mouse_on = false;
                if (mouse_off_func) return mouse_off_func(this);
            }
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            if (in_bounds(x + e->mouse.x, y + e->mouse.y)) {
                if (click_func) return click_func(this);
            }
            break;
    }
    return 0;
}



TextWidget::TextWidget(ALLEGRO_USTR* t, Point p, ALLEGRO_FONT* f, int w, bool pr) {
    crd = p;
    text = t;
    font = f;
    width = w;
    protect_text = pr;
    init();
}

TextWidget::TextWidget(int s, Point p) {
    crd = p;
    text = al_ustr_newf("%d", s);
    font = asset::get_pixel_bold();
    protect_text = false;
    init();
}

TextWidget::~TextWidget() {
    if (!protect_text)  al_ustr_free(text);
}

void TextWidget::init() {
    bmp = nullptr;
    mouse_on = false;
    if (font == asset::get_pixel_bold())
        tint = al_map_rgb(255, 255, 255);
    else
        tint = al_map_rgb(0, 0, 0);
    flags = ALLEGRO_ALIGN_LEFT;
}

void TextWidget::draw(int x, int y) {
    if (width > 0)
        graphics::draw_ustr_multiline(font, tint, text, x + crd.x, y + crd.y, width, 0, flags);
    else
        graphics::draw_ustr(font, tint, text, x + crd.x, y + crd.y, flags);
}

bool TextWidget::in_bounds(int x, int y) {
    return (x >= crd.x && x <= crd.x + al_get_ustr_width(asset::get_pixel_bold(), text) && y >= crd.y && y <= crd.y + al_get_font_line_height(asset::get_pixel_bold()));
}


ValueWidget::ValueWidget(u_16* v, Point p, int m) : TextWidget(al_ustr_newf("%02d", std::max(1, (int)(*v) + m)), p, asset::get_pixel_large()) {
    value = v;
    last = *v;
    mod = m;
    init();
}

void ValueWidget::init() {
    bmp = nullptr;
    mouse_on = false;
    if (mod > 0)
        tint = al_map_rgb(0, 128, 0);
    else if (mod < 0)
        tint = al_map_rgb(128, 0, 0);
    else
        tint = al_map_rgb(0, 0, 0);
    flags = ALLEGRO_ALIGN_CENTER;
}

void ValueWidget::draw(int x, int y) {
    if (*value != last) {
        al_ustr_free(text);
        text = al_ustr_newf("%02d", std::max(1, (int)(*value) + mod));
        last = *value;
    }
    TextWidget::draw(x, y);
}


BarWidget::BarWidget(u_16* c, u_16* m, Point p, int w, int h, ALLEGRO_COLOR t) {
    bmp = asset::get_tower(31);

    width = w;
    height = h;

    cur_value = c;
    cur_last = *c;
    max_value = m;
    max_last = *m;
    perc = (float)cur_last / max_last;

    crd = p;
    tint = t;
}

void BarWidget::draw(int x, int y) {
    if (*cur_value != cur_last || *max_value != max_last) {
        cur_last = *cur_value;
        max_last = *max_value;
        perc = (float)cur_last / max_last;
    }
    graphics::draw_tinted_bar(bmp, tint, x + crd.x, y + crd.y, width*perc, height, 0);
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

void ObjectWidget::draw(int x, int y) {
    obj->draw(x + crd.x, y + crd.y);
}


ItemWidget::ItemWidget(charstuff::Item** i, Point p, widg_func f, ALLEGRO_BITMAP* b) {
    item = i;
    crd = p;
    bmp = b;

    mouse_on_func = wfunc_item_mouse_on;
    mouse_off_func = wfunc_item_mouse_off;
    click_func = f;
}

int ItemWidget::update_event(ALLEGRO_EVENT* e, int x, int y) {
    switch (e->type) {
        case ALLEGRO_EVENT_MOUSE_AXES:
            return FWidget::update_event(e, x, y);
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            if (in_bounds(x + e->mouse.x, y + e->mouse.y)) {
                get_state()->set_mouse_widget(new ItemWidget(item, Point{ crd.x - (e->mouse.x + x), crd.y - (e->mouse.y + y) }), Point{ 0, 0 });
                return 1;
            }
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            if (in_bounds(x + e->mouse.x, y + e->mouse.y)) {
                if (ItemWidget* iw = dynamic_cast<ItemWidget*>(get_state()->get_mouse_widget())) {
                    if (iw->item == item) {
                        if (click_func) click_func(this);
                    } else {
                        charstuff::Item* it = *item;
                        *item = *iw->item;
                        if (*item) {
                            (*item)->parent = item;
                            charstuff::Equip* eq = dynamic_cast<charstuff::Equip*>(*item);
                            if (eq != nullptr && eq->equipped) {
                                if (!tower::get_player()->check(eq->parent))
                                    tower::get_player()->toggle_equip(eq);
                            }
                        }
                        *iw->item = it;
                        if (it) {
                            it->parent = iw->item;
                            charstuff::Equip* eq = dynamic_cast<charstuff::Equip*>(it);
                            if (eq != nullptr && eq->equipped) {
                                if (!tower::get_player()->check(eq->parent))
                                    tower::get_player()->toggle_equip(eq);
                            }
                        }
                    }
                    if (mouse_off_func) mouse_off_func(this);
                    if (mouse_on_func)  mouse_on_func(this);
                    return 1;
                }
            }
            break;
    }
    return 0;
}

void ItemWidget::draw(int x, int y) {
    if (*item) {
        if (charstuff::Equip* e = dynamic_cast<charstuff::Equip*>(*item)) {
            if (e->equipped && bmp != nullptr) graphics::draw(bmp, x + crd.x, y + crd.y, 0);
        }
        (*item)->draw(x + crd.x, y + crd.y, 0);
    }
}


Widget* Window::insert(Widget* w) {
    child.emplace_front(w);
    return child.front().get();
}

void Window::draw(int x, int y) {
    Widget::draw(x, y);
    for (WidgetList::iterator it = child.begin(); it != child.end(); ++it)
        (*it)->draw(x + crd.x, y + crd.y);
}

int Window::update_event(ALLEGRO_EVENT* e, int x, int y) {
    int r1 = 0;
    switch (e->type) {
        case ALLEGRO_EVENT_MOUSE_AXES:
            if (in_bounds(x + e->mouse.x, y + e->mouse.y)) {
                if (!mouse_on) mouse_on = true;
                for (WidgetList::iterator it = child.begin(); it != child.end(); ++it) {
                    int r2 = (*it)->update_event(e, x - crd.x, y - crd.y);
                    if (r1 == 0 && r2 != 0) r1 = r2;
                }
            } else if (!in_bounds(x + e->mouse.x, y + e->mouse.y) && mouse_on) {
                mouse_on = false;
                for (WidgetList::iterator it = child.begin(); it != child.end(); ++it) {
                    int r2 = (*it)->update_event(e, x - crd.x, y - crd.y);
                    if (r1 == 0 && r2 != 0) r1 = r2;
                }
            }
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            if (in_bounds(x + e->mouse.x, y + e->mouse.y)) {
                for (WidgetList::iterator it = child.begin(); it != child.end(); ++it) {
                    int r2 = (*it)->update_event(e, x - crd.x, y - crd.y);
                    if (r1 == 0 && r2 != 0) r1 = r2;
                }
            }
            break;
    }
    return r1;
}


InfoWindow::InfoWindow(charstuff::Item* i, Point p) : Window(asset::get_tower(33), p) {
    item = i;

    charstuff::Equip* e = dynamic_cast<charstuff::Equip*>(item);
    if (e != nullptr && e->slot != charstuff::TAROT) {
        bmp = asset::get_tower(34);

        int* f_mods = tower::get_player()->get_replace(e);
        for (int i = 5; i >= 0; --i)
            f_mods[i] = e->attrm[i] - f_mods[i];
        insert(new ValueWidget(&tower::get_player()->attr[tower::ATK], Point{ 40, 65 }, f_mods[tower::ATK]));
        insert(new ValueWidget(&tower::get_player()->attr[tower::DEF], Point{ 40, 77 }, f_mods[tower::DEF]));
        insert(new ValueWidget(&tower::get_player()->attr[tower::SPD], Point{ 87, 65 }, f_mods[tower::SPD]));
        insert(new ValueWidget(&tower::get_player()->attr[tower::MAG], Point{ 87, 77 }, f_mods[tower::MAG]));
        delete[] f_mods;
    }

    insert(new TextWidget(item->desc, Point{ 9, 22 }, asset::get_pixel_large(), 87, true));
    insert(new TextWidget(item->name, Point{ 9, 9 }, asset::get_pixel_large(), 87, true));
}


InventoryWindow::InventoryWindow(charstuff::Inventory* inv, bool player_inv, Point p) : Window(nullptr, p) {
    u_16 len = 0;
    widg_func cfunc = nullptr;
    if (!player_inv) {
        bmp = asset::get_tower(30);
        len = 18;
        cfunc = wfunc_item_take;
    } else {
        bmp = asset::get_tower(29);
        len = 42;
        cfunc = wfunc_item_interact;
    }
    for (u_16 i = 0; i < len; ++i)
        insert(new ItemWidget(inv->get(i), Point{ (player_inv ? 9 : 17) + 17*(int)(i%6), 9 + 17*(int)(i/6) }, cfunc, asset::get_tower(32)));
}
