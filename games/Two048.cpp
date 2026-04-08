
#include "Two048.h"
#include <cstdlib>
#include <cstring>

// ──────────────────────────────────────────────
// Tile colour palette (classic 2048 style)
// ──────────────────────────────────────────────
Color TileColor(int val) {
    switch (val) {
        case    2: return {238, 228, 218, 255};
        case    4: return {237, 224, 200, 255};
        case    8: return {242, 177, 121, 255};
        case   16: return {245, 149,  99, 255};
        case   32: return {246, 124,  95, 255};
        case   64: return {246,  94,  59, 255};
        case  128: return {237, 207, 114, 255};
        case  256: return {237, 204,  97, 255};
        case  512: return {237, 200,  80, 255};
        case 1024: return {237, 197,  63, 255};
        case 2048: return {237, 194,  46, 255};
        default:   return {60,  58,  50, 255};  // >2048 dark
    }
}

// ──────────────────────────────────────────────
// Two048Grid implementation
// ──────────────────────────────────────────────
Two048Grid::Two048Grid() {
    Reset();
}

void Two048Grid::Reset() {
    grid.assign(GRID_SIZE, vector<int>(GRID_SIZE, 0));
    score = 0;
    won   = false;
    SpawnTile();
    SpawnTile();
}

void Two048Grid::SpawnTile() {
    // Collect empty positions
    vector<pair<int,int>> empty;
    for (int r = 0; r < GRID_SIZE; r++)
        for (int c = 0; c < GRID_SIZE; c++)
            if (grid[r][c] == 0)
                empty.push_back({r, c});

    if (empty.empty()) return;

    int idx = GetRandomValue(0, (int)empty.size() - 1);
    int r = empty[idx].first;
    int c = empty[idx].second;
    grid[r][c] = (GetRandomValue(0, 9) < 9) ? 2 : 4;  // 90% chance of 2
}

// Slide a single row/column leftward; returns score gained
int Two048Grid::slide(vector<int>& line) {
    int gained = 0;
    // Pack non-zeros to the front
    vector<int> packed;
    for (int v : line) if (v) packed.push_back(v);

    // Merge adjacent equal pairs left-to-right
    for (int i = 0; i + 1 < (int)packed.size(); i++) {
        if (packed[i] == packed[i+1]) {
            packed[i] *= 2;
            gained += packed[i];
            packed.erase(packed.begin() + i + 1);
        }
    }

    // Pad with zeros on the right
    while ((int)packed.size() < GRID_SIZE) packed.push_back(0);
    line = packed;
    return gained;
}

bool Two048Grid::MoveLeft() {
    int gained = 0;
    vector<vector<int>> old = grid;
    for (int r = 0; r < GRID_SIZE; r++) gained += slide(grid[r]);
    score += gained;
    return old != grid;
}

bool Two048Grid::MoveRight() {
    int gained = 0;
    vector<vector<int>> old = grid;
    for (int r = 0; r < GRID_SIZE; r++) {
        reverse(grid[r].begin(), grid[r].end());
        gained += slide(grid[r]);
        reverse(grid[r].begin(), grid[r].end());
    }
    score += gained;
    return old != grid;
}

bool Two048Grid::MoveUp() {
    int gained = 0;
    vector<vector<int>> old = grid;
    for (int c = 0; c < GRID_SIZE; c++) {
        vector<int> col;
        for (int r = 0; r < GRID_SIZE; r++) col.push_back(grid[r][c]);
        gained += slide(col);
        for (int r = 0; r < GRID_SIZE; r++) grid[r][c] = col[r];
    }
    score += gained;
    return old != grid;
}

bool Two048Grid::MoveDown() {
    int gained = 0;
    vector<vector<int>> old = grid;
    for (int c = 0; c < GRID_SIZE; c++) {
        vector<int> col;
        for (int r = 0; r < GRID_SIZE; r++) col.push_back(grid[r][c]);
        reverse(col.begin(), col.end());
        gained += slide(col);
        reverse(col.begin(), col.end());
        for (int r = 0; r < GRID_SIZE; r++) grid[r][c] = col[r];
    }
    score += gained;
    return old != grid;
}

bool Two048Grid::HasWon() const {
    for (int r = 0; r < GRID_SIZE; r++)
        for (int c = 0; c < GRID_SIZE; c++)
            if (grid[r][c] == 2048) return true;
    return false;
}

bool Two048Grid::IsGameOver() const {
    // Any empty cell → not over
    for (int r = 0; r < GRID_SIZE; r++)
        for (int c = 0; c < GRID_SIZE; c++)
            if (grid[r][c] == 0) return false;

    // Any adjacent equal tiles → not over
    for (int r = 0; r < GRID_SIZE; r++)
        for (int c = 0; c < GRID_SIZE; c++) {
            if (c + 1 < GRID_SIZE && grid[r][c] == grid[r][c+1]) return false;
            if (r + 1 < GRID_SIZE && grid[r][c] == grid[r+1][c]) return false;
        }
    return true;
}

