#include "space_invaders.h"
#include <iostream>
#include <fstream>
#include <string>

// ==================== Global constants ====================
Color spaceInvadersGray   = {29, 29, 27, 255};
Color spaceInvadersYellow = {243, 216, 63, 255};
int   spaceInvadersCellSize = 3;
int   spaceInvadersOffset   = 50;

// ==================== spaceInvadersBlock ====================

spaceInvadersBlock::spaceInvadersBlock(Vector2 position) {
    this->position = position;
}

void spaceInvadersBlock::Draw() {
    DrawRectangle((int)position.x, (int)position.y,
                  spaceInvadersCellSize, spaceInvadersCellSize,
                  spaceInvadersYellow);
}

Rectangle spaceInvadersBlock::getRect() {
    return {position.x, position.y,
            (float)spaceInvadersCellSize, (float)spaceInvadersCellSize};
}

// ==================== spaceInvadersObstacles ====================

vector<vector<int>> spaceInvadersObstacles::grid = {
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1}
};

spaceInvadersObstacles::spaceInvadersObstacles(Vector2 position) {
    this->position = position;
    for (unsigned int row = 0; row < grid.size(); ++row) {
        for (unsigned int col = 0; col < grid[0].size(); ++col) {
            if (grid[row][col] == 1) {
                float px = position.x + col * spaceInvadersCellSize;
                float py = position.y + row * spaceInvadersCellSize;
                blocks.push_back(spaceInvadersBlock({px, py}));
            }
        }
    }
}

void spaceInvadersObstacles::Draw() {
    for (auto& block : blocks) block.Draw();
}

// ==================== siLaser ====================

siLaser::siLaser(Vector2 position, int speed, Color color) {
    this->position = position;
    this->speed    = speed;
    this->color    = color;
    active = true;
}

void siLaser::draw() {
    if (active)
        DrawRectangle((int)position.x, (int)position.y, 4, 15, color);
}

void siLaser::update() {
    position.y += speed;
    if (position.y > GetScreenHeight() - 100 || position.y < 25) {
        active = false;
    }
}

Rectangle siLaser::getRect() {
    return {position.x, position.y, 4, 15};
}

// ==================== siSpaceship ====================

siSpaceship::siSpaceship() {
    position = {10, 10};
}

siSpaceship::~siSpaceship() {
    Unload();
}

void siSpaceship::Load() {
    image = LoadTexture("spaceInvaders/spaceship.png");
    textureLoaded = (image.id != 0);
    position.x = GetScreenWidth()  / 2.0f - image.width  / 2.0f;
    position.y = GetScreenHeight() - image.height - 100.0f;

    laserSound  = LoadSound("spaceInvaders/Sounds/laser.ogg");
    soundLoaded = true;
}

void siSpaceship::Unload() {
    if (textureLoaded) {
        UnloadTexture(image);
        textureLoaded = false;
    }
    if (soundLoaded) {
        UnloadSound(laserSound);
        soundLoaded = false;
    }
}

void siSpaceship::Draw() {
    if (textureLoaded)
        DrawTextureV(image, position, WHITE);
}

void siSpaceship::MoveLeft() {
    position.x -= 7;
    if (position.x < 25) position.x = 25;
}

void siSpaceship::MoveRight() {
    position.x += 7;
    if (position.x > GetScreenWidth() - image.width - 25)
        position.x = GetScreenWidth() - image.width - 25;
}

void siSpaceship::FireLaser() {
    if (GetTime() - lastFireTime >= 0.35) {
        lasers.push_back(siLaser({position.x + image.width / 2.0f - 2, position.y}, -6));
        lastFireTime = GetTime();
        if (soundLoaded) PlaySound(laserSound);
    }
}

void siSpaceship::Reset() {
    position.x = GetScreenWidth()  / 2.0f - image.width  / 2.0f;
    position.y = GetScreenHeight() - image.height - 100.0f;
    lasers.clear();
    lastFireTime = 0.0;
}

Rectangle siSpaceship::getRect() {
    return {position.x, position.y, (float)image.width, (float)image.height};
}

// ==================== siAlien ====================

Texture2D siAlien::alienImages[3] = {};

siAlien::siAlien(int type, Vector2 position) {
    this->type     = type;
    this->position = position;
}

