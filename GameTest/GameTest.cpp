///////////////////////////////////////////////////////////////////////////////
// Filename: GameTest.cpp
// Provides a demo of how to use the API
///////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
#include "stdafx.h"
//------------------------------------------------------------------------
#include <windows.h> 
#include <math.h>  
//------------------------------------------------------------------------
#include "app\app.h" 
#include "GolfBall.h"
#include "GolfHole.h"
#include "GolfMechanics.h"
#include "GolfWalls.h"
#include "Score.h"
#include <algorithm>
#include <iostream>

//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Example data....
//------------------------------------------------------------------------
CSimpleSprite* testSprite;
enum
{
    ANIM_FORWARDS,
    ANIM_BACKWARDS,
    ANIM_LEFT,
    ANIM_RIGHT,
};
//------------------------------------------------------------------------
GolfBall* golfBall;
GolfHole* golfHole;
GolfMechanics* golfMechanics;
GolfWalls* golfWalls;
Score* m_score;

bool collisionMessagePrinted = false;
bool playerIsMoving = true; // Add this to control player movement
bool overlapPlayerBall = false;
bool CheckBallInHole();
bool isGameOver = false;

int m_currentHole = 1;
const int MAX_HOLES = 5;
int m_parValues[MAX_HOLES] = { 3, 4, 5, 3, 4 };
//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{
    //------------------------------------------------------------------------
    // Example Sprite Code....
    testSprite = App::CreateSprite(".\\TestData\\Test.bmp", 8, 4);
    testSprite->SetPosition(400.0f, 400.0f);
    const float speed = 1.0f / 15.0f;
    m_currentHole = 1;
    testSprite->CreateAnimation(ANIM_BACKWARDS, speed, { 0,1,2,3,4,5,6,7 });
    testSprite->CreateAnimation(ANIM_LEFT, speed, { 8,9,10,11,12,13,14,15 });
    testSprite->CreateAnimation(ANIM_RIGHT, speed, { 16,17,18,19,20,21,22,23 });
    testSprite->CreateAnimation(ANIM_FORWARDS, speed, { 24,25,26,27,28,29,30,31 });
    testSprite->SetScale(1.0f);
    //------------------------------------------------------------------------
    m_score = new Score();
    m_score->StartGame();

    golfBall = new GolfBall();
   
    golfBall->Init(".\\TestData\\golf_ball.bmp", 1, 1);
    srand(time(0));

    golfHole = new GolfHole();
    golfHole->Init(0.0f, 0.0f);

    golfWalls = new GolfWalls();
   
    golfWalls->Init();
    golfWalls->GenerateRandomWalls(10, 50.0f, 150.0f, 10.0f, golfBall, golfHole);


    // Get the walls
    const std::vector<Wall>& generatedWalls = golfWalls->GetWalls();

    // Initialize and check for overlaps with the golf ball
    do {
        golfBall->Init(".\\TestData\\golf_ball.bmp", 1, 1);
    } while (golfBall->CheckOverlapWithWalls(generatedWalls));

    // Initialize and check for overlaps with the golf hole
    do {
        float holeWidth = golfHole->GetSprite()->GetWidth() * golfHole->GetSprite()->GetScale();
        float holeHeight = golfHole->GetSprite()->GetHeight() * golfHole->GetSprite()->GetScale();
        float holeX = rand() % (int)(APP_VIRTUAL_WIDTH - holeWidth) + holeWidth / 2.0f;
        float holeY = rand() % (int)(APP_VIRTUAL_HEIGHT - holeHeight) + holeHeight / 2.0f;

        // Reinitialize the golf hole with the new random position
        golfHole->Init(holeX, holeY);

    } while (golfHole->CheckOverlapWithWalls(generatedWalls));

    golfMechanics = new GolfMechanics(golfBall);

}

