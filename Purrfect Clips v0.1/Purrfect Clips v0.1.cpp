#include "raylib.h"
#include "raymath.h"        // Required for: Vector2Clamp()

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

int main() {
    const int windowWidth = GetMonitorWidth(0);
    const int windowHeight = GetMonitorHeight(0);

    // Enable config flags for resizable window and vertical synchro
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(windowWidth, windowHeight, "raylib [core] example - window scale letterbox");
    SetWindowMinSize(1280, 720);

    int gameScreenWidth = 640;
    int gameScreenHeight = 360;
    Vector2 characterPosition = { (float)gameScreenWidth / 2, (float)gameScreenHeight / 2 };

    // Render texture initialization, used to hold the rendering result so we can easily resize it
    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);  // Texture scale filter to use

    // Set framerate and fullscreen
    SetTargetFPS(60);
    ToggleFullscreen();

    // Texture loading
    Texture2D boy = LoadTexture("C:\\Users\\GamingPC\\Pictures\\Aseprite\\Character1.png");
    Texture2D map = LoadTexture("C:\\Users\\GamingPC\\Pictures\\Aseprite\\map01.png");
    Texture2D cat = LoadTexture("C:\\Users\\GamingPC\\Pictures\\Aseprite\\kitty03.png");

    // Main game loop
    while (!WindowShouldClose()) {
        // Get user inputs
        if (IsKeyDown(KEY_RIGHT)) { characterPosition.x += 2.0f; }
        if (IsKeyDown(KEY_LEFT)) { characterPosition.x -= 2.0f; }
        if (IsKeyDown(KEY_UP)) { characterPosition.y -= 2.0f; }
        if (IsKeyDown(KEY_DOWN)) { characterPosition.y += 2.0f; }

        // Compute required framebuffer scaling
        float scale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);

        // Update virtual mouse (clamped mouse value behind game screen)
        Vector2 mouse = GetMousePosition();
        Vector2 virtualMouse = { 0 };
        virtualMouse.x = (mouse.x - (GetScreenWidth() - (gameScreenWidth * scale)) * 0.5f) / scale;
        virtualMouse.y = (mouse.y - (GetScreenHeight() - (gameScreenHeight * scale)) * 0.5f) / scale;
        virtualMouse = Vector2Clamp(virtualMouse, Vector2{ 0, 0 }, Vector2{ (float)gameScreenWidth, (float)gameScreenHeight });

        // Draw everything in the render texture, note this will not be rendered on screen, yet
        BeginTextureMode(target);
        ClearBackground(RAYWHITE);  // Clear render texture background color
        DrawText(TextFormat("Default Mouse: [%i , %i]", (int)mouse.x, (int)mouse.y), 350, 25, 20, GREEN);
        DrawText(TextFormat("Virtual Mouse: [%i , %i]", (int)virtualMouse.x, (int)virtualMouse.y), 350, 55, 20, BLACK);
        DrawTexture(map, 0, 0, WHITE);
        DrawTexture(cat, 360, 180, WHITE);
        DrawTexture(boy, characterPosition.x, characterPosition.y, WHITE); // WHITE to kolor t³a
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);     // Clear screen background

        // Draw render texture to screen, properly scaled
        DrawTexturePro(target.texture, Rectangle{ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
            Rectangle{
            (GetScreenWidth() - ((float)gameScreenWidth * scale)) * 0.5f, (GetScreenHeight() - ((float)gameScreenHeight * scale)) * 0.5f,
                (float)gameScreenWidth * scale, (float)gameScreenHeight * scale
            }, Vector2{ 0, 0 }, 0.0f, WHITE);
        EndDrawing();

    }

    // Releasing resources
    UnloadRenderTexture(target);        // Unload render texture
    UnloadTexture(boy);
    UnloadTexture(cat);
    UnloadTexture(map);
    

    CloseWindow(); // Close window

    return 0;
}
