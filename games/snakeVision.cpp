#include "snakeVision.h"
#include <iostream>
#include <vector>
#include "game.h"

// Colors
const Color SV_GREEN = {177, 252, 5, 255};
const Color SV_DARK_GREEN = {43, 51, 24, 255};

// Game constants
const int SV_CELL_SIZE = 30;
const int SV_CELL_COUNT_X = 35;
const int SV_CELL_COUNT_Y = 25;
const int SV_OFFSET = 75;

using namespace std;

// --- SVFood Implementation ---
SVFood::SVFood(std::deque<Vector2>& snakeBody) {
    position = GenerateRandomPosition(snakeBody);
}

void SVFood::Draw() {
    DrawRectangle(SV_OFFSET + position.x * SV_CELL_SIZE + SV_CELL_SIZE / 3, 
                  SV_OFFSET + position.y * SV_CELL_SIZE,
                  SV_CELL_SIZE / 3, SV_CELL_SIZE / 3, SV_DARK_GREEN);
    DrawRectangle(SV_OFFSET + position.x * SV_CELL_SIZE, 
                  SV_OFFSET + position.y * SV_CELL_SIZE + SV_CELL_SIZE / 3,
                  SV_CELL_SIZE / 3, SV_CELL_SIZE / 3, SV_DARK_GREEN);
    DrawRectangle(SV_OFFSET + position.x * SV_CELL_SIZE + 2 * (SV_CELL_SIZE / 3), 
                  SV_OFFSET + position.y * SV_CELL_SIZE + SV_CELL_SIZE / 3,
                  SV_CELL_SIZE / 3, SV_CELL_SIZE / 3, SV_DARK_GREEN);
    DrawRectangle(SV_OFFSET + position.x * SV_CELL_SIZE + SV_CELL_SIZE / 3, 
                  SV_OFFSET + position.y * SV_CELL_SIZE + 2 * (SV_CELL_SIZE / 3),
                  SV_CELL_SIZE / 3, SV_CELL_SIZE / 3, SV_DARK_GREEN);
}

Vector2 SVFood::GenerateRandomCell() {
    return {(float)GetRandomValue(0, SV_CELL_COUNT_X - 1),
            (float)GetRandomValue(0, SV_CELL_COUNT_Y - 1)};
}

Vector2 SVFood::GenerateRandomPosition(std::deque<Vector2>& snakeBody) {
    Vector2 pos = GenerateRandomCell();
    while (ElementInDeque(pos, snakeBody))
        pos = GenerateRandomCell();
    return pos;
}

bool SVFood::ElementInDeque(Vector2 element, std::deque<Vector2>& dq) {
    for (size_t i = 0; i < dq.size(); i++) {
        if (Vector2Equals(dq[i], element)) return true;
    }
    return false;
}

// --- SVPlayer Implementation ---
void SVPlayer::Draw() {
    for (auto& p : body) {
        Rectangle seg = {SV_OFFSET + p.x * SV_CELL_SIZE, SV_OFFSET + p.y * SV_CELL_SIZE,
                         (float)SV_CELL_SIZE, (float)SV_CELL_SIZE};
        DrawRectangleRounded(seg, 0.5, 6, SV_DARK_GREEN);
    }
}

void SVPlayer::Update(Vector2 foodPos) {
    Vector2 temp = Vector2Add(body[0], direction);
    temp.x = ((int)temp.x + SV_CELL_COUNT_X) % SV_CELL_COUNT_X;
    temp.y = ((int)temp.y + SV_CELL_COUNT_Y) % SV_CELL_COUNT_Y;

    body.push_front(temp);
    if (!Vector2Equals(body[0], foodPos))
        body.pop_back();
}

void SVPlayer::Reset() {
    body = {{6, 9}, {5, 9}, {4, 9}};
    direction = {1, 0};
}

// --- OpenCV Finger Counting Implementation ---
void SnakeVisionGameImpl::CVLoop() {
    // Make sure no old stop_signal file exists
    remove("stop_signal");

    // Spawn python script
    FILE* pipe = _popen("python exp.py", "r");
    if (!pipe) {
        std::cerr << "SnakeVision: Could not launch python exp.py!" << std::endl;
        return;
    }

    char buffer[128];
    while (cvRunning) {
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            try {
                int count = std::stoi(buffer);
                fingerCount = count;
            } catch (...) {
                // Ignore conversion errors
            }
        }
    }
    
    _pclose(pipe);
}

// --- SnakeVisionGameImpl Implementation ---
SnakeVisionGameImpl::SnakeVisionGameImpl() : config(SV_CELL_SIZE, SV_CELL_COUNT_X, SV_CELL_COUNT_Y, SV_OFFSET), 
                                 snakeFood(snakePlayer.body) {
    cvRunning = false;
    fingerCount = 0;
}

SnakeVisionGameImpl::~SnakeVisionGameImpl() {
    cvRunning = false;
    
    FILE* f = fopen("stop_signal", "w");
    if (f) {
        fprintf(f, "stop");
        fclose(f);
    }

    if (cvThread.joinable()) {
        cvThread.join();
    }
    remove("stop_signal");
}

void SnakeVisionGameImpl::Init() {
    snakePlayer.Reset();
    snakeFood.position = snakeFood.GenerateRandomPosition(snakePlayer.body);
    running = false;
    gameStarted = false;
    score = 0;
    highScore = Highscores::Load("snakevision");

    if (!cvRunning) {
        cvRunning = true;
        cvThread = std::thread(&SnakeVisionGameImpl::CVLoop, this);
    }
}

