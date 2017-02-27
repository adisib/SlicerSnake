
#include "game.h"

namespace ssnake
{

SnakeGame::SnakeGame(Display* displayHandle)
{
    display = displayHandle;

    gameType = GM_NONE;
}



void SnakeGame::decreaseGameSpeed(unsigned int numberOfTimes)
{
    gameDelay += 0.0015*numberOfTimes;
}



void SnakeGame::increaseGameSpeed(unsigned int numberOfTimes)
{
    gameDelay -= 0.0015*numberOfTimes;
}



double SnakeGame::getGameDelay() const
{
    return gameDelay;
}



void SnakeGame::setGameDelay(double numSeconds)
{
    gameDelay = numSeconds;
}



void SnakeGame::processInputs(Snake* playerSnake)
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
            input.collectInput(-1);
        } while (!input.getEnter());

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

    Vec2 snakeStartingPos = {4, 3};
    snakeList.emplace_front(display, snakeStartingPos, 3, SNAKE_TEXTURE, SNAKE_HEAD_TEXTURE);
    Snake* playerSnake = &(snakeList.front());

    spawnFood();

    display->updateLengthCounter(playerSnake->getLength());

    display->update();

    std::chrono::steady_clock::time_point beginTime = std::chrono::steady_clock::now();
    while (alive)
    {
        double loopRunDuration = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - beginTime).count();
        input.collectInput(static_cast<int>((gameDelay-loopRunDuration)*1000));

        processInputs(playerSnake);
        beginTime = std::chrono::steady_clock::now();

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
    size_t maxLength = 0;

    Vec2 snakeStartingPos = {4, 3};
    snakeList.emplace_front(display, snakeStartingPos, 3, SNAKE_TEXTURE, SNAKE_HEAD_TEXTURE);
    Snake* playerSnake = &(snakeList.front());

    snakeStartingPos = {display->getSize_x() - 4, display->getSize_y() - 3};
    snakeList.emplace_front(display, snakeStartingPos, 3, SS_SNAKE_TEXTURE, SS_SNAKE_HEAD_TEXTURE);

    spawnFood();

    display->updateLengthCounter(playerSnake->getLength());
    display->updateMaxLengthCounter(maxLength);

    display->update();

    std::chrono::steady_clock::time_point beginTime = std::chrono::steady_clock::now();
    while (alive)
    {
        for (std::list<Snake>::iterator snakeIter = snakeList.begin(); snakeIter != snakeList.end(); ++snakeIter)
        {
            bool isPlayer = playerSnake == &(*snakeIter);

            if (isPlayer)
            {
                double loopRunDuration = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - beginTime).count();
                input.collectInput(static_cast<int>((gameDelay-loopRunDuration)*1000));

                processInputs(playerSnake);
                beginTime = std::chrono::steady_clock::now();
            }
            else
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
        food.x = (((rand() % (win.x - 2)) / 2) * 2) + 1;
        food.y = rand() % (win.y - 2) + 1;

        for (std::list<Snake>::iterator snakeIter = snakeList.begin(); snakeIter != snakeList.end(); ++snakeIter)
        {
            if (snakeIter->checkTouch(food))
            {
                foodBad = true;
                break;
            }
        }
        for (std::list<Vec2>::iterator it = foodList.begin(); it != foodList.end(); ++it)
        {
            if (food.x == it->x && food.y == it->y)
            {
                foodBad = true;
                break;
            }
        }
    } while (foodBad);

    foodList.push_back(food);
    display->drawTexture(FOOD_TEXTURE, food);

    return;
}

}