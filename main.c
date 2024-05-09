#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <windows.h>
#include <stdbool.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"

// ========================== SDL Settings ========================== //
SDL_Event Event;
SDL_Window *Window;
SDL_Renderer *Renderer;

// Surfaces
SDL_Surface *sur_text;
SDL_Surface *sur_round;
SDL_Surface *sur_money;
SDL_Surface *sur_health;
SDL_Surface *sur_circle;
SDL_Surface *sur_map[3];
SDL_Surface *sur_hud[5];
SDL_Surface *sur_grid[2];
SDL_Surface *sur_main[4];
SDL_Surface *sur_icon[8];
SDL_Surface *sur_arrow[2];
SDL_Surface *sur_title[2];
SDL_Surface *sur_prices[4];
SDL_Surface *sur_gunfire[12];
SDL_Surface *sur_tower[4][3][2][8];
SDL_Surface *sur_font_money;
SDL_Surface *sur_font_health;
SDL_Surface *sur_tower_menu[3];
SDL_Surface *sur_soldier[8][4][4];
SDL_Surface *sur_font_selling[12];
SDL_Surface *sur_font_upgrading[12];

// Textures
SDL_Texture *tex_text;
SDL_Texture *tex_round;
SDL_Texture *tex_money;
SDL_Texture *tex_health;
SDL_Texture *tex_circle;
SDL_Texture *tex_map[3];
SDL_Texture *tex_hud[5];
SDL_Texture *tex_grid[2];
SDL_Texture *tex_main[4];
SDL_Texture *tex_icon[8];
SDL_Texture *tex_arrow[2];
SDL_Texture *tex_title[2];
SDL_Texture *tex_prices[4];
SDL_Texture *tex_gunfire[12];
SDL_Texture *tex_tower[4][3][2][8];
SDL_Texture *tex_font_money;
SDL_Texture *tex_font_health;
SDL_Texture *tex_tower_menu[3];
SDL_Texture *tex_font_selling[12];
SDL_Texture *tex_font_upgrading[12];

// Rects
SDL_Rect rect_text;
SDL_Rect rect_round;
SDL_Rect rect_money;
SDL_Rect rect_health;
SDL_Rect rect_circle;
SDL_Rect rect_map[3];
SDL_Rect rect_hud[5];
SDL_Rect rect_grid[2];
SDL_Rect rect_main[4];
SDL_Rect rect_icon[8];
SDL_Rect rect_arrow[2];
SDL_Rect rect_title[2];
SDL_Rect rect_prices[4];
SDL_Rect rect_gunfire[12];
SDL_Rect rect_tower[4][3][2][8];
SDL_Rect rect_font_money;
SDL_Rect rect_font_health;
SDL_Rect rect_tower_menu[3];
SDL_Rect rect_font_selling[12];
SDL_Rect rect_font_upgrading[12];

// Fonts
TTF_Font *font1, *font2, *font3, *font4;
SDL_Color color_font1 = {255, 255, 255, 255};
SDL_Color color_font2 = {0, 0, 0, 255};

// ========================== Global Vars ========================== //
bool in_game;
bool win, lose;
bool pause, fast;
int FPS = 20;
int tower_selected;
int round_kills, tick_round, tick_total;
int game_map, enemies, health, money, rounds;
int prices_[4] = {5, 10, 35, 70};
int final_wave[3] = {42, 60, 66};
int tower_radius_[4] = {100, 150, 120, 180};
int tower_damage_[12] = {25, 45, 65, 0, 0, 0, 36, 49, 62, 71, 131, 211};
int selling_prices_[12] = {2, 7, 15, 5, 15, 35, 15, 40, 75, 37, 82, 142};
int upgrading_prices_[12] = {10, 15, 0, 20, 40, 0, 50, 70, 0, 110, 145, 0};
char* prices[4] = {"$5", "$10", "$35", "$70"};
char* upgrading_prices[12] = {"$10", "$15", " ", "$20", "$40", " ", "$55", "$75", " ", "$80", "$90", " "};
char* selling_prices[12] = {"$2", "$7", "$15", "$5", "$15", "$35", "$17", "$45", "$82", "$35", "$75", "$120"};

typedef struct point_ {
    int x;
    int y;
} Point;

Point empty = {0, 0};
Point grass_des = {6, 20};
Point dry_des_1 = {1, 20};
Point dry_des_2 = {11, 20};
Point frost_des_1 = {6, 20};
Point frost_des_2 = {12, 10};

typedef struct node_ {
    Point p;
    int prev;
} Node;

typedef struct queue_ {
    Node queue[999];
    int head;
    int tail;
} Queue;

typedef struct soldier_ {
    SDL_Texture *tex;
    SDL_Rect rect;
    Point pos;
    Point des;
    Point path[99];
    int dir;
    int frame;
    int path_len;
    int type;
    int health;
    int speed;
    int coins;
    int poison_tick;
} Soldier; Soldier enemy[99];

int kills_req[3][30] = {
        { 5,  8,  5, 10,  3, 10, 12, 15, 12, 12,
                15, 18, 16, 18, 10, 20, 15, 20, 20, 25,
                40, 60, 45, 30, 11, 50, 50, 20, 75, 32767},
        {6,  10, 16,  6, 10, 24,  6, 16, 30, 24,
                30, 20, 36, 34, 23, 10, 36, 36, 26, 49,
                48, 60, 12, 80, 60, 66, 46, 70, 100, 32767},
        {6, 10, 12,  8, 16, 10, 16, 10, 20, 21,
                20, 24, 20, 37, 28, 60, 16, 24, 60, 29,
                60, 21, 48, 24, 18, 80, 24, 24, 70, 32767}
};

int viewing_tower[13][21];
int maze[13][21] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
};

// ========================== Declarations ========================== //
void LOAD();
void QUIT();
Node popleft(Queue *q);
void append(Queue *q, Node node);
int cmp(const void* e1, const void* e2);
int distance(int x1, int y1, int x2, int y2);

void InitGame(int map);
void EnemyTick(Soldier* s);
void CreateEnemy(int type, int hp, int speed, int coins, Point des);
void SpawnEnemyGrasslands();
void SpawnEnemyDrylands();
void SpawnEnemyFrostbite();
void ViewTower();
void ViewTower2();
void StorePath(Soldier *s, Queue* q);
int FindPath(Soldier *s, int src_x, int src_y, Point dest);
int NextRound();

// ========================== Overall Funcs ========================== //
void append(Queue *q, Node node) {
    q->queue[q->tail] = node;
    q->tail++;
}
Node popleft(Queue *q) {
    int tmp = q->head;
    q->head++;
    return q->queue[tmp];
}
int distance(int x1, int y1, int x2, int y2) {
    return (int)sqrt(pow(abs(x1-x2), 2) + pow(abs(y1-y2), 2));
}
int delta(int tower_x, int tower_y, int target_x, int target_y) {
    double tan = (double)(target_y-tower_y) / (target_x-tower_x);
    if ( tan <= -1 ) {
        if ( target_x >= tower_x ) { return 4; }
        else { return 0; }
    }
    else if ( tan > -1 && tan <= 0 ) {
        if ( target_x >= tower_x ) { return 3; }
        else { return 7; }
    }
    else if ( tan > 0 && tan <= 1 ) {
        if ( target_x >= tower_x ) { return 2; }
        else { return 6; }
    }
    else if ( tan > 1 ) {
        if ( target_x >= tower_x ) { return 1; }
        else { return 5; }
    }
}
int cmp(const void* e1, const void* e2) {
    return ((Soldier*)e2)->health - ((Soldier*)e1)->health;
}

// ========================== Gameplay Funcs ========================== //
void InitGame(int map) {
    win = false;
    lose = false;
    pause = true;
    in_game = true;
    rounds = 1;
    enemies = 0;
    health = 20;
    tick_round = 0;
    tick_total = 0;
    round_kills = 0;
    game_map = map;
    money = 50 + 25 * (map / 2);
    for ( int i=1; i<=11; i++ ) {
        for ( int j=1; j<=19; j++ ) {
            maze[i][j] = 0;
        }
    }
}

void CreateEnemy(int type, int hp, int speed, int coins, Point des) {
    enemy[enemies].tex = SDL_CreateTextureFromSurface(Renderer, sur_soldier[type][0][0]);
    switch ( des.x ) {
        case 6:
            enemy[enemies].dir = 0;
            enemy[enemies].pos.x = 6;
            enemy[enemies].pos.y = 0;
            enemy[enemies].rect.x = 0;
            enemy[enemies].rect.y = 468;
            break;
        case 1:
            enemy[enemies].dir = 0;
            enemy[enemies].pos.x = 1;
            enemy[enemies].pos.y = 0;
            enemy[enemies].rect.x = 0;
            enemy[enemies].rect.y = 148;
            break;
        case 11:
            enemy[enemies].dir = 0;
            enemy[enemies].pos.x = 11;
            enemy[enemies].pos.y = 0;
            enemy[enemies].rect.x = 0;
            enemy[enemies].rect.y = 788;
            break;
        case 12:
            enemy[enemies].dir = 2;
            enemy[enemies].pos.x = 0;
            enemy[enemies].pos.y = 10;
            enemy[enemies].rect.x = 712;
            enemy[enemies].rect.y = 4;
            break;
        default: break;
    }
    switch ( type ) {
        case 2: enemy[enemies].rect.w = 64; break;
        case 3: enemy[enemies].rect.w = 96; break;
        case 4: enemy[enemies].rect.w = 128; break;
        case 6: enemy[enemies].rect.w = 64; break;
        case 7: enemy[enemies].rect.w = 48; break;
        default: enemy[enemies].rect.w = 38; break;
    }
    switch ( type ) {
        case 3: enemy[enemies].rect.h = 80; break;
        case 4: enemy[enemies].rect.h = 80; break;
        case 7: enemy[enemies].rect.h = 96; break;
        default: enemy[enemies].rect.h = 64; break;
    }
    enemy[enemies].frame = 0;
    enemy[enemies].path_len = 1;
    enemy[enemies].type = type;
    enemy[enemies].health = hp;
    enemy[enemies].speed = speed;
    enemy[enemies].coins = coins;
    enemy[enemies].des = des;
    enemy[enemies].poison_tick = 0;
    enemies++;
}

void EnemyTick(Soldier* s) {
    // Find Paths
    if ( (s->type>=0 && s->type<=2) || (s->type>=5 && s->type<=7) ) {
        if ( s->pos.x>=0 && s->pos.y>=0 ) {
            FindPath(s, 0, 0, empty);
        }
    }

    // Check if an enemy reaches the destination.
    if ( (s->rect.x > 1350 && (s->pos.x==1 || s->pos.x==6 || s->pos.x==11)) || (s->rect.y > 900 && s->pos.y==10) ) {
        s->health = -32767;
        qsort(enemy, enemies, sizeof(enemy[0]), cmp);
    }
    if ( s->rect.y < 0 || s->rect.y > 960 || s->rect.x < 0 || s->rect.x > 1440 ) {
        s->health = -32768;
        qsort(enemy, enemies, sizeof(enemy[0]), cmp);
    }

    // Check poison ticks
    if ( s->poison_tick ) { s->poison_tick--; }

    // Update the enemy's pos info.
    s->frame = ( s->frame + 1 ) % 4;
    s->pos.y = ( s->rect.x - 134 + 64) / 64;
    s->pos.x = ( s->rect.y - 148 + 64) / 64;

    // Set the enemy's direction.
    for ( int i=0; i<s->path_len; i++ ) {
        if ( s->path[i].x == s->pos.x && s->path[i].y == s->pos.y ) {
            if ( !((s->rect.x - 136) % 64) && !((s->rect.y - 148) % 64) ) {
                if ( s->path[i+1].y - s->path[i].y == 1 ) { s->dir = 0; }
                else if ( s->path[i+1].y - s->path[i].y == -1 ) { s->dir = 1; }
                else if ( s->path[i+1].x - s->path[i].x == 1 ) { s->dir = 2; }
                else if ( s->path[i+1].x - s->path[i].x == -1 ) { s->dir = 3; }
                else {
                    if ( s->pos.x != s->des.x && s->pos.y != s->des.y ) {
                        printf("Pathfinding Error: An enemy deviated!\n");
                    }
                }
            }
        }
    }

    // Move the enemy.
    switch ( s->dir ) {
        case 0: s->rect.x += s->speed; break;
        case 1: s->rect.x -= s->speed; break;
        case 2: s->rect.y += s->speed; break;
        case 3: s->rect.y -= s->speed; break;
        default: s->dir = 0; s->rect.x += s->speed; break;
    }
}

