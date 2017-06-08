#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>

#include <stdlib.h>
#include <ctime>

#include "../../include/engine/state.h"
#include "../../include/asset/asset.h"
#include "../../include/tower/actor.h"
#include "../../include/graphics.h"
// TODO remove when done with debugging
#include "../../include/_debug.h"

// TODO remove later
#include "../../include/tower/towers_data.h"

#define SCREEN_WIDTH    400
#define SCREEN_HEIGHT   300

namespace engine {

static State* cstate;
static State* nstate;

static ALLEGRO_DISPLAY*     al_display;
static ALLEGRO_EVENT_QUEUE* al_queue;
static ALLEGRO_TIMER*       al_timer;


State* get_state() {
    return cstate;
}

void set_state(State* ns) {
    nstate = ns;
}

static void swap_state() {
    if (nstate) {
        ALLEGRO_EVENT e;
        ALLEGRO_BITMAP* bmp = al_create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
        for (char a = 250; a >= 0; a -= 5) {
            do { al_wait_for_event(al_queue, &e); } while (e.type != ALLEGRO_EVENT_TIMER);
            cstate->update(&e);
            al_set_target_bitmap(bmp);
            al_clear_to_color(al_map_rgba(a, a, a, 255-a));
            al_set_target_bitmap(al_get_backbuffer(al_display));
            al_draw_bitmap(bmp, 0, 0, 0);
            al_flip_display();
        }
        delete cstate;
        cstate = nstate;
        nstate = nullptr;
        for (char a = 250; a >= 0; a -= 5) {
            do { al_wait_for_event(al_queue, &e); } while (e.type != ALLEGRO_EVENT_TIMER);
            cstate->update(&e);
            al_set_target_bitmap(bmp);
            al_clear_to_color(al_map_rgba(255-a, 255-a, 255-a, a));
            al_set_target_bitmap(al_get_backbuffer(al_display));
            al_draw_bitmap(bmp, 0, 0, 0);
            al_flip_display();
        }
        al_destroy_bitmap(bmp);
    }
}



void init() {
    // init Allegro
    al_display = NULL;
    al_queue = NULL;
    al_timer = NULL;

    if (!al_init())
        throw "Allegro library failed to initialize.";
    if (!al_install_mouse())
        throw "Allegro mouse add-on failed to initialize.";
    if (!al_install_keyboard())
        throw "Allegro keyboard add-on failed to initialize.";
    if (!al_init_image_addon())
        throw "Allegro image add-on failed to initialize.";
    if (!al_init_font_addon())
        throw "Allegro font add-on failed to initialize.";
    if (!al_init_ttf_addon())
        throw "Allegro ttf add-on failed to initialize.";

    al_timer = al_create_timer(1.0/30);
    if (!al_timer)
        throw "Allegro timer failed to initialize.";

    al_display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!al_display)
        throw "Allegro display failed to initialize.";

    al_queue = al_create_event_queue();
    if (!al_queue)
        throw "Allegro event queue failed to initialize.";

    al_register_event_source(al_queue, al_get_display_event_source(al_display));
    al_register_event_source(al_queue, al_get_timer_event_source(al_timer));
    al_register_event_source(al_queue, al_get_mouse_event_source());
    al_register_event_source(al_queue, al_get_keyboard_event_source());

    // set seed for rand()
    srand(1);
    //srand(time(NULL));

    // initialize stuff
    asset::init();
    tower::init();
    tower::init_player();
    tower::new_player();
    graphics::init();

    // init main menu
    DEBUG_PRINT("Initializing state...");
    cstate = new TowerState(tower::get_t_debug(), true); //MainMenuState();
    nstate = nullptr;
    DEBUG_PRINT("State initialized.");
}

void run() {
    DEBUG_PRINT("run() called.");
    ALLEGRO_EVENT e;
    al_start_timer(al_timer);

    while (true) {
        al_wait_for_event(al_queue, &e);
        if (e.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;

        if (e.type == ALLEGRO_EVENT_TIMER)
            graphics::begin_frame();
        cstate->update(&e);
        if (e.type == ALLEGRO_EVENT_TIMER)
            graphics::end_frame();

        swap_state();
    }
}

void deconstruct() {
    if (al_queue)
        al_destroy_event_queue(al_queue);
    if (al_display)
        al_destroy_display(al_display);
    if (al_timer)
        al_destroy_timer(al_timer);
}


}


int main() {
    try {
        engine::init();
        engine::run();
        engine::deconstruct();
    } catch (const char* cstr) {
        DEBUG_PRINT(cstr); // TODO create error message box
        engine::deconstruct();
        return 1;
    }
    return 0;
}
