#include "stdafx.h"
#include "GolfWalls.h"
#include "GolfBall.h"
#include "GolfHole.h"
#include <algorithm>
#include <random>

GolfWalls::GolfWalls() {
}

GolfWalls::~GolfWalls() {
}

void GolfWalls::Init() {
    // Wall thickness (adjust as needed)
    const float WALL_THICKNESS = 15.0f;

    // Create the border walls
    m_walls.push_back(Wall(0.0f, 0.0f, APP_VIRTUAL_WIDTH, WALL_THICKNESS));
    m_walls.push_back(Wall(0.0f, APP_VIRTUAL_HEIGHT - WALL_THICKNESS, APP_VIRTUAL_WIDTH, WALL_THICKNESS));
    m_walls.push_back(Wall(0.0f, 0.0f, WALL_THICKNESS, APP_VIRTUAL_HEIGHT));
    m_walls.push_back(Wall(APP_VIRTUAL_WIDTH - WALL_THICKNESS, 0.0f, WALL_THICKNESS, APP_VIRTUAL_HEIGHT));
}

void GolfWalls::Render() {
    for (const auto& wall : m_walls) {
        for (int i = 0; i < wall.height; ++i) {
            App::DrawLine(wall.x, wall.y + i, wall.x + wall.width, wall.y + i, 1.0f, 0.0f, 0.0f); // Red
        }
    }
}

const std::vector<Wall>& GolfWalls::GetWalls() const {
    return m_walls;
}

void GolfWalls::GenerateRandomWalls(int numWalls, float minLength, float maxLength, float thickness, const GolfBall* golfBall, const GolfHole* golfHole) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distX(0.0f, APP_VIRTUAL_WIDTH);
    std::uniform_real_distribution<> distY(0.0f, APP_VIRTUAL_HEIGHT);
    std::uniform_real_distribution<> distLength(minLength, maxLength);

    for (int i = 0; i < numWalls; ++i) {
        float x, y, length;
        bool isHorizontal;

        do {
            // Generate random position and length
            x = distX(gen);
            y = distY(gen);
            length = distLength(gen);
            isHorizontal = (rand() % 2 == 0);

            // Create the wall
            Wall newWall = isHorizontal ? Wall(x, y, length, thickness) : Wall(x, y, thickness, length);

            // Check for overlap
            if (!DoesWallOverlap(newWall, golfBall) && !DoesWallOverlap(newWall, golfHole)) {
                m_walls.push_back(newWall);
                break;
            }
        } while (true);
    }
}

bool GolfWalls::DoesWallOverlap(const Wall& newWall, const GolfBall* golfBall) const {
    // Check overlap with existing walls
    for (const auto& existingWall : m_walls) {
        if (newWall.x < existingWall.x + existingWall.width &&
            newWall.x + newWall.width > existingWall.x &&
            newWall.y < existingWall.y + existingWall.height &&
            newWall.y + newWall.height > existingWall.y) {
            return true; // Overlap with an existing wall
        }
    }

    // Check overlap with the golf ball
    if (golfBall) {
        float ballLeft = golfBall->GetX() - golfBall->GetRadius();
        float ballRight = golfBall->GetX() + golfBall->GetRadius();
        float ballTop = golfBall->GetY() - golfBall->GetRadius();
        float ballBottom = golfBall->GetY() + golfBall->GetRadius();

        if (ballRight > newWall.x && ballLeft < newWall.x + newWall.width &&
            ballBottom > newWall.y && ballTop < newWall.y + newWall.height) {
            return true; // Overlap with the golf ball
        }
    }

    return false; // No overlap
}

bool GolfWalls::DoesWallOverlap(const Wall& newWall, const GolfHole* golfHole) const {
    // Check overlap with existing walls
    for (const auto& existingWall : m_walls) {
        if (newWall.x < existingWall.x + existingWall.width &&
            newWall.x + newWall.width > existingWall.x &&
            newWall.y < existingWall.y + existingWall.height &&
            newWall.y + newWall.height > existingWall.y) {
            return true; // Overlap with an existing wall
        }
    }

    // Check overlap with the golf hole
    if (golfHole && golfHole->GetSprite()) {
        CSimpleSprite* holeSprite = golfHole->GetSprite();
        float holeLeft = golfHole->GetX() - holeSprite->GetWidth() * holeSprite->GetScale() * 0.5f;
        float holeRight = golfHole->GetX() + holeSprite->GetWidth() * holeSprite->GetScale() * 0.5f;
        float holeTop = golfHole->GetY() - holeSprite->GetHeight() * holeSprite->GetScale() * 0.5f;
        float holeBottom = golfHole->GetY() + holeSprite->GetHeight() * holeSprite->GetScale() * 0.5f;

        if (holeRight > newWall.x && holeLeft < newWall.x + newWall.width &&
            holeBottom > newWall.y && holeTop < newWall.y + newWall.height) {
            return true; // Overlap with the golf hole
        }
    }

    return false; // No overlap
}


void GolfWalls::ClearWalls() {
    m_walls.clear();
}

void GolfWalls::ResetWalls(const GolfBall* golfBall, const GolfHole* golfHole) {
    ClearWalls(); // First, clear the existing walls
    Init();

    GenerateRandomWalls(10, 50.0f, 150.0f, 10.0f, golfBall, golfHole); // Then, generate new walls
}