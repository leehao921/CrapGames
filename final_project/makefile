ALLEGRO_LIBRARIES := allegro-5 allegro_image-5 allegro_font-5 allegro_ttf-5 allegro_dialog-5 allegro_primitives-5 allegro_audio-5     allegro_acodec-5
ALLEGRO_FLAGS := $(shell pkg-config --cflags --libs $(ALLEGRO_LIBRARIES)) -lallegro -lallegro_main
CC := gcc
OUT:= exec.out
MAIN:= main.c 
LINK: begin.c initial.c link.h
	

all: $(MAIN)
	$(CC) $(MAIN) initial.c begin.c $(ALLFLAGS) -o $(OUT) 
