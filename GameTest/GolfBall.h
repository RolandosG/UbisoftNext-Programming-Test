#ifndef GOLFBALL_H
#define GOLFBALL_H

#include "app\app.h"
#include "GolfWalls.h"
#include <vector>

class GolfBall {
public:
    GolfBall();
    ~GolfBall();

    void Init(const char* spriteFile, int rows, int cols);
    void Render();

    void SetPosition(float x, float y); 

    void GetPosition(float& x, float& y); 
    CSimpleSprite* GetSprite() const;
     void MoveBall(float power, float angle);
    void UpdatePosition(float deltaTime, const std::vector<Wall>& walls);

     bool ballMoving = false;

     bool IsBallMoving() const { return ballMoving; }

     bool CheckCollisionWithWall(const Wall& wall);
     bool CheckOverlapWithWalls(const std::vector<Wall>& walls) const;

     float GetSpeed() const { return m_speed; }
 
     float GetX() const { return m_x; }
     float GetY() const { return m_y; }
     float GetRadius() const { return m_radius; }

private:
    CSimpleSprite* m_sprite;
    float           m_x;
    float           m_y;
    float m_targetX; 
    float m_targetY;
    float m_speed; 
    float m_initialSpeed;
    float m_initialPower;
    static constexpr float MAX_POWER = 100.0f;

    float m_startX; 
    float m_startY;
    float m_radius = 5.0f; 

    void HandleBounce(const Wall& wall, float deltaTime);

};

#endif // GOLFBALL_H