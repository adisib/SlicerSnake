
#include "game.h"

#include <chrono>
#include <list>
#include <thread> // sleep_until

#include "display.h"
#include "input.h"
#include "snake.h"

namespace ssnake
{

SnakeGame::SnakeGame(Display* displayHandle)
{
    display = displayHandle;
}



void SnakeGame::decreaseGameSpeed(unsigned int numberOfTimes)
{
    gameDelay += 0.0015 * numberOfTimes;
}



void SnakeGame::increaseGameSpeed(unsigned int numberOfTimes)
{
    gameDelay -= 0.0015 * numberOfTimes;
}



double SnakeGame::getGameDelay() const
{
    return gameDelay;
}



void SnakeGame::setGameDelay(double numSeconds)
{
    gameDelay = numSeconds;
}



void SnakeGame::processInputs(std::chrono::steady_clock::time_point& beginTime, Snake* playerSnake)
{
    if (input.getQuit())
    {
        alive = false;
    }

    Direction_t oldDir = playerSnake->getDirection();
    Direction_t newDir;
    switch (input.getDirection())
    {
        case (LEFT_KEY) :
            newDir = LEFT;
            break;
        case (RIGHT_KEY) :
            newDir = RIGHT;
            break;
        case (UP_KEY) :
            newDir = UP;
            break;
        case (DOWN_KEY) :
            newDir = DOWN;
            break;
        default:
            newDir = oldDir;
            break;
    }
    playerSnake->setDirection(newDir);

    // Use older input if the player has an invalid input as the most recent
    if (playerSnake->getDirection() == oldDir)
    {
        switch (input.getPrevDirection())
        {
            case (LEFT_KEY) :
                newDir = LEFT;
                break;
            case (RIGHT_KEY) :
                newDir = RIGHT;
                break;
            case (UP_KEY) :
                newDir = UP;
                break;
            case (DOWN_KEY) :
                newDir = DOWN;
                break;
            default:
                break;
        }
        playerSnake->setDirection(newDir);
    }

    if (input.getPause())
    {
        display->printGameMessage("Paused");

        do
        {
            input.collectInput();
        } while (!input.getEnter());
        beginTime = std::chrono::steady_clock::now();

        display->clearGameMessage();
    }
}



void SnakeGame::startGame(Game_t newGameType)
{
    gameType = newGameType;

    snakeList.clear();
    foodList.clear();

    alive = true;

    display->clearScreen();

    switch (newGameType)
    {
        case GM_SLICER:
        runNewSlicerGame();
        break;

        case GM_CLASSIC:
        runNewClassicGame();
        break;

        default:
        break;
    }
}



void SnakeGame::runNewClassicGame()
{
    setGameDelay(0.09);

    SnakeTextureList textures;
    Vec2 snakeStartingPos = {4, 3};
    textures.head = TEXTURE_SNAKE_HEAD; textures.body = TEXTURE_SNAKE; textures.tail = TEXTURE_SNAKE;
    snakeList.emplace_front(display, textures, snakeStartingPos, 3);
    Snake* playerSnake = &(snakeList.front());

    spawnFood();

    display->updateLengthCounter(playerSnake->getLength());

    display->update();

    std::chrono::steady_clock::time_point beginTime = std::chrono::steady_clock::now();
    while (alive)
    {
        int msDelay = static_cast<int>(getGameDelay() * 1000);
        std::this_thread::sleep_until(beginTime + std::chrono::milliseconds(msDelay));
        beginTime = std::chrono::steady_clock::now();

        input.updateInputs();

        processInputs(beginTime, playerSnake);

        playerSnake->move();

        for (std::list<Vec2>::iterator it = foodList.begin(); it != foodList.end(); ++it)
        {
            if (playerSnake->checkFood(*it))
            {
                increaseGameSpeed();

                display->updateLengthCounter(playerSnake->getLength());

                foodList.erase(it);
                spawnFood();
                break;
            }
        }

        alive = alive && !(playerSnake->checkCollision());

        display->update();
    }
}



void SnakeGame::runNewSlicerGame()
{
    setGameDelay(0.09);

    SnakeTextureList textures;
    Vec2 snakeStartingPos = {4, 3};
    textures.head = TEXTURE_SNAKE_HEAD; textures.body = TEXTURE_SNAKE; textures.tail = TEXTURE_SNAKE;
    snakeList.emplace_front(display, textures, snakeStartingPos, 3);
    Snake* playerSnake = &(snakeList.front());

    snakeStartingPos = {display->getSize_x() - 4, display->getSize_y() - 3};
    textures.head = TEXTURE_SS_SNAKE_HEAD; textures.body = TEXTURE_SS_SNAKE; textures.tail = TEXTURE_SS_SNAKE;
    snakeList.emplace_front(display, textures, snakeStartingPos, 3);

    spawnFood();

    size_t maxLength = playerSnake->getLength();
    display->updateLengthCounter(playerSnake->getLength());
    display->updateMaxLengthCounter(maxLength);

    display->update();

    bool isPlayer;
    std::chrono::steady_clock::time_point beginTime = std::chrono::steady_clock::now();
    while (alive)
    {
        int msDelay = static_cast<int>(getGameDelay() * 1000);
        std::this_thread::sleep_until(beginTime + std::chrono::milliseconds(msDelay));
        beginTime = std::chrono::steady_clock::now();

        input.updateInputs();
        processInputs(beginTime, playerSnake);

        for (std::list<Snake>::iterator snakeIter = snakeList.begin(); snakeIter != snakeList.end(); ++snakeIter)
        {
            isPlayer = playerSnake == &(*snakeIter);

            if (!isPlayer)
            {
                snakeIter->ai_getDirection(foodList);
            }
            snakeIter->move();

            int n = snakeIter->checkSlice(snakeList);
            if (n > 0)
            {
                decreaseGameSpeed(n);
                display->updateLengthCounter(playerSnake->getLength());
            }

            if (!isPlayer)
            {
                if (snakeIter->checkCollision())
                {
                    std::list<Snake>::iterator deadSnake = snakeIter--;
                    snakeList.erase(deadSnake);
                    continue;
                }
            }

            for (std::list<Vec2>::iterator it = foodList.begin(); it != foodList.end(); ++it)
            {
                if (snakeIter->checkFood(*it))
                {
                    increaseGameSpeed();

                    if (isPlayer)
                    {
                        display->updateLengthCounter(playerSnake->getLength());
                        if (playerSnake->getLength() > maxLength)
                        {
                            maxLength = playerSnake->getLength();
                            display->updateMaxLengthCounter(maxLength);
                        }
                    }

                    foodList.erase(it);
                    spawnFood();
                    break;
                }
            }
        }

        alive = alive && !(playerSnake->checkCollision());

        display->update();
    }
}



void SnakeGame::spawnFood()
{
    Vec2 win = {display->getSize_x(), display->getSize_y()};
    Vec2 food;

    // don't spawn food on snakes or other food
    bool foodBad;
    do
    {
        foodBad = false;
        food.x = rand() % (win.x - 2) + 1;
        food.y = rand() % (win.y - 2) + 1;

        for (std::list<Snake>::const_iterator snakeIter = snakeList.cbegin(); snakeIter != snakeList.cend(); ++snakeIter)
        {
            if (snakeIter->checkTouch(food))
            {
                foodBad = true;
                break;
            }
        }
        for (std::list<Vec2>::const_iterator it = foodList.cbegin(); it != foodList.cend(); ++it)
        {
            if (food.x == it->x && food.y == it->y)
            {
                foodBad = true;
                break;
            }
        }
    } while (foodBad);

    foodList.push_back(food);
    display->drawTexture(TEXTURE_FOOD, food);

    return;
}

}