void SnakeVisionGameImpl::Update() {
    double currentTime = GetTime();
    
    // Slow snake rate! 0.5s per step
    if (currentTime - lastUpdateTime >= 0.5) {
        lastUpdateTime = currentTime;
        
        if (running && gameStarted) {
            
            // Read OpenCV finger count
            int fc = fingerCount.load();
            
            // Update direction based on fingers
            if (fc == 1 && snakePlayer.direction.x != 1 && directionChange) {
                snakePlayer.direction = {-1, 0}; // Left
                directionChange = false;
            } else if (fc == 2 && snakePlayer.direction.x != -1 && directionChange) {
                snakePlayer.direction = {1, 0}; // Right
                directionChange = false;
            } else if (fc == 3 && snakePlayer.direction.y != 1 && directionChange) {
                snakePlayer.direction = {0, -1}; // Up
                directionChange = false;
            } else if (fc == 4 && snakePlayer.direction.y != -1 && directionChange) {
                snakePlayer.direction = {0, 1}; // Down
                directionChange = false;
            }

            snakePlayer.Update(snakeFood.position);
            CheckFoodCollision();
            CheckSelfCollision();
        }
        directionChange = true;
    }
}

void SnakeVisionGameImpl::Draw() {
    snakeFood.Draw();
    snakePlayer.Draw();

    DrawRectangleLinesEx(Rectangle{(float)SV_OFFSET - 5, (float)SV_OFFSET - 5,
                                   (float)SV_CELL_SIZE * SV_CELL_COUNT_X + 10,
                                   (float)SV_CELL_SIZE * SV_CELL_COUNT_Y + 10},
                         5, SV_DARK_GREEN);

    DrawText("SnakeVision", SV_OFFSET - 5, 20, 40, SV_DARK_GREEN);
    DrawText(TextFormat("%i", score), SV_OFFSET - 5,
             SV_OFFSET + SV_CELL_COUNT_Y * SV_CELL_SIZE + 10, 40, SV_DARK_GREEN);
    DrawText(TextFormat("High Score %i", highScore),
             (SV_CELL_COUNT_X * SV_CELL_SIZE) / 2 + SV_OFFSET - 5,
             SV_OFFSET + SV_CELL_COUNT_Y * SV_CELL_SIZE + 10, 40, SV_DARK_GREEN);
             
    // Show current finger count for debugging
    int fh = fingerCount.load();
    DrawText(TextFormat("Detected Fingers: %i", fh), 
             SV_OFFSET + SV_CELL_COUNT_X * SV_CELL_SIZE - 350, 
             20, 30, RED);

    if (!gameStarted) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 180});
        DrawText("Snake Vision", GetScreenWidth()/2 - MeasureText("Snake Vision", 60)/2, GetScreenHeight()/2 - 100, 60, GREEN);
        DrawText("1 Finger: LEFT", GetScreenWidth()/2 - 100, GetScreenHeight()/2, 30, WHITE);
        DrawText("2 Fingers: RIGHT", GetScreenWidth()/2 - 100, GetScreenHeight()/2 + 40, 30, WHITE);
        DrawText("3 Fingers: UP", GetScreenWidth()/2 - 100, GetScreenHeight()/2 + 80, 30, WHITE);
        DrawText("4+ Fingers: DOWN", GetScreenWidth()/2 - 100, GetScreenHeight()/2 + 120, 30, WHITE);
        DrawText("Press ENTER to start", GetScreenWidth()/2 - MeasureText("Press ENTER to start", 30)/2, GetScreenHeight()/2 + 180, 30, GREEN);
    } else if (!running) {
        DrawText("GAME OVER - Press ENTER to restart", GetScreenWidth()/2 - MeasureText("GAME OVER - Press ENTER to restart", 40)/2, GetScreenHeight()/2, 40, RED);
    }
}

bool SnakeVisionGameImpl::HandleInput() {
    // Backspace to return to menu
    if (IsKeyPressed(KEY_BACKSPACE)) {
        Reset();
        return false;
    }
    
    if (!gameStarted) {
        if (IsKeyPressed(KEY_ENTER)) {
            gameStarted = true;
            running = true;
        }
        return true;
    }
    
    if (!running) {
        if (IsKeyPressed(KEY_ENTER)) {
            snakePlayer.Reset();
            snakeFood.position = snakeFood.GenerateRandomPosition(snakePlayer.body);
            score = 0;
            running = true;
        }
    }
    return true;
}

std::string SnakeVisionGameImpl::GetName() const { return "Snake Vision"; }
GameConfig SnakeVisionGameImpl::GetConfig() const { return config; }
Color SnakeVisionGameImpl::GetBackgroundColor() const { return SV_GREEN; }

void SnakeVisionGameImpl::Reset() { 
    cvRunning = false;
    
    FILE* f = fopen("stop_signal", "w");
    if (f) {
        fprintf(f, "stop");
        fclose(f);
    }

    if (cvThread.joinable()) {
        cvThread.join();
    }
    remove("stop_signal");
    
    if (score > highScore) {
        highScore = score;
        Highscores::Save("snakevision", highScore);
    }
}

void SnakeVisionGameImpl::CheckFoodCollision() {
    if (Vector2Equals(snakePlayer.body[0], snakeFood.position)) {
        snakeFood.position = snakeFood.GenerateRandomPosition(snakePlayer.body);
        score++;
    }
}

void SnakeVisionGameImpl::CheckSelfCollision() {
    std::unordered_set<long long> s;
    for (auto& p : snakePlayer.body) {
        long long key = (long long)p.x * 100000 + (long long)p.y;
        s.insert(key);
    }
    if (s.size() != snakePlayer.body.size()) {
        GameOver();
    }
}

void SnakeVisionGameImpl::GameOver() {
    running = false;
    if (score > highScore) {
        highScore = score;
        Highscores::Save("snakevision", highScore);
    }
}
