#include "scene_start.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "game.h"
#include "utility.h"
#include "scene_end.h"
#include "shared.h"
#include <math.h>
#define MAX_ENEMY 5
#define MAX_BULLET 100
#define Tank_SIZE 128
#define ENEMY_SIZE_H 64
#define ENEMY_SIZE_W 48

static ALLEGRO_BITMAP *img_background;
static ALLEGRO_BITMAP *img_enemy;
static ALLEGRO_BITMAP *img_bullet_dw;
static ALLEGRO_BITMAP *img_bullet_up;
static ALLEGRO_BITMAP *img_bullet_l;
static ALLEGRO_BITMAP *img_bullet_r;
static ALLEGRO_BITMAP *img_tank;
static ALLEGRO_BITMAP *img_hp_1;
static ALLEGRO_BITMAP *img_hp_2;
static ALLEGRO_BITMAP *img_hp_3;
static ALLEGRO_BITMAP *img_hp_4;
static ALLEGRO_BITMAP *img_hp_5;
static ALLEGRO_BITMAP *bomb;

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
	int diff;
	int movement;
	int direct;

	//different looks.
	ALLEGRO_BITMAP *img;
} MovableObject;

typedef struct
{
	float hp;
	float MAX_HP;
	float att;
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
static MovableObject explode;
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
static ALLEGRO_SAMPLE *boom_bgm;
static ALLEGRO_SAMPLE_ID boom_bgm_id;

static bool draw_gizmos;

int DIRECT, Diff_HERO;

static void init(void)
{
	int i;
	img_background = load_bitmap_resized("source/start-bg.jpg", SCREEN_W, SCREEN_H);
	img_tank = tank.img = load_bitmap("source/tank.png");
	tank.x = SCREEN_W / 2;
	tank.y = SCREEN_H - SCREEN_H / 3;
	tank.w = Tank_SIZE;
	tank.h = Tank_SIZE;
	img_enemy = load_bitmap("source/zombies.png");
	_hero.Level = 1 + (_hero.EXP / 100);
	_hero.att = 2 + (_hero.Level * 0.5);
	_hero.MAX_HP = 5;
	_hero.hp = _hero.MAX_HP;
	img_hp_1 = al_load_bitmap("source/5%blood.png");
	img_hp_2 = al_load_bitmap("source/25%blood.png");
	img_hp_3 = al_load_bitmap("source/50%blood.png");
	img_hp_4 = al_load_bitmap("source/75%blood.png");
	img_hp_5 = al_load_bitmap("source/100%blood.png");

	for (i = 0; i < MAX_ENEMY; i++)
	{
		enemies[i].img = img_enemy;
		enemies[i].w = ENEMY_SIZE_W;
		enemies[i].h = ENEMY_SIZE_H;
		enemies[i].x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
		enemies[i].y = 80;
		enemies[i].hidden = false;
		enemies[i].diff = (int)rand() % 5;
		_enemies[i].MAX_HP = 10 * (_hero.Level * 0.5);
		_enemies[i].hp = _enemies[i].MAX_HP;
		_enemies[i].death = false;
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
	boom_bgm = load_audio("source/bomb.ogg");
	bomb = al_load_bitmap("source/boom.png");
	game_log("Start scene initialized");
	bgm_id = play_bgm(bgm, 1);
}

static void update(void)
{
	tank.vx = tank.vy = 0;
	if (key_state[ALLEGRO_KEY_UP] || key_state[ALLEGRO_KEY_W])
	{
		tank.vy -= 2;
		DIRECT = 384;
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
		DIRECT = 128;
		_hero.Head_x = -1;
		_hero.Head_y = 0;
	}
	if (key_state[ALLEGRO_KEY_RIGHT] || key_state[ALLEGRO_KEY_D])
	{
		tank.vx += 2;
		DIRECT = 256;
		_hero.Head_x = 1;
		_hero.Head_y = 0;
	}
	// 0.71 is (1/sqrt(2)).
	tank.y += tank.vy * 4 * (tank.vx ? 0.71f : 1);
	tank.x += tank.vx * 4 * (tank.vy ? 0.71f : 1);
	//       (x, y axes can be separated.)
	//collied damage on tank

	if (tank.x < 0)
	{
		tank.x = 0;
	}
	else if (tank.x + tank.h > SCREEN_W)
	{
		tank.x = SCREEN_W - tank.h;
	}
	if (tank.y < 0)
	{
		tank.y = 0;
	}
	else if (tank.y + tank.h > SCREEN_H)
	{
		tank.y = SCREEN_H - tank.h;
	}

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
			up_bullets[i].y = tank.y;
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
			r_bullets[i].x = tank.x + tank.w;
			r_bullets[i].y = tank.y + tank.w / 5;
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
			l_bullets[i].y = tank.y + tank.w / 5;
		}
	}

	//ENEMY MOVING
	//TODO adding detail of moving
	for (i = 0; i < MAX_ENEMY; i++)
	{
		enemies[i].vx = enemies[i].vy = 0;
		if (enemies[i].y > tank.y)
		{
			enemies[i].vy -= 2;
			enemies[i].direct = 64 * 3;
		}
		if (enemies[i].y < tank.y)
		{
			enemies[i].vy += 2;
			enemies[i].direct = 64 * 0;
		}
		if (enemies[i].x > tank.x)
		{
			enemies[i].vx -= 2;
			enemies[i].direct = 64 * 1;
		}
		if (enemies[i].x < tank.x)
		{
			enemies[i].vx += 2;
			enemies[i].direct = 64 * 2;
		}
		// 0.71 is (1/sqrt(2)).
		enemies[i].y += enemies[i].vy * _hero.Level * (enemies[i].vx ? 0.71f : 1);
		enemies[i].x += enemies[i].vx * _hero.Level * (enemies[i].vy ? 0.71f : 1);
		//       (x, y axes can be separated.)
		//collied damage on enemies[i]
		/*
		for (int e = 0; e < MAX_ENEMY; e++)
		{
			if (enemies[i].x < enemies[e].x + enemies[e].w / 2)
			{
				enemies[i].x = enemies[e].x - enemies[i].w / 2;
			}
			else if (enemies[i].x + enemies[i].h > enemies[e].x)
			{
				enemies[i].x = enemies[e].x - enemies[e].w / 2;
			}
			if (enemies[i].y < enemies[e].y + enemies[e].h / 2)
			{
				enemies[i].y = enemies[e].y - enemies[e].h;
			}
			else if (enemies[i].y + enemies[i].h > enemies[e].y)
			{
				enemies[i].y = enemies[e].y;
			}
		}
		*/
		for (int i = 0; i < MAX_ENEMY; i++)
		{
			if (
					tank.x < enemies[i].x + enemies[i].w && tank.x + tank.w > enemies[i].x && tank.y + tank.h > enemies[i].y && tank.y < enemies[i].y + enemies[i].h && enemies[i].hidden == false && _enemies[i].death == false)
			{
				_hero.hp -= 1;
				enemies[i].x -= 50;
				enemies[i].y -= 50;
				_enemies[i].hp -= 1;
				_hero.EXP += 5;
			}
		}
	}
	//
	//ENEMY Damaging
	for (int e = 0; e < MAX_ENEMY; e++)
	{
		for (i = 0; i < MAX_BULLET; i++)
		{
			if (!up_bullets[i].hidden && !enemies[e].hidden && up_bullets[i].y > enemies[e].y - enemies[e].h / 2 && up_bullets[i].y < enemies[e].y + 2 * enemies[e].h && up_bullets[i].x - up_bullets[i].w / 2 < enemies[e].x + enemies[e].w / 2 && up_bullets[i].x + up_bullets[i].w / 2 > enemies[e].x - enemies[e].w / 2)
			{
				up_bullets[i].hidden = true;
				_hero.EXP += 5;
				_enemies[e].hp -= _hero.att;
			}
		}
	}
	for (int e = 0; e < MAX_ENEMY; e++)
	{
		for (i = 0; i < MAX_BULLET; i++)
		{
			if (!dw_bullets[i].hidden && !enemies[e].hidden && dw_bullets[i].y > enemies[e].y - enemies[e].h / 2 && dw_bullets[i].y < enemies[e].y + 2 * enemies[e].h && dw_bullets[i].x - dw_bullets[i].w / 2 < enemies[e].x + enemies[e].w / 2 && dw_bullets[i].x + dw_bullets[i].w / 2 > enemies[e].x - enemies[e].w / 2)
			{
				dw_bullets[i].hidden = true;
				_enemies[e].hp -= _hero.att;
			}
		}
	}
	for (int e = 0; e < MAX_ENEMY; e++)
	{
		for (i = 0; i < MAX_BULLET; i++)
		{
			if (!l_bullets[i].hidden && !enemies[e].hidden && l_bullets[i].y > enemies[e].y - enemies[e].h / 2 && l_bullets[i].y < enemies[e].y + 2 * enemies[e].h && l_bullets[i].x - l_bullets[i].w / 2 < enemies[e].x + enemies[e].w / 2 && l_bullets[i].x + l_bullets[i].w / 2 > enemies[e].x - enemies[e].w / 2)
			{
				l_bullets[i].hidden = true;
				_enemies[e].hp -= _hero.att;
			}
		}
	}
	for (int e = 0; e < MAX_ENEMY; e++)
	{
		for (i = 0; i < MAX_BULLET; i++)
		{
			if (!r_bullets[i].hidden && !enemies[e].hidden && r_bullets[i].y > enemies[e].y - enemies[e].h / 2 && r_bullets[i].y < enemies[e].y + 2 * enemies[e].h && r_bullets[i].x - r_bullets[i].w / 2 < enemies[e].x + enemies[e].w / 2 && r_bullets[i].x + r_bullets[i].w / 2 > enemies[e].x - enemies[e].w / 2)
			{
				r_bullets[i].hidden = true;
				_enemies[e].hp -= _hero.att;
			}
		}
	}

	//ENEMY death

	double deathtime;
	for (int e = 0; e < MAX_ENEMY; e++)
	{
		if (_enemies[e].hp <= 0)
		{
			if (!_enemies[e].death)
			{

				enemies[e].hidden = true;
				_enemies[e].death = true;
				_hero.EXP += 20;
				explode.x = enemies[e].x - explode.w / 4;
				explode.y = enemies[e].y - explode.h / 4;
				explode.hidden = false;
				al_play_sample(boom_bgm, 5.0, 1.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &boom_bgm_id);
				deathtime = al_get_time();
			}
		} //an enemy can only dye one time
	}

	int alldead = 1;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemies[i].hidden == false)
			alldead = 0;
	}
	if (alldead == 1)
	{
		for (int i = 0; i < MAX_ENEMY; i++)
		{
			enemies[i].x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
			enemies[i].y = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_H - enemies[i].w);
			;
			enemies[i].hidden = false;
			enemies[i].diff = (int)rand() % 5;
			_enemies[i].MAX_HP = 10 * (_hero.Level * 0.5);
			_enemies[i].hp = _enemies[i].MAX_HP;
			_enemies[i].death = false;
			_hero.Level = 1 + (_hero.EXP / 100);
			_hero.att = 2 + (_hero.Level * 0.5);
			_hero.MAX_HP = 5;
			_hero.hp = _hero.MAX_HP;
		}
	}
	if (_hero.hp <= 0)
	{
		game_change_scene(scene_end_create());
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
	al_draw_bitmap_region(img_tank, 128 * 8, DIRECT, Tank_SIZE, Tank_SIZE, tank.x, tank.y, 0);
	if (_hero.hp == 5)
		al_draw_bitmap(img_hp_5, 0, 850, 0);
	if (_hero.hp == 4)
		al_draw_bitmap(img_hp_4, 0, 850, 0);
	if (_hero.hp == 3)
		al_draw_bitmap(img_hp_3, 0, 850, 0);
	if (_hero.hp == 2)
		al_draw_bitmap(img_hp_2, 0, 850, 0);
	if (_hero.hp == 1)
		al_draw_bitmap(img_hp_1, 0, 850, 0);

	for (i = 0; i < MAX_BULLET; i++)
		draw_movable_object(up_bullets[i]);
	for (i = 0; i < MAX_BULLET; i++)
		draw_movable_object(dw_bullets[i]);
	for (i = 0; i < MAX_BULLET; i++)
		draw_movable_object(l_bullets[i]);
	for (i = 0; i < MAX_BULLET; i++)
		draw_movable_object(r_bullets[i]);
	//bomb effect
	//血條位置
	for (i = 0; i < MAX_ENEMY; i++)
	{
		if (!enemies[i].hidden)
		{
			al_draw_filled_rectangle(enemies[i].x - 1.5 * enemies[i].w / 2, enemies[i].y + enemies[i].h + 25, enemies[i].x + 1.5 * enemies[i].w / 2, enemies[i].y + enemies[i].h + 35, al_map_rgb(255, 0, 0));
			al_draw_filled_rectangle(enemies[i].x - 1.5 * enemies[i].w / 2, enemies[i].y + enemies[i].h + 25, enemies[i].x + 1.5 * enemies[i].w / 2 - (enemies[i].w - enemies[i].w * _enemies[i].hp / _enemies[i].MAX_HP), enemies[i].y + enemies[i].h + 35, al_map_rgb(0, 224, 208));
			al_draw_bitmap_region(img_enemy, 48 * enemies[i].diff, enemies[i].direct, ENEMY_SIZE_W, ENEMY_SIZE_H, enemies[i].x, enemies[i].y, 0);
		}
	}
	al_draw_textf(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, 0, ALLEGRO_ALIGN_CENTER, "Stage = %d", _hero.Level);
	al_draw_textf(font_pirulen_32, al_map_rgb(255, 255, 255), 0, 40, ALLEGRO_ALIGN_LEFT, "score = %d", _hero.EXP);
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
	al_destroy_bitmap(img_hp_1);
	al_destroy_bitmap(img_hp_2);
	al_destroy_bitmap(img_hp_3);
	al_destroy_bitmap(img_hp_4);
	al_destroy_bitmap(img_hp_5);
	al_destroy_bitmap(img_tank);
	al_destroy_bitmap(bomb);
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
