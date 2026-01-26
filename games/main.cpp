#include <iostream>
#include <raylib.h>
#include <deque>
#include <bits/stdc++.h>
#include <raymath.h>

using namespace std;

Color green ={177,252,5,255};
Color darkGreen={43,51,24,255};

int cellSize =30;
int cellCountx=35;
int cellCounty=25;
int offset=75;

double lastUpdateTime=0;

bool eventTriggered(double intervel){
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= intervel){
        lastUpdateTime=currentTime;
        return true;
    }
    return false;
}

bool ElementInDeque(Vector2 element, deque<Vector2>& dq){
    for(int i=0;i<dq.size();i++){
        if(Vector2Equals(dq[i],element)) return true;
    }
    return false;
}

void drawPause(int x,int y,int width,int height,Color dark ,Color light){
    DrawRectangle(x,y,width/3,height,dark);
    DrawRectangle(x+width/3,y,width/3,height,light);
    DrawRectangle(x+(2*width)/3,y,width/3,height,dark);
}

/* -------------------------------------------------
   --------------  SNAKE GAME OBJECTS --------------
   ------------------------------------------------- */

class SnakeFood {
public:
    Vector2 position;

    SnakeFood(deque<Vector2>& SnakeBody){
        position = GenerateRandomPosition(SnakeBody);
    }

    void Draw(){
        DrawRectangle(offset+position.x*cellSize+cellSize/3, offset+position.y*cellSize,
                      cellSize/3, cellSize/3, darkGreen);

        DrawRectangle(offset+position.x*cellSize, offset+position.y*cellSize+cellSize/3,
                      cellSize/3, cellSize/3, darkGreen);

        DrawRectangle(offset+position.x*cellSize+2*(cellSize/3), offset+position.y*cellSize+cellSize/3,
                      cellSize/3, cellSize/3, darkGreen);

        DrawRectangle(offset+position.x*cellSize+cellSize/3, offset+position.y*cellSize+2*(cellSize/3),
                      cellSize/3, cellSize/3, darkGreen);
    }

    Vector2 GenerateRandomCell(){
        return {(float)GetRandomValue(0, cellCountx-1),
                (float)GetRandomValue(0, cellCounty-1)};
    }

    Vector2 GenerateRandomPosition(deque<Vector2>& snakeBody){
        Vector2 position = GenerateRandomCell();
        while(ElementInDeque(position, snakeBody))
            position = GenerateRandomCell();
        return position;
    }
};

class SnakePlayer {
public:
    deque<Vector2> body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
    Vector2 direction = {1,0};

    void Draw(){
        for(auto &p : body){
            Rectangle seg = { offset + p.x * cellSize, offset + p.y * cellSize, 
                              (float)cellSize, (float)cellSize };
            DrawRectangleRounded(seg, 0.5, 6, darkGreen);
        }
    }

    void Update(Vector2 foodPos){
        Vector2 temp = Vector2Add(body[0], direction);
        temp.x = ((int)temp.x + cellCountx) % cellCountx;
        temp.y = ((int)temp.y + cellCounty) % cellCounty;

        body.push_front(temp);
        if(!Vector2Equals(body[0], foodPos))
            body.pop_back();
    }

    void Reset(){
        body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
        direction = {1,0};
    }
};

class SnakeGame {
public:
    SnakePlayer snakePlayer;
    SnakeFood snakeFood = SnakeFood(snakePlayer.body);
    bool running = false;
    bool directionChange = true;
    int score = 0;
    int highScore=0;

    void Draw(){
        snakeFood.Draw();
        snakePlayer.Draw();
    }

    void Update(){
        if(running){
            snakePlayer.Update(snakeFood.position);
            CheckFoodCollison();
            CheckSelfCollision();
        }
    }

    void CheckFoodCollison(){
        if(Vector2Equals(snakePlayer.body[0], snakeFood.position)){
            snakeFood.position = snakeFood.GenerateRandomPosition(snakePlayer.body);
            score++;
        }
    }

    void CheckSelfCollision(){
        unordered_set<long long> s;
        for(auto &p : snakePlayer.body){
            long long key = (long long)p.x * 100000 + (long long)p.y;
            s.insert(key);
        }
        if(s.size() != snakePlayer.body.size()){
            GameOver();
        }
    }

    void GameOver(){
        snakePlayer.Reset();
        snakeFood.position = snakeFood.GenerateRandomPosition(snakePlayer.body);
        running = false;
        highScore=max(score,highScore);
        score = 0;
    }
};

