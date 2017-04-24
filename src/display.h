
// display.h
// Handles display messily using curses backend
//

#ifndef SLICERSNAKE_DISPLAY_H
#define SLICERSNAKE_DISPLAY_H


#include <cstdlib> // system (windows)
#include <cassert>
#include <cstring> // strlen

#ifdef _WIN32
    #include "curses.h" // pdcurses for windows
#else
    #include <ncurses.h> // ncurses for linux (and whatever else it happens to work on)
#endif


namespace ssnake
{

typedef int coordType;
typedef WINDOW* PWINDOW;



struct Vec2
{
    coordType x;
    coordType y;
};



// Texture names
// TEXTURE_COUNT is a sentinel that indicates how many textures there are, and is not the name of a texture
enum Texture_t
{
    SNAKE_TEXTURE = 1, SNAKE_HEAD_TEXTURE, SS_SNAKE_TEXTURE, SS_SNAKE_HEAD_TEXTURE,
    FOOD_TEXTURE, COLLISION_TEXTURE, BORDER_TEXTURE, GAME_TEXT_TEXTURE, BACKGROUND_TEXTURE,
    TEXTURE_COUNT
};



struct SnakeTextureList
{
    Texture_t head;
    Texture_t body;
    Texture_t tail;
};



class Display
{

public:

    // PreConditions:
    //   X and Y should be smaller than the actual window
    //   X and Y should be big enough to not wrap the text
    //   Y should be larger than x to allow for game messages
    // PostConditions:
    //   A Display is created with X and Y size (units of "snake chunks")
    // Size might later be difficulty depdendant?
    Display() : Display(27, 30) {};
    Display(const coordType size_x, const coordType size_y);
    ~Display();

    // PreConditions:
    // PostConditions:
    //   The screen is reset to initial state
    void clearScreen();

    // PreConditions:
    // PostConditions:
    //   Returns the size of the game window (inside the snake border) (units of "snake chunks")
    coordType getSize_x() const;
    coordType getSize_y() const;

    // PreConditions:
    //   texture is a valid texture name
    // PostConditions:
    //   Places a texture at the specified position in the display
    void drawTexture(Texture_t texture, const Vec2& pos);

    // PreConditions:
    // PostConditions:
    //   A snake head with headTexture texture is moved from oldPos to newPos
    //   A snake body with bodyTexture texture is moved into oldPos
    void moveSnakeHead(const Vec2& oldPos, const Vec2& newPos, const SnakeTextureList& snakeTextures);

    // PreConditions:
    // PostConditions:
    //   A snake body with bodyTexture texture is moved from oldPos into newPos
    void moveSnakeBody(const Vec2& oldPos, const Vec2& newPos, const SnakeTextureList& snakeTextures);

    // PreConditions:
    // PostConditions:
    //   A snake tail with a texture as its texture is moved into newPos
    //   A snake tail is moved out of oldPos
    void moveSnakeTail(const Vec2& oldPos, const Vec2& newPos, const SnakeTextureList& snakeTextures);

    // PreConditions:
    //   There is enough space on a line of the display to fit message
    // PostConditions:
    //   Displays a centered line of text on the specified line of the message screen (in snake area)
    void printTextLine(unsigned int lineNumber, const char* message);

    // PreConditions:
    //   There is enough space between length labels to fit message
    // PostConditions:
    //   Displays a small one-line text message outside of the snake area (bottom center of screen)
    void printGameMessage(const char* message);
    void clearGameMessage();

    // PreConditions:
    // PostConditions:
    //   Updates display if necessary
    void update();

    // PreConditions:
    // PostConditions:
    //   Updates the length counter with newLength
    void updateLengthCounter(std::size_t newLength);

    // PreConditions:
    // PostConditions:
    //   Updates the max length counter with maxLength
    void updateMaxLengthCounter(std::size_t maxLength);


private:
    // curses back-end

    // Initialize curses back-end
    void initCurses();

    // Resizes console if necessary and creates the snakeWin and gameWin windows.
    // If the windows already exist, reset them to the initial static text and border.
    void initScreen(coordType size_x, coordType size_y);

    // Initialize textures and curses color definitions
    void setTextures();

    // global size of application window
    Vec2 ScreenSize;

    // Bordered window that contains the snakes
    PWINDOW snakeWin = nullptr;
    // Textbox located under the window
    PWINDOW gameWin = nullptr;
    // Window for outputing text, for menu or instructions (same as snakeWin for now)
    PWINDOW messageWin = nullptr;

    // A window is modified if it called curses functions directly and needs curses to therefore update that window
    bool snakeWinModified = true;
    bool gameWinModified = true;
    bool messageWinModified = true;

    // List of loaded textures (chtypes in curses)
    // gameTextures holds colored characters, so that you can use addchstr(textures[Texture_t])
    // If only want to set the color of custom text, use attron(COLOR_PAIR(Texture_t)), since textures have colors as well
    chtype** gameTextures = nullptr;

    // Size of empty space from game content and window edges
    const unsigned int windowPadding = 1;
    // Amount of vertical space alloted for Game Text
    const unsigned int gameTextLines = 1;

    const char* lengthLabel = "Length: ";
    const char* maxLengthLabel = "Max Length: ";
};

}

#endif
