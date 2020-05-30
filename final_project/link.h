#ifdef _LINK_H_
#define _LINK_H_

#include<stdio.h>
#include<allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define GAME_TERMINATE -1

const int width = 1200;
const int height = 800;

void show_err_msg(int msg);
void game_init();
void game_begin();
void game_destroy();


#endif
