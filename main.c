#include <math.h>
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
#define BULLETMULTIPLYER 1.5

#define rectSizeX (SIZE + GAP) * GRIDX
#define rectSizeY (SIZE + GAP) * GRIDY

#define startX (SCREENX - rectSizeX) / 2
#define startY (SCREENY - rectSizeY) / 2

Color darken(Color color, float factor) {
  if (factor < 0.0f)
    factor = 0.0f;
  if (factor > 1.0f)
    factor = 1.0f;

  return (Color){(unsigned char)(color.r * factor),
                 (unsigned char)(color.g * factor),
                 (unsigned char)(color.b * factor), color.a};
}

Color brighten(Color color, float factor) {
  if (factor < 0.0f)
    factor = 0.0f;
  if (factor > 1.0f)
    factor = 1.0f;

  return (Color){(unsigned char)(color.r + (255 - color.r) * factor),
                 (unsigned char)(color.g + (255 - color.g) * factor),
                 (unsigned char)(color.b + (255 - color.b) * factor), color.a};
}

// clang-format off
#define YELLOW (Color) { 0xff, 0xdb, 0x58, 0xff } // NOLINT
// clang-format on

const Color BACKGROUND = BLACK;
const Color teams[] = {DARKGREEN, RED,      PURPLE,
                       YELLOW,    DARKBLUE, {0xff, 0x7d, 0x31, 0xff}};
const Color teamsother[] = {LIME, PINK, RAYWHITE, BEIGE, BLUE};

typedef struct {
  int pos[2];
  int team;
  int health;
  Rectangle rect;
} Cell;
Cell field[GRIDX][GRIDY];

typedef struct {
  float pos[2];
  float vel[2];
  int team;
  int damage;
  Rectangle rect;
} Bullet;

// https://www.geeksforgeeks.org/c-program-to-insert-an-element-in-an-array/
// me: i need to define a variable without knowing its type. if only there was
// some "object" to do that the ever so delightful Object
void insert(Bullet arr[], int *n, Bullet val) {
  // printf("!!!!!!!!!!!!!inserting onto %d\n", *n);
  arr[*n] = val;
  (*n)++;
}

void _remove(Bullet arr[], int *n, int index) {
  for (int i = index; i < *n - 1; i++) {
    arr[i] = arr[i + 1];
  }

  (*n)--;
}

void processBullet(Bullet *bullet) {
  // printf("pos: %d, %d; vel: %d, %d\n", bullet->pos[0], bullet->pos[1],
  // bullet->vel[0], bullet->vel[1]);

  bullet->pos[0] += bullet->vel[0];
  bullet->pos[1] += bullet->vel[1];
  // printf("pos: %d, %d; vel: %d, %d\n", bullet->pos[0], bullet->pos[1],
  // bullet->vel[0], bullet->vel[1]);

  if (bullet->pos[0] < startX) { // NOLINT
    // printf("1\n");
    bullet->pos[0] = startX; // NOLINT
    bullet->vel[0] *= -1;
  } else if (bullet->pos[0] + BULLETSIZE * 2 > startX + rectSizeX) { // NOLINT
    // printf("2\n");
    bullet->pos[0] = startX + rectSizeX - (BULLETSIZE * 2); // NOLINT
    bullet->vel[0] *= -1;
  }
  if (bullet->pos[1] < startY) { // NOLINT
    // printf("3\n");
    bullet->pos[1] = startY; // NOLINT
    bullet->vel[1] *= -1;
  } else if (bullet->pos[1] + BULLETSIZE * 2 > startY + rectSizeY) { // NOLINT
    // printf("4\n");
    bullet->pos[1] = startY + rectSizeY - (BULLETSIZE * 2); // NOLINT
    bullet->vel[1] *= -1;
  }
  // fflush(stdout);

  bullet->rect = (Rectangle){bullet->pos[0], bullet->pos[1], BULLETSIZE * 2,
                             BULLETSIZE * 2};
}

// max 1024 per player !important
// + 4 just in case
Bullet onFieldBullets[4100];
int onFieldBulletsSize;

