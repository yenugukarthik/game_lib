#include "Two048Vision.h"
#include <iostream>
#include <stdio.h>

Two048VisionGameImpl::Two048VisionGameImpl() {
    cvRunning = false;
    fingerCount = 0;
    lastFingerCount = 0;
}

Two048VisionGameImpl::~Two048VisionGameImpl() {
    Reset();
}

void Two048VisionGameImpl::CVLoop() {
    // Make sure no old stop_signal file exists
    remove("stop_signal");

    // Spawn python script
    FILE* pipe = _popen("python exp.py", "r");
    if (!pipe) {
        std::cerr << "Two048Vision: Could not launch python exp.py!" << std::endl;
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

void Two048VisionGameImpl::Init() {
    // Save highscore from previous round
    if (score > highScore) {
        highScore = score;
        Highscores::Save("2048vision", highScore);
    }
    if (highScore == 0) highScore = Highscores::Load("2048vision");

    score    = 0;
    gameOver = false;
    won      = false;
    started  = false;
    grid.Reset();
    lastFingerCount = 0;

    // Start OpenCV thread
    if (!cvRunning) {
        cvRunning = true;
        cvThread = std::thread(&Two048VisionGameImpl::CVLoop, this);
    }
}

void Two048VisionGameImpl::Update() {
    if (!started || won || gameOver) return;

    // Read OpenCV finger count
    int currentFingers = fingerCount.load();

    // Only move if we transition from 0 fingers to 1, 2, 3, or 4 fingers
    // This requires the user to form a fist (0 fingers) between moves.
    bool moved = false;
    if (lastFingerCount == 0 && currentFingers != 0) {
        if (currentFingers == 1) { // Left
            moved = grid.MoveLeft();
            lastFingerCount = currentFingers;
        } else if (currentFingers == 2) { // Right
            moved = grid.MoveRight();
            lastFingerCount = currentFingers;
        } else if (currentFingers == 3) { // Up
            moved = grid.MoveUp();
            lastFingerCount = currentFingers;
        } else if (currentFingers >= 4) { // Down
            moved = grid.MoveDown();
            lastFingerCount = currentFingers;
        }
    } else if (currentFingers == 0) {
        lastFingerCount = 0; // Reset last count when user closes hand
    }

    if (moved) {
        score = grid.GetScore();
        if (score > highScore) {
            highScore = score;
            Highscores::Save("2048vision", highScore);
        }
        grid.SpawnTile();
        if (grid.HasWon())      won      = true;
        if (grid.IsGameOver())  gameOver = true;
    }
}

void Two048VisionGameImpl::Draw() {
    // ── Title & scores ──
    int boardW = GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * TILE_SPACING;

    DrawText("2048 Vision", OFFSET_X - TILE_SPACING, 20, 42, {119, 110, 101, 255});

    // Score panel
    int panelX  = OFFSET_X + boardW - 195;
    Color panelBg = {187, 173, 160, 255};
    DrawRectangleRounded({(float)panelX, 20, 90, 54}, 0.2f, 8, panelBg);
    DrawText("SCORE", panelX + 5, 24, 16, {238, 228, 218, 255});
    DrawText(TextFormat("%d", grid.GetScore()), panelX + 5, 46, 22, WHITE);

    DrawRectangleRounded({(float)(panelX + 105), 20, 90, 54}, 0.2f, 8, panelBg);
    DrawText("BEST", panelX + 110, 24, 16, {238, 228, 218, 255});
    DrawText(TextFormat("%d", highScore), panelX + 110, 46, 22, WHITE);

    // Show current finger count for debugging
    int fc = fingerCount.load();
    DrawText(TextFormat("Fingers: %i", fc), OFFSET_X - TILE_SPACING, 65, 20, RED);

    // ── Grid ──
    grid.Draw();

    // ── Overlays ──
    if (!started) {
        // Welcome screen overlay
        int bx = OFFSET_X - TILE_SPACING;
        int by = OFFSET_Y - TILE_SPACING;
        DrawRectangle(bx, by, boardW, boardW, {238, 228, 218, 200});
        const char* t1 = "2048 Vision!";
        const char* t2 = "Show 1-4 fingers to move.";
        const char* t4 = "Close hand between moves.";
        const char* t3 = "Press ENTER to start";
        DrawText(t1, bx + (boardW - MeasureText(t1, 40)) / 2, by + boardW/2 - 80, 40, {119, 110, 101, 255});
        DrawText(t2, bx + (boardW - MeasureText(t2, 22)) / 2, by + boardW/2 - 20, 22, {119, 110, 101, 255});
        DrawText(t4, bx + (boardW - MeasureText(t4, 22)) / 2, by + boardW/2 + 20, 22, RED);
        DrawText(t3, bx + (boardW - MeasureText(t3, 22)) / 2, by + boardW/2 + 60, 22, {119, 110, 101, 255});
    }

    if (started && won) {
        int bx = OFFSET_X - TILE_SPACING;
        int by = OFFSET_Y - TILE_SPACING;
        DrawRectangle(bx, by, boardW, boardW, {237, 194, 46, 180});
        const char* t1 = "You Win!";
        const char* t2 = "Press ENTER to play again";
        DrawText(t1, bx + (boardW - MeasureText(t1, 48)) / 2, by + boardW/2 - 40, 48, WHITE);
        DrawText(t2, bx + (boardW - MeasureText(t2, 22)) / 2, by + boardW/2 + 20, 22, WHITE);
    }

    if (started && gameOver) {
        int bx = OFFSET_X - TILE_SPACING;
        int by = OFFSET_Y - TILE_SPACING;
        DrawRectangle(bx, by, boardW, boardW, {119, 110, 101, 200});
        const char* t1 = "Game Over!";
        const char* t2 = "Press ENTER to restart";
        DrawText(t1, bx + (boardW - MeasureText(t1, 44)) / 2, by + boardW/2 - 40, 44, WHITE);
        DrawText(t2, bx + (boardW - MeasureText(t2, 22)) / 2, by + boardW/2 + 20, 22, WHITE);
    }

    // ── Controls hint ──
    int hintY = GetConfig().windowHeight - 30;
    DrawText("1:Left  2:Right  3:Up  4+:Down  |  Backspace:Menu", OFFSET_X - TILE_SPACING, hintY, 18, {119, 110, 101, 255});
}

bool Two048VisionGameImpl::HandleInput() {
    if (IsKeyPressed(KEY_BACKSPACE)) return false;

    if (!started) {
        if (IsKeyPressed(KEY_ENTER)) started = true;
        return true;
    }

    if (won || gameOver) {
        if (IsKeyPressed(KEY_ENTER)) Init();
        return true;
    }

    // Optional keyboard fallback for testing (but Vision is the primary control)
    bool moved = false;
    if (IsKeyPressed(KEY_LEFT)) {
        moved = grid.MoveLeft();
    } else if (IsKeyPressed(KEY_RIGHT)) {
        moved = grid.MoveRight();
    } else if (IsKeyPressed(KEY_UP)) {
        moved = grid.MoveUp();
    } else if (IsKeyPressed(KEY_DOWN)) {
        moved = grid.MoveDown();
    }

    if (moved) {
        score = grid.GetScore();
        if (score > highScore) {
            highScore = score;
            Highscores::Save("2048vision", highScore);
        }
        grid.SpawnTile();
        if (grid.HasWon())      won      = true;
        if (grid.IsGameOver())  gameOver = true;
    }

    return true;
}

std::string Two048VisionGameImpl::GetName() const { return "2048 Vision"; }

GameConfig Two048VisionGameImpl::GetConfig() const {
    // Board = 4*(100+10)+10 = 450 wide
    int boardPx = GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * TILE_SPACING; // 450
    int w = boardPx + 2 * OFFSET_X;              // 550
    int h = boardPx + OFFSET_Y + 50;             // 600
    GameConfig cfg;
    cfg.windowWidth  = w;
    cfg.windowHeight = h;
    return cfg;
}

Color Two048VisionGameImpl::GetBackgroundColor() const {
    return {250, 248, 239, 255};   // Creamy white
}

void Two048VisionGameImpl::Reset() {
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
        Highscores::Save("2048vision", highScore);
    }
}