void StorePath(Soldier *s, Queue* q) {
    Node curNode = q->queue[q->tail-1];
    Point path_rev[999];
    int path_idx = 0;
    while ( curNode.prev != -1 ) {
        path_rev[path_idx].x = curNode.p.x;
        path_rev[path_idx].y = curNode.p.y;
        path_idx++;
        curNode = q->queue[curNode.prev];
    }
    path_rev[path_idx].x = curNode.p.x;
    path_rev[path_idx].y = curNode.p.y;
    path_idx++;
    s->path_len = path_idx;

    for ( int i=0; i<path_idx; i++ ) {
        s->path[i] = path_rev[path_idx-i-1];
    }
}

int FindPath(Soldier *s, int src_x, int src_y, Point dest) {
    /*
     * Pathfinding mode:
     *  0 - Using a specific enemy's position.
     *  1 - Using a given position.
     */
    int enemy_mode = ( s != NULL );

    // Copy the game map.
    int maze2[13][21];
    for ( int i=0; i<13; i++ ) {
        for ( int j=0; j<21; j++ ) {
            maze2[i][j] = maze[i][j];
        }
    }

    // Getting destination.
    Point destination;
    if ( enemy_mode ) {
        switch ( s->des.x ) {
            case 6: destination = grass_des; break;
            case 1: destination = dry_des_1; break;
            case 11: destination = dry_des_2; break;
            case 12: destination = frost_des_2; break;
            default: break;
        }
    } else {
        destination.x = dest.x;
        destination.y = dest.y;
    }

    // Making attempts in 4 directions.
    int delta[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

    // path - The actual route;
    // attempted - All grids travelled before.
    Queue path, attempted;

    // Init.
    path.head = path.tail = 0;
    attempted.head = attempted.tail = 0;
    Node node = {src_x, src_y, -1};
    if ( enemy_mode ) {
        node.p.x = s->pos.x;
        node.p.y = s->pos.y;
    }
    append(&path, node);

    // Pathfinding.
    while ( path.head != path.tail ) {
        Node curNode = popleft(&path);
        append(&attempted, curNode);
        if ( curNode.p.x == destination.x && curNode.p.y == destination.y ) {
            if ( enemy_mode ) { StorePath(s, &attempted); }
            return 1;
        }
        for ( int i=0; i<4; i++ ) {
            Node nextNode = {curNode.p.x+delta[i][0], curNode.p.y+delta[i][1], -1};
            if ( !maze2[nextNode.p.x][nextNode.p.y] ) {
                nextNode.prev = attempted.tail - attempted.head - 1;
                append(&path, nextNode);
                maze2[nextNode.p.x][nextNode.p.y] = 2;
            }
        }
    }
    if ( enemy_mode ) {
        printf("Pathfinding Error: No path available from (%d, %d) to (%d, %d)!\n", s->pos.x, s->pos.y, s->des.x, s->des.y);
    }
    return 0;
}

int NextRound() {
    rounds++;
    round_kills = 0;
    tick_round = 50;
}

void SpawnEnemyGrasslands() {
    switch ( rounds ) {
        case 1:
            // Drawing arrows
            if ( tick_round < 100 ) {
                rect_arrow[0].x = 64;
                rect_arrow[0].y = 454;
                tex_arrow[0] = SDL_CreateTextureFromSurface(Renderer, sur_arrow[0]);
                SDL_RenderCopy(Renderer, tex_arrow[0], NULL, &rect_arrow[0]);
                SDL_DestroyTexture(tex_arrow[0]);
            }
            // 5 light_soldiers(2000): 100, 120, 140, 160, 180
            if ( tick_round >= 100 && tick_round <= 180 && !(tick_round%20) ) { CreateEnemy(0, 2000, 8, 2, grass_des); }
            break;
        case 2:
            // 8 light_soldiers(2200): 100, 120, 140, 160, 180, 200, 220, 240
            if ( tick_round >= 100 && tick_round <= 240 && !(tick_round%20) ) { CreateEnemy(0, 2200, 8, 2, grass_des); }
            break;
        case 3:
            // 5 heavy_soldiers(4800): 100, 120, 140, 160, 180
            if ( tick_round >= 100 && tick_round <= 180 && !(tick_round%20) ) { CreateEnemy(1, 4800, 8, 3, grass_des); }
            break;
        case 4:
            // 5 light_soldiers(2600): 100, 140, 180, 220, 260
            // 5 heavy_soldiers(5200): 120, 160, 200, 240, 280
            if ( tick_round >= 100 && tick_round <= 260 && tick_round%40==20 ) { CreateEnemy(0, 2600, 8, 2, grass_des); }
            if ( tick_round >= 120 && tick_round <= 280 && !(tick_round%40) ) { CreateEnemy(1, 5200, 8, 3, grass_des); }
            break;
        case 5:
            // 3 tanks(9000): 150, 250, 350
            if ( tick_round == 150 || tick_round == 250 || tick_round == 350 ) { CreateEnemy(2, 9000, 8, 10, grass_des); }
            break;
        case 6:
            // 10 light_soldiers(3000): 100, 120, 140, ... 260, 280
            if ( tick_round >= 100 && tick_round <= 280 && !(tick_round%20) ) { CreateEnemy(0, 3000, 8, 2, grass_des); }
            break;
        case 7:
            // 12 light_soldiers(3200): 105, 120, 135, ... 255, 270
            if ( tick_round >= 105 && tick_round <= 270 && !(tick_round%15) ) { CreateEnemy(0, 3200, 8, 2, grass_des); }
            break;
        case 8:
            // 10 light_soldiers(3400): 100, 120, 140, ... 260, 280
            // 5 heavy_soldiers(6800): 300, 320, 340, 360, 380
            if ( tick_round >= 100 && tick_round <= 280 && !(tick_round%20) ) { CreateEnemy(0, 3400, 8, 2, grass_des); }
            if ( tick_round >= 300 && tick_round <= 380 && !(tick_round%20) ) { CreateEnemy(1, 6800, 8, 3, grass_des); }
            break;
        case 9:
            // 12 heavy_soldiers(7200): 100, 125, 150, ... 350, 375
            if ( tick_round >= 100 && tick_round <= 375 && !(tick_round%25) ) { CreateEnemy(1, 7200, 8, 3, grass_des); }
            break;
        case 10:
            // 9 heavy_soldiers(7600): 125, 150, 175; 225, 250, 275; 325, 350, 375
            // 3 planes(12000): 200, 300, 400
            if ( tick_round >= 125 && tick_round <= 375 && !(tick_round%25) && (tick_round%100) ) { CreateEnemy(1, 7600, 8, 3, grass_des); }
            if ( tick_round == 200 || tick_round == 300 || tick_round == 400 ) { CreateEnemy(3, 12000, 10, 15, grass_des); }
            break;
        case 11:
            // 15 light_soldiers(4000): 100, 120, 140, ... 360, 380
            if ( tick_round >= 100 && tick_round <= 380 && !(tick_round%20) ) { CreateEnemy(0, 4000, 8, 2, grass_des); }
            break;
        case 12:
            // 18 light_soldiers(4200): 105, 120, 135, ... 255, 360
            if ( tick_round >= 105 && tick_round <= 360 && !(tick_round%15) ) { CreateEnemy(0, 4200, 8, 2, grass_des); }
            break;
        case 13:
            // 10 light_soldiers(4400): 105, 120, 135, ... 225, 240
            // 6 heavy_soldiers(8800): 255, 270, 285, 300, 315, 330
            if ( tick_round >= 105 && tick_round <= 240 && !(tick_round%15) ) { CreateEnemy(0, 4400, 8, 2, grass_des); }
            if ( tick_round >= 255 && tick_round <= 330 && !(tick_round%15) ) { CreateEnemy(1, 8800, 8, 3, grass_des); }
            break;
        case 14:
            // 18 heavy_soldiers(9200): 100, 120, 140, ... 420, 440
            if ( tick_round >= 100 && tick_round <= 440 && !(tick_round%20) ) { CreateEnemy(1, 9200, 8, 3, grass_des); }
            break;
        case 15:
            // 5 tanks(20000): 150, 250, 350, 450, 550
            // 5 planes(18000): 200, 300, 400, 500, 600
            if ( tick_round >= 150 && tick_round <= 550 && (tick_round%100==50) ) { CreateEnemy(2, 20000, 8, 10, grass_des); }
            if ( tick_round >= 200 && tick_round <= 600 && !(tick_round%100) ) { CreateEnemy(3, 18000, 10, 15, grass_des); }
            break;
        case 16:
            // 20 light_soldiers(5000): 100, 120, 140, ... 360, 480
            if ( tick_round >= 100 && tick_round <= 480 && !(tick_round%20) ) { CreateEnemy(0, 5000, 8, 2, grass_des); }
            break;
        case 17:
            // 15 heavy_soldiers(10400): 100, 120, 140, 160, 380
            if ( tick_round >= 100 && tick_round <= 380 && !(tick_round%20) ) { CreateEnemy(1, 10400, 8, 3, grass_des); }
            break;
        case 18:
            // 15 light_soldiers(5400): 125, 150, 175; ... ; 525, 550, 575
            // 5 tanks(25000): 200, 300, 400, 500, 600
            if ( tick_round >= 125 && tick_round <= 575 && !(tick_round%25) && (tick_round%100) ) { CreateEnemy(0, 5400, 8, 2, grass_des); }
            if ( tick_round >= 200 && tick_round <= 600 && !(tick_round%100) ) { CreateEnemy(2, 25000, 8, 10, grass_des); }
            break;
        case 19:
            // 15 heavy_soldiers(11200): 125, 150, 175; ... ; 525, 550, 575
            // 5 tanks(25000): 200, 300, 400, 500, 600
            if ( tick_round >= 125 && tick_round <= 575 && !(tick_round%25) && (tick_round%100) ) { CreateEnemy(1, 11200, 8, 3, grass_des); }
            if ( tick_round >= 200 && tick_round <= 600 && !(tick_round%100) ) { CreateEnemy(2, 25000, 8, 10, grass_des); }
            break;
        case 20:
            // 20 heavy_soldiers(11600): 120, 140, 160, 180; ... ; 520, 540, 560, 580
            // 5 planes(22500): 200, 300, 400, 500, 600
            if ( tick_round >= 120 && tick_round <= 580 && !(tick_round%20) && (tick_round%100) ) { CreateEnemy(1, 11600, 8, 3, grass_des); }
            if ( tick_round >= 200 && tick_round <= 600 && !(tick_round%100) ) { CreateEnemy(3, 22500, 10, 15, grass_des); }
            break;
        case 21:
            // 40 light_soldiers(6000): 100, 105, 110, ... 290, 295
            if ( tick_round >= 100 && tick_round <= 295 && !(tick_round%5) ) { CreateEnemy(0, 6000, 8, 2, grass_des); }
            break;
        case 22:
            // 57 light_soldiers(6200): 105, 110, 115, ..., 195; ... ; 305, 310, 315, ..., 395
            // 3 tanks(30000): 200, 300, 400
            if ( tick_round >= 105 && tick_round <= 395 && !(tick_round%5) && (tick_round%100) ) { CreateEnemy(0, 6200, 8, 2, grass_des); }
            if ( tick_round == 200 || tick_round == 300 || tick_round == 400 ) { CreateEnemy(2, 30000, 8, 10, grass_des); }
            break;
        case 23:
            // 45 heavy_soldiers(12800): 100, 110, 120, ... 530, 540
            if ( tick_round >= 100 && tick_round <= 540 && !(tick_round%10) ) { CreateEnemy(1, 12800, 8, 2, grass_des); }
            break;
        case 24:
            // 27 heavy_soldiers(13200): 110, 120, 130, ..., 190; ... ; 310, 320, 330, ..., 390
            // 3 planes(27000): 200, 300, 400
            if ( tick_round >= 110 && tick_round <= 390 && !(tick_round%10) && (tick_round%100) ) { CreateEnemy(1, 13200, 8, 2, grass_des); }
            if ( tick_round == 200 || tick_round == 300 || tick_round == 400 ) { CreateEnemy(3, 27000, 10, 10, grass_des); }
            break;
        case 25:
            // 10 tanks(30000): 100, 130, 160, ... 370
            // 1 blimp(90000): 400
            if ( tick_round >= 100 && tick_round <= 370 && (tick_round%30==10) ) { CreateEnemy(2, 30000, 8, 10, grass_des); }
            if ( tick_round == 400 ) { CreateEnemy(4, 90000, 4, 25, grass_des); }
            break;
        case 26:
            // 50 light_soldiers(7000): 100, 105, 110, ... 340, 345
            if ( tick_round >= 100 && tick_round <= 345 && !(tick_round%5) ) { CreateEnemy(0, 7000, 8, 2, grass_des); }
            break;
        case 27:
            // 45 heavy_soldiers(14000): 110, 120, 130, ..., 190; ... ; 510, 520, 530, ..., 590
            // 5 planes(30000): 200, 300, 400, 500, 600
            if ( tick_round >= 110 && tick_round <= 590 && !(tick_round%10) && (tick_round%100) ) { CreateEnemy(1, 14000, 8, 2, grass_des); }
            if ( tick_round >= 200 && tick_round <= 600 && !(tick_round%100) ) { CreateEnemy(3, 35000, 10, 15, grass_des); }
            break;
        case 28:
            // 10 tanks(35000): 100, 150, 200, ... 500, 550
            // 10 planes(32000): 125, 175, 225, ... 525, 575
            if ( tick_round >= 100 && tick_round <= 550 && !(tick_round%50) ) { CreateEnemy(2, 35000, 8, 10, grass_des); }
            if ( tick_round >= 125 && tick_round <= 575 && (tick_round%50==25) ) { CreateEnemy(3, 32000, 10, 15, grass_des); }
            break;
        case 29:
            // 70 light_soldiers(7000): 100, 105, 110, ... 440, 445
            // 5 planes(32000): 450, 465, 480, 495, 510
            if ( tick_round >= 100 && tick_round <= 445 && !(tick_round%5) ) { CreateEnemy(0, 7000, 8, 2, grass_des); }
            if ( tick_round >= 450 && tick_round <= 510 && !(tick_round%15) ) { CreateEnemy(3, 32000, 10, 15, grass_des); }
            break;
        case 30:
            // 12 heavy_soldiers(14000): 120, 160; ... ; 620, 660
            // 24 tanks(35000): 140, 180; ... ; 640, 680
            // 4 planes(32000): 200, 300, 500, 600
            // 2 blimps(120000): 400, 700
            if ( tick_round >= 120 && tick_round <= 680 && (tick_round%100==20 || tick_round%100==60) ) { CreateEnemy(1, 14000, 8, 2, grass_des); }
            if ( tick_round >= 120 && tick_round <= 680 && (tick_round%100==40 || tick_round%100==80) ) { CreateEnemy(2, 35000, 8, 12, grass_des); }
            if ( tick_round == 200 || tick_round == 300 || tick_round == 500 || tick_round == 600) { CreateEnemy(3, 32000, 10, 12, grass_des); }
            if ( tick_round == 400 || tick_round == 700 ) { CreateEnemy(4, 120000, 4, 25, grass_des); }
        default:
            break;
    }
}

void SpawnEnemyDrylands() {
    switch ( rounds ) {
        case 1:
            // Drawing arrows
            if ( tick_round < 100 ) {
                rect_arrow[0].x = 64;
                rect_arrow[0].y = 134;
                tex_arrow[0] = SDL_CreateTextureFromSurface(Renderer, sur_arrow[0]);
                SDL_RenderCopy(Renderer, tex_arrow[0], NULL, &rect_arrow[0]);
                SDL_DestroyTexture(tex_arrow[0]);
                rect_arrow[0].x = 64;
                rect_arrow[0].y = 774;
                tex_arrow[0] = SDL_CreateTextureFromSurface(Renderer, sur_arrow[0]);
                SDL_RenderCopy(Renderer, tex_arrow[0], NULL, &rect_arrow[0]);
                SDL_DestroyTexture(tex_arrow[0]);
            }
            // 1 - 6 light_soldiers(2000): 100, 120, 140, 160, 180, 200
            if (tick_round>=100 && tick_round<=200 && !(tick_round%20)) { CreateEnemy(0, 2000, 8, 2, dry_des_1); }
            break;
        case 2:
            // 1 - 5 light_soldiers(2200): 100, 120, 140, 160, 180
            // 2 - 5 light_soldiers(2200): 100, 120, 140, 160, 180
            if (tick_round>=100 && tick_round<=180 && !(tick_round%20) ) { CreateEnemy(0, 2200, 8, 2, dry_des_1); }
            if (tick_round>=100 && tick_round<=180 && !(tick_round%20) ) { CreateEnemy(0, 2200, 8, 2, dry_des_2); }
            break;
        case 3:
            // 1 - 8 light_soldiers(2400): 100, 140, 180, ... 340, 380
            // 2 - 8 light_soldiers(2400): 120, 160, 200, ... 360, 400
            if (tick_round>=100 && tick_round<=380 && (tick_round%40==20)) { CreateEnemy(0, 2400, 8, 2, dry_des_1); }
            if (tick_round>=120 && tick_round<=400 && !(tick_round%40)) { CreateEnemy(0, 2400, 8, 2, dry_des_2); }
            break;
        case 4:
            // 2 - 6 heavy_soldiers(5200): 100, 140, 180, 220, 260, 300
            if (tick_round>=100 && tick_round<=300 && (tick_round%40==20)) { CreateEnemy(1, 5200, 8, 4, dry_des_2); }
            break;
        case 5:
            // 1 - 5 heavy_soldiers(5600): 100, 140, 180, 220, 260
            // 2 - 5 heavy_soldiers(5600): 120, 160, 200, 240, 280
            if (tick_round>=100 && tick_round<=260 && (tick_round%40==20)) { CreateEnemy(1, 5600, 8, 4, dry_des_1); }
            if (tick_round>=120 && tick_round<=280 && !(tick_round%40)) { CreateEnemy(1, 5600, 8, 4, dry_des_2); }
            break;
        case 6:
            // 1 - 10 light_soldiers(3000): 105, 120, 135, ... 225, 240
            // 1 - 2 heavy_soldiers(6000): 255, 270
            // 2 - 10 light_soldiers(3000): 105, 120, 135, ... 225, 240
            // 2 - 2 heavy_soldiers(6000): 255, 270
            if (tick_round>=105 && tick_round<=240 && !(tick_round%15)) { CreateEnemy(0, 3000, 8, 2, dry_des_1); }
            if (tick_round>=105 && tick_round<=240 && !(tick_round%15)) { CreateEnemy(0, 3000, 8, 2, dry_des_2); }
            if (tick_round==255 || tick_round==270) { CreateEnemy(1, 6000, 8, 3, dry_des_1); }
            if (tick_round==255 || tick_round==270) { CreateEnemy(1, 6000, 8, 3, dry_des_2); }
            break;
        case 7:
            // 1 - 3 tanks(12800): 100, 200, 300
            // 2 - 3 tanks(12800): 100, 200, 300
            if (tick_round==100 || tick_round==200 || tick_round==300) { CreateEnemy(2, 12800, 8, 15, dry_des_1); }
            if (tick_round==100 || tick_round==200 || tick_round==300) { CreateEnemy(2, 12800, 8, 15, dry_des_2); }
            break;
        case 8:
            // 1 - 8 heavy_soldiers(6800): 100, 120, 140, ... 220, 240
            // 2 - 8 heavy_soldiers(6800): 100, 120, 140, ... 220, 240
            if (tick_round>=100 && tick_round<=240 && !(tick_round%20)) { CreateEnemy(1, 6800, 8, 4, dry_des_1); }
            if (tick_round>=100 && tick_round<=240 && !(tick_round%20)) { CreateEnemy(1, 6800, 8, 4, dry_des_2); }
            break;
        case 9:
            // 1 - 10 light_soldiers(3600): 110, 120, 130, ... 190, 200
            // 1 - 5 heavy_soldiers(7200): 210, 220, 230, 240, 250
            // 2 - 10 light_soldiers(3600): 110, 120, 130, ... 190, 200
            // 2 - 5 heavy_soldiers(7200): 210, 220, 230, 240, 250
            if (tick_round>=110 && tick_round<=200 && !(tick_round%10)) { CreateEnemy(0, 3600, 8, 2, dry_des_1); }
            if (tick_round>=110 && tick_round<=200 && !(tick_round%10)) { CreateEnemy(0, 3600, 8, 2, dry_des_2); }
            if (tick_round>=210 && tick_round<=250 && !(tick_round%10)) { CreateEnemy(1, 7200, 8, 4, dry_des_1); }
            if (tick_round>=210 && tick_round<=250 && !(tick_round%10)) { CreateEnemy(1, 7200, 8, 4, dry_des_2); }
            break;
        case 10:
            // 1 - 9 heavy_soldiers(7600): 125, 150, 175; 225, 250, 275; 325, 350, 375
            // 1 - 3 planes(11400): 200, 300, 400
            // 2 - 9 heavy_soldiers(7600): 125, 150, 175; 225, 250, 275; 325, 350, 375
            // 2 - 3 planes(11400): 200, 300, 400
            if ( tick_round >= 125 && tick_round <= 375 && !(tick_round%25) && (tick_round%100) ) { CreateEnemy(1, 7600, 8, 4, dry_des_1); }
            if ( tick_round >= 125 && tick_round <= 375 && !(tick_round%25) && (tick_round%100) ) { CreateEnemy(1, 7600, 8, 4, dry_des_2); }
            if ( tick_round == 200 || tick_round == 300 || tick_round == 400 ) { CreateEnemy(3, 11400, 10, 15, dry_des_1); }
            if ( tick_round == 200 || tick_round == 300 || tick_round == 400 ) { CreateEnemy(3, 11400, 10, 15, dry_des_2); }
            break;
        case 11:
            // 1 - 15 light_soldiers(4000): 100, 110, 120, ... 230, 240
            // 2 - 15 light_soldiers(4000): 105, 115, 125, ... 235, 245
            if (tick_round>=100 && tick_round<=240 && !(tick_round%10)) { CreateEnemy(0, 4000, 8, 2, dry_des_1); }
            if (tick_round>=105 && tick_round<=245 && (tick_round%10==5)) { CreateEnemy(0, 4000, 8, 2, dry_des_2); }
            break;
        case 12:
            // 1 - 10 heavy_soldiers(8400): 100, 115, 130, ... 235
            // 2 - 10 heavy_soldiers(8400): 105, 120, 135, ... 240
            if (tick_round>=100 && tick_round<=235 && (tick_round%15==10)) { CreateEnemy(0, 8400, 8, 3, dry_des_1); }
            if (tick_round>=105 && tick_round<=240 && !(tick_round%15)) { CreateEnemy(0, 8400, 8, 3, dry_des_2); }
            break;
        case 13:
            // 1 - 10 light_soldiers(4400): 110, 120, 130, ... 190, 200
            // 1 - 8 heavy_soldiers(8800): 210, 220, 230, ... 270, 280
            // 2 - 10 light_soldiers(4400): 110, 120, 130, ... 190, 200
            // 2 - 8 heavy_soldiers(8800): 210, 220, 230, ... 270, 280
            if (tick_round>=110 && tick_round<=200 && !(tick_round%10)) { CreateEnemy(0, 4400, 8, 2, dry_des_1); }
            if (tick_round>=110 && tick_round<=200 && !(tick_round%10)) { CreateEnemy(0, 4400, 8, 2, dry_des_2); }
            if (tick_round>=210 && tick_round<=280 && !(tick_round%10)) { CreateEnemy(1, 8800, 8, 3, dry_des_1); }
            if (tick_round>=210 && tick_round<=280 && !(tick_round%10)) { CreateEnemy(1, 8800, 8, 3, dry_des_2); }
            break;
        case 14:
            // 1 - 30 light_soldiers(4600): 100, 110, 120, ... 380, 390
            // 2 - 4 tanks(18400): 100, 200, 300, 400
            if (tick_round>=100 && tick_round<=390 && !(tick_round%10)) { CreateEnemy(0, 4600, 8, 2, dry_des_1); }
            if (tick_round>=100 && tick_round<=400 && !(tick_round%100)) { CreateEnemy(2, 18400, 8, 15, dry_des_2); }
            break;
        case 15:
            // 1 - 3 tanks(19200): 100, 200, 300
            // 2 - 20 heavy_soldiers(9600): 100, 110, 120, ... 280, 290
            if (tick_round>=100 && tick_round<=300 && !(tick_round%100)) { CreateEnemy(2, 19200, 8, 15, dry_des_1); }
            if (tick_round>=100 && tick_round<=290 && !(tick_round%10)) { CreateEnemy(1, 9600, 8, 3, dry_des_2); }
            break;
        case 16:
            // 1 - 5 tanks(20000): 100, 200, 300, 400, 500
            // 2 - 5 tanks(20000): 150, 250, 350, 450, 550
            if (tick_round>=100 && tick_round<=500 && !(tick_round%100)) { CreateEnemy(2, 20000, 8, 15, dry_des_1); }
            if (tick_round>=150 && tick_round<=550 && (tick_round%100==50)) { CreateEnemy(2, 20000, 8, 15, dry_des_2); }
            break;
        case 17:
            // 1 - 18 light_soldiers(5200): 100, 110, 120, ... 260, 270
            // 2 - 18 light_soldiers(5200): 105, 115, 125, ... 265, 275
            if (tick_round>=100 && tick_round<=270 && !(tick_round%10)) { CreateEnemy(0, 5200, 8, 2, dry_des_1); }
            if (tick_round>=105 && tick_round<=275 && (tick_round%10==5)) { CreateEnemy(0, 5200, 8, 2, dry_des_2); }
            break;
        case 18:
            // 1 - 18 heavy_soldiers(10800): 100, 110, 120, ... 260, 270
            // 2 - 18 heavy_soldiers(10800): 105, 115, 125, ... 265, 275
            if (tick_round>=100 && tick_round<=270 && !(tick_round%10)) { CreateEnemy(1, 10800, 8, 3, dry_des_1); }
            if (tick_round>=105 && tick_round<=275 && (tick_round%10==5)) { CreateEnemy(1, 10800, 8, 3, dry_des_2); }
            break;
        case 19:
            // 1 - 10 heavy_soldiers(11200): 105, 120, 135, ... 225, 240
            // 1 - 3 tanks(22400): 255, 270, 285
            // 2 - 10 heavy_soldiers(11200): 105, 120, 135, ... 225, 240
            // 2 - 3 tanks(22400): 255, 270, 285
            if (tick_round>=105 && tick_round<=240 && !(tick_round%15)) { CreateEnemy(1, 11200, 8, 3, dry_des_1); }
            if (tick_round>=105 && tick_round<=240 && !(tick_round%15)) { CreateEnemy(1, 11200, 8, 3, dry_des_2); }
            if (tick_round==255 || tick_round==270 || tick_round==285) { CreateEnemy(2, 22400, 8, 15, dry_des_1); }
            if (tick_round==255 || tick_round==270 || tick_round==285) { CreateEnemy(2, 22400, 8, 15, dry_des_2); }
            break;
        case 20:
            // 1 - 40 heavy_soldiers(11600): 100, 110, 120, ... 490
            // 2 - 4 tanks(23200): 150, 250, 350, 450
            // 2 - 5 planes(17400): 100, 200, 300, 400, 500
            if (tick_round >= 100 && tick_round <= 490 && !(tick_round%10)) { CreateEnemy(1, 11600, 8, 3, dry_des_1); }
            if (tick_round >= 150 && tick_round <= 450 && (tick_round%100==50)) { CreateEnemy(2, 23200, 8, 15, dry_des_2); }
            if (tick_round >= 100 && tick_round <= 500 && !(tick_round%100)) { CreateEnemy(3, 17400, 10, 15, dry_des_2); }
            break;
        case 21:
            // 1 - 40 light_soldiers(6000): 100, 105, 110, ... 290, 295
            // 2 - 8 heavy_soldiers(12000): 100, 125, 150, ... 250, 275
            if ( tick_round >= 100 && tick_round <= 295 && !(tick_round%5) ) { CreateEnemy(0, 6000, 8, 1, dry_des_1); }
            if ( tick_round >= 100 && tick_round <= 275 && !(tick_round%25) ) { CreateEnemy(1, 12000, 8, 2, dry_des_2); }
            break;
        case 22:
            // 1 - 10 heavy_soldiers(12400): 100, 125, 150, ... 300, 325
            // 2 - 50 light_soldiers(6200): 100, 105, 110, ... 340, 345
            if ( tick_round >= 100 && tick_round <= 325 && !(tick_round%25) ) { CreateEnemy(1, 12400, 8, 2, dry_des_1); }
            if ( tick_round >= 100 && tick_round <= 345 && !(tick_round%5) ) { CreateEnemy(0, 6200, 8, 1, dry_des_2); }
            break;
        case 23:
            // 1 - 6 tanks(25600): 100, 150, 200, 250, 300, 350
            // 2 - 6 tanks(25600): 125, 175, 225, 275, 325, 375
            if (tick_round>=100 && tick_round<=350 && !(tick_round%50)) { CreateEnemy(2, 25600, 8, 10, dry_des_1); }
            if (tick_round>=125 && tick_round<=375 && (tick_round%50==25)) { CreateEnemy(2, 25600, 8, 10, dry_des_2); }
            break;
        case 24:
            // 1 - 40 light_soldiers(6600): 100, 105, 110, ... 290, 295
            // 2 - 40 light_soldiers(6600): 100, 105, 110, ... 290, 295
            if ( tick_round >= 100 && tick_round <= 295 && !(tick_round%5) ) { CreateEnemy(0, 6600, 8, 1, dry_des_1); }
            if ( tick_round >= 100 && tick_round <= 295 && !(tick_round%5) ) { CreateEnemy(0, 6600, 8, 1, dry_des_2); }
            break;
        case 25:
            // 1 - 27 heavy_soldiers(13600): 110, 120, 130, ..., 190; ... ; 310, 320, 330, ..., 390
            // 1 - 3 planes(20400): 200, 300, 400
            // 2 - 27 heavy_soldiers(13600): 110, 120, 130, ..., 190; ... ; 310, 320, 330, ..., 390
            // 2 - 3 planes(20400): 200, 300, 400
            if ( tick_round >= 110 && tick_round <= 390 && !(tick_round%10) && (tick_round%100) ) { CreateEnemy(1, 13600, 8, 2, dry_des_1); }
            if ( tick_round >= 110 && tick_round <= 390 && !(tick_round%10) && (tick_round%100) ) { CreateEnemy(1, 13600, 8, 2, dry_des_2); }
            if ( tick_round == 200 || tick_round == 300 || tick_round == 400 ) { CreateEnemy(3, 20400, 10, 12, dry_des_1); }
            if ( tick_round == 200 || tick_round == 300 || tick_round == 400 ) { CreateEnemy(3, 20400, 10, 12, dry_des_2); }
            break;
        case 26:
            // 1 - 6 planes(24500): 125, 175, 225, 275, 325, 375
            // 2 - 60 light_soldiers(7000): 100, 105, 110, ... 390, 395
            if (tick_round >= 125 && tick_round <= 375 && (tick_round%50==25)) { CreateEnemy(3, 21000, 10, 12, dry_des_1); }
            if (tick_round >= 100 && tick_round <= 395 && !(tick_round%5)) { CreateEnemy(0, 7000, 8, 1, dry_des_2); }
            break;
        case 27:
            // 1 - 40 heavy_soldiers(14000): 100, 105, 110, ... 290, 295
            // 2 - 6 tanks(31500): 100, 140, 180, 220, 260, 300
            if (tick_round >= 100 && tick_round <= 295 && !(tick_round%5)) { CreateEnemy(1, 14000, 8, 2, dry_des_1); }
            if (tick_round >= 100 && tick_round <= 300 && (tick_round%40==20)) { CreateEnemy(2, 28000, 8, 12, dry_des_2); }
            break;
        case 28:
            // 1 - 35 heavy_soldiers(14000): 100, 105, 110, ... 265, 270
            // 2 - 35 heavy_soldiers(14000): 100, 105, 110, ... 265, 270
            if (tick_round >= 100 && tick_round <= 270 && !(tick_round%5)) { CreateEnemy(1, 14000, 8, 2, dry_des_1); }
            if (tick_round >= 100 && tick_round <= 270 && !(tick_round%5)) { CreateEnemy(1, 14000, 8, 2, dry_des_1); }
            break;
        case 29:
            // 1 - 45 light_soldiers(7000): 110, 120, 130, ..., 190; ... ; 510, 520, 530, ..., 590
            // 1 - 5 tanks(31500): 200, 300, 400, 500, 600
            // 2 - 45 light_soldiers(7000): 110, 120, 130, ..., 190; ... ; 510, 520, 530, ..., 590
            // 2 - 5 tanks(31500): 200, 300, 400, 500, 600
            if ( tick_round >= 110 && tick_round <= 590 && !(tick_round%10) && (tick_round%100) ) { CreateEnemy(0, 7000, 8, 1, dry_des_1); }
            if ( tick_round >= 110 && tick_round <= 590 && !(tick_round%10) && (tick_round%100) ) { CreateEnemy(0, 7000, 8, 1, dry_des_2); }
            if ( tick_round >= 200 && tick_round <= 600 && !(tick_round%100) ) { CreateEnemy(3, 28000, 10, 12, dry_des_1); }
            if ( tick_round >= 200 && tick_round <= 600 && !(tick_round%100) ) { CreateEnemy(3, 28000, 10, 12, dry_des_2); }
            break;
        case 30:
            // 1 - 12 heavy_soldiers(14000): 120, 160; ... ; 620, 660
            // 1 - 12 tanks(31500): 140, 180; ... ; 640, 680
            // 1 - 4 planes(24500): 200, 300, 500, 600
            // 1 - 2 blimps(90000): 400, 700
            // 2 - 12 heavy_soldiers(14000): 120, 160; ... ; 620, 660
            // 2 - 12 tanks(31500): 140, 180; ... ; 640, 680
            // 2 - 4 planes(24500): 200, 300, 500, 600
            // 2 - 2 blimps(90000): 400, 700
            if ( tick_round >= 120 && tick_round <= 680 && (tick_round%100==20 || tick_round%100==60) ) { CreateEnemy(1, 14000, 8, 2, dry_des_1); }
            if ( tick_round >= 120 && tick_round <= 680 && (tick_round%100==20 || tick_round%100==60) ) { CreateEnemy(1, 14000, 8, 2, dry_des_2); }
            if ( tick_round >= 120 && tick_round <= 680 && (tick_round%100==40 || tick_round%100==80) ) { CreateEnemy(2, 31500, 8, 12, dry_des_1); }
            if ( tick_round >= 120 && tick_round <= 680 && (tick_round%100==40 || tick_round%100==80) ) { CreateEnemy(2, 31500, 8, 12, dry_des_2); }
            if ( tick_round == 200 || tick_round == 300 || tick_round == 500 || tick_round == 600) { CreateEnemy(3, 21000, 10, 12, dry_des_1); }
            if ( tick_round == 200 || tick_round == 300 || tick_round == 500 || tick_round == 600) { CreateEnemy(3, 21000, 10, 12, dry_des_2); }
            if ( tick_round == 400 || tick_round == 700 ) { CreateEnemy(4, 90000, 4, 25, dry_des_1); }
            if ( tick_round == 400 || tick_round == 700 ) { CreateEnemy(4, 90000, 4, 25, dry_des_2); }
            break;
        default:
            break;
    }
}

void SpawnEnemyFrostbite() {
    switch ( rounds ) {
        case 1:
            // Drawing arrows
            if ( tick_round < 100 ) {
                rect_arrow[0].x = 64;
                rect_arrow[0].y = 454;
                tex_arrow[0] = SDL_CreateTextureFromSurface(Renderer, sur_arrow[0]);
                SDL_RenderCopy(Renderer, tex_arrow[0], NULL, &rect_arrow[0]);
                SDL_DestroyTexture(tex_arrow[0]);
                rect_arrow[1].x = 724;
                rect_arrow[1].y = 64;
                tex_arrow[1] = SDL_CreateTextureFromSurface(Renderer, sur_arrow[1]);
                SDL_RenderCopy(Renderer, tex_arrow[1], NULL, &rect_arrow[1]);
                SDL_DestroyTexture(tex_arrow[1]);
            }
            // 1 - 6 light_soldiers(2000): 100, 120, 140, 160, 180, 200
            if (tick_round>=100 && tick_round<=200 && !(tick_round%20)) { CreateEnemy(0, 2000, 8, 2, frost_des_1); }
            break;
        case 2:
            // 1 - 10 light_soldiers(2200): 100, 120, 140, ... 260, 280
            if (tick_round>=100 && tick_round<=280 && !(tick_round%20)) { CreateEnemy(0, 2200, 8, 2, frost_des_1); }
            break;
        case 3:
            // 1 - 6 light_soldiers(2400): 100, 120, 140, 160, 180, 200
            // 2 - 6 light_soldiers(2400): 110, 130, 150, 170, 190, 210
            if (tick_round>=100 && tick_round<=200 && !(tick_round%20)) { CreateEnemy(0, 2400, 8, 2, frost_des_1); }
            if (tick_round>=110 && tick_round<=210 && (tick_round%20==10)) { CreateEnemy(0, 2400, 8, 2, frost_des_2); }
            break;
        case 4:
            // 1 - 8 heavy_soldiers(5200): 100, 120, 140, ... 220, 240
            if (tick_round>=100 && tick_round<=240 && !(tick_round%20)) { CreateEnemy(1, 5200, 8, 3, frost_des_1); }
            break;
        case 5:
            // 1 - 8 heavy_soldiers(5600): 100, 120, 140, ... 220, 240
            // 2 - 8 light_soldiers(2800): 110, 130, 150, ... 230, 250
            if (tick_round>=100 && tick_round<=240 && !(tick_round%20)) { CreateEnemy(1, 5600, 8, 2, frost_des_1); }
            if (tick_round>=110 && tick_round<=250 && (tick_round%20==10)) { CreateEnemy(0, 2800, 8, 2, frost_des_2); }
            break;
        case 6:
            // 1 - 10 ice_soldiers(6000): 105, 120, 135, ... 225, 240
            if (tick_round>=105 && tick_round<=240 && !(tick_round%15)) { CreateEnemy(5, 6000, 8, 2, frost_des_1); }
            break;
        case 7:
            // 1 - 8 heavy_soldiers(6400): 100, 120, 140, ... 220, 240
            // 2 - 8 ice_soldiers(6400): 110, 130, 150, ... 230, 250
            if (tick_round>=100 && tick_round<=240 && !(tick_round%20)) { CreateEnemy(1, 6400, 8, 2, frost_des_1); }
            if (tick_round>=110 && tick_round<=250 && (tick_round%20==10)) { CreateEnemy(5, 6400, 8, 2, frost_des_2); }
            break;
        case 8:
            // 1 - 10 ice_bikes(10200): 100, 125, 150, ... 300, 325
            if (tick_round>=100 && tick_round<=325 && !(tick_round%25)) { CreateEnemy(6, 10200, 8, 5, frost_des_1); }
            break;
        case 9:
            // 1 - 10 ice_bikes(10800): 100, 120, 140, ... 260, 280
            // 2 - 10 heavy_soldiers(7200): 110, 130, 150, ... 270, 290
            if (tick_round>=100 && tick_round<=280 && !(tick_round%20)) { CreateEnemy(6, 10600, 8, 4, frost_des_1); }
            if (tick_round>=110 && tick_round<=290 && (tick_round%20==10)) { CreateEnemy(1, 6400, 8, 3, frost_des_2); }
            break;
        case 10:
            // 1 - 6 tanks(19000): 100, 150, 200, 250, 300, 350
            // 2 - 15 heavy_soldiers(7600): 100, 120, 140, ... 360, 380
            if (tick_round>=100 && tick_round<=350 && !(tick_round%50)) { CreateEnemy(2, 19000, 8, 12, frost_des_1); }
            if (tick_round>=100 && tick_round<=380 && !(tick_round%20)) { CreateEnemy(1, 7600, 8, 3, frost_des_2); }
            break;
        case 11:
            // 1 - 10 ice_soldiers(8000): 100, 120, 140, ... 260, 280
            // 2 - 10 ice_bikes(12000): 110, 130, 150, ... 270, 290
            if (tick_round>=100 && tick_round<=280 && !(tick_round%20)) { CreateEnemy(5, 8000, 8, 2, frost_des_1); }
            if (tick_round>=110 && tick_round<=290 && (tick_round%20==10)) { CreateEnemy(6, 12000, 8, 4, frost_des_1); }
            break;
        case 12:
            // 1 - 12 heavy_soldiers(8400): 100, 120, 140, ... 300, 320
            // 1 - 12 ice_bikes(12600): 110, 130, 150, ... 310, 330
            if (tick_round>=100 && tick_round<=320 && !(tick_round%20)) { CreateEnemy(1, 8400, 8, 3, frost_des_1); }
            if (tick_round>=110 && tick_round<=330 && (tick_round%20==10)) { CreateEnemy(6, 12600, 8, 4, frost_des_1); }
            break;
        case 13:
            // 1 - 10 ice_bikes(13200): 90, 120, 150, ... 330, 360
            // 2 - 10 ice_bikes(13200): 105, 135, 165, ... 345, 375
            if (tick_round>=90 && tick_round<=360 && !(tick_round%30)) { CreateEnemy(6, 13200, 8, 4, frost_des_1); }
            if (tick_round>=105 && tick_round<=375 && (tick_round%30==15)) { CreateEnemy(6, 13200, 8, 4, frost_des_2); }
            break;
        case 14:
            // 1 - 27 ice_soldiers(9200): 100, 110, 120, ... 350, 360
            // 2 - 10 ice_bikes(13800): 90, 120, 150, ... 330, 360
            if (tick_round>=100 && tick_round<=360 && !(tick_round%10)) { CreateEnemy(5, 9200, 8, 2, frost_des_1); }
            if (tick_round>=105 && tick_round<=375 && (tick_round%30==15)) { CreateEnemy(6, 13800, 8, 4, frost_des_2); }
            break;
        case 15:
            // 1 - 8 planes(19200): 100, 150, 200, ... 400, 450
            // 2 - 20 heavy soldiers(9600): 100, 120, 140, ... 460, 480
            if (tick_round>=100 && tick_round<=450 && !(tick_round%50)) { CreateEnemy(3, 19200, 10, 10, frost_des_1); }
            if (tick_round>=100 && tick_round<=480 && !(tick_round%20)) { CreateEnemy(1, 9600, 8, 3, frost_des_2); }
            break;
        case 16:
            // 1 - 40 light_soldiers(5000): 100, 105, 110, ... 290, 295
            // 2 - 20 ice_soldiers(10000): 100, 110, 120, ... 280, 290
            if (tick_round>=100 && tick_round<=295 && !(tick_round%5)) { CreateEnemy(0, 5000, 8, 2, frost_des_1); }
            if (tick_round>=100 && tick_round<=290 && !(tick_round%10)) { CreateEnemy(5, 10000, 8, 2, frost_des_2); }
            break;
        case 17:
            // 1 - 8 heavy_soldiers(10400): 120, 160, 200, ... 360, 400
            // 1 - 8 tanks(26000): 100, 140, 180, ... 340, 380
            if (tick_round>=120 && tick_round<=400 && !(tick_round%40)) { CreateEnemy(1, 10400, 8, 3, frost_des_1); }
            if (tick_round>=100 && tick_round<=380 && (tick_round%40==20)) { CreateEnemy(2, 26000, 8, 10, frost_des_1); }
            break;
        case 18:
            // 1 - 8 heavy_soldiers(10800): 120, 160, 200, ... 360, 400
            // 2 - 8 ice_soldiers(10800): 120, 160, 200, ... 360, 400
            // 2 - 8 tanks(27000): 100, 140, 180, ... 340, 380
            if (tick_round>=120 && tick_round<=400 && !(tick_round%40)) { CreateEnemy(1, 10800, 8, 3, frost_des_1); }
            if (tick_round>=120 && tick_round<=400 && !(tick_round%40)) { CreateEnemy(5, 10800, 8, 2, frost_des_2); }
            if (tick_round>=100 && tick_round<=380 && (tick_round%40==20)) { CreateEnemy(2, 27000, 8, 10, frost_des_2); }
            break;
        case 19:
            // 1 - 40 ice_soldiers(11200): 100, 105, 110, ... 290, 295
            // 2 - 20 heavy_soldiers(11200): 100, 110, 120, ... 280, 290
            if (tick_round>=100 && tick_round<=295 && !(tick_round%5)) { CreateEnemy(5, 11200, 8, 2, frost_des_1); }
            if (tick_round>=100 && tick_round<=290 && !(tick_round%10)) { CreateEnemy(1, 11200, 8, 3, frost_des_2); }
            break;
        case 20:
            // 1 - 12 ice_soldiers(11600): 120, 160, 200, ... 520, 560
            // 1 - 12 ice_bikes(17400): 100, 140, 180, ... 500, 540
            // 2 - 5 planes(23200): 150, 250, 350, 450, 550
            if (tick_round>=120 && tick_round<=560 && !(tick_round%40)) { CreateEnemy(5, 11600, 8, 2, frost_des_1); }
            if (tick_round>=100 && tick_round<=540 && (tick_round%40==20)) { CreateEnemy(6, 17400, 8, 4, frost_des_1); }
            if (tick_round>=150 && tick_round<=550 && (tick_round%100==50)) { CreateEnemy(3, 23200, 10, 10, frost_des_2); }
            break;
        case 21:
            // 1 - 30 ice_soldiers(12000): 100, 105, 110, ... 240, 245
            // 2 - 30 ice_soldiers(12000): 100, 105, 110, ... 240, 245
            if (tick_round>=100 && tick_round<=245 && !(tick_round%5)) { CreateEnemy(5, 12000, 8, 1, frost_des_1); }
            if (tick_round>=100 && tick_round<=245 && !(tick_round%5)) { CreateEnemy(5, 12000, 8, 1, frost_des_2); }
            break;
        case 22:
            // 1 - 21 ice_bikes(18600): 100, 110, 120, ... 290, 300
            if (tick_round>=100 && tick_round<=300 && !(tick_round%10)) { CreateEnemy(6, 18600, 8, 3, frost_des_1); }
            break;
        case 23:
            // 1 - 8 planes(25600): 100, 125, 150, ... 250, 275
            // 2 - 36 ice_soldiers(12800): 105, 110, 115, ... 290, 295
            // 2 - 4 ice_bikes(19200): 100, 150, 200, 250
            if (tick_round>=100 && tick_round<=275 && !(tick_round%25)) { CreateEnemy(3, 25600, 10, 10, frost_des_1); }
            if (tick_round>=105 && tick_round<=295 && !(tick_round%5) && (tick_round%50)) { CreateEnemy(5, 12800, 8, 1, frost_des_2); }
            if (tick_round>=100 && tick_round<=250 && !(tick_round%50)) { CreateEnemy(6, 19200, 8, 3, frost_des_1); }
            break;
        case 24:
            // 1 - 12 ice_bikes(19800): 100, 120, 140, ... 300, 320
            // 2 - 12 ice_bikes(19800): 110, 130, 150, ... 310, 330
            if (tick_round>=100 && tick_round<=320 && !(tick_round%20)) { CreateEnemy(6, 19800, 8, 3, frost_des_1); }
            if (tick_round>=110 && tick_round<=330 && (tick_round%20==10)) { CreateEnemy(6, 19800, 8, 3, frost_des_2); }
            break;
        case 25:
            // 1 - 8 planes(27200): 100, 140, 180, ... 340, 380
            // 1 - 1 blimp(80000): 420
            // 2 - 8 planes(27200): 120, 160, 200, ... 360, 400
            // 2 - 1 blimp(80000): 440
            if (tick_round>=100 && tick_round<=380 && (tick_round%40==20)) { CreateEnemy(3, 27200, 10, 8, frost_des_1); }
            if (tick_round>=120 && tick_round<=400 && !(tick_round%40)) { CreateEnemy(3, 27200, 10, 8, frost_des_2); }
            if (tick_round==420) { CreateEnemy(4, 80000, 4, 25, frost_des_1); }
            if (tick_round==440) { CreateEnemy(4, 80000, 4, 25, frost_des_2); }
            break;
        case 26:
            // 1 - 40 heavy_soldiers(14000): 100, 105, 110, ... 290, 295
            // 2 - 40 light_soldiers(7000): 100, 105, 110, ... 290, 295
            if (tick_round>=100 && tick_round<=295 && !(tick_round%5)) { CreateEnemy(0, 12000, 8, 1, frost_des_1); }
            if (tick_round>=100 && tick_round<=295 && !(tick_round%5)) { CreateEnemy(1, 12000, 8, 2, frost_des_2); }
            break;
        case 27:
            // 1 - 8 ice_bikes(21000): 120, 160, 200, ... 360, 400
            // 2 - 8 heavy_soldiers(14000): 120, 160, 200, ... 360, 400
            // 2 - 8 tanks(35000): 100, 140, 180, ... 340, 380
            if (tick_round>=120 && tick_round<=400 && !(tick_round%40)) { CreateEnemy(6, 21000, 8, 3, frost_des_1); }
            if (tick_round>=120 && tick_round<=400 && !(tick_round%40)) { CreateEnemy(1, 14000, 8, 2, frost_des_2); }
            if (tick_round>=100 && tick_round<=380 && (tick_round%40==20)) { CreateEnemy(2, 35000, 8, 8, frost_des_2); }
            break;
        case 28:
            // 1 - 12 tanks(37500): 100, 125, 150, ... 350, 375
            // 2 - 12 tanks(37500): 100, 125, 150, ... 350, 375
            if (tick_round>=100 && tick_round<=375 && !(tick_round%25)) { CreateEnemy(2, 37500, 8, 8, frost_des_1); }
            if (tick_round>=100 && tick_round<=375 && !(tick_round%25)) { CreateEnemy(2, 37500, 8, 8, frost_des_2); }
            break;
        case 29:
            // 1 - 35 ice_soldiers(14000): 100, 110, 120, ... 430, 440
            // 1 - 35 ice_bikes(21000): 105, 115, 125, ... 435, 445
            if (tick_round>=100 && tick_round<=440 && !(tick_round%10)) { CreateEnemy(5, 14000, 8, 1, frost_des_1); }
            if (tick_round>=105 && tick_round<=445 && (tick_round%10==5)) { CreateEnemy(6, 21000, 8, 3, frost_des_1); }
            break;
        case 30:
            // 1 - 30 heavy_soldiers(14000): 100, 120, 140, ... 660, 680
            // 1 - 30 ice_bikes(24500): 110, 130, 150, ... 670, 690
            // 2 - 4 planes(30000): 200, 300, 500, 600
            // 2 - 2 blimps(100000): 400, 700
            if (tick_round>=100 && tick_round<=680 && !(tick_round%20)) { CreateEnemy(1, 14000, 8, 2, frost_des_1); }
            if (tick_round>=110 && tick_round<=690 && (tick_round%20==10)) { CreateEnemy(6, 24500, 8, 3, frost_des_1); }
            if (tick_round==200 || tick_round==300 || tick_round==500 || tick_round==600) { CreateEnemy(3, 30000, 10, 8, frost_des_2); }
            if (tick_round==400 || tick_round==700) { CreateEnemy(4, 90000, 4, 20, frost_des_2); }
            break;
        default:
            break;
    }
}

void ViewTower() {
    if ( tower_selected ) {
        for ( int i=0; i<=19; i++ ) {
            rect_grid[0].x = 148 + 64 * i;
            rect_grid[0].y = 134;
            tex_grid[0] = SDL_CreateTextureFromSurface(Renderer, sur_grid[0]);
            SDL_RenderCopy(Renderer, tex_grid[0], NULL, &rect_grid[0]);
            SDL_DestroyTexture(tex_grid[0]);
        }
        for ( int i=0; i<=11; i++ ) {
            rect_grid[1].x = 148;
            rect_grid[1].y = 134 + 64 * i;
            tex_grid[1] = SDL_CreateTextureFromSurface(Renderer, sur_grid[1]);
            SDL_RenderCopy(Renderer, tex_grid[1], NULL, &rect_grid[1]);
            SDL_DestroyTexture(tex_grid[1]);
        }
    }

    for ( int i=1; i<=11; i++ ) {
        for ( int j=1; j<=19; j++ ) {
            if ( viewing_tower[i][j] ) {
                rect_circle.w = 2 * tower_radius_[(viewing_tower[i][j] - 1) / 3];
                rect_circle.h = 2 * tower_radius_[(viewing_tower[i][j] - 1) / 3];
                rect_circle.x = (64 * j - 64 + 134 + 44) - rect_circle.w / 2;
                rect_circle.y = (64 * i - 64 + 148 + 24) - rect_circle.h / 2;
                tex_circle = SDL_CreateTextureFromSurface(Renderer, sur_circle);
                SDL_RenderCopy(Renderer, tex_circle, NULL, &rect_circle);
                SDL_DestroyTexture(tex_circle);
            }
        }
    }
}

void ViewTower2() {
    for ( int i=1; i<=11; i++ ) {
        for ( int j=1; j<=19; j++ ) {
            if ( viewing_tower[i][j] ) {
                rect_tower_menu[0].x = 64 * j - 64 + 134 - 70;
                rect_tower_menu[0].y = 64 * i - 64 + 148 - 20;
                tex_tower_menu[0] = SDL_CreateTextureFromSurface(Renderer, sur_tower_menu[0]);
                SDL_RenderCopy(Renderer, tex_tower_menu[0], NULL, &rect_tower_menu[0]);
                SDL_DestroyTexture(tex_tower_menu[0]);
                if ( maze[i][j] % 3 ) {
                    if ( money >= upgrading_prices_[maze[i][j]-1] ) {
                        rect_tower_menu[1].x = 64 * j - 64 + 134 + 80;
                        rect_tower_menu[1].y = 64 * i - 64 + 148 - 20;
                        tex_tower_menu[1] = SDL_CreateTextureFromSurface(Renderer, sur_tower_menu[1]);
                        SDL_RenderCopy(Renderer, tex_tower_menu[1], NULL, &rect_tower_menu[1]);
                        SDL_DestroyTexture(tex_tower_menu[1]);
                    } else {
                        rect_tower_menu[2].x = 64 * j - 64 + 134 + 80;
                        rect_tower_menu[2].y = 64 * i - 64 + 148 - 20;
                        tex_tower_menu[2] = SDL_CreateTextureFromSurface(Renderer, sur_tower_menu[2]);
                        SDL_RenderCopy(Renderer, tex_tower_menu[2], NULL, &rect_tower_menu[2]);
                        SDL_DestroyTexture(tex_tower_menu[2]);
                    }
                }
                rect_font_upgrading[viewing_tower[i][j]-1].x = 64 * j - 64 + 134 + 120;
                rect_font_upgrading[viewing_tower[i][j]-1].y = 64 * i - 64 + 148 + 30;
                tex_font_upgrading[viewing_tower[i][j]-1] = SDL_CreateTextureFromSurface(Renderer, sur_font_upgrading[viewing_tower[i][j]-1]);
                SDL_RenderCopy(Renderer, tex_font_upgrading[viewing_tower[i][j]-1], NULL, &rect_font_upgrading[viewing_tower[i][j]-1]);
                SDL_DestroyTexture(tex_font_upgrading[viewing_tower[i][j]-1]);
                rect_font_selling[viewing_tower[i][j]-1].x = 64 * j - 64 + 134 - 25;
                rect_font_selling[viewing_tower[i][j]-1].y = 64 * i - 64 + 148 + 30;
                tex_font_selling[viewing_tower[i][j]-1] = SDL_CreateTextureFromSurface(Renderer, sur_font_selling[viewing_tower[i][j]-1]);
                SDL_RenderCopy(Renderer, tex_font_selling[viewing_tower[i][j]-1], NULL, &rect_font_selling[viewing_tower[i][j]-1]);
                SDL_DestroyTexture(tex_font_selling[viewing_tower[i][j]-1]);
            }
        }
    }
}

void LOAD() {
    char pic_name[20];

    // Background Pictures in-game
    sur_main[0] = IMG_Load("img/grasslands.jpg");
    rect_main[0].x = 0;
    rect_main[0].y = 0;
    rect_main[0].w = sur_main[0]->w;
    rect_main[0].h = sur_main[0]->h;
    sur_main[1] = IMG_Load("img/drylands.jpg");
    rect_main[1].x = 0;
    rect_main[1].y = 0;
    rect_main[1].w = sur_main[1]->w;
    rect_main[1].h = sur_main[1]->h;
    sur_main[2] = IMG_Load("img/frostbite.jpg");
    rect_main[2].x = 0;
    rect_main[2].y = 0;
    rect_main[2].w = sur_main[2]->w;
    rect_main[2].h = sur_main[2]->h;
    sur_main[3] = IMG_Load("img/main.jpg");

    // Background picture in menu
    rect_main[3].x = 0;
    rect_main[3].y = 0;
    rect_main[3].w = sur_main[3]->w;
    rect_main[3].h = sur_main[3]->h;

    // Health and money
    sur_health = IMG_Load("img/icon_health.png");
    rect_health.x = 0;
    rect_health.y = 0;
    rect_health.w = sur_health->w;
    rect_health.h = sur_health->h;
    sur_money = IMG_Load("img/icon_money.png");
    rect_money.x = 1200;
    rect_money.y = 0;
    rect_money.w = sur_money->w;
    rect_money.h = sur_money->h;

    // Area of towers
    sur_circle = IMG_Load("img/circle.png");

    // Main menu map-selection
    for ( int i=0; i<3; i++ ) {
        sprintf(pic_name, "img/map_%d.png", i);
        sur_map[i] = IMG_Load(pic_name);
        rect_map[i].x = 100;
        rect_map[i].y = 80 + 250 * i;
        rect_map[i].w = sur_map[i]->w;
        rect_map[i].h = sur_map[i]->h;
    }

    // Win/lose titles
    for ( int i=0; i<2; i++ ) {
        sprintf(pic_name, "img/title_%d.png", i);
        sur_title[i] = IMG_Load(pic_name);
        rect_title[i].x = 400;
        rect_title[i].y = 400;
        rect_title[i].w = 2 * sur_title[i]->w;
        rect_title[i].h = 2 * sur_title[i]->h;
    }

    // Icons of towers
    for ( int i=0; i<8; i++ ) {
        sprintf(pic_name, "img/icon_%d.png", i);
        sur_icon[i] = IMG_Load(pic_name);
        rect_icon[i].x = 150 * (i % 4);
        rect_icon[i].y = 840;
        rect_icon[i].w = sur_icon[i]->w;
        rect_icon[i].h = sur_icon[i]->h;
    }

    // enemies
    for ( int i=0; i<4; i++ ) {
        for ( int j=0; j<4; j++ ) {
            sprintf(pic_name, "img/light_soldier/soldier_%d%d.png", i, j);
            sur_soldier[0][i][j] = IMG_Load(pic_name);
            sprintf(pic_name, "img/heavy_soldier/soldier_%d%d.png", i, j);
            sur_soldier[1][i][j] = IMG_Load(pic_name);
            sprintf(pic_name, "img/tank/tank_%d%d.png", i, j);
            sur_soldier[2][i][j] = IMG_Load(pic_name);
            sprintf(pic_name, "img/ice_soldier/soldier_%d%d.png", i, j);
            sur_soldier[5][i][j] = IMG_Load(pic_name);
            sprintf(pic_name, "img/ice_bike/soldier_%d%d.png", i, j);
            sur_soldier[6][i][j] = IMG_Load(pic_name);
            sprintf(pic_name, "img/robot/robot_%d%d.png", i, j);
            sur_soldier[7][i][j] = IMG_Load(pic_name);
        }
        sprintf(pic_name, "img/plane/plane_0%d.png", i);
        sur_soldier[3][0][i] = IMG_Load(pic_name);
        sprintf(pic_name, "img/plane/plane_2%d.png", i);
        sur_soldier[3][2][i] = IMG_Load(pic_name);
        sprintf(pic_name, "img/blimp/blimp_0%d.png", i);
        sur_soldier[4][0][i] = IMG_Load(pic_name);
        sprintf(pic_name, "img/blimp/blimp_2%d.png", i);
        sur_soldier[4][2][i] = IMG_Load(pic_name);
    }

    // Towers
    for ( int i=0; i<4; i++ ) {
        for ( int j=0; j<3; j++ ) {
            for ( int m=0; m<2; m++ ) {
                for ( int n=0; n<8; n++ ) {
                    sprintf(pic_name, "img/tower/%d/tower_0%d%d%d.png", i, j, m, n);
                    sur_tower[i][j][m][n] = IMG_Load(pic_name);
                    rect_tower[i][j][m][n].w = 64;
                    rect_tower[i][j][m][n].h = 64;
                }
            }
        }
    }

    // Gunfire
    for ( int i=0; i<12; i++ ) {
        sprintf(pic_name, "img/gunfire_%d.png", i);
        sur_gunfire[i] = IMG_Load(pic_name);
        rect_gunfire[i].w = sur_gunfire[i]->w;
        rect_gunfire[i].h = sur_gunfire[i]->h;
    }

    // HUD in-game
    for ( int i=0; i<5; i++ ) {
        sprintf(pic_name, "img/hud_%d.png", i);
        sur_hud[i] = IMG_Load(pic_name);
        rect_hud[i].w = (int)(sur_hud[i]->w * 0.7);
        rect_hud[i].h = (int)(sur_hud[i]->h * 0.7);
        rect_hud[i].x = 180 + 70 * (i/2);
        rect_hud[i].y = 20;
    }

    // Tower menu
    for ( int i=0; i<3; i++ ) {
        sprintf(pic_name, "img/tower_menu_%d.png", i);
        sur_tower_menu[i] = IMG_Load(pic_name);
        rect_tower_menu[i].w = (int)(sur_tower_menu[i]->w * 0.7);
        rect_tower_menu[i].h = (int)(sur_tower_menu[i]->h * 0.7);
    }

    // Arrows
    for ( int i=0; i<2; i++ ) {
        sprintf(pic_name, "img/arrow_%d.png", i);
        sur_arrow[i] = IMG_Load(pic_name);
        rect_arrow[i].w = sur_arrow[i]->w;
        rect_arrow[i].h = sur_arrow[i]->h;
    }

    // Grids
    for ( int i=0; i<2; i++ ) {
        sprintf(pic_name, "img/grid_%d.png", i);
        sur_grid[i] = IMG_Load(pic_name);
        rect_grid[0].w = sur_grid[0]->w;
        rect_grid[0].h = sur_grid[0]->h;
        rect_grid[1].w = sur_grid[i]->w;
        rect_grid[1].h = sur_grid[i]->h;
    }

    // Fonts
    font1 = TTF_OpenFont("font/a.ttf", 48);
    font2 = TTF_OpenFont("font/b.ttf", 24);
    font3 = TTF_OpenFont("font/b.ttf", 20);
    font4 = TTF_OpenFont("font/a.ttf", 96);

    // Main menu title
    sur_text = TTF_RenderUTF8_Blended(font4, "Field-runners", color_font2);
    rect_text.x = 500;
    rect_text.y = 200;
    rect_text.w = sur_text->w;
    rect_text.h = sur_text->h;

    // Health, money and round
    rect_font_health.x = 90;
    rect_font_health.y = 20;
    rect_font_money.x = 1290;
    rect_font_money.y = 20;
    rect_round.x = 1000;
    rect_round.y = 900;

    // Prices of towers
    for ( int i=0; i<4; i++ ) {
        sur_prices[i] = TTF_RenderUTF8_Blended(font2, prices[i], color_font2);
        rect_prices[i].x = 70 + 150 * i;
        rect_prices[i].y = 915;
        rect_prices[i].w = sur_prices[i]->w;
        rect_prices[i].h = sur_prices[i]->h;
    }
    for ( int i=0; i<12; i++ ) {
        sur_font_upgrading[i] = TTF_RenderUTF8_Blended(font3, upgrading_prices[i], color_font2);
        rect_font_upgrading[i].w = sur_font_upgrading[i]->w;
        rect_font_upgrading[i].h = sur_font_upgrading[i]->h;
        sur_font_selling[i] = TTF_RenderUTF8_Blended(font3, selling_prices[i], color_font2);
        rect_font_selling[i].w = sur_font_selling[i]->w;
        rect_font_selling[i].h = sur_font_selling[i]->h;
    }
}

void QUIT() {
    SDL_FreeSurface(sur_text);
    SDL_FreeSurface(sur_round);
    SDL_FreeSurface(sur_money);
    SDL_FreeSurface(sur_health);
    SDL_FreeSurface(sur_circle);
    SDL_FreeSurface(sur_font_money);
    SDL_FreeSurface(sur_font_health);
    for ( int i=0; i<3; i++ ) { SDL_FreeSurface(sur_map[i]); }
    for ( int i=0; i<5; i++ ) { SDL_FreeSurface(sur_hud[i]); }
    for ( int i=0; i<4; i++ ) { SDL_FreeSurface(sur_main[i]); }
    for ( int i=0; i<8; i++ ) { SDL_FreeSurface(sur_icon[i]); }
    for ( int i=0; i<2; i++ ) { SDL_FreeSurface(sur_title[i]); }
    for ( int i=0; i<4; i++ ) { SDL_FreeSurface(sur_prices[i]); }
    for ( int i=0; i<12; i++ ) { SDL_FreeSurface(sur_gunfire[i]); }
    for ( int i=0; i<3; i++ ) { SDL_FreeSurface(sur_tower_menu[i]); }
    for ( int i=0; i<12; i++ ) { SDL_FreeSurface(sur_font_selling[i]); }
    for ( int i=0; i<12; i++ ) { SDL_FreeSurface(sur_font_upgrading[i]); }
    for ( int i=0; i<5; i++ ) {
        for ( int j=0; j<4; j++ ) {
            for ( int k=0; k<4; k++ ) {
                SDL_FreeSurface(sur_soldier[i][j][k]);
            }
        }
    }
    SDL_DestroyWindow(Window);
    SDL_DestroyRenderer(Renderer);
    SDL_Quit();
    exit(834289395);
}

int SDL_main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    Window = SDL_CreateWindow("Field-runners", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1440, 960, SDL_WINDOW_SHOWN);
    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
    LOAD();
    SDL_COMPILE_TIME_ASSERT(Event, sizeof(SDL_Event) == sizeof(((SDL_Event *) NULL)->padding));

    while ( true ) {
        if ( !in_game ) {
            SDL_RenderClear(Renderer);
            tex_main[3] = SDL_CreateTextureFromSurface(Renderer, sur_main[3]);
            SDL_RenderCopy(Renderer, tex_main[3], NULL, &rect_main[3]);
            SDL_DestroyTexture(tex_main[3]);
            tex_text = SDL_CreateTextureFromSurface(Renderer, sur_text);
            SDL_RenderCopy(Renderer, tex_text, NULL, &rect_text);
            SDL_DestroyTexture(tex_text);
            for ( int i=0; i<3; i++ ) {
                tex_map[i] = SDL_CreateTextureFromSurface(Renderer, sur_map[i]);
                SDL_RenderCopy(Renderer, tex_map[i], NULL, &rect_map[i]);
                SDL_DestroyTexture(tex_main[i]);
            }
            SDL_RenderPresent(Renderer);

            while ( SDL_PollEvent(&Event) ) {
                switch ( Event.type ) {
                    case SDL_QUIT:
                        QUIT();
                        exit(0);
                        break;
                    case SDL_KEYDOWN:
                        switch ( Event.key.keysym.sym ) {
                            case SDLK_ESCAPE:
                                QUIT();
                                exit(0);
                                break;
                            default:
                                break;
                        }
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        if ( Event.button.x > 170 && Event.button.x < 440 && Event.button.y > 137 && Event.button.y < 336 ) {
                            InitGame(1);
                        } else if ( Event.button.x > 170 && Event.button.x < 444 && Event.button.y > 405 && Event.button.y < 582 ) {
                            InitGame(2);
                        } else if ( Event.button.x > 170 && Event.button.x < 437 && Event.button.y > 651 && Event.button.y < 818 ) {
                            InitGame(3);
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        else {
            /*
             * DRAWING
            */
            SDL_RenderClear(Renderer);

            // Background, health and money
            tex_main[game_map-1] = SDL_CreateTextureFromSurface(Renderer, sur_main[game_map-1]);
            SDL_RenderCopy(Renderer, tex_main[game_map-1], NULL, &rect_main[game_map-1]);
            SDL_DestroyTexture(tex_main[game_map-1]);
            tex_health = SDL_CreateTextureFromSurface(Renderer, sur_health);
            SDL_RenderCopy(Renderer, tex_health, NULL, &rect_health);
            SDL_DestroyTexture(tex_health);
            tex_money = SDL_CreateTextureFromSurface(Renderer, sur_money);
            SDL_RenderCopy(Renderer, tex_money, NULL, &rect_money);
            SDL_DestroyTexture(tex_money);

            // Icons of towers
            for (int i = 0; i < 4; i++) {
                if (money >= prices_[i]) {
                    tex_icon[i] = SDL_CreateTextureFromSurface(Renderer, sur_icon[i]);
                    SDL_RenderCopy(Renderer, tex_icon[i], NULL, &rect_icon[i]);
                    SDL_DestroyTexture(tex_icon[i]);
                }
                else {
                    tex_icon[i+4] = SDL_CreateTextureFromSurface(Renderer, sur_icon[i+4]);
                    SDL_RenderCopy(Renderer, tex_icon[i+4], NULL, &rect_icon[i+4]);
                    SDL_DestroyTexture(tex_icon[i+4]);
                }
            }

            // Click event of towers - Circle
            ViewTower();

            // Towers on the map
            for (int i = 1; i <= 11; i++) {
                for (int j = 1; j <= 19; j++) {
                    if (maze[i][j] > 0) {
                        rect_tower[(maze[i][j] - 1) / 3][(maze[i][j] - 1) % 3][0][0].x = 148 + 64 * (j - 1);
                        rect_tower[(maze[i][j] - 1) / 3][(maze[i][j] - 1) % 3][0][0].y = 134 + 64 * (i - 1);
                        tex_tower[(maze[i][j] - 1) / 3][(maze[i][j] - 1) % 3][0][0] =
                                SDL_CreateTextureFromSurface(Renderer, sur_tower[(maze[i][j] - 1) / 3][(maze[i][j] - 1) % 3][0][0]);
                        SDL_RenderCopy(Renderer, tex_tower[(maze[i][j] - 1) / 3][(maze[i][j] - 1) % 3][0][0], NULL,
                                       &rect_tower[(maze[i][j] - 1) / 3][(maze[i][j] - 1) % 3][0][0]);
                        SDL_DestroyTexture(tex_tower[(maze[i][j] - 1) / 3][(maze[i][j] - 1) % 3][0][0]);
                    }
                }
            }

            // Click event of towers - Menu
            ViewTower2();

            // Prices of towers
            for (int i = 0; i < 4; i++) {
                tex_prices[i] = SDL_CreateTextureFromSurface(Renderer, sur_prices[i]);
                SDL_RenderCopy(Renderer, tex_prices[i], NULL, &rect_prices[i]);
                SDL_DestroyTexture(tex_prices[i]);
            }

            // HUD in-game
            if (!pause) {
                tex_hud[0] = SDL_CreateTextureFromSurface(Renderer, sur_hud[0]);
                SDL_RenderCopy(Renderer, tex_hud[0], NULL, &rect_hud[0]);
                SDL_DestroyTexture(tex_hud[0]);
            } else {
                tex_hud[1] = SDL_CreateTextureFromSurface(Renderer, sur_hud[1]);
                SDL_RenderCopy(Renderer, tex_hud[1], NULL, &rect_hud[1]);
                SDL_DestroyTexture(tex_hud[1]);
            }
            if (!fast) {
                tex_hud[2] = SDL_CreateTextureFromSurface(Renderer, sur_hud[2]);
                SDL_RenderCopy(Renderer, tex_hud[2], NULL, &rect_hud[2]);
                SDL_DestroyTexture(tex_hud[2]);
            } else {
                tex_hud[3] = SDL_CreateTextureFromSurface(Renderer, sur_hud[3]);
                SDL_RenderCopy(Renderer, tex_hud[3], NULL, &rect_hud[3]);
                SDL_DestroyTexture(tex_hud[3]);
            }
            tex_hud[4] = SDL_CreateTextureFromSurface(Renderer, sur_hud[4]);
            SDL_RenderCopy(Renderer, tex_hud[4], NULL, &rect_hud[4]);
            SDL_DestroyTexture(tex_hud[4]);

            // Settings of health, money and round
            char health_[3], money_[3], rounds_[3];
            itoa(health, health_, 10);
            sur_font_health = TTF_RenderUTF8_Blended(font1, health_, color_font1);
            tex_font_health = SDL_CreateTextureFromSurface(Renderer, sur_font_health);
            rect_font_health.w = sur_font_health->w;
            rect_font_health.h = sur_font_health->h;
            itoa(money, money_, 10);
            sur_font_money = TTF_RenderUTF8_Blended(font1, money_, color_font1);
            tex_font_money = SDL_CreateTextureFromSurface(Renderer, sur_font_money);
            rect_font_money.w = sur_font_money->w;
            rect_font_money.h = sur_font_money->h;
            itoa(rounds, rounds_, 10);
            switch (game_map) {
                case 1:
                    sprintf(rounds_, "Grasslands - Round %d", rounds);
                    break;
                case 2:
                    sprintf(rounds_, "Drylands - Round %d", rounds);
                    break;
                case 3:
                    sprintf(rounds_, "Frostbite - Round %d", rounds);
                    break;
                default:
                    break;
            }
            sur_round = TTF_RenderUTF8_Blended(font1, rounds_, color_font1);
            tex_round = SDL_CreateTextureFromSurface(Renderer, sur_round);
            rect_round.w = sur_round->w;
            rect_round.h = sur_round->h;
            SDL_RenderCopy(Renderer, tex_font_health, NULL, &rect_font_health);
            SDL_DestroyTexture(tex_font_health);
            SDL_RenderCopy(Renderer, tex_font_money, NULL, &rect_font_money);
            SDL_DestroyTexture(tex_font_money);
            SDL_RenderCopy(Renderer, tex_round, NULL, &rect_round);
            SDL_DestroyTexture(tex_round);

            // Skipping all actions if paused
            if (pause) { goto pause; }

            /*
             * OVERALL ACTIONS
             */
            tick_round++;
            tick_total++;

            // Spawning enemies
            switch (game_map) {
                case 1:
                    SpawnEnemyGrasslands();
                    break;
                case 2:
                    SpawnEnemyDrylands();
                    break;
                case 3:
                    SpawnEnemyFrostbite();
                    break;
                default:
                    break;
            }

            // Checking if a round is finished
            if (round_kills >= kills_req[game_map - 1][rounds - 1]) {
                NextRound();
            }

            // Checking if won or lost
            if (rounds == 30 && round_kills == final_wave[game_map-1]) {
                tex_title[0] = SDL_CreateTextureFromSurface(Renderer, sur_title[0]);
                SDL_RenderCopy(Renderer, tex_title[0], NULL, &rect_title[0]);
                SDL_DestroyTexture(tex_title[0]);
                win = true;
                enemies = 0;
            }
            if (health <= 0) {
                tex_title[1] = SDL_CreateTextureFromSurface(Renderer, sur_title[1]);
                SDL_RenderCopy(Renderer, tex_title[1], NULL, &rect_title[1]);
                SDL_DestroyTexture(tex_title[1]);
                lose = true;
                enemies = 0;
            }

            /*
             * TOWER ACTIONS
             */
            for (int i = 1; i <= 11; i++) {
                for (int j = 1; j <= 19; j++) {

                    if ( maze[i][j] > 0 ) {
                        int type = (maze[i][j] - 1) / 3;
                        int level = (maze[i][j] - 1) % 3;
                        int tower_xm = 134 + 64 * (j - 1) + 32;
                        int tower_ym = 148 + 64 * (i - 1) + 32;

                        for ( int k = 0; k < enemies; k++ ) {
                            if ( distance(tower_xm, tower_ym, enemy[k].rect.x + enemy[k].rect.w / 2,
                                          enemy[k].rect.y + enemy[k].rect.h / 2) < tower_radius_[type] ) {

                                // Changing the texture of the tower
                                int del = ( type == 0 ) ? delta(tower_xm, tower_ym, enemy[k].rect.x + enemy[k].rect.w / 2,
                                                                enemy[k].rect.y + enemy[k].rect.h / 2) : 0;
                                rect_tower[type][level][1][del].x = 148 + 64 * (j - 1);
                                rect_tower[type][level][1][del].y = 134 + 64 * (i - 1);
                                tex_tower[type][level][1][del] = SDL_CreateTextureFromSurface(Renderer, sur_tower[type][level][1][del]);
                                SDL_RenderCopy(Renderer, tex_tower[type][level][1][del], NULL, &rect_tower[type][level][1][del]);
                                SDL_DestroyTexture(tex_tower[type][level][1][del]);

                                // Drawing gunfire
                                rect_gunfire[maze[i][j]-1].x = enemy[k].rect.x - rect_gunfire[maze[i][j]-1].w / 4;
                                rect_gunfire[maze[i][j]-1].y = (type==2) ? enemy[k].rect.y - rect_gunfire[maze[i][j]-1].h / 2 : enemy[k].rect.y;
                                tex_gunfire[maze[i][j]-1] = SDL_CreateTextureFromSurface(Renderer, sur_gunfire[maze[i][j]-1]);
                                SDL_RenderCopy(Renderer, tex_gunfire[maze[i][j]-1], NULL, &rect_gunfire[maze[i][j]-1]);
                                SDL_DestroyTexture(tex_gunfire[maze[i][j]-1]);

                                // Dealing damage
                                if ( type == 1 ) { enemy[k].poison_tick = 25 * ( level + 1 ); }
                                enemy[k].health -= (int)(tower_damage_[maze[i][j]-1] * (1 + 0.2 * (enemy[k].poison_tick > 0)) *
                                                         (1 + 0.2 * (type==3 && enemy[k].type>=3 && enemy[k].type<=4)) *
                                                         (1 + 0.2 * (type==2 && enemy[k].type==5)));

                                // Deciding if AOE damage is dealt
                                if ( !(type == 1 || type == 2) ) { break; }
                            }
                        }
                    }

                }
            }

            /*
             * ENEMY ACTIONS
             */

            // Re-drawing
            for (int i = 0; i < enemies; i++) {
                enemy[i].tex = SDL_CreateTextureFromSurface(Renderer, sur_soldier[enemy[i].type][enemy[i].dir %4][enemy[i].frame]);
                SDL_RenderCopy(Renderer, enemy[i].tex, NULL, &enemy[i].rect);
                SDL_DestroyTexture(enemy[i].tex);
                if (enemy[i].poison_tick) {
                    rect_gunfire[2].x = enemy[i].rect.x - rect_gunfire[2].w / 4;
                    rect_gunfire[2].y = enemy[i].rect.y;
                    tex_gunfire[2] = SDL_CreateTextureFromSurface(Renderer, sur_gunfire[2]);
                    SDL_RenderCopy(Renderer, tex_gunfire[2], NULL, &rect_gunfire[2]);
                    SDL_DestroyTexture(tex_gunfire[2]);
                }
            }

            // Sort enemies according to their health, from high to low.
            // Reduce the len of enemy list if enemy with non-positive HP detected.
            qsort(enemy, enemies, sizeof(enemy[0]), cmp);

            // Actions of a single enemy
            for (int i = 0; i < enemies; i++) { EnemyTick(&enemy[i]); }

            // Refresh enemy amount & kills
            int dead = 0;
            for (int i = 0; i < enemies; i++) {
                if (enemy[i].health <= 0) {
                    if (enemy[i].health == -32767) { health--; }
                    else { money += enemy[i].coins; }
                    round_kills++;
                    dead++;
                }
            }
            enemies -= dead;

            pause:
            SDL_RenderPresent(Renderer);
            Sleep((1000 / FPS) / (1 + fast));

            while (SDL_PollEvent(&Event)) {
                switch (Event.type) {
                    case SDL_KEYDOWN:
                        switch (Event.key.keysym.sym) {
                            case SDLK_BACKSPACE:
                                in_game = false;
                                break;
                            case SDLK_SPACE:
                                pause = !pause;
                                break;
                            case SDLK_RETURN:
                                fast = !fast;
                                break;
                            case SDLK_ESCAPE:
                                QUIT();
                                break;
                            default:
                                break;
                        }
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        if ( win || lose ) { in_game = false; }

                        // Resetting tower-viewing menu
                        for (int i = 1; i <= 11; i++) {
                            for (int j = 1; j <= 19; j++) {
                                if (viewing_tower[i][j]) {
                                    // Upgrading towers
                                    if (Event.button.x > 64 * j && Event.button.x < 64 * j + 93 &&
                                        Event.button.y > 64 * i + 64 && Event.button.y < 64 * i + 146) {
                                        money += selling_prices_[maze[i][j] - 1];
                                        maze[i][j] = 0;
                                        for (int k = 0; k < enemies; k++) { FindPath(&enemy[i], 0, 0, empty); }
                                    }
                                        // Selling towers
                                    else if (Event.button.x > 64 * j + 150 && Event.button.x < 64 * j + 243 &&
                                             Event.button.y > 64 * i + 64 && Event.button.y < 64 * i + 146) {
                                        if (maze[i][j] % 3 && money >= upgrading_prices_[maze[i][j] - 1]) {
                                            money -= upgrading_prices_[maze[i][j] - 1];
                                            maze[i][j]++;
                                        }
                                    }
                                }
                                viewing_tower[i][j] = false;
                            }
                        }

                        // Placing towers
                        if (Event.button.x > 0 && Event.button.x < 100 && Event.button.y > 840 &&
                            Event.button.y < 940 && money >= 5) {
                            tower_selected = 1;
                        } else if (Event.button.x > 150 && Event.button.x < 250 && Event.button.y > 840 &&
                                   Event.button.y < 940 && money >= 10) {
                            tower_selected = 4;
                        } else if (Event.button.x > 300 && Event.button.x < 400 && Event.button.y > 840 &&
                                   Event.button.y < 940 && money >= 30) {
                            tower_selected = 7;
                        } else if (Event.button.x > 450 && Event.button.x < 550 && Event.button.y > 840 &&
                                   Event.button.y < 940 && money >= 75) {
                            tower_selected = 10;
                        }

                        // Switching huds
                        if (Event.button.x > 175 && Event.button.x < 228 && Event.button.y > 18 &&
                            Event.button.y < 67) {
                            pause = !pause;
                        } else if (Event.button.x > 246 && Event.button.x < 307 && Event.button.y > 18 &&
                                   Event.button.y < 62) {
                            fast = !fast;
                        } else if (Event.button.x > 312 && Event.button.x < 364 && Event.button.y > 22 &&
                                   Event.button.y < 70) {
                            in_game = false;
                        }
                        break;
                    case SDL_MOUSEBUTTONUP: {}
                        int grid_x = (Event.button.x - 134 + 64) / 64;
                        int grid_y = (Event.button.y - 148 + 64) / 64;
                        if (grid_x >= 1 && grid_x <= 19 && grid_y >= 1 && grid_y <= 11) {
                            bool occupied = 0;
                            bool placed = maze[grid_y][grid_x];
                            if ( tower_selected ) {
                                for ( int i = 0; i < enemies; i++ ) {
                                    if ( (enemy[i].pos.x == grid_y && enemy[i].pos.y == grid_x)
                                         || (enemy[i].pos.x == grid_y && enemy[i].pos.y+1 == grid_x && enemy[i].dir == 0)
                                         || (enemy[i].pos.x == grid_y && enemy[i].pos.y-1 == grid_x && enemy[i].dir == 1)
                                         || (enemy[i].pos.x+1 == grid_y && enemy[i].pos.y == grid_x && enemy[i].dir == 2)
                                         || (enemy[i].pos.x-1 == grid_y && enemy[i].pos.y == grid_x && enemy[i].dir == 3)
                                            ) {
                                        occupied = 1;
                                        break;
                                    }
                                }
                                if ( !occupied ) {
                                    if ( !placed ) {
                                        int ret_pathfinding = 1;
                                        maze[grid_y][grid_x] = tower_selected;
                                        money -= prices_[tower_selected / 3];
                                        for ( int i = 0; i < enemies; i++ ) {
                                            if (enemy->type >= 0 && enemy->type <= 2) {
                                                ret_pathfinding = !FindPath(&enemy[i], 0, 0, empty) ? 0 : ret_pathfinding;
                                            }
                                        }
                                        switch ( game_map ) {
                                            case 1:
                                                ret_pathfinding = !FindPath(NULL, 6, 0, grass_des) ? 0 : ret_pathfinding;
                                                break;
                                            case 2:
                                                ret_pathfinding = !FindPath(NULL, 1, 0, dry_des_1) ? 0 : ret_pathfinding;
                                                ret_pathfinding = !FindPath(NULL, 11, 0, dry_des_2) ? 0 : ret_pathfinding;
                                                break;
                                            case 3:
                                                ret_pathfinding = !FindPath(NULL, 6, 0, frost_des_1) ? 0 : ret_pathfinding;
                                                ret_pathfinding = !FindPath(NULL, 0, 10, frost_des_2) ? 0 : ret_pathfinding;
                                                break;
                                            default:
                                                assert(0);
                                        }

                                        if ( !ret_pathfinding ) {
                                            maze[grid_y][grid_x] = 0;
                                            money += prices_[tower_selected / 3];
                                        }
                                    } else {
                                        printf("Tower-placing Error: There's already a tower there!\n");
                                    }
                                } else {
                                    printf("Tower-placing Error: You can't place towers on an enemy!\n");
                                }
                            } else {
                                if ( placed ) {
                                    viewing_tower[grid_y][grid_x] = maze[grid_y][grid_x];
                                }
                            }
                        }
                        tower_selected = 0;
                        break;
                    default:
                        break;
                }
            }
        }
        SDL_COMPILE_TIME_ASSERT(Event, sizeof(SDL_Event) == sizeof(((SDL_Event *) NULL)->padding));
    }
}