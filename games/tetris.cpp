#include "tetris.h"

Color tetrisDarkBlue = {44, 44, 128, 255};

vector<Color> tetrisGetColors() {
    vector<Color> colors;
    colors.push_back(BLACK); // empty
    colors.push_back(RED);   // piece 1
    colors.push_back(GREEN); // piece 2 
    colors.push_back(BLUE);  // piece 3
    colors.push_back(YELLOW); // piece 4
    colors.push_back(PURPLE); // piece 5
    colors.push_back(ORANGE); // piece 6
    colors.push_back({21,204,209,255}); // piece 7   
    return colors;
}

tetrisGrid::tetrisGrid(int rows, int cols, int cellSize) : numRows(rows), numCols(cols), cellSize(cellSize) {
    grid.resize(numRows, vector<int>(numCols, 0));
}

void tetrisGrid::Draw(int offsetX, int offsetY) {
    for (int r = 0; r < numRows; r++) {
        for (int c = 0; c < numCols; c++) {
            Color cellColor = colors[grid[r][c]];
            DrawRectangle(offsetX + c * cellSize, offsetY + r * cellSize, cellSize, cellSize, cellColor);
            DrawRectangleLines(offsetX + c * cellSize, offsetY + r * cellSize, cellSize, cellSize, DARKGRAY);
        }
    }
}

bool tetrisGrid::isCellOutside(int row, int col) {
    return row < 0 || row >= numRows || col < 0 || col >= numCols;
}

bool tetrisGrid::isCellEmpty(int row, int col) {
    if (isCellOutside(row, col)) return false;
    return grid[row][col] == 0;
}

bool tetrisGrid::isRowFull(int row) {
    for (int c = 0; c < numCols; c++) {
        if (grid[row][c] == 0) return false;
    }
    return true;
}

int tetrisGrid::clearFullRows() {
    int cleared = 0;
    for (int r = numRows - 1; r >= 0; r--) {
        if (isRowFull(r)) {
            cleared++;
        } else {
            if (cleared > 0) {
                for (int c = 0; c < numCols; c++) {
                    grid[r + cleared][c] = grid[r][c];
                }
            }
        }
    }
    return cleared;
}

tetrisPosition::tetrisPosition(int r, int c) : row(r), col(c) {}

void tetrisBlock::Draw(int offsetX, int offsetY) {
    vector<tetrisPosition> currentCells = GetCurrentCells();
    for (const auto& pos : currentCells) {
        Color cellColor = colors[id];
        DrawRectangle(offsetX + pos.col * cellSize, offsetY + pos.row * cellSize, cellSize, cellSize, cellColor);
        DrawRectangleLines(offsetX + pos.col * cellSize, offsetY + pos.row * cellSize, cellSize, cellSize, DARKGRAY);
    }
}

vector<tetrisPosition> tetrisBlock::GetCurrentCells() {
    vector<tetrisPosition> currentCells;
    for (const auto& pos : cells[rotationState]) {
        currentCells.push_back(tetrisPosition(pos.row + rowOffset, pos.col + columnOffset));
    }
    return currentCells;
}

void tetrisBlock::Move(int rowDelta, int colDelta) {
    rowOffset += rowDelta;
    columnOffset += colDelta;
}

void tetrisBlock::Rotate() {
    rotationState = (rotationState + 1) % cells.size();
}

void tetrisBlock::UndoRotate() {
    rotationState = (rotationState - 1 + cells.size()) % cells.size();
}

LtetrisBlock::LtetrisBlock() {
    id = 1;
    cells[0] = {tetrisPosition(0, 2), tetrisPosition(1, 0), tetrisPosition(1, 1), tetrisPosition(1, 2)};
    cells[1] = {tetrisPosition(0, 1), tetrisPosition(1, 1), tetrisPosition(2, 1), tetrisPosition(2, 2)};
    cells[2] = {tetrisPosition(1, 0), tetrisPosition(1, 1), tetrisPosition(1, 2), tetrisPosition(2, 0)};
    cells[3] = {tetrisPosition(0, 0), tetrisPosition(0, 1), tetrisPosition(1, 1), tetrisPosition(2, 1)};
    Move(0, 3);
}

JtetrisBlock::JtetrisBlock() {
    id = 2;
    cells[0] = {tetrisPosition(0, 0), tetrisPosition(1, 0), tetrisPosition(1, 1), tetrisPosition(1, 2)};
    cells[1] = {tetrisPosition(0, 1), tetrisPosition(0, 2), tetrisPosition(1, 1), tetrisPosition(2, 1)};
    cells[2] = {tetrisPosition(1, 0), tetrisPosition(1, 1), tetrisPosition(1, 2), tetrisPosition(2, 2)};
    cells[3] = {tetrisPosition(0, 1), tetrisPosition(1, 1), tetrisPosition(2, 0), tetrisPosition(2, 1)};
    Move(0, 3);
}

