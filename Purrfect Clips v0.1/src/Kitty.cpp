#include "Kitty.h"

constexpr auto G = 400;
constexpr auto KITTY_JUMP_SPD = 350.0f;
constexpr auto KITTY_HOR_SPD = 200.0f;

//Constructor
Kitty::Kitty(Rectangle r, float spd, bool jump, Texture2D tex, bool fall)
    : rect(r), speed(spd), canJump(jump), texture(tex), isFalling(fall) {}


// Getters and setters
Rectangle Kitty::GetRect() { return rect; }
void Kitty::SetRect(Rectangle r) { rect = r; }

Vector2 Kitty::GetPosition() const { return Vector2{ rect.x, rect.y }; }
void Kitty::SetPosition(Vector2 v) {
    rect.x = v.x;
    rect.y = v.y;
}

float Kitty::GetSpeed() const { return speed; }
void Kitty::SetSpeed(float spd) { speed = spd; }

bool Kitty::CanJump() const { return canJump; }
void Kitty::SetCanJump(bool can) { canJump = can; }

void Kitty::SetTexture(Texture2D tex) { texture = tex; }

bool Kitty::GetIsFalling() const { return isFalling; }
void Kitty::SetIsFalling(bool fall) { isFalling = fall; }

void Kitty::Draw() const {
    if (texture.id > 0) {
        DrawTexture(texture, rect.x, rect.y, WHITE);
    }
    else {
        DrawRectangleRec({ rect.x, rect.y, rect.width, rect.height }, BLACK);
    }
}

void Kitty::UpdateAI(const std::vector<EnvItem>& envItems, float delta) {
    static float timeSinceLastChange = 0;
    static float changeInterval = 1.0f; // zmiana co 1 sekundê
    static Vector2 direction = { 0, 0 };

    timeSinceLastChange += delta;

    if (timeSinceLastChange >= changeInterval) {
        // Losuj nowy kierunek
        direction.x = (rand() % 3 - 1) * KITTY_HOR_SPD; // -1, 0 lub 1
        direction.y = (rand() % 2) * -KITTY_JUMP_SPD; // 0 lub -1

        timeSinceLastChange = 0; // Resetuj timer
    }

    rect.x += direction.x * delta;
    if (direction.y < 0 && canJump) { // Upewnij siê, ¿e kotek mo¿e skoczyæ
        speed = direction.y;
        canJump = false;
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

bool Kitty::IsPlayerClose(const Player& player) const {
    double distance_limit = 64;

    Rectangle playerRectangle = player.GetRect();
    double playerX = playerRectangle.x + (0.5 * playerRectangle.width);
    double playerY = playerRectangle.y + (0.5 * playerRectangle.height);

    double kittyX = rect.x + (0.5 * rect.width);
    double kittyY = rect.y + (0.5 * rect.height);

    double distance = std::hypot(playerX - kittyX, playerY - kittyY);

    return distance < distance_limit;
}

void Kitty::PetByPlayer(std::vector<EnvItem>& envItems, const Texture2D& heartTexture) {
    for (EnvItem& item : envItems) {
        if (item.GetTexture().id == heartTexture.id) {
            item.SetRect({ this->rect.x, this->rect.y, 32, 32 });
            item.Draw();
        }
    }
}