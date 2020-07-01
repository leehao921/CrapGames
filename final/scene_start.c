#include "scene_start.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "game.h"
#include "utility.h"
#include <math.h>
#define MAX_ENEMY 5
#define MAX_BULLET 100
#define Tank_SIZE 64

static ALLEGRO_BITMAP *img_background;
static ALLEGRO_BITMAP *img_enemy;
static ALLEGRO_BITMAP *img_bullet_dw;
static ALLEGRO_BITMAP *img_bullet_up;
static ALLEGRO_BITMAP *img_bullet_l;
static ALLEGRO_BITMAP *img_bullet_r;
static ALLEGRO_BITMAP *img_tank;

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

typedef struct
{
	int hp;
	int MAX_HP;
	int EXP;
	int Level;
	int Head_x;
	int Head_y;
	bool death;
} status;

status _hero;
status _enemies[MAX_ENEMY];

static void init(void);
static void update(void);
static void draw_movable_object(MovableObject obj);
static void draw(void);
static void destroy(void);

static MovableObject tank;
static MovableObject enemies[MAX_ENEMY];
static MovableObject up_bullets[MAX_BULLET];
static MovableObject dw_bullets[MAX_BULLET];
static MovableObject l_bullets[MAX_BULLET];
static MovableObject r_bullets[MAX_BULLET];

const float MAX_COOLDOWN = 0.25;
double last_shoot_timestamp;
static ALLEGRO_SAMPLE *bgm;
static ALLEGRO_SAMPLE_ID bgm_id;
static ALLEGRO_SAMPLE *att_bgm;
static ALLEGRO_SAMPLE_ID att_bgm_id;
static bool draw_gizmos;

int DIRECT;

static void init(void)
{
	int i;
	img_background = load_bitmap_resized("source/start-bg.jpg", SCREEN_W, SCREEN_H);
	img_tank = tank.img = load_bitmap("source/tank.png");
	tank.x = SCREEN_W / 2;
	tank.y = SCREEN_H - SCREEN_H / 3;
	tank.w = Tank_SIZE;
	tank.h = Tank_SIZE;
	img_enemy = load_bitmap("source/smallfighter0006.png");
	for (i = 0; i < MAX_ENEMY; i++)
	{
		enemies[i].img = img_enemy;
		enemies[i].w = al_get_bitmap_width(img_enemy);
		enemies[i].h = al_get_bitmap_height(img_enemy);
		enemies[i].x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
		enemies[i].y = 80;
	}

	img_bullet_dw = al_load_bitmap("source/bullets_dw.png");
	img_bullet_l = al_load_bitmap("source/bullets_l.png");
	img_bullet_r = al_load_bitmap("source/bullets_r.png");
	img_bullet_up = al_load_bitmap("source/bullets_up.png");
	for (int i = 0; i < MAX_BULLET; i++)
	{
		up_bullets[i].img = img_bullet_up;
		up_bullets[i].w = al_get_bitmap_width(img_bullet_up);
		up_bullets[i].h = al_get_bitmap_height(img_bullet_up);
		up_bullets[i].vx = 0;
		up_bullets[i].vy = -3;
		up_bullets[i].hidden = true;
	}
	for (int i = 0; i < MAX_BULLET; i++)
	{
		dw_bullets[i].img = img_bullet_dw;
		dw_bullets[i].w = al_get_bitmap_width(img_bullet_dw);
		dw_bullets[i].h = al_get_bitmap_height(img_bullet_dw);
		dw_bullets[i].vx = 0;
		dw_bullets[i].vy = 3;
		dw_bullets[i].hidden = true;
	}
	for (int i = 0; i < MAX_BULLET; i++)
	{
		l_bullets[i].img = img_bullet_l;
		l_bullets[i].w = al_get_bitmap_width(img_bullet_l);
		l_bullets[i].h = al_get_bitmap_height(img_bullet_l);
		l_bullets[i].vx = -3;
		l_bullets[i].vy = 0;
		l_bullets[i].hidden = true;
	}
	for (int i = 0; i < MAX_BULLET; i++)
	{
		r_bullets[i].img = img_bullet_r;
		r_bullets[i].w = al_get_bitmap_width(img_bullet_r);
		r_bullets[i].h = al_get_bitmap_height(img_bullet_r);
		r_bullets[i].vx = 3;
		r_bullets[i].vy = 0;
		r_bullets[i].hidden = true;
	}
	att_bgm = al_load_sample("source/laser.ogg");
	// Can be moved to shared_init to decrease loading time.
	bgm = load_audio("source/epic_bgm.wav");
	game_log("Start scene initialized");
	bgm_id = play_bgm(bgm, 1);
}

