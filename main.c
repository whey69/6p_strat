#include <raylib.h>
#include <stdio.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define len(arr) (sizeof(arr) / sizeof(arr[0]))

#define SCREENX 1920
#define SCREENY 1080

// so that one square + gap around it = 32
#define SIZE 30
#define GAP 2
#define GRIDX 20
#define GRIDY 20
#define BULLETSIZE 10

#define rectSizeX (SIZE + GAP) * GRIDX
#define rectSizeY (SIZE + GAP) * GRIDY

#define startX (SCREENX - rectSizeX) / 2
#define startY (SCREENY - rectSizeY) / 2

const Color BACKGROUND = BLACK;
const Color teams[] = {DARKGREEN, RED, PURPLE, YELLOW, DARKBLUE, {0xff, 0x7d, 0x31, 0xff}};

typedef struct {
  int pos[2];
  int team;
  int health;
  Rectangle rect;
} Cell;
Cell field[GRIDX][GRIDY];

typedef struct {
  int pos[2];
  int vel[2];
  int team;
  Rectangle rect;
} Bullet;

Bullet launchBullet(int pos[2], int vel[2], int team) {
  Rectangle rect = {pos[0], pos[1], BULLETSIZE, BULLETSIZE};
  Bullet bullet = {pos[0], pos[1], vel[0], vel[1], team, rect};
  return bullet;
}

void processBullet(Bullet* bullet) {
  // printf("pos: %d, %d; vel: %d, %d\n", bullet->pos[0], bullet->pos[1], bullet->vel[0], bullet->vel[1]);
  bullet->pos[0] += bullet->vel[0];
  bullet->pos[1] += bullet->vel[1];
  // printf("pos: %d, %d; vel: %d, %d\n", bullet->pos[0], bullet->pos[1], bullet->vel[0], bullet->vel[1]);

  if (bullet->pos[0] < startX) {
    printf("1\n");
    bullet->pos[0] = startX;
    bullet->vel[0] *= -1;
  } else if (bullet->pos[0] + BULLETSIZE > startX + rectSizeX) {
    printf("2\n");
    bullet->pos[0] = startX + rectSizeX - BULLETSIZE;
    bullet->vel[0] *= -1;
  } 
  if (bullet->pos[1] < startY) {
    printf("3\n");
    bullet->pos[1] = startY;
    bullet->vel[1] *= -1;
  } else if (bullet->pos[1] + BULLETSIZE > startY + rectSizeY) {
    printf("4\n");
    bullet->pos[1] = startY + rectSizeY - BULLETSIZE;
    bullet->vel[1] *= -1;
  }
  fflush(stdout);
  
  bullet->rect = (Rectangle){bullet->pos[0], bullet->pos[1], BULLETSIZE, BULLETSIZE};
}

// max 1024 per player !important
// + 4 just in case
Bullet onFieldBullets[4100];
int onFieldBulletsSize;

// these guys are for visual storage
Bullet greenBullets[1024]; 
int greenBulletsSize;
Bullet redBullets[1024];
int redBulletsSize;
Bullet yellowBullets[1024];
int yellowBulletsSize;
Bullet blueBullets[1024];
int blueBulletsSize;

// https://www.geeksforgeeks.org/c-program-to-insert-an-element-in-an-array/
// me: i need to define a variable without knowing its type. if only there was some "object" to do that
// the ever so delightful Object 
void insert(Bullet arr[], int *n, Bullet val) {
  // printf("!!!!!!!!!!!!!inserting onto %d\n", *n);
  arr[*n] = val;
  (*n)++;
}

enum Teams {
  _GREEN,
  _RED,
  _PURPLE,
  _YELLOW,
  _DARKBLUE,
  _DARKORANGE
};

Color getTeamColor(int i, int k) {
  if (k < GRIDY / 2) {
    if (i < GRIDX / 2) {
      return teams[0];
    } else {
      return teams[1];
    }
    // return teams[2];
  } else {
    if (i < GRIDX / 2) {
      return teams[3];
    } else {
      return teams[4];
    }
    // return teams[5];
  }
}

#define BUTTONWIDTH 150
#define BUTTONHEIGHT 50
#define BUTTONGAPX 15
#define BUTTONGAPY 15