void DisplayPressAMessage(float playerX, float playerY) {
    App::Print(playerX, playerY - 30, "Press A"); // Adjust position as needed
}
//------------------------------------------------------------------------
// Update your simulation here. deltaTime is the elapsed time since the last update in ms.
// This will be called at no greater frequency than the value of APP_MAX_FRAME_RATE
//------------------------------------------------------------------------
void Update(const float deltaTime)
{
    //------------------------------------------------------------------------
    // Example Sprite Code....

    testSprite->Update(deltaTime);

    //------------------------------------------------------------------------
    // boundary checks
    static bool wasMoving = false;

    float x, y;
    testSprite->GetPosition(x, y);
    float playerSpeed = 2.0f;

    if (playerIsMoving) { // Only move if playerIsMoving is true
        if (App::GetController().GetLeftThumbStickX() > 0.5f)
        {
            testSprite->SetAnimation(ANIM_RIGHT);
            x += playerSpeed;
        }
        if (App::GetController().GetLeftThumbStickX() < -0.5f)
        {
            testSprite->SetAnimation(ANIM_LEFT);
            x -= playerSpeed;
        }
        if (App::GetController().GetLeftThumbStickY() > 0.5f)
        {
            testSprite->SetAnimation(ANIM_FORWARDS);
            y += playerSpeed;
        }
        if (App::GetController().GetLeftThumbStickY() < -0.5f)
        {
            testSprite->SetAnimation(ANIM_BACKWARDS);
            y -= playerSpeed;
        }
    }

    // Set animation based on movement
    if (playerIsMoving)
    {
        // Use the correct animation indices as defined in Init()
        if (App::GetController().GetLeftThumbStickX() > 0.5f)
        {
            testSprite->SetAnimation(ANIM_RIGHT); // 16, 17, 18, 19, 20, 21, 22, 23
        }
        else if (App::GetController().GetLeftThumbStickX() < -0.5f)
        {
            testSprite->SetAnimation(ANIM_LEFT); // 8, 9, 10, 11, 12, 13, 14, 15
        }
        else if (App::GetController().GetLeftThumbStickY() > 0.5f)
        {
            testSprite->SetAnimation(ANIM_FORWARDS); // 24, 25, 26, 27, 28, 29, 30, 31
        }
        else if (App::GetController().GetLeftThumbStickY() < -0.5f)
        {
            testSprite->SetAnimation(ANIM_BACKWARDS); // 0, 1, 2, 3, 4, 5, 6, 7
        }
    }
    else
    {
        // Stop animation or set idle animation if not moving
        testSprite->SetAnimation(-1); // Assuming -1 stops the animation
        // Or, if you have an idle animation:
        // testSprite->SetAnimation(ANIM_IDLE); 
    }

    // --- Check for Collision with Golf Ball ---

    // Get the positions of the player and the golf ball
    float playerX, playerY;
    testSprite->GetPosition(playerX, playerY);

    float ballX, ballY;
    golfBall->GetPosition(ballX, ballY);

    // Calculate the distance between the centers of the sprites
    float dx = playerX - ballX;
    float dy = playerY - ballY;
    float distance = sqrt(dx * dx + dy * dy);

    // Estimate the radii of the sprites (assuming they are roughly circular)
    float playerRadius = testSprite->GetWidth() * testSprite->GetScale() * 0.5f;
    float ballRadius = golfBall->GetSprite()->GetWidth() * golfBall->GetSprite()->GetScale() * 0.5f;

    // Check if the distance is less than the sum of the radii
    if (distance < playerRadius + ballRadius) {
        if (!golfBall->ballMoving) {
            if (!collisionMessagePrinted) {
                overlapPlayerBall = true;
                DisplayPressAMessage(playerX, playerY);
                std::cout << "Collision Detected!" << std::endl;
                collisionMessagePrinted = true;
                playerIsMoving = false;
                golfMechanics->StartAiming(); // Start aiming mode
            }
        }
    }
    if (golfBall->ballMoving) {
        overlapPlayerBall = false;
        collisionMessagePrinted = false;
        playerIsMoving = true;
        golfMechanics->StopAiming();  // Stop aiming mode
        golfMechanics->StopPowerMode(); // Stop power mode

    }
    m_score->Update(deltaTime);
    // Update golf mechanics
    golfMechanics->Update(deltaTime);
    if (wasMoving && !golfBall->ballMoving) {
        m_score->AddStroke();
    }

    wasMoving = golfBall->ballMoving;
    
    golfBall->UpdatePosition(App::GetDeltaTime(), golfWalls->GetWalls()); // Corrected call

    // Boundary checks (after movement)
    float playerWidth = testSprite->GetWidth() * testSprite->GetScale();
    float playerHeight = testSprite->GetHeight() * testSprite->GetScale();

    // Left
    if (x - playerWidth * 0.5f < 0.0f) {
        x = playerWidth * 0.5f;
    }
    // Right
    if (x + playerWidth * 0.5f > APP_VIRTUAL_WIDTH) {
        x = APP_VIRTUAL_WIDTH - playerWidth * 0.5f;
    }
    // Top
    if (y - playerHeight * 0.5f < 0.0f) {
        y = playerHeight * 0.5f;
    }
    // Bottom
    if (y + playerHeight * 0.5f > APP_VIRTUAL_HEIGHT) {
        y = APP_VIRTUAL_HEIGHT - playerHeight * 0.5f;
    }
    // Update the sprite's position after the checks
    testSprite->SetPosition(x, y);

    if (App::GetController().CheckButton(XINPUT_GAMEPAD_DPAD_UP, false))
    {
        testSprite->SetScale(testSprite->GetScale() + 0.1f);
    }
    if (App::GetController().CheckButton(XINPUT_GAMEPAD_DPAD_DOWN, false))
    {
        testSprite->SetScale(testSprite->GetScale() - 0.1f);
    }
    if (App::GetController().CheckButton(XINPUT_GAMEPAD_DPAD_LEFT, false))
    {
        testSprite->SetAngle(testSprite->GetAngle() + 0.1f);
    }
    if (App::GetController().CheckButton(XINPUT_GAMEPAD_DPAD_RIGHT, false))
    {
        testSprite->SetAngle(testSprite->GetAngle() - 0.1f);
    }
    if (App::GetController().CheckButton(XINPUT_GAMEPAD_A, true))
    {
        testSprite->SetAnimation(-1);
    }

    // Check if the ball is in the hole
    if (CheckBallInHole()) {
        // Ball is in the hole!
        m_score->EndHole();

        if (m_currentHole < MAX_HOLES) {
            // Move to the next hole
            m_currentHole++;
            m_score->StartHole(m_currentHole, m_parValues[m_currentHole - 1]);

            // Reset the walls
            golfWalls->ResetWalls(golfBall, golfHole);

            // Reposition the ball and the hole for the next hole
            float newBallX = 100; 
            float newBallY = 100; 
            golfBall->SetPosition(newBallX, newBallY);

            float holeWidth = golfHole->GetSprite()->GetWidth() * golfHole->GetSprite()->GetScale();
            float holeHeight = golfHole->GetSprite()->GetHeight() * golfHole->GetSprite()->GetScale();
            float newHoleX = rand() % (int)(APP_VIRTUAL_WIDTH - holeWidth) + holeWidth / 2.0f;
            float newHoleY = rand() % (int)(APP_VIRTUAL_HEIGHT - holeHeight) + holeHeight / 2.0f;
            golfHole->SetPosition(newHoleX, newHoleY);
            m_score->StartHole(m_currentHole, m_parValues[m_currentHole - 1]); // Start the new hole here

        }
        else {
            // Game over
            isGameOver = true; // Set game over flag
        }
    }

    //------------------------------------------------------------------------
    // Sample Sound.
    //------------------------------------------------------------------------
    if (App::GetController().CheckButton(XINPUT_GAMEPAD_B, true))
    {
        App::PlaySound(".\\TestData\\Test.wav", true);
    }
    if (App::GetController().CheckButton(XINPUT_GAMEPAD_X, true))
    {
        App::StopSound(".\\TestData\\Test.wav");
    }
}