void Two048Grid::Draw() const {
    // Board background
    Color bgBoard = {187, 173, 160, 255};
    int boardW = GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * TILE_SPACING;
    int boardH = boardW;
    DrawRectangle(OFFSET_X - TILE_SPACING, OFFSET_Y - TILE_SPACING, boardW, boardH, bgBoard);

    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            int val = grid[r][c];
            int x = OFFSET_X + c * (TILE_SIZE + TILE_SPACING);
            int y = OFFSET_Y + r * (TILE_SIZE + TILE_SPACING);

            Color bgTile = (val == 0) ? Color{204, 192, 179, 255} : TileColor(val);
            DrawRectangleRounded({(float)x, (float)y, (float)TILE_SIZE, (float)TILE_SIZE}, 0.12f, 8, bgTile);

            if (val != 0) {
                const char* txt = TextFormat("%d", val);
                int fontSize = (val >= 1024) ? 28 : (val >= 256 ? 36 : 44);
                int tw = MeasureText(txt, fontSize);
                Color fc = (val <= 4) ? Color{119, 110, 101, 255} : WHITE;
                DrawText(txt, x + (TILE_SIZE - tw) / 2, y + (TILE_SIZE - fontSize) / 2, fontSize, fc);
            }
        }
    }
}

// ──────────────────────────────────────────────
// Two048GameImpl implementation
// ──────────────────────────────────────────────
void Two048GameImpl::Init() {
    // Save highscore from previous round
    if (score > highScore) {
        highScore = score;
        Highscores::Save("2048", highScore);
    }
    if (highScore == 0) highScore = Highscores::Load("2048");

    score    = 0;
    gameOver = false;
    won      = false;
    started  = false;
    grid.Reset();
}

void Two048GameImpl::Update() {
    // Nothing timed — 2048 is purely input-driven
}

void Two048GameImpl::Draw() {
    // ── Background ──
    // Already cleared by main loop using GetBackgroundColor()

    // ── Title & scores ──
    int boardW = GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * TILE_SPACING;

    DrawText("2048", OFFSET_X - TILE_SPACING, 20, 56, {119, 110, 101, 255});

    // Score panel
    int panelX  = OFFSET_X + boardW - 195;
    Color panelBg = {187, 173, 160, 255};
    DrawRectangleRounded({(float)panelX, 20, 90, 54}, 0.2f, 8, panelBg);
    DrawText("SCORE", panelX + 5, 24, 16, {238, 228, 218, 255});
    DrawText(TextFormat("%d", grid.GetScore()), panelX + 5, 46, 22, WHITE);

    DrawRectangleRounded({(float)(panelX + 105), 20, 90, 54}, 0.2f, 8, panelBg);
    DrawText("BEST", panelX + 110, 24, 16, {238, 228, 218, 255});
    DrawText(TextFormat("%d", highScore), panelX + 110, 46, 22, WHITE);

    // ── Grid ──
    grid.Draw();

    // ── Overlays ──
    if (!started) {
        // Welcome screen overlay
        int bx = OFFSET_X - TILE_SPACING;
        int by = OFFSET_Y - TILE_SPACING;
        DrawRectangle(bx, by, boardW, boardW, {238, 228, 218, 200});
        const char* t1 = "Welcome to 2048!";
        const char* t2 = "Use arrow keys to play";
        const char* t3 = "Press ENTER to start";
        DrawText(t1, bx + (boardW - MeasureText(t1, 28)) / 2, by + boardW/2 - 60, 28, {119, 110, 101, 255});
        DrawText(t2, bx + (boardW - MeasureText(t2, 22)) / 2, by + boardW/2 - 20, 22, {119, 110, 101, 255});
        DrawText(t3, bx + (boardW - MeasureText(t3, 22)) / 2, by + boardW/2 + 20, 22, {119, 110, 101, 255});
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
    DrawText("Arrow Keys - Move   |   Backspace - Menu", OFFSET_X - TILE_SPACING, hintY, 20, {119, 110, 101, 255});
}

bool Two048GameImpl::HandleInput() {
    if (IsKeyPressed(KEY_BACKSPACE)) return false;

    if (!started) {
        if (IsKeyPressed(KEY_ENTER)) started = true;
        return true;
    }

    if (won || gameOver) {
        if (IsKeyPressed(KEY_ENTER)) Init();
        return true;
    }

    // Remember old grid state to detect if a move actually changed anything
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
            Highscores::Save("2048", highScore);
        }
        grid.SpawnTile();
        if (grid.HasWon())      won      = true;
        if (grid.IsGameOver())  gameOver = true;
    }

    return true;
}

std::string Two048GameImpl::GetName() const { return "2048"; }

GameConfig Two048GameImpl::GetConfig() const {
    // Board = 4*(100+10)+10 = 450 wide
    int boardPx = GRID_SIZE * TILE_SIZE + (GRID_SIZE + 1) * TILE_SPACING; // 450
    int w = boardPx + 2 * OFFSET_X;              // 550
    int h = boardPx + OFFSET_Y + 50;             // 600
    GameConfig cfg;
    cfg.windowWidth  = w;
    cfg.windowHeight = h;
    return cfg;
}

Color Two048GameImpl::GetBackgroundColor() const {
    return {250, 248, 239, 255};   // Creamy white, classic 2048 bg
}

void Two048GameImpl::Reset() {
    Init();
}
