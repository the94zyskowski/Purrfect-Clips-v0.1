#include "raylib.h"
#include "raymath.h"        // Required for: Vector2Clamp()
#include <vector>

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

constexpr auto G = 400;
constexpr auto PLAYER_JUMP_SPD = 350.0f;
constexpr auto PLAYER_HOR_SPD = 200.0f;

class EnvItem {
private:
    Rectangle rect;
    int blocking;
    Color color;
    bool isTeleportArea;
    Vector2 teleportPosition;
    Texture2D texture;
    bool isFalling;

public:
    EnvItem(Rectangle r, int block, Color col, bool teleport = false, Vector2 teleportPos = { 0, 0 }, Texture2D tex = {}, bool fall = false)
        : rect(r), blocking(block), color(col), isTeleportArea(teleport), teleportPosition(teleportPos), texture(tex), isFalling(fall) {}

    // Getters and setters
    Rectangle GetRect() const { return rect; }
    void SetRect(Rectangle r) { r = rect; }
    int GetBlocking() const { return blocking; }
    Color GetColor() const { return color; }
    bool IsTeleportArea() const { return isTeleportArea; }
    Vector2 GetTeleportPosition() const { return teleportPosition; }
    void SetTexture(Texture2D tex) { texture = tex;  }
    bool GetIsFalling() const { return isFalling;  }
    void SetIsFalling(bool fall) { isFalling = fall; }

    void Draw() const {
        if (texture.id > 0) { 
            DrawTexture(texture, rect.x, rect.y, WHITE);
        }
        else {
            DrawRectangleRec(rect, color);
        }
    }

    void Update(float deltaTime, float floorY) {
        if (isFalling) {
            // Przyk³adowa prêdkoœæ spadania, mo¿na dostosowaæ
            const float fallingSpeed = 98.0f; // Piksele na sekundê

            // Aktualizacja pozycji obiektu
            rect.y += fallingSpeed * deltaTime;

            // Sprawdzenie, czy obiekt dotkn¹³ pod³ogi (uwzglêdniaj¹c jego wysokoœæ)
            if (rect.y + rect.height >= floorY) {
                rect.y = floorY - rect.height; // Dostosowanie pozycji, aby unikn¹æ "wbijania siê" w pod³ogê
                isFalling = false;   // Zatrzymanie spadania
            }
        }
    }
};

class Player {
private:
    Rectangle rect;
    float speed;
    bool canJump;
    Texture2D texture;

public:
    Player(Rectangle r, float spd = 0.0f, bool jump = false, Texture2D tex = {})
        : rect(r), speed(spd), canJump(jump), texture(tex) {}


    // Getters and setters
    Rectangle GetRect() { return rect; }
    void SetRect(Rectangle r) { rect = r; }
    Vector2 GetPosition() const { return Vector2{rect.x, rect.y}; }
    void SetPosition(Vector2 v) {
        rect.x = v.x;
        rect.y = v.y;
    }

    float GetSpeed() const { return speed; }
    void SetSpeed(float spd) { speed = spd; }

    bool CanJump() const { return canJump; }
    void SetCanJump(bool can) { canJump = can; }

    void Update(const std::vector<EnvItem>& envItems, float delta);
    void UpdateCameraCenter(Camera2D* camera, int width, int height);
    void UpdateCameraCenterSmoothFollow(Camera2D* camera, float delta, int width, int height);
    void UpdateCameraPlayerBoundsPush(Camera2D* camera, int width, int height);

    void Draw() const {
        if (texture.id > 0) {
            DrawTexture(texture, rect.x, rect.y, WHITE);
        }
        else {
            DrawRectangleRec({ rect.x, rect.y, rect.width, rect.height }, BLACK);
        }
    }
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
    Texture2D boy_texture = LoadTexture("assets/Character1_border.png");
    Texture2D map = LoadTexture("assets/map01.png");
    Texture2D cat = LoadTexture("assets/kitty03.png");
    Texture2D painting_texture = LoadTexture("assets/painting02.png"); //Zajebiœcie jakby tak zrobiæ, ¿e da siê str¹ciæ obrazek.

    //Object declaration
    std::vector<EnvItem> envItems;

    EnvItem painting({ 400, 350, 30, 24 }, 1, WHITE, false, { 0, 0 }, painting_texture, true);
    envItems.push_back(painting);

