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

    virtual void draw(int, int);
    virtual bool in_bounds(int, int);

    Widget();
    Widget(ALLEGRO_BITMAP*, Point);
    virtual ~Widget() {}
    virtual int update_event(ALLEGRO_EVENT* e, int x, int y) { return 0; }
};

struct FWidget: public Widget {
    widg_func mouse_on_func;
    widg_func mouse_off_func;
    widg_func click_func;

    FWidget();
    virtual int update_event(ALLEGRO_EVENT*, int, int);
};

struct TextWidget: public Widget {
    ALLEGRO_USTR* text;
    ALLEGRO_FONT* font;
    bool protect_text;
    int flags;
    int width;

    TextWidget(ALLEGRO_USTR*, Point, ALLEGRO_FONT*, int = 0, bool = false, int = 0);
    TextWidget(int, Point, int = 0);
    virtual ~TextWidget();
    virtual void init();
    virtual void draw(int, int);
    bool in_bounds(int, int);
};

struct ValueWidget: public TextWidget {
    u_16* value;
    u_16 last;
    int mod;

    ValueWidget(u_16*, Point, int = 0, int = ALLEGRO_ALIGN_CENTER);
    void init();
    void draw(int, int);
};

struct BarWidget: public Widget {
    int width;
    int height;
    u_16* cur_value;
    u_16 cur_last;
    u_16* max_value;
    u_16 max_last;
    float perc;

    BarWidget(u_16*, u_16*, Point, int, int, ALLEGRO_COLOR);
    void draw(int, int);
};

struct TileWidget: public FWidget {
    tower::Floor* cfloor;
    TileWidget* enws[4];
    Widget* obj;
    u_16 t_index;

    TileWidget(tower::Floor*, u_16, Point*, /*std::forward_list<TileWidget>*,*/ Widget* = nullptr);
    ~TileWidget();

};

struct ObjectWidget: public FWidget {
    tower::Object* obj;
    Widget** parent;

    ObjectWidget(tower::Object*, tower::Floor*, Point*);
    void reset_crd(Point*);
    virtual void draw(int, int);
};

struct ItemWidget: public FWidget {
    charstuff::Item** item;

    ItemWidget(charstuff::Item**, Point, widg_func = nullptr, ALLEGRO_BITMAP* = nullptr);
    int update_event(ALLEGRO_EVENT*, int, int);
    void draw(int, int);
};

typedef std::list<std::unique_ptr<Widget> > WidgetList;

struct Window: public Widget {
    WidgetList child;

    Window(ALLEGRO_BITMAP* b, Point p) : Widget(b, p) {}
    Widget* insert(Widget*);
    void draw(int, int);
    int update_event(ALLEGRO_EVENT*, int, int);
};

struct InfoWindow: public Window {
    charstuff::Item* item;

    InfoWindow(charstuff::Item*, Point);
};

struct InventoryWindow: public Window {
    InventoryWindow(charstuff::Inventory*, bool, Point);
};


int wfunc_item_mouse_on(Widget*);
int wfunc_item_mouse_off(Widget*);
int wfunc_item_take(Widget*);
int wfunc_item_interact(Widget*);


}

#endif // AE_WIDGET_H
