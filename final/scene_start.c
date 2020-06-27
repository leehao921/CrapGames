#include "scene_start.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "game.h"
#include "utility.h"
#include <math.h>

// Variables and functions with 'static' prefix at the top level of a
// source file is only accessible in that file ("file scope", also
// known as "internal linkage"). If other files has the same variable
// name, they'll be different variables.

/* Define your static vars / function prototypes below. */

// TODO: More variables and functions that will only be accessed
// inside this scene. They should all have the 'static' prefix.

static ALLEGRO_BITMAP *img_background;
static ALLEGRO_BITMAP *img_plane;
static ALLEGRO_BITMAP *img_enemy;
static ALLEGRO_BITMAP *img_bullet;

typedef struct
{
	// The center coordinate of the image.
	float x, y;
	// The width and height of the object.
	float w, h;
	// The velocity in x, y axes.
	float vx, vy;
	// Should we draw this object on the screen.
	bool hidden;
	// The pointer to the object’s image.
	ALLEGRO_BITMAP *img;
} MovableObject;

static void init(void);
static void update(void);
static void draw_movable_object(MovableObject obj);
static void draw(void);
static void destroy(void);

#define MAX_ENEMY 3
#define MAX_BULLET 999
static MovableObject plane;
static MovableObject enemies[MAX_ENEMY];
static MovableObject bullets[MAX_BULLET];
// [HACKATHON 2-4]
// TODO: Set up bullet shooting cool-down variables.
// 1) Declare your shooting cool-down time as constant. (0.2f will be nice)
// 2) Declare your last shoot timestamp.
const float MAX_COOLDOWN = 0.25;
double last_shoot_timestamp;
static ALLEGRO_SAMPLE *bgm;
static ALLEGRO_SAMPLE_ID bgm_id;
static bool draw_gizmos;

static void init(void)
{
	int i;
	img_background = load_bitmap_resized("source/start-bg.jpg", SCREEN_W, SCREEN_H);
	img_plane = plane.img = load_bitmap("source/plane.png");
	plane.x = SCREEN_W / 2;
	plane.y = SCREEN_H - SCREEN_H / 3;
	plane.w = al_get_bitmap_width(plane.img);
	plane.h = al_get_bitmap_height(plane.img);
	img_enemy = load_bitmap("source/smallfighter0006.png");
	for (i = 0; i < MAX_ENEMY; i++)
	{
		enemies[i].img = img_enemy;
		enemies[i].w = al_get_bitmap_width(img_enemy);
		enemies[i].h = al_get_bitmap_height(img_enemy);
		enemies[i].x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
		enemies[i].y = 80;
	}
	// [HACKATHON 2-5]
	// TODO: Initialize bullets.
	// 1) Search for a bullet image online and put it in your project.
	//    You can use the image we provided.
	// 2) Load it in by 'load_bitmap' or 'load_bitmap_resized'.
	// 3) For each bullets in array, set their w and h to the size of
	//    the image, and set their img to bullet image, hidden to true,
	//    (vx, vy) to (0, -3).
	// Uncomment and fill in the code below.
	img_bullet = al_load_bitmap("source/image12.png");
	for (int i = 0; i < MAX_BULLET; i++)
	{
		bullets[i].img = img_bullet;
		bullets[i].w = al_get_bitmap_width(img_bullet);
		bullets[i].h = al_get_bitmap_height(img_bullet);
		bullets[i].vx = 0;
		bullets[i].vy = -3;
		bullets[i].hidden = true;
	}
	// Can be moved to shared_init to decrease loading time.
	bgm = load_audio("source/mythica.ogg");
	game_log("Start scene initialized");
	bgm_id = play_bgm(bgm, 1);
}

