#include "stdafx.h"
#include "Score.h"
#include "app\app.h"

Score::Score() :
    m_currentHole(0),
    m_totalStrokes(0),
    m_strokesThisHole(0),
    m_parForCurrentHole(0),
    m_timeElapsed(0.0f),
    m_gameStarted(false)
{
}

Score::~Score() {
}

void Score::StartGame() {
    m_currentHole = 1; // Start at hole 1
    m_totalStrokes = 0;
    m_strokesThisHole = 0;
    m_parForCurrentHole = 0; 
    m_startTime = std::chrono::high_resolution_clock::now(); // Get the current time point
    m_timeElapsed = 0.0f;
    m_gameStarted = true;
}

void Score::StartHole(int holeNumber, int par) {
    m_currentHole = holeNumber;
    m_strokesThisHole = 0;
    m_parForCurrentHole = par; // Set par for the hole 
    m_gameStarted = true;
}

void Score::AddStroke() {
    if (m_gameStarted) {
        m_strokesThisHole++;
        m_totalStrokes++; 
    }
}

void Score::EndHole() {
  
    m_gameStarted = false; // You might want to stop the timer/increment score when the hole is finished
}

int Score::GetCurrentHole() const {
    return m_currentHole;
}

int Score::GetTotalStrokes() const {
    return m_totalStrokes;
}

int Score::GetStrokesThisHole() const {
    return m_strokesThisHole;
}

float Score::GetTimeElapsed() const {
    if (m_gameStarted) {
        // Calculate elapsed time using std::chrono
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_startTime);
        return elapsed.count() / 1000.0f; // Convert milliseconds to seconds
    }
    else {
        return m_timeElapsed;
    }
}

void Score::Update(float deltaTime) {
    if (m_gameStarted) {
        m_timeElapsed = GetTimeElapsed(); // Update elapsed time
    }
}

void Score::Render() {
    // Display score, strokes, and time using App::Print()
    App::PrintTwo(870, 730, "Hole: ", m_currentHole);
    App::PrintTwo(870, 710, "Strokes: ", m_strokesThisHole);
    App::PrintTwo(870, 690, "Total Strokes: ", m_totalStrokes);

    // Optional: Display par and time
    if (m_parForCurrentHole > 0) {
        App::PrintTwo(870, 670, "Par: ", m_parForCurrentHole);
    }
    App::PrintTwo(870, 750 , "Time: ", m_timeElapsed);
}