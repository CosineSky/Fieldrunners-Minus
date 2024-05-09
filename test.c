//
// Created by asus on 2023/5/13.
//
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <windows.h>
#include <stdbool.h>

typedef struct point_ {
    int x;
    int y;
} Point;

typedef struct node_ {
    Point p;
    int prev;
} Node;

typedef struct queue_ {
    Node queue[999];
    int head;
    int tail;
} Queue;

void StorePath(Queue* q) {
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

int FindPath(int src_x, int src_y, Point dest) {
    /*
     * Pathfinding mode:
     *  0 - Using a specific enemy's position.
     *  1 - Using a given position.
     */
    int enemy_mode = 1;

    // Copy the game map.
    int maze2[13][21];
    for ( int i=0; i<13; i++ ) {
        for ( int j=0; j<21; j++ ) {
            maze2[i][j] = maze[i][j];
        }
    }

    // Getting destination.
    Point destination;
    destination.x = dest.x;
    destination.y = dest.y;

    // Making attempts in 4 directions.
    int delta[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

    // path - The actual route;
    // attempted - All grids travelled before.
    Queue path, attempted;

    // Init.
    path.head = path.tail = 0;
    attempted.head = attempted.tail = 0;
    Node node = {src_x, src_y, -1};

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

int main() {

}
