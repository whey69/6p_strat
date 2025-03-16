#include <raylib.h>
#include <stdio.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

const int SCREENX = 1920;
const int SCREENY = 1080;

// so that one square + gap around it = 32
const int SIZE = 30;
const int GAP = 2;
const int GRIDX = 30;
const int GRIDY = 20;

const Color BACKGROUND = BLACK;
const Color teams[] = {DARKGREEN, RED, PURPLE, YELLOW, DARKBLUE, {0xff, 0x7d, 0x31, 0xff}};

Color getTeamColor(int i, int k) {
  if (k < GRIDY / 2) {
    if (i < GRIDX / 3) {
      return teams[0];
    } else if (i >= GRIDX / 3 && i < GRIDX / 3 * 2) {
      return teams[1];
    }
    return teams[2];
  } else {
    if (i < GRIDX / 3) {
      return teams[3];
    } else if (i >= GRIDX / 3 && i < GRIDX / 3 * 2) {
      return teams[4];
    }
    return teams[5];
  }
}

int turn = 0;
void draw() {
  BeginDrawing();

  ClearBackground(BACKGROUND);

  int rectSizeX = (SIZE + GAP) * GRIDX;
  int rectSizeY = (SIZE + GAP) * GRIDY;

  int startX = (SCREENX - rectSizeX) / 2;
  int startY = (SCREENY - rectSizeY) / 2;

  if (turn == 0) {
    DrawRectangle(0, 0, startX + (rectSizeX / 3), startY + (rectSizeY / 2), teams[turn]);
  } else if (turn == 1) {
    DrawRectangle(startX + (rectSizeX / 3), 0, (rectSizeX / 3), startY + (rectSizeY / 2), teams[turn]);
  } else if (turn == 2) {
    DrawRectangle(startX + (rectSizeX / 3 * 2), 0, startX + (rectSizeX / 3), startY + (rectSizeY / 2), teams[turn]);
  } else if (turn == 3) {
    DrawRectangle(0, startY + (rectSizeY / 2), startX + (rectSizeX / 3), startY + (rectSizeY / 2), teams[turn]);
  } else if (turn == 4) {
    DrawRectangle(startX + (rectSizeX / 3), startY + (rectSizeY / 2), (rectSizeX / 3), startY + (rectSizeY / 2), teams[turn]);
  } else if (turn == 5) {
    DrawRectangle(startX + (rectSizeX / 3 * 2), startY + (rectSizeY / 2), startX + (rectSizeX / 3), startY + (rectSizeY / 2), teams[turn]);
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

  if (GuiButton((Rectangle){100, 100, 120, 30}, "hello raygui")) {
    if (turn == 0) {
      turn = 1;
    } else if (turn == 1) {
      turn = 2;
    } else if (turn == 2) {
      turn = 5;
    } else if (turn == 5) {
      turn = 4;
    } else if (turn == 4) {
      turn = 3;
    } else if (turn == 3) {
      turn = 0;
    }
  }

  EndDrawing();
}

void processInput() {
  if (IsKeyPressed(KEY_Q)) {
    if (turn == 0) {
      turn = 1;
    } else if (turn == 1) {
      turn = 2;
    } else if (turn == 2) {
      turn = 5;
    } else if (turn == 5) {
      turn = 4;
    } else if (turn == 4) {
      turn = 3;
    } else if (turn == 3) {
      turn = 0;
    }
  }
}

int main() {
  InitWindow(SCREENX, SCREENY, "window");
  ToggleFullscreen();

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    processInput();
    draw();
  }

  CloseWindow();

  return 0;
}