ItetrisBlock::ItetrisBlock() {
    id = 3;
    cells[0] = {tetrisPosition(1, 0), tetrisPosition(1, 1), tetrisPosition(1, 2), tetrisPosition(1, 3)};
    cells[1] = {tetrisPosition(0, 2), tetrisPosition(1, 2), tetrisPosition(2, 2), tetrisPosition(3, 2)};
    cells[2] = {tetrisPosition(2, 0), tetrisPosition(2, 1), tetrisPosition(2, 2), tetrisPosition(2, 3)};
    cells[3] = {tetrisPosition(0, 1), tetrisPosition(1, 1), tetrisPosition(2, 1), tetrisPosition(3, 1)};
    Move(-1, 3);
}

OtetrisBlock::OtetrisBlock() {
    id = 4;
    cells[0] = {tetrisPosition(0, 0), tetrisPosition(0, 1), tetrisPosition(1, 0), tetrisPosition(1, 1)};
    Move(0, 4);
}

StetrisBlock::StetrisBlock() {
    id = 5;
    cells[0] = {tetrisPosition(0, 1), tetrisPosition(0, 2), tetrisPosition(1, 0), tetrisPosition(1, 1)};
    cells[1] = {tetrisPosition(0, 1), tetrisPosition(1, 1), tetrisPosition(1, 2), tetrisPosition(2, 2)};
    cells[2] = {tetrisPosition(1, 1), tetrisPosition(1, 2), tetrisPosition(2, 0), tetrisPosition(2, 1)};
    cells[3] = {tetrisPosition(0, 0), tetrisPosition(1, 0), tetrisPosition(1, 1), tetrisPosition(2, 1)};
    Move(0, 3);
}

TtetrisBlock::TtetrisBlock() {
    id = 6;
    cells[0] = {tetrisPosition(0, 1), tetrisPosition(1, 0), tetrisPosition(1, 1), tetrisPosition(1, 2)};
    cells[1] = {tetrisPosition(0, 1), tetrisPosition(1, 1), tetrisPosition(1, 2), tetrisPosition(2, 1)};
    cells[2] = {tetrisPosition(1, 0), tetrisPosition(1, 1), tetrisPosition(1, 2), tetrisPosition(2, 1)};
    cells[3] = {tetrisPosition(0, 1), tetrisPosition(1, 0), tetrisPosition(1, 1), tetrisPosition(2, 1)};
    Move(0, 3);
}

ZtetrisBlock::ZtetrisBlock() {
    id = 7;
    cells[0] = {tetrisPosition(0, 0), tetrisPosition(0, 1), tetrisPosition(1, 1), tetrisPosition(1, 2)};
    cells[1] = {tetrisPosition(0, 2), tetrisPosition(1, 1), tetrisPosition(1, 2), tetrisPosition(2, 1)};
    cells[2] = {tetrisPosition(1, 0), tetrisPosition(1, 1), tetrisPosition(2, 1), tetrisPosition(2, 2)};
    cells[3] = {tetrisPosition(0, 1), tetrisPosition(1, 0), tetrisPosition(1, 1), tetrisPosition(2, 0)};
    Move(0, 3);
}

tetrisGame::tetrisGame() {
    currentBlock = getRandomBlock();
    nextBlock = getRandomBlock();
}

bool tetrisGame::isBlockOuside(tetrisBlock* block) {
    vector<tetrisPosition> positions = block->GetCurrentCells();
    for (const auto& pos : positions) {
        if (grid.isCellOutside(pos.row, pos.col)) {
            return true;
        }
    }
    return false;
}

tetrisBlock* tetrisGame::getRandomBlock() {
    if (blocks.empty()) {
        blocks.push_back(new LtetrisBlock());
        blocks.push_back(new JtetrisBlock());
        blocks.push_back(new ItetrisBlock());
        blocks.push_back(new OtetrisBlock());
        blocks.push_back(new StetrisBlock());
        blocks.push_back(new TtetrisBlock());
        blocks.push_back(new ZtetrisBlock());
    }
    int randIndex = GetRandomValue(0, blocks.size() - 1);
    tetrisBlock* block = blocks[randIndex];
    blocks.erase(blocks.begin() + randIndex);
    return block;
}

void tetrisGame::Draw() {
    grid.Draw(110, 110);
    currentBlock->Draw(110, 110);
}

