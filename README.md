# 🎮 Game Hub — C++ Raylib Game Library

A growing collection of **classic and AI-powered games** built with **C++ and Raylib**, unified under a single launcher. What makes this project stand out is its integration of **computer vision** (MediaPipe + OpenCV) and **offline speech recognition** (Vosk) to enable hands-free, voice-controlled gameplay.

---

## 🕹️ Games Included

| Game | Input Method | Description |
|------|-------------|-------------|
| 🐍 **Snake** | Keyboard | Classic snake with high-score tracking |
| 🐍 **SnakeVision** | Hand gestures (webcam) | Snake controlled by counting fingers via MediaPipe |
| 🧱 **Tetris** | Keyboard | Full Tetris with all 7 tetrominoes |
| 👾 **Space Invaders** | Keyboard | Multi-level Space Invaders with mystery ship, shields & sound |
| ♟️ **Chess** | Keyboard | Chess board (work in progress) |
| 🔢 **2048** | Keyboard | Classic 2048 tile-merge puzzle |
| 🔢 **2048 Vision** | Hand gestures (webcam) | 2048 controlled by finger-count gestures |
| 🔢 **2048 Speech** | Voice commands | 2048 controlled by saying "up", "down", "left", "right" |

---

## ✨ Highlights

- **Unified game hub** — one launcher, all games; navigate with arrow keys and press Enter to play
- **Persistent high scores** — scores saved to `highscores.txt` and restored between sessions
- **Computer vision input** — uses MediaPipe hand tracking (via OpenCV + Python bridge) for gesture control
- **Offline voice control** — uses Vosk speech recognition (no internet required) for voice commands
- **Plug-and-play architecture** — each game implements a common `Game` interface, making it trivial to add new games

---

## 🛠️ Tech Stack

