#pragma once

#include "raylib.h"
#include <vector>
#include "EnvItem.h"

class Player {
private:
    Rectangle rect;
    float speed;
    bool canJump;
    Texture2D texture;

public:
    Player(Rectangle r, float spd = 0.0f, bool jump = false, Texture2D tex = {});


    // Getters and setters
    Rectangle GetRect();
    void SetRect(Rectangle r);

    Vector2 GetPosition() const;
    void SetPosition(Vector2 v);

    float GetSpeed() const;
    void SetSpeed(float spd);

    bool CanJump() const;
    void SetCanJump(bool can);

    void Update(const std::vector<EnvItem>& envItems, float delta);
    void UpdateCameraCenter(Camera2D* camera, int width, int height);
    void UpdateCameraCenterSmoothFollow(Camera2D* camera, float delta, int width, int height);
    void UpdateCameraPlayerBoundsPush(Camera2D* camera, int width, int height);

    void Draw() const;
};