    EnvItem block1({ 64, 458, 320, 16 }, 1, BLACK);
    envItems.push_back(block1);
    EnvItem block2({ 64, 640, 1152, 16 }, 1, BLANK);
    envItems.push_back(block2);
    EnvItem block3({ 384, 448, 80, 16 }, 0, RED, true, Vector2{ 320, 448 });
    envItems.push_back(block3);

    int envItemsLength = sizeof(envItems) / sizeof(envItems[0]);

    Player player({ 400, 280, 26, 63}, 0.0f, false, boy_texture);

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
        painting.Update(deltaTime, 448);

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
    UnloadTexture(painting_texture);
    UnloadTexture(map);
    

    CloseWindow(); // Close window

    return 0;
}
void Player::Update(const std::vector<EnvItem>& envItems, float delta) {
    // Twoja logika aktualizacji, u¿ywaj getterów dla envItems, np. envItems[i].GetRect()
    if (IsKeyDown(KEY_LEFT)) this->rect.x -= PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_RIGHT)) this->rect.x += PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_SPACE) && this->canJump) {
        this->speed = -PLAYER_JUMP_SPD;
        this->canJump = false;
    }

    bool hitObstacle = false;
    for (const auto& item : envItems) {
        if (item.IsTeleportArea() && CheckCollisionRecs(this->GetRect(), item.GetRect())) {
            if (IsKeyPressed(KEY_E)) {
                Vector2 position = { item.GetTeleportPosition().x, item.GetTeleportPosition().y - this->GetRect().height };
                this->SetPosition(position);
                break;
            }
        }

        if (item.GetBlocking()) {
            if (item.GetRect().x < rect.x + rect.width &&
                item.GetRect().x + item.GetRect().width > rect.x &&
                item.GetRect().y >= rect.y + rect.height &&
                item.GetRect().y < rect.y + rect.height + speed * delta) {
                hitObstacle = true;
                speed = 0.0f;
                // Ajustowanie pozycji Y, aby postaæ sta³a na obiekcie EnvItem
                rect.y = item.GetRect().y - rect.height;
                break;
            }
        }
    }

    if (!hitObstacle) {
        this->rect.y += this->speed * delta;
        this->speed += G * delta;
        this->canJump = false;
    }
    else {
        this->canJump = true;
    }
}

void Player::UpdateCameraCenter(Camera2D* camera, int width, int height)
{
    camera->offset = Vector2{ width / 2.0f, height / 2.0f };
    camera->target = this->GetPosition();
}

void Player::UpdateCameraCenterSmoothFollow(Camera2D* camera, float delta, int width, int height)
{
    static float minSpeed = 30;
    static float minEffectLength = 10;
    static float fractionSpeed = 0.8f;

    camera->offset = Vector2{ width / 2.0f, height / 2.0f };
    Vector2 diff = Vector2Subtract(this->GetPosition(), camera->target);
    float length = Vector2Length(diff);

    if (length > minEffectLength)
    {
        float speed = fmaxf(fractionSpeed * length, minSpeed);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed * delta / length));
    }
}

void Player::UpdateCameraPlayerBoundsPush(Camera2D* camera, int width, int height)
{
    static Vector2 bbox = { 0.2f, 0.2f };

    Vector2 bboxWorldMin = GetScreenToWorld2D(Vector2{ (1 - bbox.x) * 0.5f * width, (1 - bbox.y) * 0.5f * height }, *camera);
    Vector2 bboxWorldMax = GetScreenToWorld2D(Vector2{ (1 + bbox.x) * 0.5f * width, (1 + bbox.y) * 0.5f * height }, *camera);
    camera->offset = Vector2{ (1 - bbox.x) * 0.5f * width, (1 - bbox.y) * 0.5f * height };

    if (this->rect.x < bboxWorldMin.x) camera->target.x = this->rect.x;
    if (this->rect.y < bboxWorldMin.y) camera->target.y = this->rect.y;
    if (this->rect.x > bboxWorldMax.x) camera->target.x = bboxWorldMin.x + (this->rect.x - bboxWorldMax.x);
    if (this->rect.y > bboxWorldMax.y) camera->target.y = bboxWorldMin.y + (this->rect.y - bboxWorldMax.y);
}