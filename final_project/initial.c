#include"initial.h" 
void game_init()
{ 
    if(!al_init()) show_err_msg(-1);
    if(!al_install_audio())
    { 
        fprintf(stderr, "failed to initialize audio!\n");
        show_err_msg(-2);
    }
    if(!al_init_acodec_addon())
    { 
        fprintf(stderr, " to initialize audio codecs!\n");
        show_err_msg(-3);
    }
    if(!al_reserve_samples(1))
    { 
        fprintf(stderr, " to reserve samples!\n");
        show_err_msg(-4);
    }
    //display
    display=al_create_display(Width, Height);
    event_quene = al_create_event_queue();
    if(display ==NULL || event_queue ==NULL)
    { 
        show_err_msg(-5);
    }
    al_set_window_position(display, 0, 0);
    al_set_window_title(display, title);
    al_init_primitives_addon();
    al_install_keyboard();
    al_install_audio();
    al_init_image_addon();
    al_init_acodec_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    //Register event
    al_register_event_source(event_queue, al_get_display_event_source(displayi));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
}

void show_err_msg(int msg) 
{ 
    fprintf(stderr, "unexpected msg: %d\n", msg);
    game_destroy();
    exit(9);
}

void game_destroy()        
{ 
    // Make sure you destroy all things
    al_destroy_sample(song);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
}