static void update(void)
{
	tank.vx = tank.vy = 0;
	if (key_state[ALLEGRO_KEY_UP] || key_state[ALLEGRO_KEY_W])
	{
		tank.vy -= 2;
		DIRECT = 192;
		_hero.Head_y = 1;
		_hero.Head_x = 0;
	}
	if (key_state[ALLEGRO_KEY_DOWN] || key_state[ALLEGRO_KEY_S])
	{
		tank.vy += 2;
		DIRECT = 0;
		_hero.Head_y = -1;
		_hero.Head_x = 0;
	}
	if (key_state[ALLEGRO_KEY_LEFT] || key_state[ALLEGRO_KEY_A])
	{
		tank.vx -= 2;
		DIRECT = 64;
		_hero.Head_x = -1;
		_hero.Head_y = 0;
	}
	if (key_state[ALLEGRO_KEY_RIGHT] || key_state[ALLEGRO_KEY_D])
	{
		tank.vx += 2;
		DIRECT = 128;
		_hero.Head_x = 1;
		_hero.Head_y = 0;
	}
	// 0.71 is (1/sqrt(2)).
	tank.y += tank.vy * 4 * (tank.vx ? 0.71f : 1);
	tank.x += tank.vx * 4 * (tank.vy ? 0.71f : 1);
	//       (x, y axes can be separated.)
	if ((tank.x - tank.w / 2) == 0)
		tank.x = tank.x + tank.w;
	else if ((tank.x + tank.w / 2) == SCREEN_W)
		tank.x = tank.x - tank.w;
	if ((tank.y - tank.h / 2) == 0)
		tank.y = tank.y + tank.h / 2;
	else if ((tank.y + tank.h / 2) == SCREEN_H)
		tank.y = tank.y - tank.h / 2;
	//hero_bullet
	int i;
	for (i = 0; i < MAX_BULLET; i++)
	{
		if (up_bullets[i].hidden == false)
		{
			up_bullets[i].x += up_bullets[i].vx;
			up_bullets[i].y += up_bullets[i].vy;
		}
		if (up_bullets[i].y < 0)
			up_bullets[i].hidden = true;
	}
	for (i = 0; i < MAX_BULLET; i++)
	{
		if (dw_bullets[i].hidden == false)
		{
			dw_bullets[i].x += dw_bullets[i].vx;
			dw_bullets[i].y += dw_bullets[i].vy;
		}
		if (dw_bullets[i].y < 0)
			dw_bullets[i].hidden = true;
	}
	for (i = 0; i < MAX_BULLET; i++)
	{
		if (l_bullets[i].hidden == false)
		{
			l_bullets[i].x += l_bullets[i].vx;
			l_bullets[i].y += l_bullets[i].vy;
		}
		if (l_bullets[i].y < 0)
			l_bullets[i].hidden = true;
	}
	for (i = 0; i < MAX_BULLET; i++)
	{
		if (r_bullets[i].hidden == false)
		{
			r_bullets[i].x += r_bullets[i].vx;
			r_bullets[i].y += r_bullets[i].vy;
		}
		if (r_bullets[i].y < 0)
			r_bullets[i].hidden = true;
	}
	//hero_attaking
	double now = al_get_time();
	if (_hero.Head_y > 0 && !_hero.Head_x && key_state[ALLEGRO_KEY_SPACE] && now - last_shoot_timestamp >= MAX_COOLDOWN)
	{
		for (i = 0; i < MAX_BULLET; i++)
		{
			if (up_bullets[i].hidden == true)
				break;
		}
		if (i < MAX_BULLET)
		{
			last_shoot_timestamp = now;
			al_play_sample(att_bgm, 5.0, 1.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &att_bgm_id);
			up_bullets[i].hidden = false;
			up_bullets[i].x = tank.x + tank.w / 2;
			up_bullets[i].y = tank.y - tank.h / 3;
		}
	}

	if (_hero.Head_y < 0 && !_hero.Head_x && key_state[ALLEGRO_KEY_SPACE] && now - last_shoot_timestamp >= MAX_COOLDOWN)
	{
		for (i = 0; i < MAX_BULLET; i++)
		{
			if (dw_bullets[i].hidden == true)
				break;
		}
		if (i < MAX_BULLET)
		{
			last_shoot_timestamp = now;
			al_play_sample(att_bgm, 5.0, 1.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &att_bgm_id);
			dw_bullets[i].hidden = false;
			dw_bullets[i].x = tank.x + tank.w / 2;
			dw_bullets[i].y = tank.y + tank.h / 3;
		}
	}

	if (!_hero.Head_y && _hero.Head_x > 0 && key_state[ALLEGRO_KEY_SPACE] && now - last_shoot_timestamp >= MAX_COOLDOWN)
	{
		for (i = 0; i < MAX_BULLET; i++)
		{
			if (r_bullets[i].hidden == true)
				break;
		}
		if (i < MAX_BULLET)
		{
			last_shoot_timestamp = now;
			al_play_sample(att_bgm, 5.0, 1.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &att_bgm_id);
			r_bullets[i].hidden = false;
			r_bullets[i].x = tank.x + tank.w / 3;
			r_bullets[i].y = tank.y;
		}
	}

	if (!_hero.Head_y && _hero.Head_x < 0 && key_state[ALLEGRO_KEY_SPACE] && now - last_shoot_timestamp >= MAX_COOLDOWN)
	{
		for (i = 0; i < MAX_BULLET; i++)
		{
			if (l_bullets[i].hidden == true)
				break;
		}
		if (i < MAX_BULLET)
		{
			last_shoot_timestamp = now;
			al_play_sample(att_bgm, 5.0, 1.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &att_bgm_id);
			l_bullets[i].hidden = false;
			l_bullets[i].x = tank.x + tank.w / 5;
			l_bullets[i].y = tank.y;
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
	al_draw_bitmap_region(img_tank, 0, DIRECT, Tank_SIZE, Tank_SIZE, tank.x, tank.y, 0);

	//l_draw_bitmap_region(img_tank, 0, 0, Tank_SIZE, Tank_SIZE, tank.x, tank.y, 0);
	for (i = 0; i < MAX_BULLET; i++)
		draw_movable_object(up_bullets[i]);
	for (i = 0; i < MAX_BULLET; i++)
		draw_movable_object(dw_bullets[i]);
	for (i = 0; i < MAX_BULLET; i++)
		draw_movable_object(l_bullets[i]);
	for (i = 0; i < MAX_BULLET; i++)
		draw_movable_object(r_bullets[i]);

	for (i = 0; i < MAX_ENEMY; i++)
	{

		draw_movable_object(enemies[i]);
		if (!enemies[i].hidden)
		{
			al_draw_filled_rectangle(enemies[i].x - 1.5 * enemies[i].w / 2, enemies[i].y + enemies[i].h + 25, enemies[i].x + 1.5 * enemies[i].w / 2, enemies[i].y + enemies[i].h + 35, al_map_rgb(255, 0, 0));
			al_draw_filled_rectangle(enemies[i].x - 1.5 * enemies[i].w / 2, enemies[i].y + enemies[i].h + 25, enemies[i].x + 1.5 * enemies[i].w / 2 - (enemies[i].w - enemies[i].w * _enemies[i].hp / _enemies[i].MAX_HP), enemies[i].y + enemies[i].h + 35, al_map_rgb(0, 224, 208));
		}
	}
}

static void destroy(void)
{
	al_destroy_bitmap(img_background);
	al_destroy_bitmap(img_enemy);
	al_destroy_sample(bgm);
	al_destroy_bitmap(img_bullet_dw);
	al_destroy_bitmap(img_bullet_up);
	al_destroy_bitmap(img_bullet_l);
	al_destroy_bitmap(img_bullet_r);
	stop_bgm(bgm_id);
	game_log("Start scene destroyed");
}

static void on_key_down(int keycode)
{
	if (keycode == ALLEGRO_KEY_TAB)
		draw_gizmos = !draw_gizmos;
}

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
