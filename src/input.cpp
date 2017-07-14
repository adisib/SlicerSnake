
#include "input.h"

#include <chrono>
#include <thread> // sleep_for

#ifdef _WIN32
    #include "curses.h" // pdcurses for windows
#else
    #include <ncurses.h> // ncurses for linux (and whatever else it happens to work on)
#endif

namespace ssnake
{

PlayerInput::PlayerInput()
{
    initCurses();
}



void PlayerInput::clearInputs()
{
    pause = restart = quit = enter = false;
}



void PlayerInput::collectInput(int duration)
{
    clearInputs();

    prevDirection = direction;

    if (duration < 0)
    {
        nodelay(stdscr, FALSE);
    }
    else
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));
    }

    int input;
    do
    {
        input = getch();

        switch (input)
        {
            case (KEY_LEFT) :
            case ('a') :
            case ('A') :
                if (direction != LEFT_KEY)
                {
                    prevDirection = direction;
                    direction = LEFT_KEY;
                }
                break;

            case (KEY_RIGHT) :
            case ('d') :
            case ('D') :
                if (direction != RIGHT_KEY)
                {
                    prevDirection = direction;
                    direction = RIGHT_KEY;
                }
                break;

            case (KEY_UP) :
            case ('w') :
            case ('W') :
                if (direction != UP_KEY)
                {
                    prevDirection = direction;
                    direction = UP_KEY;
                }
                break;

            case (KEY_DOWN) :
            case ('s') :
            case ('S') :
                if (direction != DOWN_KEY)
                {
                    prevDirection = direction;
                    direction = DOWN_KEY;
                }
                break;

            case ('q') :
            case ('Q') :
            case (KEY_EXIT) :
            case (KEY_END) :
                quit = true;
                break;

            case (KEY_ENTER) :
            case ('\n') :
            case ('\r') :
                enter = true;
                pause = true;
                break;

            case ('r') :
            case ('R') :
            case (KEY_RESTART) :
                restart = true;

            default:
                break;
        }

    } while (duration > 0 && input != ERR);

    if (duration <= 0)
    {
        nodelay(stdscr, TRUE);
    }
}



DirectionalKey_t PlayerInput::getDirection() const
{
    return direction;
}



DirectionalKey_t PlayerInput::getPrevDirection() const
{
    return prevDirection;
}



bool PlayerInput::getPause() const
{
    return pause;
}



bool PlayerInput::getRestart() const
{
    return restart;
}



bool PlayerInput::getQuit() const
{
    return quit;
}



bool PlayerInput::getEnter() const
{
    return enter;
}



void PlayerInput::initCurses()
{
    /*
    // Not sure of any proper way to check if initscr() has already been called.
    // Display (using curses) will always init curses first, so don't bother
    // adding mess to the code to add a boolean for this for now
    if (curscr != NULL)
    {
        initscr();
        refresh();
    }
    */

    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    cbreak();
}

}