void siAlien::LoadImages() {
    alienImages[0] = LoadTexture("spaceInvaders/alien_1.png");
    alienImages[1] = LoadTexture("spaceInvaders/alien_2.png");
    alienImages[2] = LoadTexture("spaceInvaders/alien_3.png");
}

void siAlien::UnloadImages() {
    for (int i = 0; i < 3; i++) {
        if (alienImages[i].id != 0) {
            UnloadTexture(alienImages[i]);
            alienImages[i] = {};
        }
    }
}

void siAlien::Draw() {
    int idx = type - 1;
    if (idx < 0) idx = 0;
    DrawTextureV(alienImages[idx], position, WHITE);
}

void siAlien::Update(int direction) {
    position.x += direction;
}

Rectangle siAlien::getRect() {
    int idx = type - 1;
    if (idx < 0) idx = 0;
    return {position.x, position.y,
            (float)alienImages[idx].width,
            (float)alienImages[idx].height};
}

// ==================== siMysteryShip ====================

siMysteryShip::siMysteryShip() {}

siMysteryShip::~siMysteryShip() {
    Unload();
}

void siMysteryShip::Load() {
    image = LoadTexture("spaceInvaders/mystery.png");
    textureLoaded = (image.id != 0);
    alive = false;
}

void siMysteryShip::Unload() {
    if (textureLoaded) {
        UnloadTexture(image);
        textureLoaded = false;
    }
}

void siMysteryShip::Spawn() {
    position.y = 90;
    int side = GetRandomValue(0, 1);
    if (side == 0) {
        position.x = 25;
        speed = 3;
    } else {
        position.x = GetScreenWidth() - image.width - 25;
        speed = -3;
    }
    alive = true;
}

void siMysteryShip::Update() {
    if (alive) {
        position.x += speed;
        if (position.x > GetScreenWidth() - image.width - 25 || position.x < 25) {
            alive = false;
        }
    }
}

void siMysteryShip::Draw() {
    if (alive && textureLoaded)
        DrawTextureV(image, position, WHITE);
}

Rectangle siMysteryShip::getRect() {
    if (alive)
        return {position.x, position.y, (float)image.width, (float)image.height};
    return {position.x, position.y, 0, 0};
}

// ==================== spaceInvadersGameImpl ====================

spaceInvadersGameImpl::spaceInvadersGameImpl() {}

spaceInvadersGameImpl::~spaceInvadersGameImpl() {
    siAlien::UnloadImages();
    if (musicLoaded)      { UnloadMusicStream(music); musicLoaded = false; }
    if (soundLoaded)      { UnloadSound(explosionSound); soundLoaded = false; }
    if (hudImgLoaded)     { UnloadTexture(spaceshipHudImg); hudImgLoaded = false; }
}

void spaceInvadersGameImpl::Init() {
    player.Load();
    mysteryShip.Load();

    // Load alien images once
    siAlien::LoadImages();

    // Load audio
    if (!musicLoaded) {
        music = LoadMusicStream("spaceInvaders/Sounds/music.ogg");
        musicLoaded = true;
        PlayMusicStream(music);
    }
    if (!soundLoaded) {
        explosionSound = LoadSound("spaceInvaders/Sounds/explosion.ogg");
        soundLoaded = true;
    }

    // HUD life icon
    spaceshipHudImg = LoadTexture("spaceInvaders/spaceship.png");
    hudImgLoaded    = (spaceshipHudImg.id != 0);

    highscore = LoadHighscore();
    InitGame();
}

void spaceInvadersGameImpl::InitGame() {
    obstacles  = CreateObstacles();
    aliens     = CreateAliens();
    alienLasers.clear();
    player.Reset();

    aliensDirection          = 1;
    timeLastAlienFired       = 0.0f;
    timeLastSpawn            = 0.0f;
    mysteryShipSpawnInterval = (float)GetRandomValue(10, 20);
    gameRunning              = true;
    gameStarted              = false;
    lives                    = 3;
    score                    = 0;
    level                    = 1;
    alienSpeed               = 1;
    alienLaserInterval       = 0.5f;
}

