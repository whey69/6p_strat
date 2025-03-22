x86_64-w64-mingw32-gcc main.c -o main.exe -Irl/include -Lrl/lib -lraylib -lopengl32 -lgdi32 -lwinmm
mv main.exe build/