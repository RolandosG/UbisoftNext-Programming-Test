// GolfBall.cpp
#include "stdafx.h" // Remember to include this first!
#include "GolfBall.h"
#include <GolfWalls.h>

GolfBall::GolfBall() : m_sprite(nullptr), m_x(0.0f), m_y(0.0f) // Initialize m_x and m_y
{
}

GolfBall::~GolfBall() {
    delete m_sprite; // Important to avoid memory leaks
}

void GolfBall::Init(const char* spriteFile, int rows, int cols) {
    m_sprite = App::CreateSprite(spriteFile, rows, cols);

    // Check if sprite creation was successful (add error handling)
    if (m_sprite == nullptr) {
        App::Print(10, 10, "ERROR: Could not create golf ball sprite!");
    }
    else {
        m_sprite->SetScale(0.1f); // Set initial scale (if needed)
        m_sprite->SetPosition(APP_VIRTUAL_WIDTH * 0.5f, APP_VIRTUAL_HEIGHT * 0.5f); // Center the ball initially
        m_x = APP_VIRTUAL_WIDTH * 0.5f; // Update m_x
        m_y = APP_VIRTUAL_HEIGHT * 0.5f; // Update m_y
    }
}

void GolfBall::Render() {
    if (m_sprite) {
        m_sprite->Draw();
    }
   /* if (ballMoving)
    {
		App::Print(100, 100, "Ball is MOVING");
	}
	if (!ballMoving)
    {
		App::Print(100, 100, "Ball is NOT MOVING");
    }*/
}

// Add these functions back in:
void GolfBall::SetPosition(float x, float y) {
    m_x = x;
    m_y = y;
    if (m_sprite) {
        m_sprite->SetPosition(m_x, m_y);
    }
}

void GolfBall::GetPosition(float& x, float& y) {
    x = m_x;
    y = m_y;
}

CSimpleSprite* GolfBall::GetSprite() const {
    return m_sprite;
}

void GolfBall::MoveBall(float power, float angle) {
    ballMoving = true;

    // Adjust the power using a non-linear factor
    float powerFactor = power / MAX_POWER; // Normalize power to 0-1 range

    // You can adjust the exponent (here 1.5) to control the curve
    float adjustedPower = pow(powerFactor, 1.5f) * MAX_POWER * 5.0f; // Increase the scaling factor

    // Ensure a minimum distance of movement even at low power
    const float MIN_DISTANCE = 20.0f; // Increased minimum distance
    if (adjustedPower < MIN_DISTANCE) {
        adjustedPower = MIN_DISTANCE;
    }

    // Calculate the new position of the ball
    float newX = m_x + adjustedPower * cos(angle);
    float newY = m_y + adjustedPower * sin(angle);

    // Update the ball's position
    m_targetX = newX;
    m_targetY = newY;

    m_speed = adjustedPower; // Initial speed after the shot
}

void GolfBall::UpdatePosition(float deltaTime, const std::vector<Wall>& walls) {
    // Calculate the direction vector towards the target
    float dirX = m_targetX - m_x;
    float dirY = m_targetY - m_y;

    // Calculate the distance to the target
    float distance = sqrtf(dirX * dirX + dirY * dirY);

    if (distance > 0.5f) { // Using a small threshold

        // Normalize the direction vector
        dirX /= distance;
        dirY /= distance;

        // Calculate the movement amount for this frame
        float moveDistance = m_speed * deltaTime;

        // Check if we're close enough to the target to stop
        if (moveDistance > distance) {
            m_x = m_targetX;
            m_y = m_targetY;
            m_speed = 0.0f;
            ballMoving = false;
        }
        else {
            // Move towards the target
            m_x += dirX * moveDistance;
            m_y += dirY * moveDistance;

            // Apply friction (decelerate) AFTER moving
            const float FRICTION_DECEL = 150.0f; // Deceleration due to friction (units/s^2) - Adjust as needed
            float frictionAmount = FRICTION_DECEL * deltaTime;

            // Reduce speed, but don't allow it to go below zero
            if (m_speed > frictionAmount) {
                m_speed -= frictionAmount;
            }
            else {
                m_speed = 0.0f;
                ballMoving = false;
            }
        }

        // --- Wall Collision ---
        for (const auto& wall : walls) {
            if (CheckCollisionWithWall(wall)) {
                HandleBounce(wall, deltaTime);
            }
        }

        // Update the sprite's position
        if (m_sprite) {
            m_sprite->SetPosition(m_x, m_y);
        }
    }
    else {
        m_speed = 0.0f;
        ballMoving = false;
    }
}