// ---------- Update ----------
void spaceInvadersGameImpl::Update() {
    if (musicLoaded) UpdateMusicStream(music);

    if (!gameStarted || !gameRunning) return;

    // Mystery ship spawn timer
    float now = (float)GetTime();
    if (now - timeLastSpawn > mysteryShipSpawnInterval) {
        mysteryShip.Spawn();
        timeLastSpawn            = now;
        mysteryShipSpawnInterval = (float)GetRandomValue(10, 20);
    }

    // Update player lasers
    for (auto& las : player.lasers) las.update();

    // Move aliens
    MoveAliens();

    // Alien shooting
    AlienShootLaser();

    // Update alien lasers
    for (auto& las : alienLasers) las.update();

    // Mystery ship
    mysteryShip.Update();

    // Collisions
    CheckForCollisions();

    // Cleanup dead lasers
    DeleteInactiveLasers();

    // Wave clear → next level
    if (aliens.empty()) {
        level++;
        alienSpeed         = 1 + (level - 1);          // +1 px/frame every level
        alienLaserInterval = max(0.1f, 0.5f - (level - 1) * 0.04f); // faster shooting
        aliensDirection    = 1;
        aliensDirection   *= alienSpeed;                // propagate speed immediately
        aliens             = CreateAliens();
        obstacles          = CreateObstacles();
        alienLasers.clear();
    }
}

// ---------- Draw ----------
void spaceInvadersGameImpl::Draw() {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Border
    DrawRectangleRoundedLinesEx({10, 10, (float)screenW - 20, (float)screenH - 20},
                                 0.18f, 20, 2, spaceInvadersYellow);

    // Bottom separator line
    DrawLineEx({25, (float)screenH - 50}, {(float)screenW - 25, (float)screenH - 50},
               3, spaceInvadersYellow);

    // Score
    DrawText("SCORE", 50, 15, 24, spaceInvadersYellow);
    string scoreText = FormatScore(score, 5);
    DrawText(scoreText.c_str(), 50, 42, 24, spaceInvadersYellow);

    // Level (center-top)
    string levelText = "LEVEL " + FormatScore(level, 2);
    int levelW = MeasureText(levelText.c_str(), 24);
    DrawText(levelText.c_str(), screenW / 2 - levelW / 2, 28, 24, spaceInvadersYellow);

    // High score
    string hsLabel = "HIGH-SCORE";
    int hsLabelW   = MeasureText(hsLabel.c_str(), 24);
    DrawText(hsLabel.c_str(), screenW - hsLabelW - 50, 15, 24, spaceInvadersYellow);
    string hsText = FormatScore(highscore, 5);
    int    hsW    = MeasureText(hsText.c_str(), 24);
    DrawText(hsText.c_str(), screenW - hsW - 50, 42, 24, spaceInvadersYellow);

    // Game state label (bottom bar)
    if (!gameRunning) {
        DrawText("GAME OVER", screenW / 2 - MeasureText("GAME OVER", 22) / 2,
                 screenH - 40, 22, spaceInvadersYellow);
    } else {
        DrawText("LEVEL 01", screenW - 160, screenH - 40, 22, spaceInvadersYellow);
    }

    // Centered game-over overlay
    if (!gameRunning) {
        // Dim overlay
        DrawRectangle(0, 0, screenW, screenH, {0, 0, 0, 150});

        const char* goText      = "GAME OVER";
        const char* restartText = "Press ENTER to Restart";
        const char* menuText    = "Press BACKSPACE to Return to Menu";

        int goSize      = 60;
        int restartSize = 28;
        int menuSize    = 22;

        int goW      = MeasureText(goText,      goSize);
        int restartW = MeasureText(restartText, restartSize);
        int menuW    = MeasureText(menuText,    menuSize);

        int centerX = screenW / 2;
        int centerY = screenH / 2;

        DrawText(goText,      centerX - goW      / 2, centerY - 70, goSize,      spaceInvadersYellow);
        DrawText(restartText, centerX - restartW / 2, centerY + 10, restartSize, WHITE);
        DrawText(menuText,    centerX - menuW    / 2, centerY + 50, menuSize,    {180, 180, 180, 255});
    }

    // Lives (small spaceship icons)
    if (hudImgLoaded) {
        float lx = 50;
        for (int i = 0; i < lives; i++) {
            DrawTextureV(spaceshipHudImg, {lx, (float)screenH - 38}, WHITE);
            lx += spaceshipHudImg.width + 10.0f;
        }
    }

    // Game objects
    player.Draw();
    for (auto& las : player.lasers) las.draw();

    for (auto& obs : obstacles) obs.Draw();

    for (auto& alien : aliens) alien.Draw();
    for (auto& las  : alienLasers) las.draw();

    mysteryShip.Draw();
}

