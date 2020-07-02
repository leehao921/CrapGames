
#include "scene_settings.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_acodec.h>
#include "game.h"
#include "utility.h"
#include "shared.h"
#include "scene_start.h"
#include "scene_menu.h"

static ALLEGRO_SAMPLE *bgm;
static ALLEGRO_BITMAP *img_background;
static ALLEGRO_BITMAP *img_return;
static ALLEGRO_BITMAP *img_return2;
static ALLEGRO_BITMAP *img_wasd;
static ALLEGRO_BITMAP *img_space;
static const char *txt_info1 = "MOVE";
static const char *txt_info2 = "ATTACK";
static void init(void)
{
	bgm = load_audio("source/epic_bgm.wav");
	img_background = load_bitmap_resized("source/start-bg.jpg", SCREEN_W, SCREEN_H);
	img_return = al_load_bitmap("source/return.png");
	img_return2 = al_load_bitmap("source/return2.png");
	img_wasd = al_load_bitmap("source/wasd.png");
	img_space = al_load_bitmap("source/space.png");
}

static void draw(void)
{
	al_clear_to_color(al_map_rgb(105, 105, 105));
	al_draw_bitmap(img_background, 0, 0, 0);

	al_draw_bitmap(img_wasd, 50, 490, 0);
	al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2 - SCREEN_W / 4, 500, 0, txt_info1);
	al_draw_bitmap(img_space, SCREEN_W - SCREEN_W / 3, 490, 0);
	al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W - SCREEN_W / 3, 500, 0, txt_info2);
	if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W - 162, 10, 152, 152))
		al_draw_bitmap(img_return2, SCREEN_W - 162, 10, 0);
	else
		al_draw_bitmap(img_return, SCREEN_W - 162, 10, 0);
}

static void destroy(void)
{
	al_destroy_sample(bgm);
	al_destroy_bitmap(img_background);
	al_destroy_bitmap(img_return);
	al_destroy_bitmap(img_return2);
	al_destroy_bitmap(img_space);
	al_destroy_bitmap(img_wasd);
	game_log("Setting scene destroyed");
}

static void on_key_down(int keycode)
{
	if (keycode == ALLEGRO_KEY_ENTER)
		game_change_scene(scene_start_create());
}

static void on_mouse_down(int btn, int x, int y, int dz)
{

	if (btn == 1)
	{
		if (pnt_in_rect(x, y, SCREEN_W - 162, 10, 152, 152))
			game_change_scene(scene_menu_create());
	}
}

// The only function that is shared across files.
Scene scene_settings_create(void)
{
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Settings";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_down = &on_mouse_down;
	game_log("Settings scene created");
	return scene;
}
