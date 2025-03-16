#include <stdio.h>
#include <raylib.h>

const int screenWidth = 1920;
const int screenHeight = 1080;

Color textColor = LIGHTGRAY;
void draw() {
  BeginDrawing();
  
  ClearBackground(RAYWHITE);
  DrawText("const char *text", screenWidth/2, screenHeight/2, 24, textColor);

  EndDrawing();
}

void processInput() {
  if (IsMouseButtonDown(0)) {
    textColor = RED;
  } else {
    textColor = LIGHTGRAY;
  }
}

int main() {
  InitWindow(screenWidth, screenHeight, "window");
  ToggleFullscreen();
  
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    processInput();
    draw();
  }

  CloseWindow();

  return 0;
}