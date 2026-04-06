#include "chess.h"

using namespace std;

// Define globals declared as extern in header
const std::vector<int> whitePawns = {6, 7, 8, 9, 10, 11, 12, 13};
const std::vector<int> blackPawns = {-6, -7, -8, -9, -10, -11, -12, -13};

std::unordered_map<int, std::unique_ptr<chessPiece>> board;



chessGrid::chessGrid(){
    grid.resize(8, vector<int>(8, 0));
}



void chessGameImpl::Init() {
        // Initialize chess game state here (e.g., set up board, pieces, etc.)
}

void chessGameImpl::Update() {
    // Update chess game logic each frame here (e.g., handle piece movement, check for check/checkmate, etc.)
}

void chessGameImpl::Draw() {
    // Draw chess game state here (e.g., draw board, pieces, etc.)
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if((i+j)%2==0){
                DrawRectangle(j*50, i*50, 50, 50, LIGHTGRAY);
            }else{
                DrawRectangle(j*50, i*50, 50, 50, DARKGRAY);
            }
        }
    }
}

bool chessGameImpl::HandleInput() {
        if (IsKeyPressed(KEY_BACKSPACE)) {
        return false;
        }
        return true;
    } 
    
std::string chessGameImpl::GetName() const {
    return "Chess";
}

GameConfig chessGameImpl::GetConfig() const {
    return GameConfig(50, 8, 8, 0);
}

Color chessGameImpl::GetBackgroundColor() const {
    return LIGHTGRAY;
}

void chessGameImpl::Reset() {
    Init();
}