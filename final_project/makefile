ALLEGRO_LIBRARIES := allegro-5 allegro_image-5 allegro_font-5 allegro_ttf-5 allegro_dialog-5 allegro_primitives-5 allegro_audio-5     allegro_acodec-5
ALLEGRO_FLAGS := $(shell pkg-config --cflags --libs $(ALLEGRO_LIBRARIES)) -lallegro -lallegro_main
CC := gcc
OUT:= exec.out
OBJECTS:= main.c begin.c initial.c
INCLUDE:=begin.h initial.h
MAIN:= main.c 
	

all:  $(MAIN)
	$(CC) -o $(OUT) $(ALLFLAGS) $(OBJECTS) -I 
