#include <stdio.h>
#include <math.h>
#include "raylib.h"
#include <stdbool.h>
// This code is a simple raylib application that demonstrates a player moving around a grid-based level

// Define ColorEquals if not available in your raylib version
#ifndef ColorEquals
inline bool ColorEquals(Color a, Color b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}
#endif
    // Initialization
    //--------------------------------------------------------------------------------------
const int screenWidth = 800;
const int screenHeight = 450;
const int levelW = screenWidth / 10;
const int levelH = screenHeight / 10;
Color levelData[levelH][levelW];
void
generateLevel() {
    for (size_t y = 0; y < levelH; ++y) {
        for (size_t x = 0; x < levelW; ++x) {
            if ((x + y) % 2 == 0) {
                levelData[y][x] = WHITE; // Wall
            } else {
                levelData[y][x] = LIGHTGRAY; // Empty space
            }
        }
    }
}
void
generateLevelWithWalls() {
    for (size_t y = 0; y < levelH; ++y) {
        for (size_t x = 0; x < levelW; ++x) {
            if (x == 0 || x == levelW - 1 || y == 0 || y == levelH - 1) {
                levelData[y][x] = WHITE; // Wall
            } else {
                levelData[y][x] = LIGHTGRAY; // Empty space
            }
        }
    }
}
bool
loadLevel(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Could not open level file: %s\n", filename);
        return false;
    }
    int count = 0;
    for (size_t y = 0; y < levelH; ++y) {
        for (size_t x = 0; x < levelW; ++x) {
            char value = 0;
            // Skip newlines and other non-level characters
            do {
                if (fscanf(file, "%c", &value) != 1) {
                    value = ' '; // Default to empty if file ends early
                    break;
                }
            } while (value == '\n' || value == '\r');
            if (value == '*') {
                levelData[y][x] = WHITE;
            } else {
                levelData[y][x] = LIGHTGRAY;
            }
        }
    }
    fclose(file);
    printf("Loaded level with %d cells\n", count);
    return true;
}
class Player {
public:
    int x, y;
    Player(int startX, int startY) : x(startX), y(startY) {}
    void move(int dx, int dy) {
        // Check bounds to prevent moving out of the level
        if (x + dx < 0 || x + dx >= levelW || y + dy < 0 || y + dy >= levelH) {
            return; // Prevent moving out of bounds
        }
        
        x += dx;
        y += dy;
    }
    void draw() const {
        DrawRectangle(x * 10, y * 10, 10, 10, RED);
    }
};
void
drawCone(const Player& player) {
    // Cone origin is player's center
    int startX = player.x * 10 + 5;
    int startY = player.y * 10 + 5;
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();
    // Angle starts at 270 degrees (downwards), then rotates towards mouse
    float baseAngle = 270.0f * (PI / 180.0f); // 270 degrees in radians
    float angleToMouse = atan2f(mouseY - startY, mouseX - startX);
    float angle = baseAngle + angleToMouse;
    int length = 500;
    float fov = 30.0f * (PI / 180.0f);
    for (int i = 0; i < screenWidth; i++) {
        for (int j = 0; j < screenHeight; j++) {
            float dx = i - startX;
            float dy = j - startY;
            float distance = sqrtf(dx * dx + dy * dy);
            float angleToPixel = atan2f(dy, dx);
            float delta = angleToPixel - angle;
            while (delta > PI) delta -= 2 * PI;
            while (delta < -PI) delta += 2 * PI;
            if (distance < length && fabsf(delta) < fov / 2.0f) {
                Color colorValue = levelData[j * levelH / screenHeight][i * levelW / screenWidth];
                DrawPixel(i, j, colorValue);
            }

            //draw the level area around the player in a circle
            //of radius 100 pixels
            if (distance < 30) {
                Color colorValue = levelData[j * levelH / screenHeight][i * levelW / screenWidth];
                DrawPixel(i, j, colorValue);
            }
        }
    }
}
void
drawLevel(int screenWidth, int screenHeight) {
   
}

int main(void)
{
/*
    if (!loadLevel("level1.txt")) {
        fprintf(stderr, "Failed to load level data\n");
        return 1;
    }
*/
    generateLevel();
    InitWindow(screenWidth, screenHeight, "cone");
    SetMousePosition(screenWidth / 2, screenHeight / 2); // Center the mouse cursor
    SetTargetFPS(60); // Set desired framerate (frames-per-second)
    //--------------------------------------------------------------------------------------
    Player player(screenWidth / 2 / 10, screenHeight / 2 / 10); // Initialize player at center of the screen
    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(BLACK);
        // Update player position based on input
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            player.move(1, 0);
        }
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            player.move(-1, 0);
        }
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
            player.move(0, -1);
        }
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
            player.move(0, 1);
        }
        //drawLevel(screenWidth, screenHeight);
        
        drawLevel(screenWidth, screenHeight);
        
        drawCone(player);
        player.draw();
        EndDrawing();
    }
    CloseWindow(); // Close window and OpenGL context
    return 0;
}