void tetrisGame::lockCurrentBlock() {
    vector<tetrisPosition> positions = currentBlock->GetCurrentCells();
    for (const auto& pos : positions) {
        if (!grid.isCellOutside(pos.row, pos.col)) {
            grid.grid[pos.row][pos.col] = currentBlock->id;
        }
    }

    currentBlock = nextBlock;
    nextBlock = getRandomBlock();

    if (isBlockOuside(currentBlock) || BlockCollides(currentBlock)) {
        gameOver = true;
        grid = tetrisGrid(20, 10, 30);
        currentBlock = getRandomBlock();
        nextBlock = getRandomBlock();
    }
}

bool tetrisGame::BlockCollides(tetrisBlock* block) {
    vector<tetrisPosition> positions = block->GetCurrentCells();
    for (const auto& pos : positions) {
        if (grid.isCellOutside(pos.row, pos.col) || !grid.isCellEmpty(pos.row, pos.col)) {
            return true;
        }
    }
    return false;
}

void TetrisGameImpl::Init() {
    if (score > highScore) {
        highScore = score;
        Highscores::Save("tetris", highScore);
    }
    if (highScore == 0) {
        highScore = Highscores::Load("tetris");
    }
    score = 0;
}

void TetrisGameImpl::Update() {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= 0.25 && !game.gameOver) {
        game.currentBlock->Move(1, 0);

        bool outOfBounds = game.isBlockOuside(game.currentBlock);
        bool colliding = game.BlockCollides(game.currentBlock);

        if (outOfBounds || colliding) {
            game.currentBlock->Move(-1, 0);
            game.lockCurrentBlock();
        }

        int a = game.grid.clearFullRows();
        score += a * 10;
        lastUpdateTime = currentTime;
    }
}

void TetrisGameImpl::Draw() {
    DrawRectangleLinesEx(Rectangle{40, 50, 800, 700}, 5, WHITE);
    DrawText("Tetris", 50, 10, 40, WHITE);
    game.Draw();
    DrawText("Next Piece:", 500, 150, 30, WHITE);
    game.nextBlock->Draw(500, 200);
    if (game.gameOver && gameNum == 0) {
        DrawText("Welcome to Tetris!", 111, 300, 30, RED);
        DrawText("Press Enter to Start", 111, 350, 27, RED);
    }
    if (game.gameOver && gameNum > 0) {
        DrawText("Game Over! ", 111, 300, 40, RED);
        DrawText("Press Enter to Restart", 111, 350, 30, RED);
    }
    DrawText(TextFormat("Score: %i", score), 500, 450, 40, WHITE);
    DrawText(TextFormat("High Score: %i", highScore), 500, 580, 40, WHITE);
}

bool TetrisGameImpl::HandleInput() {
    if (IsKeyPressed(KEY_BACKSPACE)) {
        return false;
    }

    int key = GetKeyPressed();
    if (!game.gameOver) {
        if (key == KEY_LEFT) {
            game.currentBlock->Move(0, -1);
            if (game.isBlockOuside(game.currentBlock) || game.BlockCollides(game.currentBlock)) {
                game.currentBlock->Move(0, 1);
            }
        } else if (key == KEY_RIGHT) {
            game.currentBlock->Move(0, 1);
            if (game.isBlockOuside(game.currentBlock) || game.BlockCollides(game.currentBlock)) {
                game.currentBlock->Move(0, -1);
            }
        } else if (key == KEY_DOWN) {
            game.currentBlock->Move(1, 0);
            if (game.isBlockOuside(game.currentBlock) || game.BlockCollides(game.currentBlock)) {
                game.currentBlock->Move(-1, 0);
                game.lockCurrentBlock();
                if (game.BlockCollides(game.currentBlock) || game.isBlockOuside(game.currentBlock)) {
                    Init();
                    return false;
                }
            }
        } else if (key == KEY_UP) {
            game.currentBlock->Rotate();
            if (game.isBlockOuside(game.currentBlock) || game.BlockCollides(game.currentBlock)) {
                game.currentBlock->UndoRotate();
            }
        }
    } else {
        if (key == KEY_ENTER) {
            Init();
            game.gameOver = false;
            gameNum++;
        }
    }

    return true;
}

std::string TetrisGameImpl::GetName() const {
    return "Tetris";
}

GameConfig TetrisGameImpl::GetConfig() const {
    return GameConfig(30, 26, 23, 50);
}

Color TetrisGameImpl::GetBackgroundColor() const { return tetrisDarkBlue; }

void TetrisGameImpl::Reset() {
    Init();
}