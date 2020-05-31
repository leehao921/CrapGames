#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#define GAME_TERMINATE -1

#include "begin.h"
#include "initial.h"

// ALLEGRO Variables
ALLEGRO_SAMPLE *song=NULL;
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_FONT *font = NULL;
const int Width = 1200;
const int Height = 800;
const float FPS = 60;
/*
void show_err_msg(int msg);
void game_init();
void game_begin();
void game_destroy();

int game_run();
int process_event();
*/
int main(int argc, char *argv[]) 
{ 
        int msg = 0;
        printf("Hello world!!!\n");

        game_init();
        game_begin();
        while(msg != GAME_TERMINATE)
        { 
            
            if(msg == GAME_TERMINATE) printf("\n-----GAME END-----\n");
        }

        printf("See you, world!!\n");                        
        game_destroy();
        return 0;
}

