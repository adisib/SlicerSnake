
// input.h
// Handles inputs using curses
//

#ifndef SLICERSNAKE_INPUT_H
#define SLICERSNAKE_INPUT_H


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
    //   Updates state to reflect inputs since last update
    void updateInputs();

    // PreConditions:
    // PostConditions:
    //   Collects an input, then updates state to reflect inputs since last update
    void collectInput();

    // PreConditions:
    // PostConditions:
    //   Returns the most recent directional keypress
    DirectionalKey_t getDirection() const;

    // PreConditions:
    // PostConditions:
    //   Returns the direction input that was previous to the current one
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
    void getInput(bool once);
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