/* -------------------------------------------------
   ------------------ GAME MENU ---------------------
   ------------------------------------------------- */

enum Screen {
    SCREEN_MENU,
    SCREEN_SNAKE
};

int main() {

    InitWindow(2*offset + cellSize*cellCountx,
               2*offset + cellSize*cellCounty,
               "Game Hub");
    SetTargetFPS(60);

    Screen currentScreen = SCREEN_MENU;

    vector<string> gameList = {"Snake","tetris"};
    int selectedGame = 0;

    SnakeGame snakeGame;  // your snake object

    while(!WindowShouldClose()){

        BeginDrawing();
        ClearBackground(green);

        /* ------------------ HANDLE SCREENS ------------------ */

        if(currentScreen == SCREEN_MENU){

            DrawText("Select a Game", offset, 20, 40, darkGreen);

            for(int i=0; i<gameList.size(); i++)
            {
                int y = 100 + i * 50;

                // Highlight selected entry
                Color textColor = (i == selectedGame ? DARKGREEN : BLACK);
                DrawText(gameList[i].c_str(), offset + 30, y, 30, textColor);

                // Draw left-side triangle pointer for selected game
                if(i == selectedGame)
                {
                    Vector2 p1 = { (float)offset + 10, (float)y + 10 };
                    Vector2 p2 = { (float)offset + 10, (float)y + 30 };
                    Vector2 p3 = { (float)offset + 25, (float)y + 20 };

                    DrawTriangle(p1, p2, p3, DARKGREEN);
                }
            }

            // Navigation
            if(IsKeyPressed(KEY_DOWN)) selectedGame = (selectedGame + 1) % gameList.size();
            if(IsKeyPressed(KEY_UP)) selectedGame = (selectedGame - 1 + gameList.size()) % gameList.size();

            // Enter to start Snake
            if(IsKeyPressed(KEY_ENTER)){
                if(gameList[selectedGame] == "Snake"){
                    currentScreen = SCREEN_SNAKE;
                }
            }
        }

        else if(currentScreen == SCREEN_SNAKE){

            if(eventTriggered(0.1)){
                snakeGame.Update();
                snakeGame.directionChange = true;
            }
            if(snakeGame.running==0){
                DrawText(TextFormat("Press any arrow key to start"),
                     offset + 5, offset + 5 + 10, 40, darkGreen);
                
                     drawPause(offset + cellSize*cellCountx-65,offset+5,60,60,darkGreen,green);

                }

            if(IsKeyPressed(KEY_UP) && snakeGame.snakePlayer.direction.y != 1 && snakeGame.directionChange){
                snakeGame.snakePlayer.direction = {0,-1};
                snakeGame.running = true;
                snakeGame.directionChange = false;
            }
            if(IsKeyPressed(KEY_DOWN) && snakeGame.snakePlayer.direction.y != -1 && snakeGame.directionChange){
                snakeGame.snakePlayer.direction = {0,1};
                snakeGame.running = true;
                snakeGame.directionChange = false;
            }
            if(IsKeyPressed(KEY_RIGHT) && snakeGame.snakePlayer.direction.x != -1 && snakeGame.directionChange){
                snakeGame.snakePlayer.direction = {1,0};
                snakeGame.running = true;
                snakeGame.directionChange = false;
            }
            if(IsKeyPressed(KEY_LEFT) && snakeGame.snakePlayer.direction.x != 1 && snakeGame.directionChange){
                snakeGame.snakePlayer.direction = {-1,0};
                snakeGame.running = true;
                snakeGame.directionChange = false;
            }

            snakeGame.Draw();

            DrawRectangleLinesEx(Rectangle{(float)offset-5,(float)offset-5,
                (float)cellSize*cellCountx+10,(float)cellSize*cellCounty+10},5,darkGreen);

            DrawText("Retro Snake", offset - 5, 20, 40, darkGreen);
            DrawText(TextFormat("%i", snakeGame.score),
                     offset - 5, offset + cellCounty*cellSize + 10, 40, darkGreen);
            DrawText(TextFormat("high score %i", snakeGame.highScore),
                     (cellCountx*cellSize)/2+offset - 5, offset + cellCounty*cellSize + 10, 40, darkGreen);

            // ESC to return to menu
            if(IsKeyPressed(KEY_BACKSPACE)){
                currentScreen = SCREEN_MENU;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
