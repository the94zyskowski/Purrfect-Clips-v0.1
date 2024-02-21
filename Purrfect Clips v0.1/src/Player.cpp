#include "Player.h"
#include "raymath.h"

constexpr auto G = 400;
constexpr auto PLAYER_JUMP_SPD = 350.0f;
constexpr auto PLAYER_HOR_SPD = 200.0f;

Player::Player(Rectangle r, float spd, bool jump, Texture2D tex)
    : rect(r), speed(spd), canJump(jump), texture(tex) {}


// Getters and setters
Rectangle Player::GetRect() { return rect; }
void Player::SetRect(Rectangle r) { rect = r; }

Vector2 Player::GetPosition() const { return Vector2{ rect.x, rect.y }; }
void Player::SetPosition(Vector2 v) {
    rect.x = v.x;
    rect.y = v.y;
}

float Player::GetSpeed() const { return speed; }
void Player::SetSpeed(float spd) { speed = spd; }

bool Player::CanJump() const { return canJump; }
void Player::SetCanJump(bool can) { canJump = can; }

void Player::Draw() const {
    if (texture.id > 0) {
        DrawTexture(texture, rect.x, rect.y, WHITE);
    }
    else {
        DrawRectangleRec({ rect.x, rect.y, rect.width, rect.height }, BLACK);
    }
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