static void update(void)
{
	plane.vx = plane.vy = 0;
	if (key_state[ALLEGRO_KEY_UP] || key_state[ALLEGRO_KEY_W])
		plane.vy -= 2;
	if (key_state[ALLEGRO_KEY_DOWN] || key_state[ALLEGRO_KEY_S])
		plane.vy += 2;
	if (key_state[ALLEGRO_KEY_LEFT] || key_state[ALLEGRO_KEY_A])
		plane.vx -= 2;
	if (key_state[ALLEGRO_KEY_RIGHT] || key_state[ALLEGRO_KEY_D])
		plane.vx += 2;
	// 0.71 is (1/sqrt(2)).
	plane.y += plane.vy * 4 * (plane.vx ? 0.71f : 1);
	plane.x += plane.vx * 4 * (plane.vy ? 0.71f : 1);
	//       (x, y axes can be separated.)
	if ((plane.x - plane.w / 2) == 0)
		plane.x = plane.x + plane.w;
	else if ((plane.x + plane.w / 2) == SCREEN_W)
		plane.x = plane.x - plane.w;
	if ((plane.y - plane.h / 2) == 0)
		plane.y = plane.y + plane.h / 2;
	else if ((plane.y + plane.h / 2) == SCREEN_H)
		plane.y = plane.y - plane.h / 2;
	int i;
	for (i = 0; i < MAX_BULLET; i++)
	{
		if (bullets[i].hidden == false)
		{
			bullets[i].x += bullets[i].vx;
			bullets[i].y += bullets[i].vy;
		}
		if (bullets[i].y < 0)
			bullets[i].hidden = true;
	}

	// [HACKATHON 2-7]
	// TODO: Shoot if key is down and cool-down is over.
	// 1) Get the time now using 'al_get_time'.
	// 2) If Space key is down in 'key_state' and the time
	//    between now and last shoot is not less that cool
	//    down time.
	// 3) Loop through the bullet array and find one that is hidden.
	//    (This part can be optimized.)
	// 4) The bullet will be found if your array is large enough.
	// 5) Set the last shoot time to now.
	// 6) Set hidden to false (recycle the bullet) and set its x, y to the
	//    front part of your plane.
	// Uncomment and fill in the code below.
	double now = al_get_time();
	if (key_state[ALLEGRO_KEY_SPACE] && now - last_shoot_timestamp >= MAX_COOLDOWN)
	{
		for (i = 0; i < MAX_BULLET; i++)
		{
			if (bullets[i].hidden == true)
				break;
		}
		if (i < MAX_BULLET)
		{
			last_shoot_timestamp = now;
			//al_play_sample(laser_bgm,5.0,1.0,1.0,ALLEGRO_PLAYMODE_ONCE,&laser_bgm_id);
			bullets[i].hidden = false;
			bullets[i].x = plane.x;
			bullets[i].y = plane.y - plane.h / 2;
		}
	}
}

static void draw_movable_object(MovableObject obj)
{
	if (obj.hidden)
		return;
	al_draw_bitmap(obj.img, round(obj.x - obj.w / 2), round(obj.y - obj.h / 2), 0);
	if (draw_gizmos)
	{
		al_draw_rectangle(round(obj.x - obj.w / 2), round(obj.y - obj.h / 2),
											round(obj.x + obj.w / 2) + 1, round(obj.y + obj.h / 2) + 1, al_map_rgb(255, 0, 0), 0);
	}
}

static void draw(void)
{
	int i;
	al_draw_bitmap(img_background, 0, 0, 0);
	// [HACKATHON 2-8]
	// TODO: Draw all bullets in your bullet array.
	// Uncomment and fill in the code below.
	//for (???)
	//	???(???);
	draw_movable_object(plane);
	for (i = 0; i < MAX_ENEMY; i++)
		draw_movable_object(enemies[i]);
}

static void destroy(void)
{
	al_destroy_bitmap(img_background);
	al_destroy_bitmap(img_plane);
	al_destroy_bitmap(img_enemy);
	al_destroy_sample(bgm);
	al_destroy_bitmap(img_bullet);
	stop_bgm(bgm_id);
	game_log("Start scene destroyed");
}

static void on_key_down(int keycode)
{
	if (keycode == ALLEGRO_KEY_TAB)
		draw_gizmos = !draw_gizmos;
}

// TODO: Add more event callback functions such as keyboard, mouse, ...

// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

// The only function that is shared across files.
Scene scene_start_create(void)
{
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Start";
	scene.initialize = &init;
	scene.update = &update;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("Start scene created");
	return scene;
}
