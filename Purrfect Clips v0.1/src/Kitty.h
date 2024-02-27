#pragma once


#include "raylib.h"
#include <vector>
#include <cmath>
#include "EnvItem.h"
#include "Player.h"
#include <algorithm> // For std::find_if

class Kitty {
private:
    Rectangle rect;
    float speed;
    bool canJump;
    Texture2D texture;
    bool isFalling;
    int happyness;

public:
    Kitty(Rectangle r, float spd = 0.0f, bool jump = false, Texture2D tex = {}, bool fall = false, int = 0);

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

    int GetHappyness() const;
    void SetHappyness(int hapi);

    void Draw() const;

    void UpdateAI(const std::vector<EnvItem>& envItems, float delta);

    bool IsPlayerClose(const Player& player) const;

    void PetByPlayer(const Player& player, EnvItem& heart, const Texture2D& heartTexture, float deltaTime);

};