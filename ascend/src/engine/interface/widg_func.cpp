#include "../../../include/engine/state.h"
#include "../../../include/tower/actor.h"

namespace engine {

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


}
