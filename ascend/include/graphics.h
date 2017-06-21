#ifndef A_GRAPHICS_H
#define A_GRAPHICS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

namespace graphics {

void init();
void begin_frame();
void end_frame();

void fade_out_black();
void fade_in();

void set_screen_tint(ALLEGRO_COLOR);
void clear_screen_tint();

void draw(ALLEGRO_BITMAP*, int, int, int);
void draw_tinted(ALLEGRO_BITMAP*, ALLEGRO_COLOR, int, int, int, bool = false);
void draw_tinted_bar(ALLEGRO_BITMAP*, ALLEGRO_COLOR, int, int, float, float, int, bool = false);

void draw_ustr(ALLEGRO_FONT*, ALLEGRO_COLOR, ALLEGRO_USTR*, int, int, int, bool = false);
void draw_ustr_multiline(ALLEGRO_FONT*, ALLEGRO_COLOR, ALLEGRO_USTR*, int, int, int, int, int, bool = false);

}

#endif // A_GRAPHICS_H
