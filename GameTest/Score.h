#ifndef SCORE_H
#define SCORE_H

#include "app\app.h"
#include <chrono>

class Score {
public:
    Score();
    ~Score();

    void StartGame();
    void StartHole(int holeNumber, int par = 0); 
    void AddStroke();
    void EndHole();

    int GetCurrentHole() const;
    int GetTotalStrokes() const;
    int GetStrokesThisHole() const;
    float GetTimeElapsed() const; 

    void Update(float deltaTime); // Update timer, etc.
    void Render(); // Display the score, strokes, and time

private:
    int m_currentHole;
    int m_totalStrokes;
    int m_strokesThisHole;
    int m_parForCurrentHole; 

    // Use std::chrono for time tracking
    std::chrono::high_resolution_clock::time_point m_startTime;
    float m_timeElapsed;

    bool m_gameStarted;   // Flag to indicate if the game is running
};

#endif // SCORE_H