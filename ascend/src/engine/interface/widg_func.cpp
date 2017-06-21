#include "../../../include/engine/state.h"
#include "../../../include/tower/actor.h"
#include "../../../include/_debug.h"

namespace engine {

State* get_state();
void set_state(State*);


int cfunc_new_game(UIState* ui) {
    tower::new_player();
    // TODO change this
    return 1;
}

int cfunc_continue_game(UIState* ui) {
    // TODO stuff
    return 1;
}

int wfunc_item_mouse_on(Widget* w) {
    if (ItemWidget* it = dynamic_cast<ItemWidget*>(w)) {
        if (charstuff::Item* item = *(it->item)) {
            Widget* wnd = new InfoWindow(item, Point{ 0, 0 });
            Point afx = { 13, 0 };
            if (get_state()->mouse_crd.x + al_get_bitmap_width(wnd->bmp) >= 387)
                afx.x -= al_get_bitmap_width(wnd->bmp);
            if (get_state()->mouse_crd.y + al_get_bitmap_height(wnd->bmp) >= 300)
                afx.y -= al_get_bitmap_height(wnd->bmp);
            get_state()->set_mouse_widget(wnd, afx);
        }
        return 1;
    }
    return 0;
}

int wfunc_item_mouse_off(Widget* w) {
    if (ItemWidget* iw = dynamic_cast<ItemWidget*>(w)) {
        if (charstuff::Item* item = *iw->item) {
            if (InfoWindow* wnd = dynamic_cast<InfoWindow*>(get_state()->get_mouse_widget())) {
                if (item == wnd->item) {
                    get_state()->clear_mouse_widget();
                    return 1;
                }
            }
        }
    }
    return 0;
}

int wfunc_item_take(Widget* w) {
    if (ItemWidget* iw = dynamic_cast<ItemWidget*>(w)) {
        if (charstuff::Item* item = *iw->item) {
            tower::get_player()->insert(item);
            return 1;
        }
    }
    return 0;
}

int wfunc_item_interact(Widget* w) {
    if (ItemWidget* iw = dynamic_cast<ItemWidget*>(w)) {
        if (charstuff::Item* item = *iw->item) {
            item->interact();
            return 1;
        }
    }
    return 0;
}


}
