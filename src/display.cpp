
#include "display.h"

#include <cstdlib> // size_t, system (windows)
#include <cassert>
#include <cstring> // strlen

#ifdef _WIN32
    #include "curses.h" // pdcurses for windows
#else
    #include <ncurses.h> // ncurses for linux (and whatever else it happens to work on)
#endif


namespace ssnake
{

Display::Display(const coordType size_x, const coordType size_y)
{
    initCurses();
    setTextures();
    initScreen(size_x * 2, size_y);

#ifdef _WIN32
    system("title SlicerSnake");
#endif
}



Display::~Display()
{
    if (messageWin != nullptr)
    {
        delwin(messageWin);
    }
    if (snakeWin != nullptr)
    {
        delwin(snakeWin);
    }
    if (gameWin != nullptr)
    {
        delwin(gameWin);
    }

    clear();
    refresh();

    endwin();

    if (gameTextures != nullptr)
    {
        if (gameTextures[0] != nullptr)
        {
            delete gameTextures[0];
        }
        delete gameTextures;
    }
}



void Display::clearGameMessage()
{
    std::size_t lengthLabelSize = std::strlen(lengthLabel) + windowPadding + 3;
    std::size_t maxLengthLabelSize = std::strlen(maxLengthLabel) + windowPadding + 3;
    unsigned int endPos = ScreenSize.x - maxLengthLabelSize - 1;

    wmove(gameWin, 0, lengthLabelSize);
    for (unsigned int i = lengthLabelSize; i < endPos; ++i)
    {
        waddch(gameWin, ' ');
    }

    gameWinModified = true;
}



void Display::clearScreen()
{
    wclear(snakeWin);
    wclear(gameWin);
    wclear(messageWin);

    wattron(snakeWin, COLOR_PAIR(COLORS_CYAN));
    box(snakeWin, 0, 0);
    wattroff(snakeWin, COLOR_PAIR(COLORS_CYAN));

    snakeWinModified = true;
    gameWinModified = true;
    messageWinModified = true;

    // Clear screen immediately
    update();
}



void Display::drawTexture(Texture_t texture, const Vec2& pos)
{
    mvwaddchnstr(snakeWin, pos.y, pos.x * 2, gameTextures[texture], 2);

    snakeWinModified = true;
}



coordType Display::getSize_x() const
{
    return getmaxx(snakeWin) / 2;
}



coordType Display::getSize_y() const
{
    return getmaxy(snakeWin);
}



void Display::initCurses()
{
    /*
    if (curscr != NULL)
    {
        initscr();
        refresh();
    }
    */
    initscr();
    start_color();
    leaveok(stdscr, TRUE);
    refresh();

    curs_set(0);
}



void Display::initScreen(coordType size_x, coordType size_y)
{
    ScreenSize.x = size_x;
    ScreenSize.y = size_y;

#ifdef _WIN32
    // Ensure window is large enough for requested display size on windows
    // Since window users generally want things to work more than to have control
    if (ScreenSize.y > getmaxy(stdscr) || ScreenSize.x > getmaxx(stdscr))
    {
        int err = resize_term(ScreenSize.y, ScreenSize.x);
        assert(err == OK);
        refresh();
    }
#endif
    if (ScreenSize.y > getmaxy(stdscr) || ScreenSize.x > getmaxx(stdscr))
    {
        // probably warning or exit here, but for now do nothing
    }

    if (snakeWin != nullptr)
    {
        wclear(snakeWin);
    }
    else
    {
        snakeWin = newpad(ScreenSize.y - (gameTextLines + windowPadding * 2), ScreenSize.x - (windowPadding * 2));
    }
    if (gameWin != nullptr)
    {
        wclear(gameWin);
    }
    else
    {
        gameWin = newpad(gameTextLines, ScreenSize.x - (windowPadding * 2));
    }
    if (messageWin != nullptr)
    {
        wclear(messageWin);
    }
    else
    {
        // We have to make it slightly smaller here for Windows builds with pdcurses, because pdcurses doesn't allow it to be the same size
        messageWin = subpad(snakeWin, ScreenSize.y - (gameTextLines + windowPadding * 2) - 2, ScreenSize.x - (windowPadding * 2) - 2, 1, 1);
    }

    // gameWin and messageWin always uses the same color for now
    // But don't set always for messageWin because is a subPad
    wattron(gameWin, COLOR_PAIR(COLORS_RED));

    wattron(snakeWin, COLOR_PAIR(COLORS_CYAN));
    box(snakeWin, 0, 0);
    wattroff(snakeWin, COLOR_PAIR(COLORS_CYAN));

    snakeWinModified = true;
    gameWinModified = true;
    messageWinModified = true;
    update();

    return;
}



void Display::moveSnakeHead(const Vec2& oldPos, const Vec2& newPos, const SnakeTextureList& snakeTextures)
{
    drawTexture(snakeTextures.body, oldPos);

    drawTexture(snakeTextures.head, newPos);
}



void Display::moveSnakeBody(const Vec2& oldPos, const Vec2& newPos, const SnakeTextureList& snakeTextures)
{
    // unused in curses implementation
}



void Display::moveSnakeTail(const Vec2& oldPos, const Vec2& newPos, const SnakeTextureList& snakeTextures)
{
    drawTexture(snakeTextures.tail, newPos);

    // Don't overwrite anything else when clearing old tail
    chtype prevChar = mvwinch(snakeWin, oldPos.y, oldPos.x * 2);
    if (prevChar == gameTextures[snakeTextures.tail][0])
    {
        drawTexture(TEXTURE_BACKGROUND, oldPos);
    }
}



void Display::printTextLine(unsigned int lineNumber, const char* message)
{
    int lineNum = static_cast<int>(lineNumber);
    if (lineNum > ScreenSize.y)
    {
        return;
    }

    std::size_t size = std::strlen(message);
    int maxTextLength = getmaxx(messageWin);

    for (int i = 0; i < maxTextLength; ++i)
    {
        chtype curChar = mvwinch(messageWin, lineNumber, i);
        if ((curChar & A_COLOR) == COLOR_PAIR(COLORS_RED))
        {
            waddch(messageWin, ' ');
        }
    }
    wattron(messageWin, COLOR_PAIR(COLORS_RED));
    mvwaddnstr(messageWin, lineNum, (getmaxx(messageWin) / 2) - (size / 2), message, maxTextLength);
    wattroff(messageWin, COLOR_PAIR(COLORS_RED));

    messageWinModified = true;
}



void Display::printGameMessage(const char* message)
{
    clearGameMessage();

    std::size_t size = std::strlen(message);
    std::size_t maxGameTextLength = 2 * ((getmaxx(gameWin) / 2) - (std::strlen(maxLengthLabel) + windowPadding + 2) - 1);
    if (size > maxGameTextLength)
    {
        size = maxGameTextLength;
    }

    mvwaddnstr(gameWin, 0, (getmaxx(gameWin) / 2) - (size / 2), message, static_cast<int>(maxGameTextLength));

    gameWinModified = true;
    // Game messages should be printed immediately
    update();
}



void Display::setTextures()
{
    init_pair(COLORS_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLORS_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLORS_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLORS_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLORS_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLORS_BLACK, COLOR_BLACK, COLOR_BLACK);
    if (can_change_color() && COLORS >= 256 && COLOR_PAIRS >= 16)
    {
        init_color(9, 40, 1000, 90);
        init_color(10, 900, 70, 1000);

        init_pair(COLORS_ALT_GREEN, 9, COLOR_BLACK);
        init_pair(COLORS_ALT_MAGENTA, 10, COLOR_BLACK);
    }
    else
    {
        init_pair(COLORS_ALT_GREEN, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLORS_ALT_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    }

    const chtype missingTexture = '?';

    gameTextures = new chtype*[TEXTURE_COUNT];
    gameTextures[0] = new chtype[TEXTURE_COUNT * 2];
    for (int i = 1; i < TEXTURE_COUNT; ++i)
    {
        gameTextures[i] = *gameTextures + (2 * i);
        gameTextures[i][1] = gameTextures[i][0] = missingTexture;
    }

    gameTextures[TEXTURE_SNAKE][0]         = '(' | COLOR_PAIR(COLORS_GREEN);
    gameTextures[TEXTURE_SNAKE][1]         = ')' | COLOR_PAIR(COLORS_GREEN);
    gameTextures[TEXTURE_SNAKE_HEAD][0]    = '<' | COLOR_PAIR(COLORS_ALT_GREEN);
    gameTextures[TEXTURE_SNAKE_HEAD][1]    = '>' | COLOR_PAIR(COLORS_ALT_GREEN);
    gameTextures[TEXTURE_SS_SNAKE][0]      = '(' | COLOR_PAIR(COLORS_MAGENTA);
    gameTextures[TEXTURE_SS_SNAKE][1]      = ')' | COLOR_PAIR(COLORS_MAGENTA);
    gameTextures[TEXTURE_SS_SNAKE_HEAD][0] = '<' | COLOR_PAIR(COLORS_ALT_MAGENTA);
    gameTextures[TEXTURE_SS_SNAKE_HEAD][1] = '>' | COLOR_PAIR(COLORS_ALT_MAGENTA);
    gameTextures[TEXTURE_FOOD][0]          = '{' | COLOR_PAIR(COLORS_YELLOW);
    gameTextures[TEXTURE_FOOD][1]          = '}' | COLOR_PAIR(COLORS_YELLOW);
    gameTextures[TEXTURE_COLLISION][0]     = '*' | COLOR_PAIR(COLORS_RED);
    gameTextures[TEXTURE_COLLISION][1]     = '*' | COLOR_PAIR(COLORS_RED);
    gameTextures[TEXTURE_BACKGROUND][0]    = ' ';
    gameTextures[TEXTURE_BACKGROUND][1]    = ' ';
}



void Display::updateLengthCounter(std::size_t length)
{
    mvwaddstr(gameWin, 0, windowPadding, lengthLabel);

    std::size_t lengthLabelSize = std::strlen(lengthLabel);

    mvwprintw(gameWin, 0, lengthLabelSize + windowPadding, "%-2u", length);

    gameWinModified = true;
}



void Display::updateMaxLengthCounter(std::size_t maxLength)
{
    std::size_t lengthLabelSize = std::strlen(maxLengthLabel);
    mvwaddstr(gameWin, 0, getmaxx(gameWin) - (lengthLabelSize + windowPadding + 2), maxLengthLabel);

    mvwprintw(gameWin, 0, getmaxx(gameWin) - (windowPadding + 2), "%-2u", maxLength);

    gameWinModified = true;
}



void Display::update()
{
    if ( !(snakeWinModified || gameWinModified || messageWinModified) )
    {
        return;
    }

    if (snakeWinModified)
    {
        pnoutrefresh(snakeWin, 0, 0,
                     windowPadding, windowPadding,
                     ScreenSize.y - (gameTextLines + windowPadding), ScreenSize.x - windowPadding);
    }
    if (messageWinModified)
    {
        pnoutrefresh(messageWin, 0, 0,
                     windowPadding + 1, windowPadding + 1,
                     ScreenSize.y - (gameTextLines + windowPadding) - 1, ScreenSize.x - windowPadding - 1);
    }
    if (gameWinModified)
    {
        pnoutrefresh(gameWin, 0, 0,
                     ScreenSize.y - (gameTextLines + windowPadding), windowPadding,
                     ScreenSize.y - windowPadding, ScreenSize.x - windowPadding);
    }

    doupdate();

    snakeWinModified = gameWinModified = messageWinModified = false;
}

}