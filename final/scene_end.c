
#include "scene_end.h"
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
static const char *txt_title = "YOU DID NoT Survive";

static void init(void)
{
  bgm = load_audio("source/epic_bgm.wav");
  img_background = load_bitmap_resized("source/start-bg.jpg", SCREEN_W, SCREEN_H);
  img_return = al_load_bitmap("source/return.png");
  img_return2 = al_load_bitmap("source/return2.png");
}

static void draw(void)
{
  al_clear_to_color(al_map_rgb(105, 105, 105));
  al_draw_bitmap(img_background, 0, 0, 0);
  if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W - 162, 10, 152, 152))
    al_draw_bitmap(img_return2, SCREEN_W - 162, 10, 0);
  else
    al_draw_bitmap(img_return, SCREEN_W - 162, 10, 0);
  al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, 30, ALLEGRO_ALIGN_CENTER, txt_title);
}

static void destroy(void)
{
  al_destroy_sample(bgm);
  al_destroy_bitmap(img_background);
  al_destroy_bitmap(img_return);
  al_destroy_bitmap(img_return2);

  game_log("end scene destroyed");
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
Scene scene_end_create(void)
{
  Scene scene;
  memset(&scene, 0, sizeof(Scene));
  scene.name = "end";
  scene.initialize = &init;
  scene.draw = &draw;
  scene.destroy = &destroy;
  scene.on_mouse_down = &on_mouse_down;
  game_log("Settings end created");
  return scene;
}
