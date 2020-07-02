ALLEGRO_LIBRARIES := allegro-5 allegro_image-5 allegro_font-5 allegro_ttf-5 allegro_dialog-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5
ALLEGRO_FLAGS := $(shell pkg-config --cflags --libs $(ALLEGRO_LIBRARIES)) -lallegro -lallegro_main

CC := gcc
OUT:= exec.out
OBJECTS:= main.c game.c scene_menu.c scene_start.c shared.c utility.c scene_settings.c scene_end.c
INCLUDE:= game.h scene_menu.h scene_start.h shared.h utility.h scene_settings.h scene_end.h
MAIN:= main.c 
	

all:  $(OBJECTS)
		$(CC) -o $(OUT) $(OBJECTS) -I $(ALLEGRO_FLAGS) 