bool GolfBall::CheckCollisionWithWall(const Wall& wall) {
    // Simple AABB collision detection (for now)
    float ballLeft = m_x - m_radius;
    float ballRight = m_x + m_radius;
    float ballTop = m_y - m_radius;
    float ballBottom = m_y + m_radius;

    return (ballRight > wall.x && ballLeft < wall.x + wall.width &&
        ballBottom > wall.y && ballTop < wall.y + wall.height);
}

void GolfBall::HandleBounce(const Wall& wall, float deltaTime) {
    // 1. Calculate the reflection angle:
    // (This part remains the same as before)
    float normalX = 0.0f;
    float normalY = 0.0f;

    bool hitFromLeft = (m_x < wall.x);
    bool hitFromRight = (m_x > wall.x + wall.width);
    bool hitFromTop = (m_y < wall.y);
    bool hitFromBottom = (m_y > wall.y + wall.height);

    if (hitFromLeft) {
        normalX = -1.0f;
    }
    else if (hitFromRight) {
        normalX = 1.0f;
    }
    else if (hitFromTop) {
        normalY = -1.0f;
    }
    else if (hitFromBottom) {
        normalY = 1.0f;
    }

    // 2. Calculate the new direction using the reflection formula:
    float dirX = m_targetX - m_x;
    float dirY = m_targetY - m_y;
    float distanceToTarget = sqrtf(dirX * dirX + dirY * dirY);

    if (distanceToTarget > 0.0f) {
        dirX /= distanceToTarget; // Normalize
        dirY /= distanceToTarget;

        float dotProduct = dirX * normalX + dirY * normalY;
        float reflectedDirX = dirX - 2.0f * dotProduct * normalX;
        float reflectedDirY = dirY - 2.0f * dotProduct * normalY;

        // 3. Move the ball back to the collision point:
        
        // Determine the overlap amount
        float overlapX = 0.0f;
        float overlapY = 0.0f;

        if (hitFromLeft) {
            overlapX = (m_x + m_radius) - wall.x;
        }
        else if (hitFromRight) {
            overlapX = (m_x - m_radius) - (wall.x + wall.width);
        }
        else if (hitFromTop) {
            overlapY = (m_y + m_radius) - wall.y;
        }
        else if (hitFromBottom) {
            overlapY = (m_y - m_radius) - (wall.y + wall.height);
        }

        // Move the ball back to the collision point (negate overlap)
        m_x -= overlapX;
        m_y -= overlapY;

        // 4. Set the new target position based on the reflected direction:

        // Set the new target position based on the reflected direction and distance to target
        m_targetX = m_x + reflectedDirX * distanceToTarget;
        m_targetY = m_y + reflectedDirY * distanceToTarget;

        // 5. Apply a small offset in the reflected direction
        m_x += reflectedDirX * 1.0f; 
        m_y += reflectedDirY * 1.0f;
    }

    // 6. Reduce the ball's speed (energy loss)
    m_speed *= 0.95f; // Adjust the factor (0.95 = 5% speed reduction)
}

bool GolfBall::CheckOverlapWithWalls(const std::vector<Wall>& walls) const {
    for (const auto& wall : walls) {
        // Check if the bounding box of the golf ball overlaps with the wall
        float ballLeft = m_x - m_radius;
        float ballRight = m_x + m_radius;
        float ballTop = m_y - m_radius;
        float ballBottom = m_y + m_radius;

        if (ballRight >= wall.x && ballLeft <= wall.x + wall.width &&
            ballBottom >= wall.y && ballTop <= wall.y + wall.height) {
            return true; // Overlap detected
        }
    }
    return false; // No overlap
}