#include "raylib.h"
#include <raylib.h>
#include <stdio.h>

const int SCREENX = 1920;
const int SCREENY = 1080;

// so that one square + gap around it = 32
const int SIZE = 30;
const int GAP = 2;
const int GRIDX = 30;
const int GRIDY = 20;

Color getTeamColor(int i, int k) {
  if (k < GRIDY / 2) {
    if (i < GRIDX / 3) {
      return DARKGREEN;
    } else if (i >= GRIDX / 3 && i < GRIDX / 3 * 2) {
      return RED;
    }
    return PURPLE;
  } else {
    if (i < GRIDX / 3) {
      return YELLOW;
    } else if (i >= GRIDX / 3 && i < GRIDX / 3 * 2) {
      return DARKBLUE;
    }
    return ORANGE;
  }
}

void draw() {
  BeginDrawing();

  ClearBackground(BLACK);

  int rectSizeX = (SIZE + GAP) * GRIDX;
  int rectSizeY = (SIZE + GAP) * GRIDY;

  int startX = (SCREENX - rectSizeX) / 2;
  int startY = (SCREENY - rectSizeY) / 2;

  // DrawRectangle(startX, startY, rectSizeX, rectSizeY, GREEN);
  for (int i = 0; i < GRIDX; i++) {
    for (int k = 0; k < GRIDY; k++) {
      DrawRectangle(startX + 1 + ((SIZE + GAP) * i),
                    startY + 1 + ((SIZE + GAP) * k),
                    SIZE, SIZE,
                    getTeamColor(i, k));
      // DrawText(TextFormat("%d", k), startX + 1 + ((size + gap) * i), startY +
      // 1 + ((size + gap) * k), 12, BLACK);
    }
  }

  EndDrawing();
}

void processInput() {}

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