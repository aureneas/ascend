#include <cmath>
#include "../include/graphics.h"
// TODO remove later
#include "../include/_debug.h"

namespace graphics {


ALLEGRO_COLOR   screen_tint;
float           screen_tint_alpha = 0.0f;
ALLEGRO_COLOR   target_tint;


unsigned        fade_frame;
float           fade_rate;
unsigned        FADE_SPEED;

void set_fade_speed(unsigned fspd) {
    fade_frame = 0;
    FADE_SPEED = fspd;
    fade_rate = pow(0.003921569, 1.0/FADE_SPEED);
}



/**
 *  Multiplies the color values. Takes on the alpha value of the first color.
 */
ALLEGRO_COLOR color_multmix(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2) {
    float r1, r2, g1, g2, b1, b2, a1, a2;
    al_unmap_rgba_f(c1, &r1, &g1, &b1, &a1);
    al_unmap_rgba_f(c2, &r2, &g2, &b2, &a2);
    return al_map_rgba_f((1 + a1*(r1 - 1)) * (1 + a2*(r2 - 1)),
                         (1 + a1*(g1 - 1)) * (1 + a2*(g2 - 1)),
                         (1 + a1*(b1 - 1)) * (1 + a2*(b2 - 1)),
                         a1);
}


ALLEGRO_COLOR black;
ALLEGRO_COLOR white;
ALLEGRO_COLOR clear;

void init() {
    black = al_map_rgb(   0,   0,   0   );
    white = al_map_rgb(  255, 255, 255  );
    clear = al_map_rgba(  0,   0,   0,   0  );

    screen_tint = black;
    screen_tint_alpha = 1.0f;
    target_tint = clear;
    set_fade_speed(300);
}

void begin_frame() {
    al_clear_to_color(black);

    float r1, r2, g1, g2, b1, b2, a1, a2;
    al_unmap_rgba_f(screen_tint, &r1, &g1, &b1, &a1);
    al_unmap_rgba_f(target_tint, &r2, &g2, &b2, &a2);

    if (fade_frame < FADE_SPEED) {
        screen_tint = al_map_rgba_f((fade_rate*r1) + ((1-fade_rate)*r2),
                                    (fade_rate*g1) + ((1-fade_rate)*g2),
                                    (fade_rate*b1) + ((1-fade_rate)*b2),
                                    (fade_rate*a1) + ((1-fade_rate)*a2));
        screen_tint_alpha = (fade_rate*a1) + ((1-fade_rate)*a2);
        ++fade_frame;
    } else if (r1 != r2 || g1 != g2 || b1 != b2 || a1 != a2) {
        screen_tint = target_tint;
        screen_tint_alpha = a2;
    }
}

void end_frame() {
    al_flip_display();
}


void fade_out_black() {
    target_tint = black;
    set_fade_speed(30);
}

void fade_out_white() {
    target_tint = white;
    set_fade_speed(30);
}

void fade_in() {
    target_tint = clear;
    set_fade_speed(60);
}


void set_screen_tint(ALLEGRO_COLOR color) {
    screen_tint = color;
    float rgb;
    al_unmap_rgba_f(color, &rgb, &rgb, &rgb, &screen_tint_alpha);
}

void clear_screen_tint() {
    screen_tint_alpha = 0.0f;
}


void draw_tinted(ALLEGRO_BITMAP* bmp, ALLEGRO_COLOR color, int x, int y, int flags, bool override_scr_tint) {
    if (!override_scr_tint && screen_tint_alpha > 0)
        draw_tinted(bmp, color_multmix(color, screen_tint), x, y, flags, true);
    else
        al_draw_tinted_bitmap(bmp, color, x, y, flags);
}

void draw(ALLEGRO_BITMAP* bmp, int x, int y, int flags) {
    if (screen_tint_alpha > 0)
        draw_tinted(bmp, color_multmix(white, screen_tint), x, y, flags, true);
    else
        al_draw_bitmap(bmp, x, y, flags);
}

void draw_ustr(ALLEGRO_FONT* font, ALLEGRO_COLOR color, ALLEGRO_USTR* ustr, int x, int y, int flags, bool override_scr_tint) {
    if (!override_scr_tint)
        color = color_multmix(color, screen_tint);
    al_draw_ustr(font, color, x, y, flags, ustr);
}

}
