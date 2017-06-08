#ifndef AE_ANIMATION_H
#define AE_ANIMATION_H

#include <cstdarg>
#include <forward_list>
#include <allegro5/allegro.h>
#include "../../types.h"
#include "../../point.h"

namespace engine {


struct Animation {
    void (*afunc)(Animation*);
    u_16 frame;     // current frame of animation
    u_16 speed;     // total number of frames in animation

    virtual bool update_frame();
};

struct SpriteAnimation: public Animation {
    ALLEGRO_BITMAP** pbmp;

    int* delay_arr;
    ALLEGRO_BITMAP** bmp_arr;

    SpriteAnimation(ALLEGRO_BITMAP**, u_16, int, ...);
};

struct FlashAnimation: public Animation {
    u_16 delay;

    ALLEGRO_COLOR* tint;
    ALLEGRO_COLOR original;

    FlashAnimation(ALLEGRO_COLOR*, u_16, u_16);
};

struct FadeAnimation: public Animation {
    ALLEGRO_COLOR* tint;
    int dc[4];

    FadeAnimation(ALLEGRO_COLOR*, ALLEGRO_COLOR, u_16);
};

struct MoveAnimation: public Animation {
    Point* point;
    Point dist;

    MoveAnimation(void (*f)(Animation*), u_16 s, Point d, Point* p) {
        afunc = f;
        speed = s;
        frame = 0;
        point = p;
        dist = d;
    }
};

struct VelocityAnimation: public Animation {
    Point* point;
    Point vel;

    VelocityAnimation(void (*f)(Animation*), u_16 s, Point v, Point* p) {
        afunc = f;
        speed = s;
        frame = 0;
        point = p;
        vel = v;
    }
};

struct DelayAnimation: public Animation {
    DelayAnimation(void (*f)(Animation*), u_16 s) {
        afunc = f;
        speed = s;
        frame = 0;
    }
    bool update_frame();
};

struct ArithmeticAnimation: public DelayAnimation {
    u_16* value;
    int change;

    ArithmeticAnimation(void (*f)(Animation*), u_16 s, u_16* v, int c) : DelayAnimation(f,s) {
        value = v;
        change = c;
    }
};

template <typename S, typename T> void animation_deletion(Animation*);

template <typename S, typename T>
struct DeletionAnimation: public DelayAnimation {
    S* container;
    T* element;
    void (S::*del_func)(T*);

    DeletionAnimation(void (S::*f)(T*), u_16 s, S* c, T* e) : DelayAnimation(animation_deletion<S, T>, s) {
        del_func = f;
        container = c;
        element = e;
    }
};


void animation_sprite(Animation*);
void animation_flash(Animation*);
void animation_linmove(Animation*);
void animation_camlinmove(Animation*);
void animation_linfade(Animation*);
void animation_fallvel(Animation*);
void animation_addition(Animation*);


/**  Element removal  **/
template <typename S, typename T>
void animation_deletion(Animation* a) {
    DeletionAnimation<S, T>* da = dynamic_cast<DeletionAnimation<S, T>*>(a);
    (da->container->*(da->del_func))(da->element);
}


}

#endif // AE_ANIMATION_H
