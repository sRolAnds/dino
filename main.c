#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#define MAP_HEIGHT 10
#define MAP_WIDTH 80

#define EMPTY ' '
#define DINO_SIGN '$'
#define CACTUS_SIGN '#'
#define BIRD_SIGN '<'

#define OBJ_CNT 10

#define DINO_HEIGHT 2
#define DINO_WIDTH 2
#define CACTUS_HEIGHT 2
#define CACTUS_WIDTH 1
#define BIRD_HEIGHT 1
#define BIRD_WIDTH 2

typedef struct {
    int height, width;
    int x, y;
    float speed;
    char symbol;
} object_t;

//GAME-MAP
char map[MAP_HEIGHT][MAP_WIDTH + 1];

object_t objects[OBJ_CNT];
int cnt_objects = 0;

//DINOSAUR
object_t dino;

//CACTUS
object_t cactus;

//BIRDS
object_t bird;
object_t bird2;

//jump state
bool isJump = false;
//
bool isOnFloor = false;
//sit down state
bool isSitDown = false;

void initMap() {
    char c = 'a';
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (i < MAP_HEIGHT - 1) {
                map[i][j] = EMPTY;
            }else {
                map[i][j] = c;
                c++;
                if (c > 122) {
                    c = 97;
                }
            }
        }
        map[i][MAP_WIDTH] = 0;
    }
}

void printMap() {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            printf("%c", map[i][j]);
        }
        printf("\n");
    }
}

void initObj(object_t* obj, int height, int width, int x, int y, float speed, char c) {
    obj->height = height;
    obj->width = width;
    obj->x = x;
    obj->y = y;
    obj->speed = speed;
    obj->symbol = c;
    //notify us if count of objects > defined size of array
    assert(cnt_objects <= OBJ_CNT - 1);
    //add object to array
    objects[cnt_objects++] = *obj;
}

void putObjOnMap(object_t obj) {
    for (int i = obj.y; i < obj.y + obj.height; i++) {
        for (int j = obj.x; j < obj.x + obj.width; j++) {
            map[i][j] = obj.symbol;
        }
    }
}

void cleanObjFromMap(object_t* obj) {
    for (int i = obj->y; i < obj->y + obj->height; i++) {
        for (int j = obj->x; j < obj->x + obj->width; j++) {
            map[i][j] = EMPTY;
        }
    }
}

void moveFloor() {
    char c = map[MAP_HEIGHT - 1][0];
    for (int w = 1; w < MAP_WIDTH; w++) {
        map[MAP_HEIGHT - 1][w - 1] = map[MAP_HEIGHT - 1][w];
    }
    map[MAP_HEIGHT - 1][MAP_WIDTH - 1] = c;
}

void moveObject(object_t* object) {
    cleanObjFromMap(object);
    object->x -= object->speed;
    if (object->x < 0) {
        object->x = MAP_WIDTH - 1;
    }
    putObjOnMap(*object);
}

void moveMap() {
    if (isOnFloor) {
        moveFloor();
        for (int i = 1; i < cnt_objects; i++) {
            moveObject(objects + i);
        }
    }
}

void setConsoleCursor(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

//beta version :)
bool isCollision(object_t dino) {
    for (int i = 1; i < cnt_objects; i++) {
        for (int j = dino.y; j < dino.y + dino.height; j++) {
            for (int k = dino.x; k < dino.x + dino.width; k++) {
                if (map[j][k] == objects[i].symbol) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool isEndGame(object_t dino) {
    return isCollision(dino);
}

void jumpDino(object_t* dino) {
    cleanObjFromMap(dino);
    dino->y -= 6;
}

void fallDino(object_t* dino) {
    int prev_y = dino->y;
    for (int i = 0; i < dino->width; i++)
        map[prev_y][dino->x + i] = EMPTY;
    dino->y += 1;
    if ( isalpha(map[dino->y + dino->height - 1][dino->x]) ) {
        dino->y -= 1;
        isOnFloor = true;
        isJump = false;
    }
    putObjOnMap(*dino);
}

void sitDino(object_t* dino);

void controlDino(object_t* dino) {
    if (GetKeyState(VK_SPACE) < 0 && !isJump) {
        isJump = true;
        jumpDino(dino);
        putObjOnMap(*dino);
    }
    if (GetKeyState('S') < 0) {
        isSitDown = true;
        sitDino(dino);
    }else isSitDown = false;
}

void sitDino(object_t* dino) {
    for (int i = dino->x; i < dino->x + dino->width; i++) {
        map[dino->y][i] = EMPTY;
    }
}

void initGame() {
    initMap();
    initObj(&dino, DINO_HEIGHT, DINO_WIDTH, 4, MAP_HEIGHT - 1 - DINO_HEIGHT - 6, 1, DINO_SIGN);
    initObj(&cactus, CACTUS_HEIGHT, CACTUS_WIDTH, MAP_WIDTH - 10, MAP_HEIGHT - 1 - CACTUS_HEIGHT, 3, CACTUS_SIGN);
    initObj(&bird, BIRD_HEIGHT, BIRD_WIDTH, MAP_WIDTH - 20, MAP_HEIGHT - 1 - BIRD_HEIGHT - 2, 3.4, BIRD_SIGN);
    initObj(&bird2, BIRD_HEIGHT, BIRD_WIDTH, MAP_WIDTH - 1, MAP_HEIGHT - 1 - BIRD_HEIGHT - 1, 3.2, BIRD_SIGN);

    for (int i = 0; i < cnt_objects; i++)
        putObjOnMap(objects[i]);
}

void runGame() {
    do {
        setConsoleCursor(0, 0);
        fallDino(&dino);
        moveMap();
        controlDino(&dino);
        printMap();
        Sleep(15);
    }while ( (isEndGame(dino) == false) && (GetKeyState(VK_ESCAPE) >= 0) );
}

int main() {

    initGame();
    runGame();

    return 0;
}
