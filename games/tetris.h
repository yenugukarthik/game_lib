#ifndef TETRIS_H
#define TETRIS_H

#include "game.h"
#include <raylib.h>
#include <string>
#include <vector>
#include <map>
#include <random>

using namespace std;

extern Color tetrisDarkBlue;

vector<Color> tetrisGetColors();

class tetrisGrid {
    int numRows;
    int numCols;
    int cellSize;
    vector<Color> colors = tetrisGetColors();

public:
    std::vector<std::vector<int>> grid;
    tetrisGrid(int rows, int cols, int cellSize);
    void Draw(int offsetX, int offsetY);
    bool isCellOutside(int row, int col);
    bool isCellEmpty(int row, int col);
    bool isRowFull(int row);
    int clearFullRows();
};

class tetrisPosition {
public:
    int row;
    int col;
    tetrisPosition(int r, int c);
};

class tetrisBlock {
    int rotationState = 0; 
    int rowOffset = 0;
    int columnOffset = 0;
    int cellSize = 30;
    vector<Color> colors = tetrisGetColors();
public:
    int id;
    map<int, vector<tetrisPosition>> cells;

    void Draw(int offsetX, int offsetY);
    vector<tetrisPosition> GetCurrentCells();
    void Move(int rowDelta, int colDelta);
    void Rotate();
    void UndoRotate();
};

class LtetrisBlock : public tetrisBlock {
public:
    LtetrisBlock();
};

class JtetrisBlock : public tetrisBlock {
public:
    JtetrisBlock();
};

class ItetrisBlock : public tetrisBlock {
public:
    ItetrisBlock();
};

class OtetrisBlock : public tetrisBlock {
public:
    OtetrisBlock();
};

class StetrisBlock : public tetrisBlock {
public:
    StetrisBlock();
};

class TtetrisBlock : public tetrisBlock {
public:
    TtetrisBlock();
};

class ZtetrisBlock : public tetrisBlock {
public:
    ZtetrisBlock();
};

class tetrisGame {
public:
    tetrisGrid grid = tetrisGrid(20, 10, 30);
    vector<tetrisBlock*> blocks = {new LtetrisBlock(), new JtetrisBlock(), new ItetrisBlock(), new OtetrisBlock(), new StetrisBlock(), new TtetrisBlock(), new ZtetrisBlock()};
    tetrisBlock* currentBlock;
    tetrisBlock* nextBlock;
    bool gameOver = true;

    tetrisGame();
    bool isBlockOuside(tetrisBlock* block);
    tetrisBlock* getRandomBlock();
    void Draw();
    void lockCurrentBlock();
    bool BlockCollides(tetrisBlock* block);
};

class TetrisGameImpl : public Game {
private:
    int score = 0;
    int highScore = 0;
    tetrisGrid grid = tetrisGrid(10, 15, 30);
    tetrisGame game;
    double lastUpdateTime = 0;
    int gameNum = 0;

public:
    void Init() override;
    void Update() override;
    void Draw() override;
    bool HandleInput() override;
    std::string GetName() const override;
    GameConfig GetConfig() const override;
    Color GetBackgroundColor() const override;
    void Reset() override;
};

#endif // TETRIS_H