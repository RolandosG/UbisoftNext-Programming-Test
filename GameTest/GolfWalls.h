#ifndef GOLFWALLS_H
#define GOLFWALLS_H

#include <vector>
#include "app\app.h"

// Forward-declare GolfBall and GolfHole classes
class GolfBall;
class GolfHole;

struct Wall {
    float x;
    float y;
    float width;
    float height;

    Wall(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}
};

class GolfWalls {
public:
    GolfWalls();
    ~GolfWalls();

    void Init();
    void Render();
    const std::vector<Wall>& GetWalls() const;
    const std::vector<Wall>& GetBorderWalls() const; 
    void GenerateRandomWalls(int numWalls, float minLength, float maxLength, float thickness, const GolfBall* golfBall, const GolfHole* golfHole);
    void ClearWalls();
    void ResetWalls(const GolfBall* golfBall, const GolfHole* golfHole);

private:
    std::vector<Wall> m_walls;
    std::vector<Wall> m_borderWalls; 

    bool DoesWallOverlap(const Wall& newWall, const GolfBall* golfBall) const;
    bool DoesWallOverlap(const Wall& newWall, const GolfHole* golfHole) const;
};

#endif // GOLFWALLS_H