// these guys are for visual storage
Bullet greenBullets[1024];
int greenBulletsSize = 1;
Bullet redBullets[1024];
int redBulletsSize = 1;
Bullet yellowBullets[1024];
int yellowBulletsSize = 1;
Bullet blueBullets[1024];
int blueBulletsSize = 1;

void launchBullet(int pos[2], float vel[2], int team) {
  if (onFieldBulletsSize < 4097) {
    Rectangle rect = {pos[0], pos[1], BULLETSIZE, BULLETSIZE};
    // if (vel[0] == 0) {
    //   vel[0] = 5;
    // }
    // if (vel[1] == 0) {
    //   vel[1] = 5;
    // }
    Bullet bullet = {pos[0], pos[1], vel[0] * 5, vel[1] * 5, team, 1, rect};
    insert(onFieldBullets, &onFieldBulletsSize, bullet);
  }
}

enum Teams { _GREEN, _RED, _PURPLE, _YELLOW, _BLUE, _DARKORANGE };

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
int getTeam(int i, int k) {
  if (k < GRIDY / 2) {
    if (i < GRIDX / 2) {
      return 0;
    } else {
      return 1;
    }
    // return teams[2];
  } else {
    if (i < GRIDX / 2) {
      return 3;
    } else {
      return 4;
    }
    // return teams[5];
  }
}

#define BUTTONWIDTH 150
#define BUTTONHEIGHT 50
#define BUTTONGAPX 15
#define BUTTONGAPY 15

int greenAngle = 0;
int greenQueue = 0;
bool greenGameOver;
float greenDelay;
int redAngle = 0;
int redQueue = 0;
bool redGameOver;
float redDelay;
int blueAngle = 0;
int blueQueue = 0;
bool blueGameOver;
float blueDelay;
int yellowAngle = 0;
int yellowQueue = 0;
bool yellowGameOver;
float yellowDelay;

void act(char action, int who) {
  if (who == _GREEN) {
    // who = _RED;
    if (action == 'm' && greenBulletsSize < 1024) {
      if (greenBulletsSize != 1) {
        greenBulletsSize *= BULLETMULTIPLYER;
        if (greenBulletsSize > 1024) {
          greenBulletsSize = 1024;
        }
      } else {
        greenBulletsSize = 2;
      }
      // resize greenBullets array
    } else if (action == 'r') {
      greenQueue += greenBulletsSize;
      greenBulletsSize = 1;
    }
    greenDelay = 0;
  } else if (who == _RED) {
    // who = _DARKBLUE;
    if (action == 'm' && redBulletsSize < 1024) {
      if (redBulletsSize != 1) {
        redBulletsSize *= BULLETMULTIPLYER;
        if (redBulletsSize > 1024) {
          redBulletsSize = 1024;
        }
      } else {
        redBulletsSize = 2;
      }
    } else if (action == 'r') {
      redQueue += redBulletsSize;
      redBulletsSize = 1;
    }
    redDelay = 0;
  } else if (who == _BLUE) {
    // who = _YELLOW;
    if (action == 'm' && blueBulletsSize < 1024) {
      if (blueBulletsSize != 1) {
        blueBulletsSize *= BULLETMULTIPLYER;
        if (blueBulletsSize > 1024) {
          blueBulletsSize = 1024;
        }
      } else {
        blueBulletsSize = 2;
      }
    } else if (action == 'r') {
      blueQueue += blueBulletsSize;
      blueBulletsSize = 1;
    }
    blueDelay = 0;
  } else {
    // who = _GREEN;
    if (action == 'm' && yellowBulletsSize < 1024) {
      if (yellowBulletsSize != 1) {
        yellowBulletsSize *= BULLETMULTIPLYER;
        if (yellowBulletsSize > 1024) {
          yellowBulletsSize = 1024;
        }
      } else {
        yellowBulletsSize = 2;
      }
    } else if (action == 'r') {
      yellowQueue += yellowBulletsSize;
      yellowBulletsSize = 1;
    }
    yellowDelay = 0;
  }
}

Font font;

#define CUBESTRING u8"magic cube"
#define MULTIPLYSTRING u8"multiply"
#define RELEASESTRING u8"release"
#define GAMEOVERSTRING u8"GAME OVER"
#define STOREDSTRING u8"stored: %d"