// ---------- Input ----------
bool spaceInvadersGameImpl::HandleInput() {
    if (IsKeyPressed(KEY_BACKSPACE)) return false;

    // Start screen — wait for ENTER
    if (!gameStarted) {
        if (IsKeyPressed(KEY_ENTER)) gameStarted = true;
        return true;
    }

    if (gameRunning) {
        if      (IsKeyDown(KEY_LEFT))  player.MoveLeft();
        else if (IsKeyDown(KEY_RIGHT)) player.MoveRight();
        if (IsKeyDown(KEY_SPACE)) player.FireLaser();
    } else {
        // Game over — press Enter to restart
        if (IsKeyPressed(KEY_ENTER)) {
            highscore = LoadHighscore();
            InitGame();
        }
    }
    return true;
}

// ---------- Metadata ----------
std::string spaceInvadersGameImpl::GetName() const {
    return "Space Invaders";
}

GameConfig spaceInvadersGameImpl::GetConfig() const {
    // Window: 750 wide, 700 tall — use cellSize=1, offset=0 so formula gives exact pixel counts
    return GameConfig(1, 750, 700, 0);
}

Color spaceInvadersGameImpl::GetBackgroundColor() const {
    return spaceInvadersGray;
}

void spaceInvadersGameImpl::Reset() {
    player.Unload();
    mysteryShip.Unload();
    siAlien::UnloadImages();
    if (musicLoaded)  { StopMusicStream(music); UnloadMusicStream(music); musicLoaded = false; }
    if (soundLoaded)  { UnloadSound(explosionSound); soundLoaded = false; }
    if (hudImgLoaded) { UnloadTexture(spaceshipHudImg); hudImgLoaded = false; }
    aliens.clear();
    alienLasers.clear();
    obstacles.clear();
}

// ==================== Private Helpers ====================

void spaceInvadersGameImpl::DeleteInactiveLasers() {
    for (auto it = player.lasers.begin(); it != player.lasers.end();) {
        if (!it->active) it = player.lasers.erase(it);
        else             ++it;
    }
    for (auto it = alienLasers.begin(); it != alienLasers.end();) {
        if (!it->active) it = alienLasers.erase(it);
        else             ++it;
    }
}

vector<spaceInvadersObstacles> spaceInvadersGameImpl::CreateObstacles() {
    int obstacleWidth  = (int)spaceInvadersObstacles::grid[0].size() * spaceInvadersCellSize;
    int screenW        = GetScreenWidth();
    float gap          = (screenW - 4.0f * obstacleWidth) / 5.0f;

    vector<spaceInvadersObstacles> obs;
    for (int i = 0; i < 4; i++) {
        float offsetX = (i + 1) * gap + i * obstacleWidth;
        obs.push_back(spaceInvadersObstacles({offsetX, (float)(GetScreenHeight() - 200)}));
    }
    return obs;
}

vector<siAlien> spaceInvadersGameImpl::CreateAliens() {
    vector<siAlien> newAliens;
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 11; col++) {
            int alienType;
            if      (row == 0)                 alienType = 3;
            else if (row == 1 || row == 2)     alienType = 2;
            else                               alienType = 1;

            float x = 75 + col * 55;
            float y = 110 + row * 55;
            newAliens.push_back(siAlien(alienType, {x, y}));
        }
    }
    return newAliens;
}

void spaceInvadersGameImpl::MoveAliens() {
    for (auto& alien : aliens) {
        int idx = alien.type - 1;
        if (idx < 0) idx = 0;
        if (alien.position.x + siAlien::alienImages[idx].width > GetScreenWidth() - 25) {
            aliensDirection = -alienSpeed;
            MoveDownAliens(4);
        }
        if (alien.position.x < 25) {
            aliensDirection = alienSpeed;
            MoveDownAliens(4);
        }
        alien.Update(aliensDirection);
    }
}

