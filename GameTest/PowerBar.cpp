#include "stdafx.h"
#include "PowerBar.h"
#include "app\app.h"
#include <iostream>

PowerBar::PowerBar() :
    m_x(0.0f),
    m_y(0.0f),
    m_width(0.0f),
    m_height(0.0f),
    m_power(0.0f),
    m_isVisible(false)
{

}

PowerBar::~PowerBar() {
}

void PowerBar::Init(float x, float y, float width, float height) {
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
}

void PowerBar::Update(float deltaTime) {
    // 
}

void PowerBar::Render() {

    if (m_isVisible) {

        App::Print(200, 200, "powerbar render is visible");

        // Draw the power bar background (empty part)
        DrawFilledRectangle(m_x, m_y, m_width, m_height, 0.5f, 0.5f, 0.5f); // Grey
       
        // Draw the filled part of the power bar
        float filledHeight = m_height * m_power;
        DrawFilledRectangle(m_x, m_y + m_height - filledHeight, m_width, filledHeight, 1.0f, 0.0f, 0.0f); // Red

    }
}

void PowerBar::SetPower(float power) {
    m_power = power;

    // Clamp power between 0.0 and 1.0
    if (m_power < 0.0f) {
        m_power = 0.0f;
    }
    if (m_power > 1.0f) {
        m_power = 1.0f;
    }
}

void PowerBar::SetVisible(bool visible) {
    m_isVisible = visible;
}

void PowerBar::DrawFilledRectangle(float x, float y, float width, float height, float r, float g, float b) {
   
    for (int i = 0; i < height; ++i) {
        App::DrawLine(x, y + i, x + width, y + i, r, g, b);
    }
}