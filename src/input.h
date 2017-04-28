
// input.h
// Handles inputs using curses
//

#ifndef SLICERSNAKE_INPUT_H
#define SLICERSNAKE_INPUT_H

#include <chrono>
#include <thread> // sleep_for

#ifdef _WIN32
    #include "curses.h" // pdcurses for windows
#else
    #include <ncurses.h> // ncurses for linux (and whatever else it happens to work on)
#endif

namespace ssnake
{

enum DirectionalKey_t
{
    RIGHT_KEY, LEFT_KEY, UP_KEY, DOWN_KEY, NONE
};



class PlayerInput
{

public:

    PlayerInput();

    // PreConditions:
    // PostConditions:
    //   If duration_ms >= 0, collect input for duration_ms milliseconds
    //   If duration_ms < 0, wait indefinitely for next input
    void collectInput(int duration_ms);

    // PreConditions:
    // PostConditions:
    //   Returns the most recent directional keypress
    DirectionalKey_t getDirection() const;

    // PreConditions:
    // PostConditions:
    //   Returns the direction that was previous to the current one (only changes if newest direction changes)
    DirectionalKey_t getPrevDirection() const;

    // PreConditions:
    // PostConditions:
    //   Returns true if the user requested to pause during previous collection input, else returns false
    bool getPause() const;

    // PreConditions:
    // PostConditions:
    //   Returns true if the user requested to restart during previous collection input, else returns false
    bool getRestart() const;

    // PreConditions:
    // PostConditions:
    //   Returns true if the user requested to quit during previous collection input, else returns false
    bool getQuit() const;

    // PreConditions:
    // PostConditions:
    //   Returns true if the user pressed enter/acknowledge/activate during previous collection input, else returns false
    bool getEnter() const;

private:

    void clearInputs();
    static void initCurses();

    DirectionalKey_t direction = NONE;
    DirectionalKey_t prevDirection = NONE;
    bool pause = false;
    bool restart = false;
    bool quit = false;
    bool enter = false;

};

}

#endif
