#ifndef AE_ANIMATION_H
#define AE_ANIMATION_H

#include <cstdarg>
#include <forward_list>
#include <allegro5/allegro.h>
#include "../../types.h"
#include "../../point.h"
#include "../../_debug.h"

namespace engine {


struct Animation {
    void (*afunc)(Animation*);
    u_16 frame;     // current frame of animation
    u_16 speed;     // total number of frames in animation

    Animation(void (*)(Animation*), u_16);
    virtual bool update_frame();
};

struct DelayAnimation: virtual public Animation {
    u_16 delay;

    DelayAnimation(void (*)(Animation*), u_16, u_16);
    bool update_frame();
};

struct SpriteAnimation: public Animation {
    ALLEGRO_BITMAP** pbmp;

    int* delay_arr;
    ALLEGRO_BITMAP** bmp_arr;

    SpriteAnimation(ALLEGRO_BITMAP**, u_16, int, ...);
};

struct FlashAnimation: public DelayAnimation {
    ALLEGRO_COLOR* tint;
    ALLEGRO_COLOR original;

    FlashAnimation(ALLEGRO_COLOR*, u_16, u_16);
};

struct FadeAnimation: virtual public Animation {
    ALLEGRO_COLOR* tint;
    ALLEGRO_COLOR fade_to;
    int dc[4];

    FadeAnimation(ALLEGRO_COLOR*, ALLEGRO_COLOR, u_16);
};

struct DelayFadeAnimation: public FadeAnimation, public DelayAnimation {
    DelayFadeAnimation(ALLEGRO_COLOR*, ALLEGRO_COLOR, u_16, u_16);
};

struct MoveAnimation: virtual public Animation {
    Point* point;
    Point dist;

    MoveAnimation(void (*)(Animation*), u_16, Point, Point*);
};

struct DelayMoveAnimation: public MoveAnimation, public DelayAnimation {
    DelayMoveAnimation(void (*)(Animation*), u_16, u_16, Point, Point*);
};

struct VelocityAnimation: public Animation {
    Point* point;
    Point vel;

    VelocityAnimation(void (*)(Animation*), u_16, Point, Point*);
};

struct ArithmeticAnimation: public DelayAnimation {
    u_16* value;
    int change;

    ArithmeticAnimation(void (*)(Animation*), u_16, u_16*, int);
};

template <typename T> void animation_funccall(Animation*);
template <typename S, typename T> void animation_funccall2(Animation*);

template <typename T>
struct FunctionAnimation: public DelayAnimation {
    void (T::*ffunc)();
    T* container;

    FunctionAnimation(void (T::*f)(), u_16 s, T* c) : Animation(animation_funccall<T>, s + 1), DelayAnimation(animation_funccall<T>, s, s + 1) {
        ffunc = f;
        container = c;
    }
};

template <typename S, typename T>
struct FunctionAnimation2: public DelayAnimation {
    S* container;
    T* element;
    void (S::*ffunc)(T*);

    FunctionAnimation2(void (S::*f)(T*), u_16 s, S* c, T* e) : Animation(animation_funccall2<S, T>, s + 1), DelayAnimation(animation_funccall2<S, T>, s, s + 1) {
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
