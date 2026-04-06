#ifndef SNAKE_H
#define SNAKE_H

#include "game.h"
#include <raylib.h>
#include <raymath.h>
#include <deque>
#include <unordered_set>
#include <cmath>

using namespace std;

// Colors
const Color SNAKE_GREEN = {177, 252, 5, 255};
const Color SNAKE_DARK_GREEN = {43, 51, 24, 255};

// Game constants (customize these for Snake)
const int SNAKE_CELL_SIZE = 30;
const int SNAKE_CELL_COUNT_X = 35;
const int SNAKE_CELL_COUNT_Y = 25;
const int SNAKE_OFFSET = 75;

class SnakeFood {
public:
    Vector2 position;

    SnakeFood(std::deque<Vector2>& snakeBody);
    void Draw();
    Vector2 GenerateRandomCell();
    Vector2 GenerateRandomPosition(std::deque<Vector2>& snakeBody);

private:
    bool ElementInDeque(Vector2 element, std::deque<Vector2>& dq);
};

class SnakePlayer {
public:
    std::deque<Vector2> body = {{6, 9}, {5, 9}, {4, 9}};
    Vector2 direction = {1, 0};

    void Draw();
    void Update(Vector2 foodPos);
    void Reset();
};

class SnakeGameImpl : public Game {
public:
    SnakeGameImpl();

    void Init() override;
    void Update() override;
    void Draw() override;
    bool HandleInput() override;
    std::string GetName() const override;
    GameConfig GetConfig() const override;
    Color GetBackgroundColor() const override;
    void Reset() override;

private:
    // Game configuration (auto-calculates window dimensions)
    GameConfig config;
    
    SnakePlayer snakePlayer;
    SnakeFood snakeFood;
    bool running = false;
    bool directionChange = true;
    int score = 0;
    int highScore = 0;
    double lastUpdateTime = 0;

private:
    void CheckFoodCollision();
    void CheckSelfCollision();
    void GameOver();
};

#endif // SNAKE_H