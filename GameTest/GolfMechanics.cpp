#include "stdafx.h"
#include "GolfMechanics.h"
#include "GolfBall.h"
#include <math.h>
#include <iostream>

GolfMechanics::GolfMechanics(GolfBall* golfBall) :
    m_golfBall(golfBall),
    m_isAiming(false),
    m_aimAngle(0.0f),
    m_aimPower(0.0f),
    m_inPowerMode(false),
    m_indicatorY(0.0f)
{
}

GolfMechanics::~GolfMechanics() {
}

void GolfMechanics::StartAiming() {
    m_isAiming = true;
    m_inPowerMode = false; // Ensure power mode is off
    m_aimAngle = 0.0f; // Reset aim angle
    m_aimPower = 0.0f; // Reset power
}

void GolfMechanics::StopAiming() {
    m_isAiming = false;
}

void GolfMechanics::StartPowerMode() {
    m_inPowerMode = true;
    m_isAiming = false; // Ensure aiming mode is off
    m_aimPower = 0.0f; // Set initial power

    m_golfBall->GetPosition(playerX, playerY);

  
    SetPowerBarPosition(playerX + 100, playerY);

    m_indicatorY = BAR_Y_POS; // Start at the TOP of the bar
    m_indicatorDirection = -1.0f; // Move DOWNWARDS initially
}

void GolfMechanics::StopPowerMode() {
    m_inPowerMode = false;
}

void GolfMechanics::Update(float deltaTime) {
    if (m_isAiming) {
        // Aiming
        float thumbStickX = App::GetController().GetLeftThumbStickX();
        if (abs(thumbStickX) > 0.5f) {
            float sensitivityFactor = 0.005f;
            float adjustedX = thumbStickX * abs(thumbStickX);
            m_aimAngle -= adjustedX * deltaTime * sensitivityFactor;
        }

        // Always check for A button press (regardless of aiming state)
        if (App::GetController().CheckButton(XINPUT_GAMEPAD_A, false)) {
            StopAiming();    // Stop aiming
            StartPowerMode(); // Start power mode
        }

    }
    else if (m_inPowerMode) {
        // Power mode logic


        DrawPowerBar(); // Draw the power bar
       

        // Shooting
        if (App::GetController().CheckButton(XINPUT_GAMEPAD_A, true)) {
            m_capturedIndicatorY = m_indicatorY; 

            // Calculate the power
            m_lastShotPower = CalculateGolfBallPower(m_capturedIndicatorY);
            // Set the flag to display the shot power
            m_showShotPower = true;

            CommitShotPower(m_lastShotPower);
            
            StopPowerMode();

        }
    }
}

void GolfMechanics::SetPowerBarPosition(float playerX, float playerY) {
    BAR_X_POS = playerX - 80.0f;        // Adjust the offset as needed
    BAR_Y_POS = playerY - BAR_HEIGHT / 2; // Center vertically relative to the player
}


void GolfMechanics::Render() {
    if (m_isAiming) {
        DrawAimLine();
    }
    static float indicatorY = BAR_Y_POS - BAR_HEIGHT;
    if (m_inPowerMode) {
        DrawPowerBar();
        // Animate the indicator
       // Start at the bottom
        static float indicatorDirection = 1.0f; // 1 for up, -1 for down

        // Move the indicator (adjust speed as needed)
        indicatorY += indicatorDirection * 2.0f; // Example speed: 2.0f 

        // Reverse direction at the top and bottom
        if (indicatorY >= BAR_Y_POS) {
            indicatorY = BAR_Y_POS;
            indicatorDirection = -1.0f;
        }
        else if (indicatorY <= BAR_Y_POS - BAR_HEIGHT) {
            indicatorY = BAR_Y_POS - BAR_HEIGHT;
            indicatorDirection = 1.0f;
        }

        // Capture the indicatorY value
        m_indicatorY = indicatorY;

    }
}

