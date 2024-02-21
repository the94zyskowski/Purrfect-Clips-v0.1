#include "src/Kitty.h"
#include "src/EnvItem.h"
#include "src/Player.h"

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

int main() {
    const int windowWidth = GetMonitorWidth(0);
    const int windowHeight = GetMonitorHeight(0);

    // Enable config flags for resizable window and vertical synchro
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(windowWidth, windowHeight, "Purrfect Clips v0.1");
    SetWindowMinSize(1280, 720);

    int gameScreenWidth = 640;
    int gameScreenHeight = 360;
    Vector2 characterPosition = { (float)gameScreenWidth / 2, (float)gameScreenHeight / 2 };

    // Render texture initialization, used to hold the rendering result so we can easily resize it
    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);  // Texture scale filter to use

    // Set framerate
    SetTargetFPS(60);

    // Texture loading
    Texture2D boy_texture = LoadTexture("assets/Character1_border.png");
    Texture2D map = LoadTexture("assets/map01.png");
    Texture2D cat_texture = LoadTexture("assets/kitty03.png");
    Texture2D painting_texture = LoadTexture("assets/painting02.png"); //Zajebiœcie jakby tak zrobiæ, ¿e da siê str¹ciæ obrazek.

    //Object declaration
    std::vector<EnvItem> envItems;

    EnvItem painting({ 400, 350, 30, 24 }, 1, WHITE, false, { 0, 0 }, painting_texture, false);
    envItems.push_back(painting);

    EnvItem block1({ 64, 448, 320, 16 }, 1, BLANK);
    envItems.push_back(block1);
    EnvItem block2({ 64, 640, 1152, 16 }, 1, BLANK);
    envItems.push_back(block2);
    EnvItem block3({ 384, 448, 80, 16 }, 0, RED, true, Vector2{ 320, 448 });
    envItems.push_back(block3);

    Player player({ 400, 280, 26, 63 }, 0.0f, false, boy_texture);

    Kitty kitty_01({ 300, 280, 32, 32 }, 0.0f, false, cat_texture); //mo¿e dodaæ jakiœ handler, ¿e jak dodajê obiekt to mi go renderuje, dunno

    //Camera setting up
    Camera2D camera = { 0 };
    camera.target = player.GetPosition();
    camera.offset = Vector2{ gameScreenWidth / 2.0f, gameScreenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    int cameraOption = 0;

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        //----------------------------------------------------------------------------------
        TraceLog(LOG_INFO, "Collision detected: %d", CheckCollisionRecs(player.GetRect(), block3.GetRect()));

        float deltaTime = GetFrameTime();

        // Handle user input and update player
        player.Update(envItems, deltaTime);
        kitty_01.UpdateAI(envItems, deltaTime);
        if (IsKeyPressed(KEY_C)) {
            cameraOption++;
            if (cameraOption > 2) cameraOption = 0;
        }
        camera.zoom += ((float)GetMouseWheelMove() * 0.05f);
        if (IsKeyPressed(KEY_R))
        {
            camera.zoom = 1.0f;
            player.SetPosition(Vector2{ 400, 280 });
        }
        if (IsKeyPressed(KEY_ZERO)) ToggleFullscreen();
        for (auto& item : envItems) {
            item.Update(deltaTime, 448); // Zak³adaj¹c, ¿e 448 to wysokoœæ pod³ogi
        }

        // Update camera
        switch (cameraOption) {
        case 0: player.UpdateCameraCenterSmoothFollow(&camera, deltaTime, gameScreenWidth, gameScreenHeight); break;
        case 1: player.UpdateCameraPlayerBoundsPush(&camera, gameScreenWidth, gameScreenHeight); break;
        case 2: player.UpdateCameraCenter(&camera, gameScreenWidth, gameScreenHeight); break;
        }
        // Compute required framebuffer scaling
        float scale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);

        // Draw everything in the render texture
        BeginTextureMode(target);
        ClearBackground(RAYWHITE);  // Clear render texture background color

        // Note that BeginMode2D and EndMode2D are called to ensure the camera is applied to the rendering
        BeginMode2D(camera);

        // Draw game elements here
        DrawTexture(map, 0, 0, WHITE);

        player.Draw();
        kitty_01.Draw();

        //DrawCircleV(Vector2{350,350}, 32, Fade(YELLOW, 0.5f));

        //Draw all EnvItem
        for (const auto& item : envItems) {
            item.Draw();
        }

        EndMode2D();
        EndTextureMode();

        // Draw the render texture to screen, properly scaled
        BeginDrawing();
        ClearBackground(BLACK);     // Clear screen background
        DrawTexturePro(target.texture, Rectangle{ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
            Rectangle{
            (GetScreenWidth() - ((float)gameScreenWidth * scale)) * 0.5f, (GetScreenHeight() - ((float)gameScreenHeight * scale)) * 0.5f,
                (float)gameScreenWidth * scale, (float)gameScreenHeight * scale
            }, Vector2{ 0, 0 }, 0.0f, WHITE);
        EndDrawing();
    }



    // Releasing resources
    UnloadRenderTexture(target);        // Unload render texture
    UnloadTexture(boy_texture);
    UnloadTexture(cat_texture);
    UnloadTexture(painting_texture);
    UnloadTexture(map);


    CloseWindow(); // Close window

    return 0;
}
