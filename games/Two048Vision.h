#ifndef TWO048_VISION_H
#define TWO048_VISION_H

#include "game.h"
#include "Two048.h" // We will reuse the Two048Grid class
#include <thread>
#include <atomic>
#include <string>

class Two048VisionGameImpl : public Game {
private:
    Two048Grid grid;
    int score     = 0;
    int highScore = 0;
    bool gameOver = false;
    bool won      = false;
    bool started  = false;

    // OpenCV integration
    std::thread cvThread;
    std::atomic<bool> cvRunning;
    std::atomic<int> fingerCount;
    int lastFingerCount = 0; // Keep track of the last finger count to only move on transitions
    void CVLoop();

public:
    Two048VisionGameImpl();
    ~Two048VisionGameImpl();

    void Init()          override;
    void Update()        override;
    void Draw()          override;
    bool HandleInput()   override;
    std::string GetName()            const override;
    GameConfig  GetConfig()          const override;
    Color       GetBackgroundColor() const override;
    void Reset()         override;
};

#endif // TWO048_VISION_H
