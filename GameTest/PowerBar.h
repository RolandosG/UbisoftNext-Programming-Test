#ifndef POWERBAR_H
#define POWERBAR_H

#include "app\app.h"

class PowerBar {
public:
    PowerBar();
    ~PowerBar();

    void Init(float x, float y, float width, float height);
    void Update(float deltaTime);
    void Render();

    void SetPower(float power); // To set the power level
    void SetVisible(bool visible); // To show/hide the power bar

private:
    float m_x;
    float m_y;
    float m_width;
    float m_height;
    float m_power;       // Current power level (0.0 to 1.0)
    bool m_isVisible;

    void DrawFilledRectangle(float x, float y, float width, float height, float r, float g, float b);
};

#endif // POWERBAR_H