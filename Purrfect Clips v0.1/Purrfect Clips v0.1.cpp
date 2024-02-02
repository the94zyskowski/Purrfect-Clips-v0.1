#include "raylib.h"
#include "raymath.h"        // Required for: Vector2Clamp()

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

constexpr auto G = 1200;
constexpr auto PLAYER_JUMP_SPD = 350.0f;
constexpr auto PLAYER_HOR_SPD = 200.0f;

typedef struct Player {
    Vector2 position;
    float speed;
    bool canJump;
} Player;

typedef struct EnvItem {
    Rectangle rect;
    int blocking;
    Color color;
    bool isTeleportArea;
    Vector2 teleportPosition;
} EnvItem;

//----------------------------------------------------------------------------------
// Module functions declaration
//----------------------------------------------------------------------------------
void UpdatePlayer(Player* player, EnvItem* envItems, int envItemsLength, float delta);
void UpdateCameraCenter(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraCenterSmoothFollow(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraPlayerBoundsPush(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);


int main() {
    const int windowWidth = GetMonitorWidth(0);
    const int windowHeight = GetMonitorHeight(0);

    // Enable config flags for resizable window and vertical synchro
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(windowWidth, windowHeight, "Purrfect Clips v0.1");
    SetWindowMinSize(1280, 720);

    int gameScreenWidth = 640;
    int gameScreenHeight = 320;
    Vector2 characterPosition = { (float)gameScreenWidth / 2, (float)gameScreenHeight / 2 };

    // Render texture initialization, used to hold the rendering result so we can easily resize it
    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);  // Texture scale filter to use

    // Set framerate and fullscreen
    SetTargetFPS(60);
    //ToggleFullscreen();

    // Texture loading
    Texture2D boy = LoadTexture("C:\\Users\\GamingPC\\Pictures\\Aseprite\\Character1.png");
    Texture2D map = LoadTexture("C:\\Users\\GamingPC\\Pictures\\Aseprite\\map01.png");
    Texture2D cat = LoadTexture("C:\\Users\\GamingPC\\Pictures\\Aseprite\\kitty03.png");
    Texture2D painting = LoadTexture("C:\\Users\\GamingPC\\Pictures\\Aseprite\\painting01.png"); //Zajebiœcie jakby tak zrobiæ, ¿e da siê str¹ciæ obrazek.

    Player player = { 0 };
    player.position = Vector2{ 400, 280 };
    player.speed = 0;
    player.canJump = false;
    EnvItem envItems[] = {
        {{ 64, 448, 1152, 16 }, 1, BLANK, false, 0, 0 },
        {{ 64, 640, 1152, 16 }, 1, BLANK, false, 0, 0 },
        {{400, 440, 80, 16}, 1, RED, true, 320, 640}
    };

    int envItemsLength = sizeof(envItems) / sizeof(envItems[0]);

    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = Vector2{ gameScreenWidth / 2.0f, gameScreenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Store pointers to the multiple update camera functions
    void (*cameraUpdaters[])(Camera2D*, Player*, EnvItem*, int, float, int, int) = {
        UpdateCameraCenterSmoothFollow,
        UpdateCameraPlayerBoundsPush,
        UpdateCameraCenter
    };

    int cameraOption = 0;
    int cameraUpdatersLength = sizeof(cameraUpdaters) / sizeof(cameraUpdaters[0]);

    const char* cameraDescriptions[] = {
        "Follow player center",
        "Follow player center, but clamp to map edges",
        "Follow player center; smoothed",
        "Follow player center horizontally; update player center vertically after landing",
        "Player push camera on getting too close to screen edge"
    };

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();

        // Handle user input and update player
        UpdatePlayer(&player, envItems, envItemsLength, deltaTime);
        if (IsKeyPressed(KEY_C)) cameraOption = (cameraOption + 1) % cameraUpdatersLength;
        camera.zoom += ((float)GetMouseWheelMove() * 0.05f);
        if (IsKeyPressed(KEY_R))
        {
            camera.zoom = 1.0f;
            player.position = Vector2{ 400, 280 };
        }

        // Update camera
        cameraUpdaters[cameraOption](&camera, &player, envItems, envItemsLength, deltaTime, gameScreenWidth, gameScreenHeight);

        // Compute required framebuffer scaling
        float scale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);

        // Draw everything in the render texture
        BeginTextureMode(target);
        ClearBackground(RAYWHITE);  // Clear render texture background color

        // Note that BeginMode2D and EndMode2D are called to ensure the camera is applied to the rendering
        BeginMode2D(camera);

        // Draw game elements here
        DrawTexture(map, 0, 0, WHITE);
        DrawTexture(painting, 400, 350, WHITE);

        for (int i = 0; i < envItemsLength; i++) {
            DrawRectangleRec(envItems[i].rect, envItems[i].color);
        }
        for (int i = 0; i < envItemsLength; i++) DrawRectangleRec(envItems[i].rect, envItems[i].color);
        DrawTexture(boy, player.position.x - 30, player.position.y - 62, WHITE);
        DrawCircle(player.position.x, player.position.y, 5, GOLD);

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
    UnloadTexture(boy);
    UnloadTexture(cat);
    UnloadTexture(map);
    

    CloseWindow(); // Close window

    return 0;
}

void UpdatePlayer(Player* player, EnvItem* envItems, int envItemsLength, float delta)
{
    if (IsKeyDown(KEY_LEFT)) player->position.x -= PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_RIGHT)) player->position.x += PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_SPACE) && player->canJump)
    {
        player->speed = -PLAYER_JUMP_SPD;
        player->canJump = false;
    }

    bool hitObstacle = false;
    for (int i = 0; i < envItemsLength; i++)
    {
        if (envItems[i].isTeleportArea &&
            CheckCollisionPointRec(player->position, envItems[i].rect))
        {
            if (IsKeyPressed(KEY_E))  // Jeœli gracz naciœnie 'E'
            {
                player->position = envItems[i].teleportPosition;  // Teleportuj gracza
                break;  // Przerywamy pêtlê, poniewa¿ teleportacja ju¿ siê odby³a
            }
        }
        EnvItem* ei = envItems + i;
        Vector2* p = &(player->position);
        if (ei->blocking &&
            ei->rect.x <= p->x &&
            ei->rect.x + ei->rect.width >= p->x &&
            ei->rect.y >= p->y &&
            ei->rect.y <= p->y + player->speed * delta)
        {
            hitObstacle = true;
            player->speed = 0.0f;
            p->y = ei->rect.y;
            break;
        }
    }

    if (!hitObstacle)
    {
        player->position.y += player->speed * delta;
        player->speed += G * delta;
        player->canJump = false;
    }
    else player->canJump = true;
}