//------------------------------------------------------------------------
// Add your display calls here (DrawLine,Print, DrawSprite.) 
// See App.h 
//------------------------------------------------------------------------
void Render()
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set background color
    for (int y = 0; y < APP_VIRTUAL_HEIGHT; ++y) {
        App::DrawLine(0, y, APP_VIRTUAL_WIDTH, y, 0.0f, 0.3f, 0.0f); // Green
    }

    // Render based on game state
    if (!isGameOver) {
        // Render game elements
        golfWalls->Render();
        golfBall->Render();
        golfHole->Render();
        golfMechanics->Render();
        m_score->Render();
        testSprite->Draw();

        // Display message if player overlaps with the ball
        if (overlapPlayerBall) {
            App::Print(200, 200 - 30, "Press A");
        }
    }
    else {
        // Display game over message
        App::Print(APP_VIRTUAL_WIDTH * 0.5f - 100, APP_VIRTUAL_HEIGHT * 0.5f + 50, "GAME OVER", 1.0f, 0.0f, 0.0f); // Red
        App::Print(APP_VIRTUAL_WIDTH * 0.5f - 150, APP_VIRTUAL_HEIGHT * 0.5f + 20, "Press Start to restart", 1.0f, 1.0f, 1.0f); // White
        App::Print(APP_VIRTUAL_WIDTH * 0.5f - 100, APP_VIRTUAL_HEIGHT * 0.5f - 10, "Press Back to exit", 1.0f, 1.0f, 1.0f); // White

        // Display final score
        m_score->Render();
    }
    if (isGameOver) {
        if (App::GetController().CheckButton(XINPUT_GAMEPAD_START, true)) {
            // Restart the game
            isGameOver = false;
            m_currentHole = 1;
            m_score->StartGame();
            m_score->StartHole(m_currentHole, m_parValues[m_currentHole - 1]);
            golfWalls->ResetWalls(golfBall, golfHole);
            //golfMechanics->Reset();

            // Reposition the ball and the hole
            float newBallX, newBallY;
            do {
                newBallX = rand() % (int)APP_VIRTUAL_WIDTH;
                newBallY = rand() % (int)APP_VIRTUAL_HEIGHT;
            } while (golfBall->CheckOverlapWithWalls(golfWalls->GetWalls()));
            golfBall->SetPosition(newBallX, newBallY);
            golfBall->ballMoving = false;
            //golfBall->SetSpeed(0);

            float holeWidth = golfHole->GetSprite()->GetWidth() * golfHole->GetSprite()->GetScale();
            float holeHeight = golfHole->GetSprite()->GetHeight() * golfHole->GetSprite()->GetScale();
            float newHoleX, newHoleY;
            do {
                newHoleX = rand() % (int)(APP_VIRTUAL_WIDTH - holeWidth) + holeWidth / 2.0f;
                newHoleY = rand() % (int)(APP_VIRTUAL_HEIGHT - holeHeight) + holeHeight / 2.0f;
            } while (golfHole->CheckOverlapWithWalls(golfWalls->GetWalls()));
            golfHole->SetPosition(newHoleX, newHoleY);
        }

        if (App::GetController().CheckButton(XINPUT_GAMEPAD_BACK, true)) {
            exit(0); // Exit the game
        }
    }

    // Update the display
    glFlush();
    glutSwapBuffers();
}

