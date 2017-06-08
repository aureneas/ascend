#ifndef AE_STATE_H
#define AE_STATE_H

#include <forward_list>
#include <list>
#include <memory>
#include "interface/widget.h"
#include "interface/animation.h"
#include "../point.h"
#include "../tower/tower.h"
// TODO remove later
#include "../_debug.h"

namespace engine {


class State {
    protected:
        virtual int update_event(ALLEGRO_EVENT*) { return 0; }
        virtual void update_frame() = 0;
    public:
        virtual ~State() {}
        int update(ALLEGRO_EVENT*);
};

typedef std::list<std::unique_ptr<Widget> > WidgetList;
typedef std::forward_list<std::unique_ptr<Animation> > AnimationList;

class UIState: public State {
    protected:
        WidgetList widget;
        AnimationList animation;
        int update_event(ALLEGRO_EVENT*);
        void update_frame();
        void update_animation();
    public:
        virtual ~UIState() {}
        Widget* insert(Widget*);
        void remove(Widget*);
};

class MainMenuState: public UIState {
    protected:
        ~MainMenuState() {}
        int update_event(ALLEGRO_EVENT*);
        void update_frame();
};


#define NO_SUCH_TILE 65535

#define MIN_DRAW -16
#define MAX_DRAW 22

typedef std::forward_list<TileWidget>::iterator TileIterator;
typedef std::forward_list<std::shared_ptr<ObjectWidget> >::iterator ObjectIterator;
typedef std::pair<TileIterator, ObjectIterator> TowerIterator;

enum TOWER_STATE : int {
    WAIT_FOR_INPUT = 0,
    PLAYER_ANIMATION = 1,
    OTHER_ANIMATION = 2
};

class TowerState: public UIState {
    private:
        TOWER_STATE ts;

        tower::Tower* ctower;
        tower::Floor* cfloor;
        std::forward_list<TileWidget> twidget;
        std::forward_list<std::shared_ptr<ObjectWidget> > owidget;

        u_16 frame;
    protected:
        void insert_animation(tower::AnimationList);

        //bool tile_in_frame(u_16, int, int);
        void insert_tile(TowerIterator, u_16);
        void add_row(TowerIterator, int, int, int);
        void add_rows(TowerIterator, int, int);

        TOWER_STATE interact(int, int);

        int update_event(ALLEGRO_EVENT*);
        void update_frame();
    public:
        Point camera;

        TowerState(tower::TowerData*, bool);
        ~TowerState() {}
        void reset_twidget();
        void refresh_objects();
        void move_camera(int, int);
};

struct CameraAnimation: public MoveAnimation {
    TowerState* ts;
    CameraAnimation(void (*f)(Animation*), u_16 s, Point d, TowerState* t) : MoveAnimation(f, s, d, nullptr), ts(t) {}
};


}

#endif // AE_STATE_H