void UpdateCameraCenter(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height)
{
    camera->offset = Vector2{ width / 2.0f, height / 2.0f};
    camera->target = player->position;
}

void UpdateCameraCenterSmoothFollow(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height)
{
    static float minSpeed = 30;
    static float minEffectLength = 10;
    static float fractionSpeed = 0.8f;

    camera->offset = Vector2{ width / 2.0f, height / 2.0f };
    Vector2 diff = Vector2Subtract(player->position, camera->target);
    float length = Vector2Length(diff);

    if (length > minEffectLength)
    {
        float speed = fmaxf(fractionSpeed * length, minSpeed);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed * delta / length));
    }
}

void UpdateCameraPlayerBoundsPush(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height)
{
    static Vector2 bbox = { 0.2f, 0.2f };

    Vector2 bboxWorldMin = GetScreenToWorld2D(Vector2{ (1 - bbox.x) * 0.5f * width, (1 - bbox.y) * 0.5f * height }, *camera);
    Vector2 bboxWorldMax = GetScreenToWorld2D(Vector2{ (1 + bbox.x) * 0.5f * width, (1 + bbox.y) * 0.5f * height }, *camera);
    camera->offset = Vector2{ (1 - bbox.x) * 0.5f * width, (1 - bbox.y) * 0.5f * height };

    if (player->position.x < bboxWorldMin.x) camera->target.x = player->position.x;
    if (player->position.y < bboxWorldMin.y) camera->target.y = player->position.y;
    if (player->position.x > bboxWorldMax.x) camera->target.x = bboxWorldMin.x + (player->position.x - bboxWorldMax.x);
    if (player->position.y > bboxWorldMax.y) camera->target.y = bboxWorldMin.y + (player->position.y - bboxWorldMax.y);
}