void spaceInvadersGameImpl::MoveDownAliens(int distance) {
    for (auto& alien : aliens) alien.position.y += distance;
}

void spaceInvadersGameImpl::AlienShootLaser() {
    float currentTime = (float)GetTime();
    if (currentTime - timeLastAlienFired >= alienLaserInterval && !aliens.empty()) {
        int randomIdx = GetRandomValue(0, (int)aliens.size() - 1);
        siAlien& alien = aliens[randomIdx];
        int idx = alien.type - 1;
        if (idx < 0) idx = 0;
        float lx = alien.position.x + siAlien::alienImages[idx].width  / 2.0f - 2;
        float ly = alien.position.y + siAlien::alienImages[idx].height;
        alienLasers.push_back(siLaser({lx, ly}, 6, RED));
        timeLastAlienFired = currentTime;
    }
}

void spaceInvadersGameImpl::CheckForCollisions() {
    // --- Player lasers ---
    for (auto& laser : player.lasers) {
        // vs aliens
        auto alienIt = aliens.begin();
        while (alienIt != aliens.end()) {
            if (CheckCollisionRecs(alienIt->getRect(), laser.getRect())) {
                if (soundLoaded) PlaySound(explosionSound);
                if      (alienIt->type == 1) score += 100;
                else if (alienIt->type == 2) score += 200;
                else if (alienIt->type == 3) score += 300;
                CheckForHighscore();
                alienIt   = aliens.erase(alienIt);
                laser.active = false;
            } else {
                ++alienIt;
            }
        }

        // vs obstacles
        for (auto& obs : obstacles) {
            auto blockIt = obs.blocks.begin();
            while (blockIt != obs.blocks.end()) {
                if (CheckCollisionRecs(blockIt->getRect(), laser.getRect())) {
                    blockIt      = obs.blocks.erase(blockIt);
                    laser.active = false;
                } else {
                    ++blockIt;
                }
            }
        }

        // vs mystery ship
        if (CheckCollisionRecs(mysteryShip.getRect(), laser.getRect())) {
            mysteryShip.alive = false;
            laser.active      = false;
            score += 500;
            CheckForHighscore();
            if (soundLoaded) PlaySound(explosionSound);
        }
    }

    // --- Alien lasers ---
    for (auto& laser : alienLasers) {
        // vs player
        if (CheckCollisionRecs(laser.getRect(), player.getRect())) {
            laser.active = false;
            lives--;
            if (soundLoaded) PlaySound(explosionSound);
            if (lives <= 0) GameOver();
        }

        // vs obstacles
        for (auto& obs : obstacles) {
            auto blockIt = obs.blocks.begin();
            while (blockIt != obs.blocks.end()) {
                if (CheckCollisionRecs(blockIt->getRect(), laser.getRect())) {
                    blockIt      = obs.blocks.erase(blockIt);
                    laser.active = false;
                } else {
                    ++blockIt;
                }
            }
        }
    }

    // --- Aliens vs obstacles ---
    for (auto& alien : aliens) {
        for (auto& obs : obstacles) {
            auto blockIt = obs.blocks.begin();
            while (blockIt != obs.blocks.end()) {
                if (CheckCollisionRecs(blockIt->getRect(), alien.getRect())) {
                    blockIt = obs.blocks.erase(blockIt);
                } else {
                    ++blockIt;
                }
            }
        }

        // Alien reaches player row → game over
        if (CheckCollisionRecs(alien.getRect(), player.getRect())) {
            GameOver();
        }
    }
}

void spaceInvadersGameImpl::GameOver() {
    gameRunning = false;
    CheckForHighscore();
}

void spaceInvadersGameImpl::CheckForHighscore() {
    if (score > highscore) {
        highscore = score;
        SaveHighscore(highscore);
    }
}

void spaceInvadersGameImpl::SaveHighscore(int hs) {
    Highscores::Save("spaceinvaders", hs);
}

int spaceInvadersGameImpl::LoadHighscore() {
    return Highscores::Load("spaceinvaders");
}

string spaceInvadersGameImpl::FormatScore(int number, int width) {
    string s = std::to_string(number);
    int leading = width - (int)s.length();
    if (leading > 0) s = string(leading, '0') + s;
    return s;
}
