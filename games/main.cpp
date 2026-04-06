#include <iostream>
#include <raylib.h>
#include <memory>
#include <vector>
#include "game.h"
#include "snake.h"
#include "tetris.h"
#include "chess.h"
#include "space_invaders.h"
#include "snakeVision.h"

using namespace std;

// Define the shared highscores file path (one definition for the whole program)
const std::string Highscores::FILE_PATH = "highscores.txt";


// Menu configuration (auto-calculates window dimensions)
const GameConfig MENU_CONFIG(30, 35, 25, 75);

// Menu colors
const Color MENU_GREEN = {177, 252, 5, 255};
const Color MENU_DARK_GREEN = {43, 51, 24, 255};

enum Screen { SCREEN_MENU, SCREEN_GAME };

int main() {
    // Initialize window with menu config
    InitWindow(MENU_CONFIG.windowWidth, MENU_CONFIG.windowHeight, "Game Hub");
    SetTargetFPS(60);

    // Create game instances
    vector<shared_ptr<Game>> games;
    games.push_back(make_shared<SnakeGameImpl>());
    games.push_back(make_shared<TetrisGameImpl>());
    games.push_back(make_shared<spaceInvadersGameImpl>());
    // TODO: Add more games here (e.g., games.push_back(make_shared<TetrisGameImpl>());)
    games.push_back(make_shared<chessGameImpl>());
    games.push_back(make_shared<SnakeVisionGameImpl>());

    Screen currentScreen = SCREEN_MENU;
    int selectedGame = 0;
    shared_ptr<Game> activeGame = nullptr;

    while (!WindowShouldClose()) {
        BeginDrawing();
        
        // Clear background based on current screen
        if (currentScreen == SCREEN_GAME && activeGame) {
            ClearBackground(activeGame->GetBackgroundColor());
        } else {
            ClearBackground(MENU_GREEN);
        }

        if (currentScreen == SCREEN_MENU) {
            // Draw menu title
            DrawText("Select a Game", MENU_CONFIG.offset, 20, 40, MENU_DARK_GREEN);

            // Draw game list with pointer
            for (int i = 0; i < (int)games.size(); i++) {
                int y = 100 + i * 50;

                // Highlight selected entry
                Color textColor = (i == selectedGame ? DARKGREEN : BLACK);
                DrawText(games[i]->GetName().c_str(), MENU_CONFIG.offset + 30, y, 30, textColor);

                // Draw left-side triangle pointer for selected game
                if (i == selectedGame) {
                    Vector2 p1 = {(float)MENU_CONFIG.offset + 10, (float)y + 10};
                    Vector2 p2 = {(float)MENU_CONFIG.offset + 10, (float)y + 30};
                    Vector2 p3 = {(float)MENU_CONFIG.offset + 25, (float)y + 20};

                    DrawTriangle(p1, p2, p3, DARKGREEN);
                }
            }

            // Navigation
            if (IsKeyPressed(KEY_DOWN)) {
                selectedGame = (selectedGame + 1) % games.size();
            }
            if (IsKeyPressed(KEY_UP)) {
                selectedGame = (selectedGame - 1 + (int)games.size()) % games.size();
            }

            // Enter to start selected game
            if (IsKeyPressed(KEY_ENTER)) {
                activeGame = games[selectedGame];
                GameConfig cfg = activeGame->GetConfig();
                
                // Close old window and create new one with game config
                CloseWindow();
                InitWindow(cfg.windowWidth, cfg.windowHeight, "Game Hub");
                SetTargetFPS(60);
                
                activeGame->Init();
                currentScreen = SCREEN_GAME;
            }
        }

        else if (currentScreen == SCREEN_GAME && activeGame) {
            // Update and draw the active game
            activeGame->Update();
            activeGame->Draw();

            // Handle game input and check if user wants to return to menu
            bool stayInGame = activeGame->HandleInput();
            if (!stayInGame) {
                GameConfig menuCfg = MENU_CONFIG;
                
                // Close game window and restore menu window
                CloseWindow();
                InitWindow(menuCfg.windowWidth, menuCfg.windowHeight, "Game Hub");
                SetTargetFPS(60);
                
                currentScreen = SCREEN_MENU;
                activeGame->Reset();
                activeGame = nullptr;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