// Q, W, E, I, O, P, Z, X, C, COMMA, PERIOD, SLASH
#define AMOUNTOFLETTERS 12
Texture2D letters[AMOUNTOFLETTERS];

void buttons() {
  Rectangle button1;
  Rectangle button2;
  Rectangle button3;
  greenDelay += GetFrameTime();
  redDelay += GetFrameTime();
  blueDelay += GetFrameTime();
  yellowDelay += GetFrameTime();
  if (!greenGameOver) {
    button1 = (Rectangle){(float)SCREENX / 2 - BUTTONWIDTH - BUTTONGAPX,
                          BUTTONGAPY, BUTTONWIDTH, BUTTONHEIGHT};
    button2 = (Rectangle){(float)SCREENX / 2 - BUTTONWIDTH - BUTTONGAPX,
                          BUTTONGAPY + button1.y + BUTTONHEIGHT, BUTTONWIDTH,
                          BUTTONHEIGHT};
    button3 = (Rectangle){(float)SCREENX / 2 - BUTTONWIDTH - BUTTONGAPX,
                          BUTTONGAPY + button2.y + BUTTONHEIGHT, BUTTONWIDTH,
                          BUTTONHEIGHT};
    DrawTexture(letters[0], button1.x - BUTTONHEIGHT - BUTTONGAPX, button1.y,
                WHITE);
    DrawTexture(letters[1], button2.x - BUTTONHEIGHT - BUTTONGAPX, button2.y,
                WHITE);
    DrawTexture(letters[2], button3.x - BUTTONHEIGHT - BUTTONGAPX, button3.y,
                WHITE);
    if (greenDelay < 1) {
      GuiDisable();
    }
    if ((GuiButton(button1, CUBESTRING) || IsKeyPressed(KEY_Q)) &&
        greenDelay > 1) {
      act('c', _GREEN);
    }
    if (greenBulletsSize > 1023) {
      GuiDisable();
    }
    if ((GuiButton(button2, MULTIPLYSTRING) ||
         (greenBulletsSize < 1023 && IsKeyPressed(KEY_W))) &&
        greenDelay > 1) {
      act('m', _GREEN);
    }
    if (greenDelay > 1)
      GuiEnable();
    if ((GuiButton(button3, RELEASESTRING) || IsKeyPressed(KEY_E)) &&
        greenDelay > 1) {
      act('r', _GREEN);
    }
    GuiEnable();
  }
  if (!redGameOver) {
    button1 = (Rectangle){(float)SCREENX / 2 + BUTTONGAPX, BUTTONGAPY,
                          BUTTONWIDTH, BUTTONHEIGHT};
    button2 = (Rectangle){(float)SCREENX / 2 + BUTTONGAPX,
                          BUTTONGAPY + button1.y + BUTTONHEIGHT, BUTTONWIDTH,
                          BUTTONHEIGHT};
    button3 = (Rectangle){(float)SCREENX / 2 + BUTTONGAPX,
                          BUTTONGAPY + button2.y + BUTTONHEIGHT, BUTTONWIDTH,
                          BUTTONHEIGHT};
    DrawTexture(letters[3], button1.x + BUTTONWIDTH + BUTTONGAPX, button1.y,
                WHITE);
    DrawTexture(letters[4], button2.x + BUTTONWIDTH + BUTTONGAPX, button2.y,
                WHITE);
    DrawTexture(letters[5], button3.x + BUTTONWIDTH + BUTTONGAPX, button3.y,
                WHITE);
    if (redDelay < 1) {
      GuiDisable();
    }
    if ((GuiButton(button1, CUBESTRING) || IsKeyPressed(KEY_I)) &&
        redDelay > 1) {
      act('c', _RED);
    }
    if (redBulletsSize > 1023) {
      GuiDisable();
    }
    if ((GuiButton(button2, MULTIPLYSTRING) ||
         (redBulletsSize < 1023 && IsKeyPressed(KEY_O))) &&
        redDelay > 1) {
      act('m', _RED);
    }
    if (redDelay > 1)
      GuiEnable();
    if ((GuiButton(button3, RELEASESTRING) || IsKeyPressed(KEY_P)) &&
        redDelay > 1) {
      act('r', _RED);
    }
    GuiEnable();
  }
  if (!blueGameOver) {
    button3 = (Rectangle){(float)SCREENX / 2 + BUTTONGAPX,
                          SCREENY - BUTTONGAPY - BUTTONHEIGHT, BUTTONWIDTH,
                          BUTTONHEIGHT};
    button2 = (Rectangle){(float)SCREENX / 2 + BUTTONGAPX,
                          button3.y - BUTTONGAPY - BUTTONHEIGHT, BUTTONWIDTH,
                          BUTTONHEIGHT};
    button1 = (Rectangle){(float)SCREENX / 2 + BUTTONGAPX,
                          button2.y - BUTTONGAPY - BUTTONHEIGHT, BUTTONWIDTH,
                          BUTTONHEIGHT};
    DrawTexture(letters[9], button1.x + BUTTONWIDTH + BUTTONGAPX, button1.y,
                WHITE);
    DrawTexture(letters[10], button2.x + BUTTONWIDTH + BUTTONGAPX, button2.y,
                WHITE);
    DrawTexture(letters[11], button3.x + BUTTONWIDTH + BUTTONGAPX, button3.y,
                WHITE);
    if (blueDelay < 1)
      GuiDisable();
    if ((GuiButton(button1, CUBESTRING) || IsKeyPressed(KEY_COMMA)) &&
        blueDelay > 1) {
      act('c', _BLUE);
    }
    if (blueBulletsSize > 1023) {
      GuiDisable();
    }
    if ((GuiButton(button2, MULTIPLYSTRING) ||
         (blueBulletsSize < 1023 && IsKeyPressed(KEY_PERIOD))) &&
        blueDelay > 1) {
      act('m', _BLUE);
    }
    if (blueDelay > 1)
      GuiEnable();
    if ((GuiButton(button3, RELEASESTRING) || IsKeyPressed(KEY_SLASH)) &&
        blueDelay > 1) {
      act('r', _BLUE);
    }
    GuiEnable();
  }
  if (!yellowGameOver) {
    button3 = (Rectangle){(float)SCREENX / 2 - BUTTONWIDTH - BUTTONGAPX,
                          SCREENY - BUTTONGAPY - BUTTONHEIGHT, BUTTONWIDTH,
                          BUTTONHEIGHT};
    button2 = (Rectangle){(float)SCREENX / 2 - BUTTONWIDTH - BUTTONGAPX,
                          button3.y - BUTTONGAPY - BUTTONHEIGHT, BUTTONWIDTH,
                          BUTTONHEIGHT};
    button1 = (Rectangle){(float)SCREENX / 2 - BUTTONWIDTH - BUTTONGAPX,
                          button2.y - BUTTONGAPY - BUTTONHEIGHT, BUTTONWIDTH,
                          BUTTONHEIGHT};
    DrawTexture(letters[6], button1.x - BUTTONHEIGHT - BUTTONGAPX, button1.y,
                WHITE);
    DrawTexture(letters[7], button2.x - BUTTONHEIGHT - BUTTONGAPX, button2.y,
                WHITE);
    DrawTexture(letters[8], button3.x - BUTTONHEIGHT - BUTTONGAPX, button3.y,
                WHITE);
    if (yellowDelay < 1)
      GuiDisable();
    if ((GuiButton(button1, CUBESTRING) || IsKeyPressed(KEY_Z)) &&
        yellowDelay > 1) {
      act('c', _YELLOW);
    }
    if (yellowBulletsSize > 1023) {
      GuiDisable();
    }
    if ((GuiButton(button2, MULTIPLYSTRING) ||
         (yellowBulletsSize < 1023 && IsKeyPressed(KEY_X))) &&
        yellowDelay > 1) {
      act('m', _YELLOW);
    }
    if (yellowDelay > 1)
      GuiEnable();
    if ((GuiButton(button3, RELEASESTRING) || IsKeyPressed(KEY_C)) &&
        yellowDelay > 1) {
      act('r', _YELLOW);
    }
    GuiEnable();
  }
}