| Layer | Technology |
|-------|-----------|
| Language | C++14 |
| Graphics & Audio | [Raylib 4.5](https://www.raylib.com/) |
| Computer Vision | OpenCV + MediaPipe (Python) |
| Speech Recognition | [Vosk](https://alphacephei.com/vosk/) (offline, Python) |
| Build System | GNU Make / mingw32-make |
| Platforms | Windows (primary), Linux, macOS |

---

## 📁 Project Structure

```
games/
├── main.cpp              # Game Hub launcher & menu loop
├── game.h                # Abstract Game base class + GameConfig + Highscores
│
├── snake.h / .cpp        # Classic Snake (keyboard)
├── snakeVision.h / .cpp  # Snake with webcam gesture control
│
├── tetris.h / .cpp       # Tetris
│
├── space_invaders.h / .cpp # Space Invaders (multi-level, audio)
│
├── chess.h / .cpp        # Chess (WIP)
│
├── Two048.h / .cpp       # 2048 (keyboard)
├── Two048Vision.h / .cpp # 2048 (webcam gesture control)
├── Two048Speech.h / .cpp # 2048 (voice commands)
│
├── speech.py             # Vosk speech recognition bridge (chess prototype)
├── speech_2048.py        # Vosk speech bridge used by 2048 Speech
├── exp.py                # MediaPipe finger-tracking experiment script
│
├── model/                # Vosk offline language model (required for speech games)
├── highscores.txt        # Persisted high scores (auto-created)
└── Makefile              # Cross-platform build system
```

---

## 🚀 Getting Started

### Prerequisites

**C++ side:**
- [Raylib 4.5](https://www.raylib.com/) installed at `C:/raylib` (Windows default path)
- MinGW-w64 (`w64devkit`) for Windows, or `g++` on Linux/macOS
- OpenCV (required for Vision games)

**Python side (Vision & Speech games only):**
- Python 3.8+
- Install dependencies:
  ```bash
  pip install mediapipe opencv-python vosk sounddevice
  ```
- Download a [Vosk model](https://alphacephei.com/vosk/models) (e.g., `vosk-model-small-en-us`) and place it in the `model/` directory

### Building

```bash
# Windows (MinGW)
mingw32-make

# Linux / macOS
make
```

This produces `game.exe` (Windows) or `game` (Linux/macOS).

### Running

```bash
# Windows
./game.exe

# Linux / macOS
./game
```

Navigate the menu with **↑ ↓ arrow keys**, select a game with **Enter**, and return to the menu with **Escape**.

---

## 🎮 Controls

### Common
| Key | Action |
|-----|--------|
| `↑ ↓` | Navigate menu |
| `Enter` | Select / Start game |
| `Escape` | Return to menu |

### Snake / SnakeVision
| Input | Action |
|-------|--------|
| `W A S D` or arrow keys | Move snake |
| ✋ 1 finger | Move up |
| ✋ 2 fingers | Move down |
| ✋ 3 fingers | Move left |
| ✋ 4 fingers | Move right |

### Space Invaders
| Key | Action |
|-----|--------|
| `← →` | Move spaceship |
| `Space` | Fire laser |

### 2048 / 2048 Vision / 2048 Speech
| Input | Action |
|-------|--------|
| Arrow keys | Slide tiles |
| ✋ 1 finger | Slide up |
| ✋ 2 fingers | Slide down |
| ✋ 3 fingers | Slide left |
| ✋ 4 fingers | Slide right |
| 🎤 "up / down / left / right" | Slide tiles (Speech mode) |

---

## 🏗️ Architecture

### Game Interface

Every game implements the abstract `Game` base class defined in `game.h`:

```cpp
class Game {
public:
    virtual GameConfig GetConfig()          const = 0; // window size, grid config
    virtual void       Init()                     = 0; // called on game start
    virtual void       Update()                   = 0; // called every frame
    virtual void       Draw()                     = 0; // called every frame
    virtual bool       HandleInput()              = 0; // returns false to exit to menu
    virtual std::string GetName()           const = 0;
    virtual Color      GetBackgroundColor() const = 0;
    virtual void       Reset()                    = 0;
};
```

Adding a new game is as simple as:
1. Implementing the `Game` interface in a new `.h`/`.cpp` pair
2. Adding `games.push_back(make_shared<YourGameImpl>())` in `main.cpp`

### Vision Games

Vision-controlled games spawn a background thread that runs an OpenCV + MediaPipe pipeline. The finger count is published to an `std::atomic<int>` which the main game loop reads each frame, keeping the game loop and CV loop fully decoupled.

### Speech Games

Speech-controlled games spawn a background thread that runs the VOSK recognizer via Python subprocess, publishing recognized commands to a thread-safe `std::queue<string>` protected by a `std::mutex`.

### High Scores

A shared `Highscores` utility (in `game.h`) reads and writes a key=value `highscores.txt` file:
```
snake=120
tetris=4500
spaceinvaders=3200
2048=2048
```

---

## 🔮 Roadmap — Near Future Plans

Here's what's coming next for the Game Hub:

### ♟️ Chess — Full Completion
- The Chess game is currently a work in progress (board rendering only).
- **Planned**: Complete implementation of all chess rules — legal move generation, check/checkmate/stalemate detection, castling, en passant, and pawn promotion.

### 🤖 Chess Engine
- Once the base game is complete, a **chess engine** will be integrated as an AI opponent.
- The engine will support configurable difficulty levels so players can challenge themselves at any skill level.

### 🎤 Chess Speech Control — Audio Chess
- A **voice-controlled chess variant** is planned, similar to the existing 2048 Speech mode.
- Players will be able to move pieces entirely by **speaking commands** (e.g., *"e2 to e4"*, *"knight to f3"*) using the Vosk offline speech recognition pipeline — no keyboard or mouse required.
- This will make Chess the most accessible and futuristic game in the hub.

---

## 📜 License

This project is licensed under the **zlib License** — see [LICENSE.txt](LICENSE.txt) for details.

> The Makefile is adapted from the official [Raylib](https://github.com/raysan5/raylib) project template, © Ramon Santamaria.

---

## 🙏 Acknowledgements

- [Raylib](https://www.raylib.com/) — simple and easy-to-use game library
- [MediaPipe](https://mediapipe.dev/) — hand tracking and gesture recognition
- [Vosk](https://alphacephei.com/vosk/) — offline speech recognition
- [OpenCV](https://opencv.org/) — computer vision library
