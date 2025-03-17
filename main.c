#include <raylib.h>
#include <stdio.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define SCREENX 1920
#define SCREENY 1080

// so that one square + gap around it = 32
#define SIZE 30
#define GAP 2
#define GRIDX 20
#define GRIDY 20

const Color BACKGROUND = BLACK;
const Color teams[] = {DARKGREEN, RED, PURPLE, YELLOW, DARKBLUE, {0xff, 0x7d, 0x31, 0xff}};

typedef struct {
  int pos[2];
  int teams;
} Cell;
Cell field[GRIDX][GRIDY];

typedef struct {
  int pos[2];
  int vel[2];
  int team;
} Bullet;

// max 1024 per player !important
// + 4 just in case
Bullet onFieldBullets[4100];

// these guys are for visual storage
Bullet greenBullets[1024]; 
Bullet redBullets[1024];
Bullet yellowBullets[1024];
Bullet blueBullets[1024];

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

int turn = 0;
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

  /// drawing
  BeginDrawing();

  ClearBackground(BACKGROUND);

  int rectSizeX = (SIZE + GAP) * GRIDX;
  int rectSizeY = (SIZE + GAP) * GRIDY;

  int startX = (SCREENX - rectSizeX) / 2;
  int startY = (SCREENY - rectSizeY) / 2;

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

  EndDrawing();
}

int main() {
  InitWindow(SCREENX, SCREENY, "window");
  ToggleFullscreen();

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    processFrame();
  }

  CloseWindow();

  return 0;
}