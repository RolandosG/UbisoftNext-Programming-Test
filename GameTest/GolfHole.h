#pragma once
#ifndef GOLF_HOLE_H 
#define GOLF_HOLE_H

#include "app/app.h"
#include "GolfWalls.h"
#include <vector>

class GolfHole {
public:
    GolfHole();
    ~GolfHole();

    void Init(float x, float y);
    void Render();
    CSimpleSprite* GetSprite() const;

    float GetX() const { return m_x; }
    float GetY() const { return m_y; }

    bool CheckOverlapWithWalls(const std::vector<Wall>& walls) const;

    void GetPosition(float& x, float& y) const { x = m_x; y = m_y; }
    float GetRadius() const { return m_radius; } 
    void SetPosition(float x, float y);

private:
    CSimpleSprite* m_sprite;
    float m_x;
    float m_y;
    float m_radius = 20.0f;
};

#endif