bool CheckBallInHole() {
    float ballX, ballY;
    golfBall->GetPosition(ballX, ballY);

    float holeX, holeY;
    golfHole->GetPosition(holeX, holeY);

    // 1. Distance Check:
    float dx = ballX - holeX;
    float dy = ballY - holeY;
    float distance = sqrtf(dx * dx + dy * dy);

    // 2. Overlap Check:
    float ballRadius = golfBall->GetRadius();
    float holeRadius = golfHole->GetRadius(); // Use the hole's radius if you have it

    // You can adjust the overlap tolerance
    float overlapTolerance = 0.7f; // e.g., 70% of the ball's radius must be within the hole's radius

    bool isOverlapping = distance < (ballRadius * overlapTolerance + holeRadius);

    // 3. Velocity Check:
    float ballSpeed = golfBall->GetSpeed();
    const float MAX_SPEED_TO_FALL_IN = 50.0f; // Adjust as needed - this is important for realism

    // 4. Combined Condition:
    return isOverlapping && ballSpeed < MAX_SPEED_TO_FALL_IN;
}


//------------------------------------------------------------------------
// Add your shutdown code here. Called when the APP_QUIT_KEY is pressed.
// Just before the app exits.
//------------------------------------------------------------------------
void Shutdown()
{
    //------------------------------------------------------------------------
    // Example Sprite Code....
    delete testSprite;
    delete golfBall;
    delete golfHole;
    delete golfMechanics;
    delete golfWalls;
    delete m_score;
    m_score = nullptr;

    //------------------------------------------------------------------------
}