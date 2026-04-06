#ifndef SNAKE_VISION_H
#define SNAKE_VISION_H

#include "game.h"
#include <raylib.h>
#include <raymath.h>
#include <deque>
#include <unordered_set>
#include <cmath>
#include <thread>
#include <atomic>
#include <string>

// Colors
extern const Color SV_GREEN;
extern const Color SV_DARK_GREEN;

// Game constants
extern const int SV_CELL_SIZE;
extern const int SV_CELL_COUNT_X;
extern const int SV_CELL_COUNT_Y;
extern const int SV_OFFSET;

class SVFood {
public:
    Vector2 position;

    SVFood(std::deque<Vector2>& snakeBody);
    void Draw();
    Vector2 GenerateRandomCell();
    Vector2 GenerateRandomPosition(std::deque<Vector2>& snakeBody);

private:
    bool ElementInDeque(Vector2 element, std::deque<Vector2>& dq);
};

class SVPlayer {
public:
    std::deque<Vector2> body = {{6, 9}, {5, 9}, {4, 9}};
    Vector2 direction = {1, 0};

    void Draw();
    void Update(Vector2 foodPos);
    void Reset();
};

class SnakeVisionGameImpl : public Game {
public:
    SnakeVisionGameImpl();
    ~SnakeVisionGameImpl();

    void Init() override;
    void Update() override;
    void Draw() override;
    bool HandleInput() override;
    std::string GetName() const override;
    GameConfig GetConfig() const override;
    Color GetBackgroundColor() const override;
    void Reset() override;

private:
    GameConfig config;
    
    SVPlayer snakePlayer;
    SVFood snakeFood;
    bool running = false;
    bool directionChange = true;
    int score = 0;
    int highScore = 0;
    double lastUpdateTime = 0;
    bool gameStarted = false;

private:
    void CheckFoodCollision();
    void CheckSelfCollision();
    void GameOver();

    // OpenCV integration
    std::thread cvThread;
    std::atomic<bool> cvRunning;
    std::atomic<int> fingerCount;
    void CVLoop();
};

#endif // SNAKE_VISION_H
