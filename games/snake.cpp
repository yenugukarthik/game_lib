#include "snake.h"

SnakeFood::SnakeFood(std::deque<Vector2>& snakeBody) {
    position = GenerateRandomPosition(snakeBody);
}

void SnakeFood::Draw() {
    DrawRectangle(SNAKE_OFFSET + position.x * SNAKE_CELL_SIZE + SNAKE_CELL_SIZE / 3, 
                  SNAKE_OFFSET + position.y * SNAKE_CELL_SIZE,
                  SNAKE_CELL_SIZE / 3, SNAKE_CELL_SIZE / 3, SNAKE_DARK_GREEN);

    DrawRectangle(SNAKE_OFFSET + position.x * SNAKE_CELL_SIZE, 
                  SNAKE_OFFSET + position.y * SNAKE_CELL_SIZE + SNAKE_CELL_SIZE / 3,
                  SNAKE_CELL_SIZE / 3, SNAKE_CELL_SIZE / 3, SNAKE_DARK_GREEN);

    DrawRectangle(SNAKE_OFFSET + position.x * SNAKE_CELL_SIZE + 2 * (SNAKE_CELL_SIZE / 3), 
                  SNAKE_OFFSET + position.y * SNAKE_CELL_SIZE + SNAKE_CELL_SIZE / 3,
                  SNAKE_CELL_SIZE / 3, SNAKE_CELL_SIZE / 3, SNAKE_DARK_GREEN);

    DrawRectangle(SNAKE_OFFSET + position.x * SNAKE_CELL_SIZE + SNAKE_CELL_SIZE / 3, 
                  SNAKE_OFFSET + position.y * SNAKE_CELL_SIZE + 2 * (SNAKE_CELL_SIZE / 3),
                  SNAKE_CELL_SIZE / 3, SNAKE_CELL_SIZE / 3, SNAKE_DARK_GREEN);
}

Vector2 SnakeFood::GenerateRandomCell() {
    return {(float)GetRandomValue(0, SNAKE_CELL_COUNT_X - 1),
            (float)GetRandomValue(0, SNAKE_CELL_COUNT_Y - 1)};
}

Vector2 SnakeFood::GenerateRandomPosition(std::deque<Vector2>& snakeBody) {
    Vector2 position = GenerateRandomCell();
    while (ElementInDeque(position, snakeBody))
        position = GenerateRandomCell();
    return position;
}

bool SnakeFood::ElementInDeque(Vector2 element, std::deque<Vector2>& dq) {
    for (size_t i = 0; i < dq.size(); i++) {
        if (Vector2Equals(dq[i], element)) return true;
    }
    return false;
}

void SnakePlayer::Draw() {
    for (auto& p : body) {
        Rectangle seg = {SNAKE_OFFSET + p.x * SNAKE_CELL_SIZE, SNAKE_OFFSET + p.y * SNAKE_CELL_SIZE,
                         (float)SNAKE_CELL_SIZE, (float)SNAKE_CELL_SIZE};
        DrawRectangleRounded(seg, 0.5, 6, SNAKE_DARK_GREEN);
    }
}

void SnakePlayer::Update(Vector2 foodPos) {
    Vector2 temp = Vector2Add(body[0], direction);
    temp.x = ((int)temp.x + SNAKE_CELL_COUNT_X) % SNAKE_CELL_COUNT_X;
    temp.y = ((int)temp.y + SNAKE_CELL_COUNT_Y) % SNAKE_CELL_COUNT_Y;

    body.push_front(temp);
    if (!Vector2Equals(body[0], foodPos))
        body.pop_back();
}

void SnakePlayer::Reset() {
    body = {{6, 9}, {5, 9}, {4, 9}};
    direction = {1, 0};
}

SnakeGameImpl::SnakeGameImpl() : config(SNAKE_CELL_SIZE, SNAKE_CELL_COUNT_X, SNAKE_CELL_COUNT_Y, SNAKE_OFFSET), 
                                 snakeFood(snakePlayer.body) {}

