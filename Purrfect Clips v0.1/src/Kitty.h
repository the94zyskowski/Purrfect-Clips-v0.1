#pragma once


#include "raylib.h"
#include <vector>
#include "EnvItem.h"

class Kitty {
private:
    Rectangle rect;
    float speed;
    bool canJump;
    Texture2D texture;
    bool isFalling;

public:
    Kitty(Rectangle r, float spd = 0.0f, bool jump = false, Texture2D tex = {}, bool fall = false);

    // Getters and setters
    Rectangle GetRect();
    void SetRect(Rectangle r);

    Vector2 GetPosition() const;
    void SetPosition(Vector2 v);

    float GetSpeed() const;
    void SetSpeed(float spd);

    bool CanJump() const;
    void SetCanJump(bool can);

    void SetTexture(Texture2D tex);

    bool GetIsFalling() const;
    void SetIsFalling(bool fall);

    void Draw() const;

    void UpdateAI(const std::vector<EnvItem>& envItems, float delta);
};
