#include "raylib.h"
#include "raymath.h"        // Required for: Vector2Clamp()

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

constexpr auto G = 1200;
constexpr auto PLAYER_JUMP_SPD = 350.0f;
constexpr auto PLAYER_HOR_SPD = 200.0f;

class EnvItem {
private:
    Rectangle rect;
    int blocking;
    Color color;
    bool isTeleportArea;
    Vector2 teleportPosition;

public:
    // Getters and setters
    Rectangle GetRect() const { return rect; }
    int GetBlocking() const { return blocking; }
    Color GetColor() const { return color; }
    bool IsTeleportArea() const { return isTeleportArea; }
    Vector2 GetTeleportPosition() const { return teleportPosition; }

    EnvItem(Rectangle r, int block, Color col, bool teleport = false, Vector2 teleportPos = { 0, 0 })
        : rect(r), blocking(block), color(col), isTeleportArea(teleport), teleportPosition(teleportPos) {}
};

class Player {
private:
    Vector2 position;
    float speed;
    bool canJump;

public:
    Player(Vector2 pos) : position(pos), speed(0), canJump(false) {}

    // Getters and setters
    Vector2 GetPosition() const { return position; }
    void SetPosition(Vector2 pos) { position = pos; }

    float GetSpeed() const { return speed; }
    void SetSpeed(float spd) { speed = spd; }

    bool CanJump() const { return canJump; }
    void SetCanJump(bool can) { canJump = can; }

    void Update(EnvItem* envItems, int envItemsLength, float delta);
    void UpdateCameraCenter(Camera2D* camera, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
    void UpdateCameraCenterSmoothFollow(Camera2D* camera, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
    void UpdateCameraPlayerBoundsPush(Camera2D* camera, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
};

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
    Texture2D boy = LoadTexture("C:\\Users\\GamingPC\\Pictures\\Aseprite\\Character1.png");
    Texture2D map = LoadTexture("C:\\Users\\GamingPC\\Pictures\\Aseprite\\map01.png");
    Texture2D cat = LoadTexture("C:\\Users\\GamingPC\\Pictures\\Aseprite\\kitty03.png");
    Texture2D painting = LoadTexture("C:\\Users\\GamingPC\\Pictures\\Aseprite\\painting01.png"); //Zajebiœcie jakby tak zrobiæ, ¿e da siê str¹ciæ obrazek.

    Player player(Vector2{ 400, 280 });
    EnvItem envItems[] = {
      EnvItem({ 64, 448, 1152, 16 }, 1, BLANK),
      EnvItem({ 64, 640, 1152, 16 }, 1, BLANK),
      EnvItem({320, 440, 80, 16}, 1, RED, true, Vector2{320, 640})
    };

    int envItemsLength = sizeof(envItems) / sizeof(envItems[0]);

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
        float deltaTime = GetFrameTime();

        // Handle user input and update player
        player.Update(envItems, envItemsLength, deltaTime);
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

        // Update camera
        switch (cameraOption) {
            case 0: player.UpdateCameraCenterSmoothFollow(&camera, envItems, envItemsLength, deltaTime, gameScreenWidth, gameScreenHeight); break;
            case 1: player.UpdateCameraPlayerBoundsPush(&camera, envItems, envItemsLength, deltaTime, gameScreenWidth, gameScreenHeight); break;
            case 2: player.UpdateCameraCenter(&camera, envItems, envItemsLength, deltaTime, gameScreenWidth, gameScreenHeight); break;
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
        DrawTexture(painting, 400, 350, WHITE);

        for (int i = 0; i < envItemsLength; i++) {
            DrawRectangleRec(envItems[i].GetRect(), envItems[i].GetColor());
        }
        for (int i = 0; i < envItemsLength; i++) DrawRectangleRec(envItems[i].GetRect(), envItems[i].GetColor());
        DrawTexture(boy, player.GetPosition().x - 30, player.GetPosition().y - 62, WHITE);
        DrawCircle(player.GetPosition().x, player.GetPosition().y, 5, GOLD);

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
void Player::Update(EnvItem* envItems, int envItemsLength, float delta) {
    // Twoja logika aktualizacji, u¿ywaj getterów dla envItems, np. envItems[i].GetRect()
    if (IsKeyDown(KEY_LEFT)) this->position.x -= PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_RIGHT)) this->position.x += PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_SPACE) && this->canJump) {
        this->speed = -PLAYER_JUMP_SPD;
        this->canJump = false;
    }

    bool hitObstacle = false;
    for (int i = 0; i < envItemsLength; i++) {
        if (envItems[i].IsTeleportArea() && CheckCollisionPointRec(this->position, envItems[i].GetRect())) {
            if (IsKeyPressed(KEY_E)) {
                this->position = envItems[i].GetTeleportPosition();
                break;
            }
        }

        if (envItems[i].GetBlocking() &&
            envItems[i].GetRect().x <= this->position.x &&
            envItems[i].GetRect().x + envItems[i].GetRect().width >= this->position.x &&
            envItems[i].GetRect().y >= this->position.y &&
            envItems[i].GetRect().y <= this->position.y + this->speed * delta) {
            hitObstacle = true;
            this->speed = 0.0f;
            this->position.y = envItems[i].GetRect().y;
            break;
        }
    }

    if (!hitObstacle) {
        this->position.y += this->speed * delta;
        this->speed += G * delta;
        this->canJump = false;
    }
    else {
        this->canJump = true;
    }
}

void Player::UpdateCameraCenter(Camera2D* camera, EnvItem* envItems, int envItemsLength, float delta, int width, int height)
{
    camera->offset = Vector2{ width / 2.0f, height / 2.0f };
    camera->target = this->position;
}

void Player::UpdateCameraCenterSmoothFollow(Camera2D* camera, EnvItem* envItems, int envItemsLength, float delta, int width, int height)
{
    static float minSpeed = 30;
    static float minEffectLength = 10;
    static float fractionSpeed = 0.8f;

    camera->offset = Vector2{ width / 2.0f, height / 2.0f };
    Vector2 diff = Vector2Subtract(this->position, camera->target);
    float length = Vector2Length(diff);

    if (length > minEffectLength)
    {
        float speed = fmaxf(fractionSpeed * length, minSpeed);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed * delta / length));
    }
}

void Player::UpdateCameraPlayerBoundsPush(Camera2D* camera, EnvItem* envItems, int envItemsLength, float delta, int width, int height)
{
    static Vector2 bbox = { 0.2f, 0.2f };

    Vector2 bboxWorldMin = GetScreenToWorld2D(Vector2{ (1 - bbox.x) * 0.5f * width, (1 - bbox.y) * 0.5f * height }, *camera);
    Vector2 bboxWorldMax = GetScreenToWorld2D(Vector2{ (1 + bbox.x) * 0.5f * width, (1 + bbox.y) * 0.5f * height }, *camera);
    camera->offset = Vector2{ (1 - bbox.x) * 0.5f * width, (1 - bbox.y) * 0.5f * height };

    if (this->position.x < bboxWorldMin.x) camera->target.x = this->position.x;
    if (this->position.y < bboxWorldMin.y) camera->target.y = this->position.y;
    if (this->position.x > bboxWorldMax.x) camera->target.x = bboxWorldMin.x + (this->position.x - bboxWorldMax.x);
    if (this->position.y > bboxWorldMax.y) camera->target.y = bboxWorldMin.y + (this->position.y - bboxWorldMax.y);
}