#pragma once
#include "game.h"
#include <raylib.h>
#include <string>
#include <vector>

using namespace std;

// ===================== Colors & Constants =====================
extern Color spaceInvadersGray;
extern Color spaceInvadersYellow;
extern int   spaceInvadersCellSize;
extern int   spaceInvadersOffset;

// ===================== Block =====================
class spaceInvadersBlock {
public:
    spaceInvadersBlock(Vector2 position);
    void      Draw();
    Rectangle getRect();
private:
    Vector2 position;
};

// ===================== Obstacle (shield/bunker) =====================
class spaceInvadersObstacles {
public:
    spaceInvadersObstacles(Vector2 position);
    void Draw();
    Vector2 position;
    vector<spaceInvadersBlock> blocks;
    static vector<vector<int>> grid;
};

// ===================== Laser =====================
class siLaser {
public:
    siLaser(Vector2 position, int speed, Color color = {243, 216, 63, 255});
    void      update();
    void      draw();
    Rectangle getRect();
    bool active;
private:
    Vector2 position;
    int     speed;
    Color   color;
};

// ===================== Spaceship (player) =====================
class siSpaceship {
public:
    siSpaceship();
    ~siSpaceship();
    void Load();
    void Unload();
    void Draw();
    void MoveLeft();
    void MoveRight();
    void FireLaser();
    void Reset();
    Rectangle getRect();
    vector<siLaser> lasers;
private:
    Texture2D image{};
    Vector2   position{};
    bool      textureLoaded = false;
    double    lastFireTime  = 0.0;
    Sound     laserSound{};
    bool      soundLoaded  = false;
};

// ===================== Alien =====================
class siAlien {
public:
    siAlien(int type, Vector2 position);
    void      Draw();
    void      Update(int direction);
    Rectangle getRect();
    static void LoadImages();
    static void UnloadImages();
    int     type;
    Vector2 position;
    static Texture2D alienImages[3];
};

// ===================== Mystery Ship =====================
class siMysteryShip {
public:
    siMysteryShip();
    ~siMysteryShip();
    void Load();
    void Unload();
    void Update();
    void Draw();
    void Spawn();
    Rectangle getRect();
    bool alive = false;
private:
    Vector2   position{};
    Texture2D image{};
    bool      textureLoaded = false;
    int       speed = 3;
};

// ===================== Main Game Class =====================
class spaceInvadersGameImpl : public Game {
public:
    spaceInvadersGameImpl();
    ~spaceInvadersGameImpl();

    void        Init()    override;
    void        Update()  override;
    void        Draw()    override;
    bool        HandleInput() override;
    std::string GetName() const override;
    GameConfig  GetConfig() const override;
    Color       GetBackgroundColor() const override;
    void        Reset() override;

private:
    // helpers
    void DeleteInactiveLasers();
    void MoveAliens();
    void MoveDownAliens(int distance);
    void AlienShootLaser();
    void CheckForCollisions();
    void GameOver();
    void InitGame();
    void CheckForHighscore();
    void SaveHighscore(int hs);
    int  LoadHighscore();
    vector<spaceInvadersObstacles> CreateObstacles();
    vector<siAlien>                CreateAliens();
    string FormatScore(int number, int width);

    siSpaceship     player;
    siMysteryShip   mysteryShip;
    vector<spaceInvadersObstacles> obstacles;
    vector<siAlien> aliens;
    vector<siLaser> alienLasers;

    int   aliensDirection          = 1;
    float timeLastAlienFired       = 0.0f;
    float timeLastSpawn            = 0.0f;
    float mysteryShipSpawnInterval = 15.0f;
    float alienLaserInterval = 0.5f;  // gets shorter each level

    bool  gameRunning  = true;
    bool  gameStarted  = false;  // waits for ENTER on first launch
    int   lives        = 3;
    int   score        = 0;
    int   highscore    = 0;
    int   level        = 1;
    int   alienSpeed   = 1;   // pixels per frame, increases each level

    Music music{};
    bool  musicLoaded    = false;
    Sound explosionSound{};
    bool  soundLoaded    = false;

    Texture2D spaceshipHudImg{};
    bool      hudImgLoaded = false;
};
