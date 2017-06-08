#ifndef AE_WIDGET_H
#define AE_WIDGET_H

#include <forward_list>
#include <allegro5/allegro.h>
#include "../../point.h"
#include "../../tower/tower.h"
#include "../../tower/actor.h"

namespace engine {


struct Widget;
typedef int (*widg_func)(Widget*);

struct Widget {
    ALLEGRO_BITMAP* bmp;
    ALLEGRO_COLOR tint;
    Point crd;
    bool mouse_on;

    widg_func mouse_on_func;
    widg_func mouse_off_func;
    widg_func click_func;

    virtual void draw();
    virtual bool in_bounds(int, int);

    Widget();
    virtual ~Widget() {}
};

struct TextWidget: public Widget {
    ALLEGRO_USTR* text;

    TextWidget(int, Point);
    void draw();
    bool in_bounds(int, int);
};

struct TileWidget: public Widget {
    tower::Floor* cfloor;
    TileWidget* enws[4];
    Widget* obj;
    u_16 t_index;

    TileWidget(tower::Floor*, u_16, Point*, /*std::forward_list<TileWidget>*,*/ Widget* = nullptr);
    ~TileWidget();

};

struct ObjectWidget: public Widget {
    tower::Object* obj;
    Widget** parent;

    ObjectWidget(tower::Object*, tower::Floor*, Point*);
    void reset_crd(Point*);
    virtual void draw();
};


}

#endif // AE_WIDGET_H