void GolfMechanics::DrawAimLine() {
    float ballX, ballY;
    m_golfBall->GetPosition(ballX, ballY);

    float lineLength = 50.0f; // Adjust as needed
    float endX = ballX + (float)cos(m_aimAngle) * lineLength;
    float endY = ballY + (float)sin(m_aimAngle) * lineLength;

    App::DrawLine(ballX, ballY, endX, endY, 0.0f, 0.0f, 0.0f); // Black line
}

void GolfMechanics::DrawPowerBar() {
    // Draw the power bar background (empty part)
   // Flipped: Start at BAR_Y_POS - BAR_HEIGHT and draw downwards
    DrawFilledRectangle(BAR_X_POS, BAR_Y_POS, BAR_WIDTH, BAR_HEIGHT, 0.5f, 0.5f, 0.5f); // Grey

    // Draw the filled part of the power bar
    float filledHeight = BAR_HEIGHT * (CalculateGolfBallPower(m_indicatorY) / MAX_POWER);
    // Flipped: Start at BAR_Y_POS and draw downwards
    DrawFilledRectangle(BAR_X_POS, BAR_Y_POS, BAR_WIDTH, filledHeight, 1.0f, 0.0f, 0.0f); // Red

    // Print the power value above the bar
    // Flipped: Position adjusted to be below the bar
    App::PrintTwo(BAR_X_POS, BAR_Y_POS + BAR_HEIGHT, "", CalculateGolfBallPower(m_indicatorY));

}


void GolfMechanics::Shoot(float power, float angle) {
    if (m_golfBall) {
        std::cout << "Shot taken: Power = " << power << ", Angle = " << angle << std::endl;
    }
}

bool GolfMechanics::IsAiming() const {
    return m_isAiming;
}

bool GolfMechanics::IsPowerMode() const {
    return m_inPowerMode;
}

void GolfMechanics::AdjustAimAngle(float adjustment) {
    m_aimAngle += adjustment;
}

float GolfMechanics::GetAimAngle() const {
    return m_aimAngle;
}

// Helper function to draw filled rectangles
void GolfMechanics::DrawFilledRectangle(float x, float y, float width, float height, float r, float g, float b) {
    for (int i = 0; i < height; ++i) {
        App::DrawLine(x, y + i, x + width, y + i, r, g, b);
    }
}

float GolfMechanics::GetIndicatorY() const {
    
    static float indicatorY = BAR_Y_POS - BAR_HEIGHT;
    return m_indicatorY; // Return the current indicatorY value
}

float GolfMechanics::CalculateGolfBallPower(float indicatorY) {
   // 1. Calculate the position of the indicator relative to the TOP of the bar
    float relativeIndicatorY = BAR_Y_POS - indicatorY;

    // 2. Normalize the relative position to a 0-1 range
    float normalizedIndicatorY = relativeIndicatorY / BAR_HEIGHT;

    // 3. Clamp the normalized value between 0.0 and 1.0 to prevent negative power or power greater than MAX_POWER
    normalizedIndicatorY = std::max(0.0f, std::min(normalizedIndicatorY, 1.0f));

    // 4. Calculate the power based on the normalized position and MAX_POWER
    float power = normalizedIndicatorY * MAX_POWER;

    // 5. Ensure minimum power
    power = std::max(power, MIN_POWER);

    return power;
}

void GolfMechanics::CommitShotPower(float power) {
    float shotPower = power;

    // Use App::PrintTwo to display the power
    App::PrintTwo(200, 200, "Shot Power: %f", shotPower);  // Adjust position as needed

    if (shotPower > 0.0f) {

        App::PrintTwo(200, 200, "POWER: ", shotPower);
        m_golfBall->MoveBall(shotPower, m_aimAngle);
       
    }
    else {
        
        App::PrintTwo(200, 200, "NEGATIVE NUMBER, GolfMechganics.cpp LINE: ", 193);
    }
}