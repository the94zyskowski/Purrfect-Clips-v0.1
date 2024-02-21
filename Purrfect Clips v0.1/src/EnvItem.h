#pragma once

#include "raylib.h"
#include <vector>

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
    EnvItem(Rectangle r, int block, Color col, bool teleport = false, Vector2 teleportPos = { 0, 0 }, Texture2D tex = {}, bool fall = false);

    // Getters and setters
    Rectangle GetRect() const;
    void SetRect(Rectangle r);
    int GetBlocking() const;
    Color GetColor() const;
    bool IsTeleportArea() const;
    Vector2 GetTeleportPosition() const;
    void SetTexture(Texture2D tex);
    bool GetIsFalling() const;
    void SetIsFalling(bool fall);

    void Draw() const;

    void Update(float deltaTime, float floorY);
};