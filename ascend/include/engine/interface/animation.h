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

template <typename T> void animation_funccall(Animation*);
template <typename S, typename T> void animation_funccall2(Animation*);

template <typename T>
struct FunctionAnimation: public DelayAnimation {
    void (T::*ffunc)();
    T* container;

    FunctionAnimation(void (T::*f)(), u_16 s, T* c) : DelayAnimation(animation_funccall<T>,s) {
        ffunc = f;
        container = c;
    }
};

template <typename S, typename T>
struct FunctionAnimation2: public DelayAnimation {
    S* container;
    T* element;
    void (S::*ffunc)(T*);

    FunctionAnimation2(void (S::*f)(T*), u_16 s, S* c, T* e) : DelayAnimation(animation_funccall2<S, T>, s) {
        ffunc = f;
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


/**  Function call  **/
template <typename T>
void animation_funccall(Animation* a) {
    FunctionAnimation<T>* fa = dynamic_cast<FunctionAnimation<T>*>(a);
    ((fa->container)->*(fa->ffunc))();
}

/**  Element removal  **/
template <typename S, typename T>
void animation_funccall2(Animation* a) {
    FunctionAnimation2<S, T>* fa = dynamic_cast<FunctionAnimation2<S, T>*>(a);
    (fa->container->*(fa->ffunc))(fa->element);
}


}

#endif // AE_ANIMATION_H
