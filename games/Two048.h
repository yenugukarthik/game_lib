#ifndef TWO048_H
#define TWO048_H

#include "game.h"
#include <random>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

const int GRID_SIZE    = 4;
const int TILE_SIZE    = 100;
const int TILE_SPACING = 10;
const int OFFSET_X     = 50;
const int OFFSET_Y     = 100;

// Returns the draw color for a given tile value
Color TileColor(int val);

class Two048Grid {
    vector<vector<int>> grid;
    int score;
    bool won;

    // Collapse non-zero values left, merge pairs, pad with zeros
    // Returns points earned from merges
    int slide(vector<int>& line);

public:
    Two048Grid();

    void Reset();
    void SpawnTile();           // Place a 2 (90%) or 4 (10%) in a random empty cell

    // Returns whether the move changed the board
    bool MoveUp();
    bool MoveDown();
    bool MoveLeft();
    bool MoveRight();

    bool HasWon()  const;       // Any tile == 2048
    bool IsGameOver() const;    // No empty cells and no adjacent equal tiles

    void Draw() const;

    int GetScore() const { return score; }
};

class Two048GameImpl : public Game {
private:
    Two048Grid grid;
    int score     = 0;
    int highScore = 0;
    bool gameOver = false;
    bool won      = false;
    bool started  = false;

public:
    Two048GameImpl() = default;

    void Init()     override;
    void Update()   override;
    void Draw()     override;
    bool HandleInput() override;
    std::string GetName()            const override;
    GameConfig  GetConfig()          const override;
    Color       GetBackgroundColor() const override;
    void Reset()    override;
};

#endif // TWO048_H