int turn = 0;
Font font;
void buttons() {
  Rectangle button1;
  Rectangle button2;
  Rectangle button3;
  if (turn == _GREEN) {
    button1 = (Rectangle){(float) SCREENX / 2 - BUTTONWIDTH - BUTTONGAPX, BUTTONGAPY, BUTTONWIDTH, BUTTONHEIGHT};
    button2 = (Rectangle){(float) SCREENX / 2 - BUTTONWIDTH - BUTTONGAPX, BUTTONGAPY + button1.y + BUTTONHEIGHT, BUTTONWIDTH, BUTTONHEIGHT};
    button3 = (Rectangle){(float) SCREENX / 2 - BUTTONWIDTH - BUTTONGAPX, BUTTONGAPY + button2.y + BUTTONHEIGHT, BUTTONWIDTH, BUTTONHEIGHT};
  } else if (turn == _RED) {
    button1 = (Rectangle){(float) SCREENX / 2 + BUTTONGAPX, BUTTONGAPY, BUTTONWIDTH, BUTTONHEIGHT};
    button2 = (Rectangle){(float) SCREENX / 2 + BUTTONGAPX, BUTTONGAPY + button1.y + BUTTONHEIGHT, BUTTONWIDTH, BUTTONHEIGHT};
    button3 = (Rectangle){(float) SCREENX / 2 + BUTTONGAPX, BUTTONGAPY + button2.y + BUTTONHEIGHT, BUTTONWIDTH, BUTTONHEIGHT};
  } else if (turn == _DARKBLUE) {
    button3 = (Rectangle){(float) SCREENX / 2 + BUTTONGAPX, SCREENY - BUTTONGAPY - BUTTONHEIGHT, BUTTONWIDTH, BUTTONHEIGHT};
    button2 = (Rectangle){(float) SCREENX / 2 + BUTTONGAPX, button3.y - BUTTONGAPY - BUTTONHEIGHT, BUTTONWIDTH, BUTTONHEIGHT};
    button1 = (Rectangle){(float) SCREENX / 2 + BUTTONGAPX, button2.y - BUTTONGAPY - BUTTONHEIGHT, BUTTONWIDTH, BUTTONHEIGHT};
  } else if (turn == _YELLOW) {
    button3 = (Rectangle){(float) SCREENX / 2 - BUTTONWIDTH - BUTTONGAPX, SCREENY - BUTTONGAPY - BUTTONHEIGHT, BUTTONWIDTH, BUTTONHEIGHT};
    button2 = (Rectangle){(float) SCREENX / 2 - BUTTONWIDTH - BUTTONGAPX, button3.y - BUTTONGAPY - BUTTONHEIGHT, BUTTONWIDTH, BUTTONHEIGHT};
    button1 = (Rectangle){(float) SCREENX / 2 - BUTTONWIDTH - BUTTONGAPX, button2.y - BUTTONGAPY - BUTTONHEIGHT, BUTTONWIDTH, BUTTONHEIGHT};
  } 

  // TODO: translate
  GuiButton(button1, u8"magic cube");
  GuiButton(button2, u8"multiply by 2");
  GuiButton(button3, u8"release balls");
}

void processFrame() {
  /// input handling
  if (IsKeyPressed(KEY_Q)) {
    if (turn == _GREEN) {
      turn = _RED;
    } else if (turn == _RED) {
      turn = _DARKBLUE;
    } else if (turn == _DARKBLUE) {
      turn = _YELLOW;
    } else {
      turn = 0;
    }
  }
  if (IsKeyPressed(KEY_W)) {
    Bullet bullet;
    if (turn == _GREEN) {
      bullet = launchBullet((int[]){startX + 1, startY + 1}, (int[]){100, 100}, turn);
    } else if (turn == _RED) {
      bullet = launchBullet((int[]){startX, startY}, (int[]){10, 10}, turn);
    } else if (turn == _DARKBLUE) {
      bullet = launchBullet((int[]){startX, startY}, (int[]){10, 10}, turn);
    } else if (turn == _YELLOW) {
      bullet = launchBullet((int[]){startX, startY}, (int[]){10, 10}, turn);
    }

    insert(onFieldBullets, &onFieldBulletsSize, bullet);
  }

  /// drawing
  BeginDrawing();

  ClearBackground(BACKGROUND);

  if (turn == _GREEN) {
    DrawRectangle(0, 0, SCREENX / 2, SCREENY / 2, teams[turn]);
  } else if (turn == _RED) {
    DrawRectangle(SCREENX / 2, 0, SCREENX / 2, SCREENY / 2, teams[turn]);
  // } else if (turn == 2) {
  //   DrawRectangle(startX + (rectSizeX / 3 * 2), 0, startX + (rectSizeX / 3), startY + (rectSizeY / 2), teams[turn]);
  } else if (turn == _YELLOW) {
    DrawRectangle(0, SCREENY / 2, SCREENX / 2, SCREENY / 2, teams[turn]);
  } else if (turn == _DARKBLUE) {
    DrawRectangle(SCREENX / 2, SCREENY / 2, SCREENX / 2, SCREENY / 2, teams[turn]);
  // } else if (turn == 5) {
    // DrawRectangle(startX + (rectSizeX / 3 * 2), startY + (rectSizeY / 2), startX + (rectSizeX / 3), startY + (rectSizeY / 2), teams[turn]);
  }
  DrawRectangle(startX, startY, rectSizeX, rectSizeY, BACKGROUND);

  for (int i = 0; i < GRIDX; i++) {
    for (int k = 0; k < GRIDY; k++) {
      DrawRectangle(startX + 1 + ((SIZE + GAP) * i),
                    startY + 1 + ((SIZE + GAP) * k),
                    SIZE, SIZE,
                    getTeamColor(i, k));
    }
  }

  for (int i = 0; i < onFieldBulletsSize; i++) {
    // printf("processing %d\n", i);
    Bullet element = onFieldBullets[i];
    processBullet(&element);
    DrawCircle(element.pos[0] + BULLETSIZE, element.pos[1] + BULLETSIZE, BULLETSIZE, WHITE);
  }

  buttons();

  EndDrawing();
}

int main() {
  InitWindow(SCREENX, SCREENY, "window");
  ToggleFullscreen();

  // font = LoadFontEx("../assets/font.ttf", 24, NULL, 0);
  // SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
  // GuiSetFont(font);

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    processFrame();
  }

  CloseWindow();

  return 0;
}