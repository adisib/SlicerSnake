
// game.h
// Game
//

#ifndef SLICERSNAKE_GAME_H
#define SLICERSNAKE_GAME_H


#include <chrono>
#include <list>

#include "display.h"
#include "snake.h"
#include "input.h"


namespace ssnake
{

enum Game_t
{
    GM_SLICER, GM_CLASSIC, GM_NONE
};



class SnakeGame
{

public:

    // PreConditions:
    //   displayHandle points to an active display
    // PostConditions:
    //   A game becomes ready to be started using that display
    explicit SnakeGame(Display* displayHandle);

    // PreConditions:
    // PostConditions:
    //   Game speed is decreased the specified number of times (delay increases)
    void decreaseGameSpeed(unsigned int numTimes = 1);

    // PreConditions:
    // PostConditions:
    //   Game speed is increased the specified number of times (delay decreases)
    void increaseGameSpeed(unsigned int numTimes = 1);

    // PreConditions:
    // PostConditions:
    //   Returns game delay in seconds
    double getGameDelay() const;

    // PreConditions:
    // PostConditions:
    //   Game delay is set to the specified number of seconds
    void setGameDelay(double numSeconds);

    // PreConditions:
    // PostConditions:
    //   Begins a new game of the specified type
    void startGame(Game_t newGameType);

    // PreConditions:
    // PostConditions:
    //   Spawns a food somewhere on the game field, but never on top of a snake or other food.
    void spawnFood();



private:

    void runNewClassicGame();
    void runNewSlicerGame();

    void processInputs(Snake* playerSnake);

    Display* display;
    PlayerInput input;

    // delay of game loop steps in seconds
    double gameDelay = 0.0;

    // Snake list holds all snakes, including the player snake
    std::list<Snake> snakeList;
    std::list<Vec2> foodList;

    bool alive = false;
    Game_t gameType = GM_NONE;

};

}

#endif
