#include "EnvItem.h"


EnvItem::EnvItem(Rectangle r, int block, Color col, bool teleport, Vector2 teleportPos, Texture2D tex, bool fall)
    : rect(r), blocking(block), color(col), isTeleportArea(teleport), teleportPosition(teleportPos), texture(tex), isFalling(fall) {}

// Getters and setters
Rectangle EnvItem::GetRect() const { return rect; }
void EnvItem::SetRect(Rectangle r) { rect = r; }
int EnvItem::GetBlocking() const { return blocking; }
Color EnvItem::GetColor() const { return color; }
bool EnvItem::IsTeleportArea() const { return isTeleportArea; }
Vector2 EnvItem::GetTeleportPosition() const { return teleportPosition; }
void EnvItem::SetTexture(Texture2D tex) { texture = tex; }
Texture2D EnvItem::GetTexture() const { return texture; }
bool EnvItem::GetIsFalling() const { return isFalling; }
void EnvItem::SetIsFalling(bool fall) { isFalling = fall; }

void EnvItem::Draw() const {
    if (texture.id > 0) {
        DrawTexture(texture, rect.x, rect.y, WHITE);
    }
    else {
        DrawRectangleRec(rect, color);
    }
}

void EnvItem::Update(float deltaTime, float floorY) {
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
