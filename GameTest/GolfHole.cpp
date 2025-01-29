#include "stdafx.h"
#include "GolfHole.h"

GolfHole::GolfHole() : m_sprite(nullptr), m_x(0.0f), m_y(0.0f) {}

GolfHole::~GolfHole() {
    delete m_sprite;
}

void GolfHole::Init(float x, float y) {
    m_sprite = App::CreateSprite(".\\TestData\\golf_ball.bmp", 1, 1);
    if (m_sprite) {
        m_sprite->SetScale(0.10f);
        m_sprite->SetColor(0.0f, 0.0f, 0.0f);
        m_sprite->SetPosition(x, y);
        m_x = x;
        m_y = y;
    }
}

void GolfHole::Render() {
    if (m_sprite) {
        m_sprite->Draw();
    }
}

CSimpleSprite* GolfHole::GetSprite() const {
    return m_sprite; // Return the sprite pointer
}

bool GolfHole::CheckOverlapWithWalls(const std::vector<Wall>& walls) const {
    if (!m_sprite) return false; // No sprite, no overlap

    // Get the sprite's bounding box
    float holeLeft = m_x - m_sprite->GetWidth() * m_sprite->GetScale() * 0.5f;
    float holeRight = m_x + m_sprite->GetWidth() * m_sprite->GetScale() * 0.5f;
    float holeTop = m_y - m_sprite->GetHeight() * m_sprite->GetScale() * 0.5f;
    float holeBottom = m_y + m_sprite->GetHeight() * m_sprite->GetScale() * 0.5f;

    for (const auto& wall : walls) {
        // Check for overlap
        if (holeRight > wall.x && holeLeft < wall.x + wall.width &&
            holeBottom > wall.y && holeTop < wall.y + wall.height) {
            return true;
        }
    }

    return false;
}

void GolfHole::SetPosition(float x, float y) {
    m_x = x;
    m_y = y;
    if (m_sprite) {
        m_sprite->SetPosition(m_x, m_y);
    }
}