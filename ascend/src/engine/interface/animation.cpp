#include <vector>
#include <unordered_map>

#include "../../../include/engine/interface/animation.h"
#include "../../../include/engine/interface/widget.h"
#include "../../../include/engine/state.h"
// TODO remove later
#include "../../../include/_debug.h"


namespace engine {

#define SIGN(x) (((x) < 0) ? -1 : (((x) > 0) ? 1 : 0))
#define FRACTION(x) ((x)*1/2)



Animation::Animation(void (*f)(Animation*), u_16 s) {
    afunc = f;
    frame = 0;
    speed = s;
}

bool Animation::update_frame() {
    if (afunc) afunc(this);
    return (++frame == speed);
}

DelayAnimation::DelayAnimation(void (*f)(Animation*), u_16 d, u_16 s) : Animation(f, s) {
    delay = d;
}

bool DelayAnimation::update_frame() {
    if (delay > 0) {
        if (++frame == delay) {
            speed -= delay;
            frame = 0;
            delay = 0;
        }
    }

    //DEBUG_PRINT("frame = " << frame << ", speed = " << speed << ", delay = " << delay);
    if (delay == 0) {
        if (afunc) afunc(this);
        return (++frame == speed);
    }
    return false;
}


SpriteAnimation::SpriteAnimation(ALLEGRO_BITMAP** p, u_16 s, int count, ...) : Animation(animation_sprite, s), pbmp(p) {
    va_list param;
    va_start(param, count);

    delay_arr = new int[count/2];
    bmp_arr = new ALLEGRO_BITMAP*[count/2];
    for (int i = 0; i < count/2; ++i) {
        delay_arr[i] = va_arg(param, int);
        bmp_arr[i] = va_arg(param, ALLEGRO_BITMAP*);
    }
    va_end(param);
}

FlashAnimation::FlashAnimation(ALLEGRO_COLOR* c, u_16 d, u_16 l) : Animation(animation_flash, d + l), DelayAnimation(animation_flash, d, d + l) {
    tint = c;
    original = *c;
}

FadeAnimation::FadeAnimation(ALLEGRO_COLOR* c, ALLEGRO_COLOR d, u_16 s) : Animation(animation_linfade, s) {
    tint = c;
    fade_to = d;
}

DelayFadeAnimation::DelayFadeAnimation(ALLEGRO_COLOR* c, ALLEGRO_COLOR d, u_16 dl, u_16 s) : Animation(animation_linfade, s), FadeAnimation(c, d, s), DelayAnimation(animation_linfade, dl, s) {}

MoveAnimation::MoveAnimation(void (*f)(Animation*), u_16 s, Point d, Point* p) : Animation(f, s) {
    point = p;
    dist = d;
}

DelayMoveAnimation::DelayMoveAnimation(void (*f)(Animation*), u_16 dl, u_16 s, Point d, Point* p) : Animation(f, s), MoveAnimation(f, s, d, p), DelayAnimation(f, dl, s) {}

VelocityAnimation::VelocityAnimation(void (*f)(Animation*), u_16 s, Point v, Point* p) : Animation(f, s) {
    point = p;
    vel = v;
}

ArithmeticAnimation::ArithmeticAnimation(void (*f)(Animation*), u_16 s, u_16* v, int c) : Animation(f, s + 1), DelayAnimation(f, s, s + 1) {
    value = v;
    change = c;
}


/**
    ------------------------
    animation_func FUNCTIONS
    ------------------------
**/

/**  Animation of sprite movement.  **/
void animation_sprite(Animation* a) {
    SpriteAnimation* sa = static_cast<SpriteAnimation*>(a);
    u_16 index = 0;
    u_16 t_delay = 0;//sa->delay_arr[index];
    while (t_delay < sa->frame)
        t_delay += sa->delay_arr[index++];
    if (t_delay == sa->frame)
        *(sa->pbmp) = sa->bmp_arr[index];
}

/**  Animation of flashing.  **/
void animation_flash(Animation* a) {
    FlashAnimation* fa = dynamic_cast<FlashAnimation*>(a);
    if (fa->frame == fa->speed - 1)
        *fa->tint = fa->original;
    else if (fa->frame == 0)
        *fa->tint = al_map_rgba(255, 32, 32, 255);
}


std::unordered_map<u_16, std::unordered_map<int, std::vector<bool> > > lin_sol;

std::vector<bool>* get_linear(int d, u_16 t) {
    if (d < 0)  return get_linear(abs(d), t);
    std::unordered_map<u_16, std::unordered_map<int, std::vector<bool> > >::iterator it1 = lin_sol.find(t);
    std::unordered_map<int, std::vector<bool> >::iterator it2;
    if (it1 != lin_sol.end()) {
        it2 = it1->second.find(d);
        if (it2 != it1->second.end())
            return &it2->second;
    }

    std::vector<bool> sol_t;
    if (t == 1) {
        sol_t.push_back(d != 0);
    } else {
        std::vector<bool>* hsol = get_linear(d/2, t/2);
        sol_t.assign(hsol->begin(), hsol->end());
        if (t%2 == 1)   sol_t.push_back(d%2 == 1);
        sol_t.insert(sol_t.end(), hsol->begin(), hsol->end());
        if (t%2 == 0 && d%2 == 1) {
            for (std::vector<bool>::iterator it = sol_t.begin(); it < sol_t.end(); ++it) {
                if (!*it) {
                    *it = true;
                    break;
                }
            }
        }
    }
    if (it1 == lin_sol.end())
        it1 = lin_sol.emplace(t, std::unordered_map<int, std::vector<bool> >()).first;
    it2 = it1->second.emplace(d, sol_t).first;

    return &it2->second;
}

/**  Animation of linear movement.  **/
void animation_linmove(Animation* a) {
    MoveAnimation* ma = dynamic_cast<MoveAnimation*>(a);

    ma->point->x += ma->dist.x/(int)ma->speed;
    ma->point->y += ma->dist.y/(int)ma->speed;
    if (get_linear(abs(ma->dist.x)%ma->speed, ma->speed)->at(ma->frame))
        ma->point->x += SIGN(ma->dist.x);
    if (get_linear(abs(ma->dist.y)%ma->speed, ma->speed)->at(ma->frame))
        ma->point->y += SIGN(ma->dist.y);
}

/**  Camera linear movement.  **/
void animation_camlinmove(Animation* a) {
    CameraAnimation* ca = dynamic_cast<CameraAnimation*>(a);
    int dx = ca->dist.x/(int)ca->speed;
    if (get_linear(abs(ca->dist.x)%ca->speed, ca->speed)->at(ca->frame))
        dx += SIGN(ca->dist.x);
    int dy = ca->dist.y/(int)ca->speed;
    if (get_linear(abs(ca->dist.y)%ca->speed, ca->speed)->at(ca->frame))
        dy += SIGN(ca->dist.y);
    ca->ts->move_camera(dx, dy);
}

void animation_linfade(Animation* a) {
    FadeAnimation* fa = dynamic_cast<FadeAnimation*>(a);

    if (fa->frame == 0) {
        unsigned char c1[4], c2[4];
        al_unmap_rgba(*fa->tint, c1, c1+1, c1+2, c1+3);
        al_unmap_rgba(fa->fade_to, c2, c2+1, c2+2, c2+3);
        for (int i = 3; i >= 0; --i)
            fa->dc[i] = c2[i] - c1[i];
    }

    unsigned char cv[4];
    al_unmap_rgba(*fa->tint, cv, cv+1, cv+2, cv+3);
    for (int i = 3; i >= 0; --i) {
        cv[i] += (signed char)(fa->dc[i]/(int)fa->speed);
        if (get_linear(abs(fa->dc[i])%fa->speed, fa->speed)->at(fa->frame))
            cv[i] += (signed char)SIGN(fa->dc[i]);
    }
    *fa->tint = al_map_rgba(cv[0], cv[1], cv[2], cv[3]);
}

/**  Falling velocity.  **/
void animation_fallvel(Animation* a) {
    VelocityAnimation* va = static_cast<VelocityAnimation*>(a);
    va->point->x += va->vel.x;
    va->point->y += va->vel.y++;
}

/**  Number addition/subtraction  **/
void animation_addition(Animation* a) {
    ArithmeticAnimation* aa = dynamic_cast<ArithmeticAnimation*>(a);
    if (-aa->change > (int)*aa->value)
        *aa->value = 0;
    else
        *aa->value += aa->change;
}

/*
void animation_fract(Animation* a) {
    if (MoveAnimation* ma = static_cast<MoveAnimation*>(a)) {
        Point v = ma->dist;
        if (FRACTION(v.x) != 0 || v.x == 0)
            v.x = FRACTION(v.x);
        else
            v.x = SIGN(v.x);
        if (FRACTION(v.y) != 0 || v.y == 0)
            v.y = FRACTION(v.y);
        else
            v.y = SIGN(v.y);
        u_16 i = num_points;
        while (i-- > 0)
            *(points[i]) += v;
    }
}
*/


//template <> struct DeletionAnimation<TowerState, Widget>;
//template <> void animation_deletion<TowerState, Widget>(Animation*);


}
