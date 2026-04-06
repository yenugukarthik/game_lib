#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include <string>

using namespace std;

struct GameConfig {
    int cellSize;
    int cellCountX;
    int cellCountY;
    int offset;
    int windowWidth;
    int windowHeight;

    // Constructor that auto-calculates window dimensions
    GameConfig(int cellSize = 30, int cellCountX = 35, int cellCountY = 25, int offset = 75)
        : cellSize(cellSize), cellCountX(cellCountX), cellCountY(cellCountY), offset(offset) {
        windowWidth = 2 * offset + cellSize * cellCountX;
        windowHeight = 2 * offset + cellSize * cellCountY;
    }
};

// ---- Shared highscore file (highscores.txt) ----
// Format: one "key=value" pair per line, e.g.
//   snake=12
//   tetris=450
//   spaceinvaders=3200
#include <fstream>
#include <sstream>
#include <map>
struct Highscores {
    static const std::string FILE_PATH;

    static int Load(const std::string& key) {
        std::ifstream f(FILE_PATH);
        std::string line;
        while (std::getline(f, line)) {
            auto eq = line.find('=');
            if (eq != std::string::npos && line.substr(0, eq) == key) {
                return std::stoi(line.substr(eq + 1));
            }
        }
        return 0;
    }

    static void Save(const std::string& key, int value) {
        // Read all existing entries
        std::map<std::string, int> entries;
        {
            std::ifstream f(FILE_PATH);
            std::string line;
            while (std::getline(f, line)) {
                auto eq = line.find('=');
                if (eq != std::string::npos) {
                    entries[line.substr(0, eq)] = std::stoi(line.substr(eq + 1));
                }
            }
        }
        // Update this game's entry
        entries[key] = value;
        // Write back
        std::ofstream f(FILE_PATH);
        for (auto& kv : entries) {
            f << kv.first << "=" << kv.second << "\n";
        }
    }
};


class Game {
public:
    virtual ~Game() = default;
    
    // Get game configuration (cell size, grid dimensions, window size, etc.)
    virtual GameConfig GetConfig() const = 0;
    
    // Initialize game (called when entering the game screen)
    virtual void Init() = 0;
    
    // Update game logic each frame
    virtual void Update() = 0;
    
    // Draw game state
    virtual void Draw() = 0;
    
    // Handle input and return true if game is still running, false if should return to menu
    virtual bool HandleInput() = 0;
    
    // Get game name for menu display
    virtual std::string GetName() const = 0;
    
    // Get background color for this game
    virtual Color GetBackgroundColor() const = 0;
    
    // Reset game state
    virtual void Reset() = 0;
};

#endif // GAME_H
