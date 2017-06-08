#include <cmath>
#include <unordered_set>
#include <allegro5/allegro.h>
#include "../../include/engine/state.h"
#include "../../include/tower/object.h"
#include "../../include/tower/actor.h"
// TODO remove this later
#include "../../include/_debug.h"


namespace engine {


int State::update(ALLEGRO_EVENT* e) {
    if (e->type == ALLEGRO_EVENT_TIMER) {
        update_frame();
        return 0;
    } else {
        return update_event(e);
    }
}

/**
    -----------------
    UIState FUNCTIONS
    -----------------
**/

int UIState::update_event(ALLEGRO_EVENT* e) {
    switch (e->type) {
        case ALLEGRO_EVENT_MOUSE_AXES:
            {
                for (WidgetList::iterator it = widget.begin(); it != widget.end(); ++it) {
                    bool inb = (*it)->in_bounds(e->mouse.x, e->mouse.y);
                    if (!(*it)->mouse_on && inb) {
                        (*it)->mouse_on = true;
                        if ((*it)->mouse_on_func) return (*it)->mouse_on_func(it->get());
                        break;
                    } else if ((*it)->mouse_on && !inb) {
                        (*it)->mouse_on = false;
                        if ((*it)->mouse_off_func) return (*it)->mouse_off_func(it->get());
                        break;
                    }
                }
                break;
            }
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            {
                for (WidgetList::iterator it = widget.begin(); it != widget.end(); ++it) {
                    if ((*it)->in_bounds(e->mouse.x, e->mouse.y)) {
                        if ((*it)->click_func) return (*it)->click_func(it->get());
                        break;
                    }
                }
                break;
            }
    }
    return 0;
}

void UIState::update_frame() {
    update_animation();
    for (WidgetList::reverse_iterator it = widget.rbegin(); it != widget.rend(); ++it)
        (*it)->draw();
}

void UIState::update_animation() {
    //DEBUG_PRINT("update_animation() called.");
    //u_16 cnt = 1;
    AnimationList::iterator prev_it = animation.before_begin();
    for (AnimationList::iterator it = animation.begin(); it != animation.end(); ++it) {
        //DEBUG_PRINT("CURRENT ANIMATION: " << cnt++);
        if ((*it)->update_frame()) {
            animation.erase_after(prev_it);
            //DEBUG_PRINT("\tErased animation.");
            it = prev_it;
        } else
            prev_it = it;
    }
}

Widget* UIState::insert(Widget* w) {
    widget.emplace_back(w);
    return widget.back().get();
}

void UIState::remove(Widget* w) {
    for (WidgetList::iterator it = widget.begin(); it != widget.end(); ++it) {
        if (it->get() == w) {
            widget.erase(it);
            break;
        }
    }
}

/**
    -----------------------
    MainMenuState FUNCTIONS
    -----------------------
**/

int MainMenuState::update_event(ALLEGRO_EVENT* e) {
    // TODO stuff
    return 0;
}

void MainMenuState::update_frame() {
    // TODO stuff
}

/**
    --------------------
    TowerState FUNCTIONS
    --------------------
**/

TowerState::TowerState(tower::TowerData* td, bool from_bottom) {
    ctower = td->tower_generator(td, from_bottom);

    cfloor = ctower->floor[(from_bottom ? 0 : ctower->size-1)];
    u_16 iindex = (from_bottom ? ctower->t_entr.tile : ctower->t_exit.tile);

    camera.x = 18*(iindex%cfloor->size) - 30;
    camera.y = 18*(iindex/cfloor->size) - 30;

    DEBUG_PRINT("Adding widgets...");
    reset_twidget();
    ts = WAIT_FOR_INPUT;
    DEBUG_PRINT("End of TowerState initialization.");
}

int TowerState::update_event(ALLEGRO_EVENT* e) {
    if (UIState::update_event(e)) {
        return 1;
    } else if (ts == WAIT_FOR_INPUT) {
        if (e->type == ALLEGRO_EVENT_KEY_DOWN) {
            ts = PLAYER_ANIMATION;
            switch (e->keyboard.keycode) {
                case ALLEGRO_KEY_LEFT:
                    ts = interact(-18, 0);
                    break;
                case ALLEGRO_KEY_UP:
                    ts = interact(0, -18);
                    break;
                case ALLEGRO_KEY_RIGHT:
                    ts = interact(18, 0);
                    break;
                case ALLEGRO_KEY_DOWN:
                    ts = interact(0, 18);
                    break;
                case ALLEGRO_KEY_SPACE:
                    break;
                default:
                    ts = WAIT_FOR_INPUT;
                    break;
            }
        }
    }
    return 0;
}

void TowerState::update_frame() {
    static tower::Action nxt_action = { nullptr, nullptr, tower::get_player() };

    if (ts != WAIT_FOR_INPUT) {
        refresh_objects();
        if (animation.empty()) {
            if (!nxt_action.act)
                ts = (TOWER_STATE)((ts+1)%3);

            if (ts == OTHER_ANIMATION) {
                if (!nxt_action.act)
                    nxt_action = ctower->get_next_action();
                //DEBUG_PRINT("PLAYER : " << tower::get_player()->pos.x/18 << ", " << tower::get_player()->pos.y/18);
                //std::forward_list<tower::Action> acts;
                std::unordered_set<tower::Actor*> anim_act;
                while (nxt_action.act != nullptr && anim_act.find(nxt_action.actor) == anim_act.end()) {
                    anim_act.insert(nxt_action.actor);
                    insert_animation(nxt_action.act(nxt_action.actor, nxt_action.target, ctower));
                    nxt_action = ctower->get_next_action();
                }
            }
        }
    }

    for (TileIterator it = twidget.begin(); it != twidget.end(); ++it)
        it->draw();
    for (ObjectIterator oit = owidget.begin(); oit != owidget.end(); ++oit)
        (*oit)->draw();

    UIState::update_frame();
}

TOWER_STATE TowerState::interact(int dx, int dy) {
    if (dx > 0)
        tower::get_player()->face = tower::EAST;
    else if (dy < 0)
        tower::get_player()->face = tower::NORTH;
    else if (dx < 0)
        tower::get_player()->face = tower::WEST;
    else if (dy > 0)
        tower::get_player()->face = tower::SOUTH;

    Point* ppos = &tower::get_player()->pos;
    tower::Tile* trg = cfloor->tile[((ppos->x+dx)/18) + (cfloor->size*((ppos->y+dy)/18))];
    if (trg) {
        if (!trg->occupy) {
            // MOVE
            animation.emplace_front(new CameraAnimation(animation_camlinmove, 9, Point{dx,dy}, this));
            animation.emplace_front(new SpriteAnimation(&tower::get_player()->bmp, 12, 8,
                                                            4, asset::get_tower(2),
                                                            3, asset::get_tower(1),
                                                            4, asset::get_tower(3),
                                                            1, asset::get_tower(1)));
            animation.emplace_front(new MoveAnimation(animation_linmove, 9, Point{dx,dy}, ppos));
        } else if (tower::Actor* target = dynamic_cast<tower::Actor*>(trg->occupy)) {
            // ATTACK
            insert_animation(tower::action_attack(tower::get_player(), target, ctower));
        } else {
            return WAIT_FOR_INPUT;
        }
        return PLAYER_ANIMATION;
    }
    return WAIT_FOR_INPUT;
}

/*
bool TowerState::tile_in_frame(u_16 t, int dx, int dy) {
    if (-dx >= t%cfloor->size && dx <= cfloor->size-(t%cfloor->size)
            && -dy >= t/cfloor->size && dy <= cfloor->size-(t/cfloor->size)) {
        u_16 nt = t + (cfloor->size * dy) + dx;
        int r = (nt%cfloor->size - (camera.x/18)) + (nt/cfloor->size - (camera.y/18));
        return (r >= MIN_DRAW && r <= MAX_DRAW && cfloor->tile[nt] != nullptr);
    }
    return false;
}
*/

void TowerState::insert_animation(tower::AnimationList anim) {
    for (tower::AnimationList::iterator it = anim.begin(); it != anim.end(); ++it)
        animation.emplace_front(*it);
}

void TowerState::insert_tile(TowerIterator it, u_16 t) {
    if (cfloor->tile[t]) {
        TileIterator it2 = it.first;
        ++it2;
        if ((it2 == twidget.end() || it2->t_index != t) && (it.first == twidget.before_begin() || it.first->t_index != t)) {
            std::shared_ptr<ObjectWidget> obj(nullptr);
            // if tile has an object, create widget for that
            if (cfloor->tile[t]->occupy) {
                obj.reset(new ObjectWidget(cfloor->tile[t]->occupy, cfloor, &camera));
                owidget.insert_after(it.second, obj)->get();
                //DEBUG_PRINT("obj = " << obj);
            }

            // create widget for tile
            TileIterator it1 = twidget.insert_after(it.first, TileWidget(cfloor, t, &camera, /*&twidget,*/ obj.get()));
            if (obj.get()) obj->parent = &(it1->obj);

            // load TileWidgets to east, north, etc...
            u_16 enws_i[4] = { (it1->t_index%cfloor->size > 0 ? it1->t_index-1 : NO_SUCH_TILE),
                        (it1->t_index >= cfloor->size ? it1->t_index-cfloor->size : NO_SUCH_TILE),
                        (it1->t_index%cfloor->size < cfloor->size-1 ? it1->t_index+1 : NO_SUCH_TILE),
                        (it1->t_index < cfloor->size*(cfloor->size-1) ? it1->t_index+cfloor->size : NO_SUCH_TILE) };
            for (it2 = twidget.begin(); it2 != twidget.end(); ++it2) {
                bool finished = true;
                for (int i = 3; i >= 0; --i) {
                    if (enws_i[i] == it2->t_index) {
                        it1->enws[i] = &*it2;
                        it2->enws[(i+2)%4] = &*it1;
                    }
                    if (enws_i[i] != NO_SUCH_TILE && it1->enws[i] == nullptr)
                        finished = false;
                }
                if (finished) break;
            }

            //DEBUG_PRINT("\tAdded (" << (t%cfloor->size) << ", " << (t/cfloor->size) << ").");
        }
    }
}

void TowerState::add_row(TowerIterator prev, int r, int min_dy, int max_dy) {
    //if (min_dy != max_dy) DEBUG_PRINT("\tadd_row(" << r << ") " << min_dy << " -> " << max_dy);
    for (int dy = max_dy; dy >= min_dy; --dy) {
        if ((camera.y/18) + dy >= 0 && (camera.y/18) + dy < (int)cfloor->size) {
            int dx = r-dy;
            if ((camera.x/18) + dx >= 0 && (camera.x/18) + dx < (int)cfloor->size) {
                insert_tile(prev, (u_16)((cfloor->size*((camera.y/18) + dy)) + (camera.x/18) + dx));
            }
        }
    }
}

void TowerState::add_rows(TowerIterator prev, int min_r, int max_r) {
    for (int d = max_r; d >= min_r; --d)
        add_row(prev, d, (d/2) - 6, std::ceil(0.5*d) + 6);
}

void TowerState::reset_twidget() {
    owidget.clear();
    twidget.clear();
    TowerIterator prev = TowerIterator(twidget.before_begin(), owidget.before_begin());
    add_rows(prev, MIN_DRAW, MAX_DRAW);
    /*
    for (int d = MIN_DRAW; d <= MAX_DRAW; ++d) {
        int min_dy = std::floor(0.5*d) - 6;
        int max_dy = std::ceil(0.5*d) + 6;
        for (int dy = min_dy; dy <= max_dy; ++dy) {
            if ((camera.y/18) + dy >= 0 && (camera.y/18) + dy < cfloor->size) {
                int dx = d-dy;
                if ((camera.x/18) + dx >= 0 && (camera.x/18) + dx < cfloor->size) {
                    it = insert_tile(it, (u_16)((cfloor->size*((camera.y/18) + dy)) + (camera.x/18) + dx));
                }
            }
        }
    }
    */
}

bool widg_comp(std::shared_ptr<ObjectWidget> w1, std::shared_ptr<ObjectWidget> w2) {
    //DEBUG_PRINT("\t\tw1 = " << w1.get() << ", w2 = " << w2.get());
    ObjectWidget* ow1 = w1.get();
    ObjectWidget* ow2 = w2.get();
    if (ow1 != nullptr && ow2 != nullptr) {
        int r1 = ow1->obj->pos.x/18 + ow1->obj->pos.y/18;
        int r2 = ow2->obj->pos.x/18 + ow2->obj->pos.y/18;
        if (r1 != r2)
            return r1 < r2;
        else
            return ((ow1->obj->pos.x/18 - ow1->obj->pos.y/18) > (ow2->obj->pos.x/18 - ow2->obj->pos.y/18));
    }
    return false;
}

void TowerState::refresh_objects() {
    //DEBUG_PRINT("refresh_objects() called.");
    cfloor->refresh_objects();
    //owidget.sort(widg_comp);

    TowerIterator it = TowerIterator(twidget.begin(), owidget.begin());
    ObjectIterator prev = owidget.before_begin();
    while (it.second != owidget.end()) {

        if (ObjectWidget* ow = it.second->get()) {
            //DEBUG_PRINT("\tit.second.pos = (" << ow->obj->pos.x << ", " << ow->obj->pos.y << ") -> (" << ow->obj->pos.x/18 << ", " << ow->obj->pos.y/18 << ")");

            u_16 n_index = (ow->obj->pos.x/18) + (cfloor->size*(ow->obj->pos.y/18));
            while (it.first->obj != ow && it.first->t_index != n_index) {
                //DEBUG_PRINT("\t\tit.first.pos = (" << it.first->t_index%cfloor->size << ", " << it.first->t_index/cfloor->size << ")");
                ++it.first;
            }

            ow->reset_crd(&camera);
            if (it.first->t_index != n_index) {
                for (int i = 3; i >= 0; --i) {
                    /*if (it.first->enws[i])
                        DEBUG_PRINT("\t\t" << i << " (" << it.first->enws[i]->t_index%cfloor->size << ", " << it.first->enws[i]->t_index/cfloor->size << ")");
                    else
                        DEBUG_PRINT("\t\t" << i << " NULL");*/
                    if (it.first->enws[i] != nullptr && it.first->enws[i]->t_index == n_index) {
                        cfloor->tile[n_index]->occupy = ow->obj;
                        cfloor->tile[it.first->t_index]->occupy = nullptr;
                        it.first->enws[i]->obj = ow;
                        if (*ow->parent == ow) *ow->parent = nullptr;
                        ow->parent = &it.first->enws[i]->obj;
                        break;
                    }
                }
                //DEBUG_PRINT("\t1 " << ow << " (" << ow->obj->pos.x/18 << ", " << ow->obj->pos.y/18 << ")");
                //DEBUG_PRINT("AAAAAAAA");
            } else if (ow != it.first->obj) {
                if (*ow->parent == ow) *ow->parent = nullptr;
                it.first->obj = ow;
                ow->parent = &it.first->obj;
                //DEBUG_PRINT("\t2 " << ow << " (" << ow->obj->pos.x/18 << ", " << ow->obj->pos.y/18 << ")");
            }
            //DEBUG_PRINT("\t" << ow << " (" << ow->obj->pos.x/18 << ", " << ow->obj->pos.y/18 << ")");
        } else {
            DEBUG_PRINT("NON-OBJECT WIDGET FOUND");
            DEBUG_PRINT("\tPTR: " << it.second->get());
            DEBUG_PRINT("\tPREVIOUS: " << prev->get());
            throw "NON-OBJECT WIDGET";
        }
        prev = it.second++;
    }

    //DEBUG_PRINT("Sorting...");
    owidget.sort(widg_comp);
    /*for (ObjectIterator oit = owidget.begin(); oit != owidget.end(); ++oit)
        DEBUG_PRINT("\t" << oit->get());*/
}

void TowerState::move_camera(int dx, int dy) {
    //static bool just_changed = false;
    //DEBUG_PRINT("move_camera() called.");
    //DEBUG_PRINT("\tCamera: (" << ((camera.x+dx)/18) << ", " << ((camera.y+dy)/18) << ").");
    //u_16 sz = 0;
    int fdx = dy-dx;
    int fdy = (camera.x+camera.y)/2 - (camera.x+dx+camera.y+dy)/2;

    TowerIterator prev = TowerIterator(twidget.before_begin(), owidget.before_begin());
    add_rows(prev, MIN_DRAW, ((twidget.begin()->t_index%cfloor->size) - (camera.x/18)) + ((twidget.begin()->t_index/cfloor->size) - (camera.y/18)) - 1);
    TileIterator it = twidget.begin();

    while (it != twidget.end()) {
        ObjectIterator objit = prev.second;
        if (it->obj != nullptr && (++objit)->get() != it->obj) {
            DEBUG_PRINT("ERROR DETECTED");
            for (ObjectIterator oit = owidget.begin(); oit != owidget.end(); ++oit)
                DEBUG_PRINT("\t" << oit->get());

            DEBUG_PRINT("TILE: (" << it->t_index%cfloor->size << ", " << it->t_index/cfloor->size << ")");
            DEBUG_PRINT("\tPREVIOUS: " << prev.second->get());
            DEBUG_PRINT("\tEXPECTED: " << it->obj);
            DEBUG_PRINT("\tACTUAL: " << objit->get());
            throw "MISALIGNMENT OF OBJ AND TILE";
        }
        //if (just_changed)
        //DEBUG_PRINT("\t(" << it->t_index%cfloor->size << ", " << it->t_index/cfloor->size << ")");
        //DEBUG_PRINT("\t\t" << it->obj);
        //++sz;
        it->crd.x += fdx;
        it->crd.y += fdy;
        if (it->obj) {
            it->obj->crd.x += fdx;
            it->obj->crd.y += fdy;
        }
        if (it->crd.x >= 436 || it->crd.y >= 355 || it->crd.x < -72 || it->crd.y < -50) {
            if (it->obj)
                owidget.erase_after(prev.second);
            //DEBUG_PRINT("\t  ^ Erased.");
            it = twidget.erase_after(prev.first);
        } else {
            //DEBUG_PRINT("\tAdvancing...");
            if (it->obj)
                ++prev.second;
            prev.first = it;
            //DEBUG_PRINT("it.obj = " << it->obj << ", prev.obj = " << ((prev.second == owidget.before_begin() || prev.second == owidget.end()) ? nullptr : &*prev.second));
            int r_prev = ((prev.first->t_index%cfloor->size) - (camera.x/18))
                            + ((prev.first->t_index/cfloor->size) - (camera.y/18));
            //DEBUG_PRINT("FIRST: " << &*it);
            ++it;
            //DEBUG_PRINT("\tSECOND: " << &*it);
            if (it != twidget.end()) {
                int r_it = ((it->t_index%cfloor->size) - (camera.x/18))
                                + ((it->t_index/cfloor->size) - (camera.y/18));
                //if (this_row) DEBUG_PRINT("\tClause 1 (part 1) invoked.");
                if (r_it > r_prev) {
                    //DEBUG_PRINT("\t...adding (" << ((camera.x/18) + dx) << ", " << ((camera.y/18) + dy) << ") in next row...");
                    add_row(prev, r_it, (r_it/2)-6, it->t_index/cfloor->size - (camera.y/18));
                    add_rows(prev, r_prev+1, r_it-1);
                    add_row(prev, r_prev, prev.first->t_index/cfloor->size - (camera.y/18), ceil(0.5*r_prev)+6);
                }
            } else if (r_prev < MAX_DRAW) {
                // implies that <it> points to the last TileWidget in twidget AND last row is incomplete
                //DEBUG_PRINT("\tClause 2 invoked.");
                add_rows(prev, r_prev+1, MAX_DRAW);
                add_row(prev, r_prev, prev.first->t_index/cfloor->size, ceil(0.5*r_prev)+6);
            }

            it = prev.first;
            ++it;
        }
    }

    //just_changed = ((camera.x+dx)/18 != camera.x/18 || (camera.y+dy)/18 != camera.y/18);
    camera.x += dx;
    camera.y += dy;
    //DEBUG_PRINT("\tTotal size: " << sz);
    /*for (ObjectIterator oit = owidget.begin(); oit != owidget.end(); ++oit)
        DEBUG_PRINT("\t" << oit->get());*/
}


}
