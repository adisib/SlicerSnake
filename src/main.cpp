
//
// SlicerSnake
// main.cpp
// https://github.com/adisib
//


#include <cstdlib>  // srand
#include <ctime> // srand(time(NULL))

#include "display.h"
#include "game.h"



// Allows the player to select a game mode
ssnake::Game_t gameSelectMenu(ssnake::Display* display, ssnake::PlayerInput& input);


int main()
{
    //

    std::srand(static_cast<unsigned int>(std::time(NULL)));

    ssnake::Display* display = new ssnake::Display(27, 30);

    bool play = true;
    while (play)
    {
        ssnake::PlayerInput input;
        ssnake::SnakeGame game(display);

        display->clearScreen();

        ssnake::Game_t gameTypeSelected = gameSelectMenu(display, input);

        game.startGame(gameTypeSelected);

        display->printTextLine(display->getSize_y() / 2 - 2, "GAME OVER");
        display->printTextLine(display->getSize_y() / 2 - 1, "R: Restart | Enter: Quit");
        display->update();

        do
        {
            input.collectInput(-1);
        } while (!input.getEnter() && !input.getRestart());
        if (!input.getRestart())
        {
            play = false;
        }
    }

    delete display;


    return 0;
}



ssnake::Game_t gameSelectMenu(ssnake::Display* display, ssnake::PlayerInput& input)
{
    bool initialSelect = true;
    ssnake::Game_t typeSelected;
    ssnake::DirectionalKey_t direction;

    display->printTextLine(7, "Arrow keys move, enter key pauses");

    do
    {
        if (initialSelect)
        {
            direction = ssnake::LEFT_KEY;
            typeSelected = ssnake::GM_SLICER;
            initialSelect = false;
        }
        else
        {
            input.collectInput(-1);
            direction = input.getDirection();
        }

        if (direction == ssnake::LEFT_KEY)
        {
            display->printTextLine(4, "Get as long as you can");
            display->printTextLine(5, "But don't let the other snake eat you!");

            typeSelected = ssnake::GM_SLICER;
            display->printTextLine(display->getSize_y() / 2, "[ Slicer Snake ]     Classic  ");
            display->update();
        }
        else if (direction == ssnake::RIGHT_KEY)
        {
            display->printTextLine(4, "Classic Snake");
            display->printTextLine(5, "Get the food but don't hit yourself!");

            typeSelected = ssnake::GM_CLASSIC;
            display->printTextLine(display->getSize_y() / 2, "  Slicer Snake     [ Classic ]");
            display->update();
        }
    } while (!input.getEnter() && !input.getQuit());

    return typeSelected;
}
