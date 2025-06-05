#include <stdio.h>
#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <iostream>

using namespace std;

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

class Monster {
public:
    int x, y;
    Monster(int startX, int startY) : x(startX), y(startY) {
        cerr << "Monster created at (" << x << ", " << y << ")\n";
    }
    void move(int dx, int dy) {
        // Check bounds to prevent moving out of the level
        if (x + dx < 0 || x + dx >= levelW || y + dy < 0 || y + dy >= levelH) {
            return; // Prevent moving out of bounds
        }
        
        x += dx;
        y += dy;
    }
    void draw() const {
        DrawRectangle(x * 10, y * 10, 10, 10, BLUE);
    }
};


//our draw function which draws a cone from the player to the mouse position
//as well as the player and the monster
void
draw(const Player& player, const Monster& monster) {
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

            //if the monster is inside the cone, draw it
            if (monster.x * 10 <= i && i <= (monster.x + 1) * 10 &&
                monster.y * 10 <= j && j <= (monster.y + 1) * 10) {
                if (distance < length && fabsf(delta) < fov / 2.0f) {
                    DrawRectangle(monster.x * 10, monster.y * 10, 10, 10, BLUE);
                }
            }
        }
    }
}

int main(void)
{
    generateLevel();
    InitWindow(screenWidth, screenHeight, "cone");
    SetMousePosition(screenWidth / 2, screenHeight / 2); // Center the mouse cursor
    SetTargetFPS(60); // Set desired framerate (frames-per-second)
    //--------------------------------------------------------------------------------------
    Player player(screenWidth / 2 / 10, screenHeight / 2 / 10); // Initialize player at center of the screen
    Monster monster(rand() % levelW, rand() % levelH); // Initialize monster at random position within level    

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

        //set the window title to the player's position
        string title = "Player Position: (" + to_string(player.x) + ", " + to_string(player.y) + ")";
        SetWindowTitle(title.c_str());
        
        draw(player, monster); // Draw evrything

        //count seconds since the start of the game
        static float seconds = 0.0f;
        seconds += GetFrameTime();
        if (seconds >= 0.3f) {
            seconds = 0.0f; // Reset seconds
            
            //move the monster towards the player
            int dx = player.x - monster.x;
            int dy = player.y - monster.y;
            if (dx > 0) {
                monster.move(1, 0);
            } else if (dx < 0) {
                monster.move(-1, 0);
            }
            if (dy > 0) {
                monster.move(0, 1);
            } else if (dy < 0) {
                monster.move(0, -1);
            }
            
        }
        
        EndDrawing();
    }
    CloseWindow(); // Close window and OpenGL context
    return 0;
}