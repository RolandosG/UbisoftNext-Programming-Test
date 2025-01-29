#ifndef GOLFMECHANICS_H
#define GOLFMECHANICS_H

#include "app\app.h"

class GolfBall;

class GolfMechanics {
public:
    GolfMechanics(GolfBall* golfBall);
    ~GolfMechanics();

    void Update(float deltaTime);
    void Render();
    float INDICATOR_SPEED;

    void StartAiming();
    void StopAiming();
    bool IsAiming() const;

    void StartPowerMode();
    void StopPowerMode();
    bool IsPowerMode() const;

    void AdjustAimAngle(float adjustment);
    float GetAimAngle() const;

    void Shoot(float power, float angle);
    void SetPowerBarPosition(float playerX, float playerY);

private:
    GolfBall* m_golfBall;
    bool m_isAiming;
    bool m_inPowerMode;
    float m_aimAngle;
    float m_aimPower;
    float m_indicatorY;
    bool m_showShotPower = false;
    float m_capturedIndicatorY = 0.0f;
    float m_lastShotPower = 0.0f;
    bool APressedTwice = false;
    float GetIndicatorY() const;
    float m_indicatorDirection;

    // Constants for power bar
    const float MAX_POWER = 100.0f;
    const float MIN_POWER = 1.0f;
    const float BAR_WIDTH = 20.0f;
    const float BAR_HEIGHT = 100.0f;
    // Default position (will be updated in StartPowerMode)
    const float DEFAULT_BAR_X_POS = 100.0f;
    const float DEFAULT_BAR_Y_POS = APP_VIRTUAL_HEIGHT - 400.0f;

    float playerX, playerY;

    float BAR_X_POS;
    float BAR_Y_POS;

    // Helper functions
    void DrawAimLine();
    void DrawPowerBar();
    void DrawIndicator();

    void DrawFilledRectangle(float x, float y, float width, float height, float r, float g, float b);
    float CalculateGolfBallPower(float indicatorY);
    void CommitShotPower(float power);
};

#endif // GOLFMECHANICS_H