bool paused;
void processFrame() {
  /// input handling
  // if (IsKeyPressed(KEY_Q)) {
  //   if (turn == _GREEN) {
  //     turn = _RED;
  //   } else if (turn == _RED) {
  //     turn = _DARKBLUE;
  //   } else if (turn == _DARKBLUE) {
  //     turn = _YELLOW;
  //   } else {
  //     turn = 0;
  //   }
  // }
  // if (IsKeyDown(KEY_W)) {
  //   if (turn == _GREEN) {
  //     launchBullet((int[]){startX, startY}, (int[]){10, 10}, turn);
  //   } else if (turn == _RED) {
  //     launchBullet((int[]){startX + rectSizeX - (BULLETSIZE * 2), startY},
  //                  (int[]){-10, 10}, turn);
  //   } else if (turn == _DARKBLUE) {
  //     launchBullet((int[]){startX + rectSizeX - (BULLETSIZE * 2),
  //                          startY + rectSizeY - (BULLETSIZE * 2)},
  //                  (int[]){-10, -10}, turn);
  //   } else if (turn == _YELLOW) {
  //     launchBullet((int[]){startX, startY + rectSizeY - (BULLETSIZE * 2)},
  //                  (int[]){10, -10}, turn);
  //   }
  // }
  // if (IsKeyPressed(KEY_P)) {
  //   paused = !paused;
  // }

  /// game logic
  if (greenQueue > 0 && !greenGameOver) {
    launchBullet(
        (int[]){startX + (SIZE / 2), startY + (SIZE / 2)},
        (float[]){cos(DEG2RAD * greenAngle), sin(DEG2RAD * greenAngle)},
        _GREEN);
    greenQueue -= 1;
  }
  if (redQueue > 0 && !redGameOver) {
    launchBullet((int[]){startX + rectSizeX - (SIZE / 2), startY + (SIZE / 2)},
                 (float[]){cos(DEG2RAD * redAngle), sin(DEG2RAD * redAngle)},
                 _RED);
    redQueue -= 1;
  }
  if (blueQueue > 0 && !blueGameOver) {
    launchBullet((int[]){startX + rectSizeX - (SIZE / 2),
                         startY + rectSizeY - (SIZE / 2)},
                 (float[]){cos(DEG2RAD * blueAngle), sin(DEG2RAD * blueAngle)},
                 _BLUE);
    blueQueue -= 1;
  }
  if (yellowQueue > 0 && !yellowGameOver) {
    launchBullet(
        (int[]){startX + (SIZE / 2), startY + rectSizeY - (SIZE / 2)},
        (float[]){cos(DEG2RAD * yellowAngle), sin(DEG2RAD * yellowAngle)},
        _YELLOW);
    yellowQueue -= 1;
  }

  if (!greenGameOver)
    greenGameOver = field[0][0].team != _GREEN;
  if (!redGameOver)
    redGameOver = field[GRIDX - 1][0].team != _RED;
  if (!blueGameOver)
    blueGameOver = field[GRIDX - 1][GRIDY - 1].team != _BLUE;
  if (!yellowGameOver)
    yellowGameOver = field[0][GRIDY - 1].team != _YELLOW;

  /// drawing
  BeginDrawing();

  ClearBackground(BACKGROUND);

  DrawRectangle(0, 0, SCREENX / 2, SCREENY / 2, teams[_GREEN]);
  DrawRectangle(SCREENX / 2, 0, SCREENX / 2, SCREENY / 2, teams[_RED]);
  DrawRectangle(0, SCREENY / 2, SCREENX / 2, SCREENY / 2, teams[_YELLOW]);
  DrawRectangle(SCREENX / 2, SCREENY / 2, SCREENX / 2, SCREENY / 2,
                teams[_BLUE]);

  DrawRectangle(startX, startY, rectSizeX, rectSizeY, BACKGROUND);

  for (int i = 0; i < GRIDX; i++) {
    for (int k = 0; k < GRIDY; k++) {
      // DrawRectangle(startX + 1 + ((SIZE + GAP) * i),
      //               startY + 1 + ((SIZE + GAP) * k), SIZE, SIZE,
      //               getTeamColor(i, k));
      Color c = teams[field[i][k].team];
      if (field[i][k].health == 1) {
        c.a = 128;
      }
      DrawRectangleRec(field[i][k].rect, c);
    }
  }

  for (int i = 0; i < onFieldBulletsSize; i++) {
    bool skip;
    for (int x = 0; x < GRIDX; x++) {
      if (skip) {
        break;
      }
      for (int y = 0; y < GRIDY; y++) {
        Cell *el = &field[x][y];
        if (CheckCollisionRecs(onFieldBullets[i].rect, el->rect)) {
          if (onFieldBullets[i].team != el->team) {
            el->health -= onFieldBullets[i].damage;
            if (el->health <= 0) {
              el->team = onFieldBullets[i].team;
              el->health = 2;
            }
            skip = true;
            _remove(onFieldBullets, &onFieldBulletsSize, i);
          } else if (el->health == 1 && onFieldBullets[i].team == el->team) {
            el->health = 2;
          }
        }
        if (skip) {
          break;
        }
      }
    }
    if (!skip) {
      // if (!paused || IsKeyPressed(KEY_O)) {
      processBullet(&onFieldBullets[i]);
      // }
      DrawCircle(onFieldBullets[i].pos[0] + BULLETSIZE,
                 onFieldBullets[i].pos[1] + BULLETSIZE, BULLETSIZE,
                 darken(teams[onFieldBullets[i].team], 0.5));
      DrawCircle(onFieldBullets[i].pos[0] + BULLETSIZE,
                 onFieldBullets[i].pos[1] + BULLETSIZE, BULLETSIZE * 0.8,
                 brighten(teams[onFieldBullets[i].team], 0.2));
    }
    skip = false;
  }
  // DrawText(TextFormat("%d", onFieldBulletsSize), 10, 10, 24, RAYWHITE);

  if (!greenGameOver) {
    DrawRectanglePro((Rectangle){startX + (SIZE / 2), // NOLINT
                                 startY + (SIZE / 2), // NOLINT
                                 SIZE * 2, 20},       // NOLINT
                     (Vector2){10, 10}, greenAngle, brighten(GREEN, 0.4));
    greenAngle = sin(GetTime()) * 45 + 45;
    DrawText(TextFormat(STOREDSTRING, greenBulletsSize), 10, 10, 24,
             brighten(GREEN, 0.2));
  } else {
    DrawText(GAMEOVERSTRING, 10, 10, 24, brighten(GREEN, 0.2));
  }
  if (!redGameOver) {
    DrawRectanglePro((Rectangle){startX + rectSizeX - (SIZE / 2),    // NOLINT
                                 startY + (SIZE / 2), SIZE * 2, 20}, // NOLINT
                     (Vector2){10, 10}, redAngle, brighten(RED, 0.4));
    redAngle = sin(GetTime()) * 45 + 135;
    DrawText(TextFormat(STOREDSTRING, redBulletsSize), startX + rectSizeX + 10,
             10, 24, brighten(RED, 0.2));
  } else {
    DrawText(GAMEOVERSTRING, startX + rectSizeX + 10, 10, 24,
             brighten(RED, 0.2));
  }
  if (!blueGameOver) {
    DrawRectanglePro((Rectangle){startX + rectSizeX - (SIZE / 2), // NOLINT
                                 startY + rectSizeY - (SIZE / 2), // NOLINT
                                 SIZE * 2,                        // NOLINT
                                 20},                             // NOLINT
                     (Vector2){10, 10}, blueAngle, brighten(BLUE, 0.4));
    blueAngle = sin(GetTime()) * 45 - 135;
    DrawText(TextFormat(STOREDSTRING, blueBulletsSize), startX + rectSizeX + 10,
             SCREENY - 34, 24, brighten(BLUE, 0.2));
  } else {
    DrawText(GAMEOVERSTRING, startX + rectSizeX + SCREENY - 34, 10, 24,
             brighten(BLUE, 0.2));
  }
  if (!yellowGameOver) {
    DrawRectanglePro((Rectangle){startX + (SIZE / 2),             // NOLINT
                                 startY + rectSizeY - (SIZE / 2), // NOLINT
                                 SIZE * 2,                        // NOLINT
                                 20},                             // NOLINT
                     (Vector2){10, 10}, yellowAngle, brighten(YELLOW, 0.8));
    yellowAngle = sin(GetTime()) * 45 - 45;
    DrawText(TextFormat(STOREDSTRING, yellowBulletsSize), 10, SCREENY - 34, 24,
             darken(YELLOW, 0.8));
  } else {
    DrawText(GAMEOVERSTRING, 10, SCREENY - 34, 24, brighten(YELLOW, 0.2));
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

  // good lord
  // im pretty sure you can optimize this but i dont feel like it lmaooo
  const char prefix[] = "../assets/keys/";
  Image img;
  char result[100];
  sprintf(result, "%s%s", prefix, "Q-Key.png");
  img = LoadImage(result);
  ImageResize(&img, BUTTONHEIGHT, BUTTONHEIGHT);
  letters[0] = LoadTextureFromImage(img);
  UnloadImage(img);
  sprintf(result, "%s%s", prefix, "W-Key.png");
  img = LoadImage(result);
  ImageResize(&img, BUTTONHEIGHT, BUTTONHEIGHT);
  letters[1] = LoadTextureFromImage(img);
  UnloadImage(img);
  sprintf(result, "%s%s", prefix, "E-Key.png");
  img = LoadImage(result);
  ImageResize(&img, BUTTONHEIGHT, BUTTONHEIGHT);
  letters[2] = LoadTextureFromImage(img);
  UnloadImage(img);
  sprintf(result, "%s%s", prefix, "I-Key.png");
  img = LoadImage(result);
  ImageResize(&img, BUTTONHEIGHT, BUTTONHEIGHT);
  letters[3] = LoadTextureFromImage(img);
  UnloadImage(img);
  sprintf(result, "%s%s", prefix, "O-Key.png");
  img = LoadImage(result);
  ImageResize(&img, BUTTONHEIGHT, BUTTONHEIGHT);
  letters[4] = LoadTextureFromImage(img);
  UnloadImage(img);
  sprintf(result, "%s%s", prefix, "P-Key.png");
  img = LoadImage(result);
  ImageResize(&img, BUTTONHEIGHT, BUTTONHEIGHT);
  letters[5] = LoadTextureFromImage(img);
  UnloadImage(img);
  sprintf(result, "%s%s", prefix, "Z-Key.png");
  img = LoadImage(result);
  ImageResize(&img, BUTTONHEIGHT, BUTTONHEIGHT);
  letters[6] = LoadTextureFromImage(img);
  UnloadImage(img);
  sprintf(result, "%s%s", prefix, "X-Key.png");
  img = LoadImage(result);
  ImageResize(&img, BUTTONHEIGHT, BUTTONHEIGHT);
  letters[7] = LoadTextureFromImage(img);
  UnloadImage(img);
  sprintf(result, "%s%s", prefix, "C-Key.png");
  img = LoadImage(result);
  ImageResize(&img, BUTTONHEIGHT, BUTTONHEIGHT);
  letters[8] = LoadTextureFromImage(img);
  UnloadImage(img);
  sprintf(result, "%s%s", prefix, "COMMA-Key.png");
  img = LoadImage(result);
  ImageResize(&img, BUTTONHEIGHT, BUTTONHEIGHT);
  letters[9] = LoadTextureFromImage(img);
  UnloadImage(img);
  sprintf(result, "%s%s", prefix, "PERIOD-Key.png");
  img = LoadImage(result);
  ImageResize(&img, BUTTONHEIGHT, BUTTONHEIGHT);
  letters[10] = LoadTextureFromImage(img);
  UnloadImage(img);
  sprintf(result, "%s%s", prefix, "SLASH-Key.png");
  img = LoadImage(result);
  ImageResize(&img, BUTTONHEIGHT, BUTTONHEIGHT);
  letters[11] = LoadTextureFromImage(img);
  UnloadImage(img);

  SetTargetFPS(60);

  for (int x = 0; x < GRIDX; x++) {
    for (int y = 0; y < GRIDY; y++) {
      field[x][y] = (Cell){x, y, getTeam(x, y), 2,
                           (Rectangle){(float)startX + 1 + ((SIZE + GAP) * x),
                                       (float)startY + 1 + ((SIZE + GAP) * y),
                                       SIZE, SIZE}};
    }
  }

  while (!WindowShouldClose()) {
    processFrame();
  }

  CloseWindow();

  return 0;
}