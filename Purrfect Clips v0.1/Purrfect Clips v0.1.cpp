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

    int gameScreenWidth = 1280;
    int gameScreenHeight = 720;

    // Render texture initialization, used to hold the rendering result so we can easily resize it
    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use

    // Za³adowanie tekstur (upewnij siê, ¿e œcie¿ki do plików s¹ poprawne)
    // czy tekstura zosta³a poprawnie za³adowana, u¿ywaj¹c funkcji texture.id > 0 (jeœli id jest 0, tekstura nie zosta³a za³adowana poprawnie).
    Texture2D frame1 = LoadTexture("C:\\Users\\GamingPC\\Pictures\\Aseprite\\Character4.png");
    Texture2D frame2 = LoadTexture("C:\\Users\\GamingPC\\Pictures\\Aseprite\\Character1.png");
    Texture2D frame3 = LoadTexture("C:\\Users\\GamingPC\\Pictures\\Aseprite\\Character2.png");
    Texture2D frame4 = LoadTexture("C:\\Users\\GamingPC\\Pictures\\Aseprite\\Character3.png");
    Texture2D frames[] = { frame1, frame2, frame3, frame4 };

    Texture2D wooden_floor = LoadTexture("C:\\Users\\GamingPC\\Pictures\\Aseprite\\map01.png");

    // Inicjalizacja zmiennych do animacji
    int currentFrame = 0;
    int maxFrames = 0;
    float updateTime = 1.0f / 4.0f; // Aktualizacja animacji 4 razy na sekundê
    float runningTime = 0.0f;

    Vector2 characterPosition = { (float)gameScreenWidth / 2, (float)gameScreenHeight / 2 };

    // Ustawienie FPS (klatek na sekundê)
    SetTargetFPS(60);
    ToggleFullscreen();

    // G³ówna pêtla gry
    while (!WindowShouldClose()) {
        // Update
        // Compute required framebuffer scaling
        float scale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);


        // Update virtual mouse (clamped mouse value behind game screen)
        Vector2 mouse = GetMousePosition();
        Vector2 virtualMouse = { 0 };
        virtualMouse.x = (mouse.x - (GetScreenWidth() - (gameScreenWidth * scale)) * 0.5f) / scale;
        virtualMouse.y = (mouse.y - (GetScreenHeight() - (gameScreenHeight * scale)) * 0.5f) / scale;
        virtualMouse = Vector2Clamp(virtualMouse, Vector2{ 0, 0 }, Vector2{ (float)gameScreenWidth, (float)gameScreenHeight });

        // Apply the same transformation as the virtual mouse to the real mouse (i.e. to work with raygui)
        //SetMouseOffset(-(GetScreenWidth() - (gameScreenWidth*scale))*0.5f, -(GetScreenHeight() - (gameScreenHeight*scale))*0.5f);
        //SetMouseScale(1/scale, 1/scale);

        // Draw everything in the render texture, note this will not be rendered on screen, yet
        BeginTextureMode(target);
        ClearBackground(RAYWHITE);  // Clear render texture background color
        DrawText(TextFormat("Default Mouse: [%i , %i]", (int)mouse.x, (int)mouse.y), 350, 25, 20, GREEN);
        DrawText(TextFormat("Virtual Mouse: [%i , %i]", (int)virtualMouse.x, (int)virtualMouse.y), 350, 55, 20, BLACK);
        maxFrames = 2;
        if (IsKeyDown(KEY_RIGHT)) { characterPosition.x += 2.0f;  maxFrames = 4; }
        if (IsKeyDown(KEY_LEFT)) { characterPosition.x -= 2.0f;   maxFrames = 4; }
        if (IsKeyDown(KEY_UP)) { characterPosition.y -= 2.0f;     maxFrames = 4; }
        if (IsKeyDown(KEY_DOWN)) { characterPosition.y += 2.0f;   maxFrames = 4; }


        // Aktualizacja czasu
        runningTime += GetFrameTime();

        // Aktualizacja klatki animacji
        if (runningTime >= updateTime) {
            runningTime = 0.0f;
            currentFrame++;
            if (currentFrame > maxFrames - 1) currentFrame = 0;
        }

        // Rysowanie aktualnej klatki animacji
        DrawTexture(wooden_floor, 0, 0, WHITE);
        DrawTexture(frames[currentFrame], characterPosition.x, characterPosition.y, WHITE); // WHITE to kolor t³a
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

    // Zwolnienie zasobów
    UnloadRenderTexture(target);        // Unload render texture
    for (int i = 0; i < maxFrames; i++) {
        UnloadTexture(frames[i]);
    }

    CloseWindow(); // Zamkniêcie okna

    return 0;
}
