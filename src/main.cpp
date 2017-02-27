
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

    ssnake::Display* display = new ssnake::Display();

    bool play = true;
    while (play)
    {
        ssnake::PlayerInput input;
        ssnake::SnakeGame game(display);

        display->clearScreen();

        // Instructions
        display->printTextLine(4, "Get as long as you can");
        display->printTextLine(5, "But don't let the other snake eat you!");
        display->update();

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
    ssnake::Game_t typeSelected = ssnake::GM_SLICER;
    display->printTextLine(display->getSize_y() / 2, "[ Slicer Snake ]     Classic  ");
    display->update();

    do
    {
        input.collectInput(-1);

        if (input.getDirection() == ssnake::LEFT_KEY)
        {
            typeSelected = ssnake::GM_SLICER;
            display->printTextLine(display->getSize_y() / 2, "[ Slicer Snake ]     Classic  ");
            display->update();
        }
        else if (input.getDirection() == ssnake::RIGHT_KEY)
        {
            typeSelected = ssnake::GM_CLASSIC;
            display->printTextLine(display->getSize_y() / 2, "  Slicer Snake     [ Classic ]");
            display->update();
        }
    } while (!input.getEnter() && !input.getQuit());

    return typeSelected;
}