void SnakeGameImpl::Init() {
    snakePlayer.Reset();
    snakeFood.position = snakeFood.GenerateRandomPosition(snakePlayer.body);
    running = false;
    score = 0;
    highScore = Highscores::Load("snake");
}

void SnakeGameImpl::Update() {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= 0.1) {
        lastUpdateTime = currentTime;
        if (running) {
            snakePlayer.Update(snakeFood.position);
            CheckFoodCollision();
            CheckSelfCollision();
        }
        directionChange = true;
    }
}

void SnakeGameImpl::Draw() {
    snakeFood.Draw();
    snakePlayer.Draw();

    DrawRectangleLinesEx(Rectangle{(float)SNAKE_OFFSET - 5, (float)SNAKE_OFFSET - 5,
                                   (float)SNAKE_CELL_SIZE * SNAKE_CELL_COUNT_X + 10,
                                   (float)SNAKE_CELL_SIZE * SNAKE_CELL_COUNT_Y + 10},
                         5, SNAKE_DARK_GREEN);

    DrawText("Retro Snake", SNAKE_OFFSET - 5, 20, 40, SNAKE_DARK_GREEN);
    DrawText(TextFormat("%i", score), SNAKE_OFFSET - 5,
             SNAKE_OFFSET + SNAKE_CELL_COUNT_Y * SNAKE_CELL_SIZE + 10, 40, SNAKE_DARK_GREEN);
    DrawText(TextFormat("high score %i", highScore),
             (SNAKE_CELL_COUNT_X * SNAKE_CELL_SIZE) / 2 + SNAKE_OFFSET - 5,
             SNAKE_OFFSET + SNAKE_CELL_COUNT_Y * SNAKE_CELL_SIZE + 10, 40, SNAKE_DARK_GREEN);

    if (!running) {
        DrawText("Press any arrow key to start", SNAKE_OFFSET + 5, SNAKE_OFFSET + 15, 40,
                 SNAKE_DARK_GREEN);
    }
}

bool SnakeGameImpl::HandleInput() {
    if (IsKeyPressed(KEY_UP) && snakePlayer.direction.y != 1 && directionChange) {
        snakePlayer.direction = {0, -1};
        running = true;
        directionChange = false;
    }
    if (IsKeyPressed(KEY_DOWN) && snakePlayer.direction.y != -1 && directionChange) {
        snakePlayer.direction = {0, 1};
        running = true;
        directionChange = false;
    }
    if (IsKeyPressed(KEY_RIGHT) && snakePlayer.direction.x != -1 && directionChange) {
        snakePlayer.direction = {1, 0};
        running = true;
        directionChange = false;
    }
    if (IsKeyPressed(KEY_LEFT) && snakePlayer.direction.x != 1 && directionChange) {
        snakePlayer.direction = {-1, 0};
        running = true;
        directionChange = false;
    }

    // Backspace to return to menu
    if (IsKeyPressed(KEY_BACKSPACE)) {
        return false;
    }

    return true;
}

std::string SnakeGameImpl::GetName() const { return "Snake"; }

GameConfig SnakeGameImpl::GetConfig() const { return config; }

Color SnakeGameImpl::GetBackgroundColor() const { return SNAKE_GREEN; }

void SnakeGameImpl::Reset() { Init(); }

void SnakeGameImpl::CheckFoodCollision() {
    if (Vector2Equals(snakePlayer.body[0], snakeFood.position)) {
        snakeFood.position = snakeFood.GenerateRandomPosition(snakePlayer.body);
        score++;
    }
}

void SnakeGameImpl::CheckSelfCollision() {
    std::unordered_set<long long> s;
    for (auto& p : snakePlayer.body) {
        long long key = (long long)p.x * 100000 + (long long)p.y;
        s.insert(key);
    }
    if (s.size() != snakePlayer.body.size()) {
        GameOver();
    }
}

void SnakeGameImpl::GameOver() {
    snakePlayer.Reset();
    snakeFood.position = snakeFood.GenerateRandomPosition(snakePlayer.body);
    running = false;
    if (score > highScore) {
        highScore = score;
        Highscores::Save("snake", highScore);
    }
    score = 0;
}