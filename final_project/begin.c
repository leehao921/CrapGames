#include"begin.h"

void game_begin(void)
{ 
    back_ground_music=al_load_sample("bgm.wav");
    if(!back_ground_music)
    { 
        printf("audio clip sample not loaded!\n");
        show_err_msg(-6);
    }
    al_play_sample(back_ground_music,1.0，0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
    al_clear_to_color(al_map_rgb(100,100,100));
    font = al_load_ttf_font("pirulen.ttf",12,0);
    al_draw_text(font, al_map_rgb(255,255,255), Width/2, Height/2+220 , ALLEGRO_ALIGN_CENTRE, "Press 'Enter' to start");
    al_draw_rectangle(Width, 510, Height, 550, al_map_rgb(255, 255, 255), 0));
    al_flip_display();
}
