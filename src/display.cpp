
#include "display.h"

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
    if (snakeWin != nullptr)
    {
        delwin(snakeWin);
    }
    if (gameWin != nullptr)
    {
        delwin(gameWin);
    }
    if (messageWin != nullptr)
    {
        delwin(messageWin);
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
}



void Display::clearScreen()
{
    wclear(snakeWin);
    wclear(gameWin);
    wclear(messageWin);

    wattron(snakeWin, COLOR_PAIR(BORDER_TEXTURE));
    box(snakeWin, 0, 0);
    wattroff(snakeWin, COLOR_PAIR(BORDER_TEXTURE));

    wnoutrefresh(snakeWin);
    wnoutrefresh(gameWin);
    wnoutrefresh(messageWin);

    // Clear screen immediately
    doupdate();
}



void Display::drawTexture(Texture_t texture, const Vec2& pos)
{
    mvwaddchnstr(snakeWin, pos.y, pos.x * 2, gameTextures[texture], 2);
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
        messageWin = subpad(snakeWin, ScreenSize.y - (gameTextLines + windowPadding * 2), ScreenSize.x - (windowPadding * 2), 0, 0);
    }

    // gameWin and messageWin always uses the same color for now
    wattron(gameWin, COLOR_PAIR(GAME_TEXT_TEXTURE));
    wattron(messageWin, COLOR_PAIR(GAME_TEXT_TEXTURE));

    wattron(snakeWin, COLOR_PAIR(BORDER_TEXTURE));
    box(snakeWin, 0, 0);
    wattroff(snakeWin, COLOR_PAIR(BORDER_TEXTURE));

    wnoutrefresh(snakeWin);
    wnoutrefresh(gameWin);
    wnoutrefresh(messageWin);

    return;
}



void Display::moveSnakeHead(const Vec2& oldPos, const Vec2& newPos, const SnakeTextureList& snakeTextures)
{
    mvwaddchnstr(snakeWin, oldPos.y, oldPos.x * 2, gameTextures[snakeTextures.body], 2);

    mvwaddchnstr(snakeWin, newPos.y, newPos.x * 2, gameTextures[snakeTextures.head], 2);
}



void Display::moveSnakeBody(const Vec2& oldPos, const Vec2& newPos, const SnakeTextureList& snakeTextures)
{
    // unused in curses implementation
}



void Display::moveSnakeTail(const Vec2& oldPos, const Vec2& newPos, const SnakeTextureList& snakeTextures)
{
    mvwaddchnstr(snakeWin, newPos.y, newPos.x * 2, gameTextures[snakeTextures.tail], 2);

    // Don't overwrite anything else when clearing old tail
    chtype prevChar = mvwinch(snakeWin, oldPos.y, oldPos.x * 2);
    if (prevChar == gameTextures[snakeTextures.tail][0])
    {
        mvwaddchnstr(snakeWin, oldPos.y, oldPos.x * 2, gameTextures[BACKGROUND_TEXTURE], 2);
    }
}



void Display::printTextLine(unsigned int lineNumber, const char* message)
{
    std::size_t size = std::strlen(message);

    std::size_t maxTextLength = ScreenSize.x - (windowPadding * 2) - 2;

    mvwaddnstr(messageWin, lineNumber, (getmaxx(messageWin) / 2) - (size / 2), message, static_cast<int>(maxTextLength));
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

    // Game messages should be printed immediately
    update();
}



void Display::setTextures()
{
    init_pair(SNAKE_TEXTURE, COLOR_GREEN, COLOR_BLACK);
    init_pair(SS_SNAKE_TEXTURE, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(FOOD_TEXTURE, COLOR_YELLOW, COLOR_BLACK);
    init_pair(BORDER_TEXTURE, COLOR_CYAN, COLOR_BLACK);
    init_pair(GAME_TEXT_TEXTURE, COLOR_RED, COLOR_BLACK);
    init_pair(COLLISION_TEXTURE, COLOR_RED, COLOR_BLACK);
    init_pair(BACKGROUND_TEXTURE, COLOR_BLACK, COLOR_BLACK);
    if (can_change_color() && COLORS>=256)
    {
        init_color(9, 40, 1000, 90);
        init_color(10, 900, 70, 1000);

        init_pair(SNAKE_HEAD_TEXTURE, 9, COLOR_BLACK);
        init_pair(SS_SNAKE_HEAD_TEXTURE, 10, COLOR_BLACK);
    }
    else
    {
        init_pair(SNAKE_HEAD_TEXTURE, COLOR_GREEN, COLOR_BLACK);
        init_pair(SS_SNAKE_HEAD_TEXTURE, COLOR_MAGENTA, COLOR_BLACK);
    }

    const chtype missingTexture = '?';

    gameTextures = new chtype*[TEXTURE_COUNT];
    gameTextures[0] = new chtype[TEXTURE_COUNT*2];
    for (int i=1; i < TEXTURE_COUNT; ++i)
    {
        gameTextures[i] = *gameTextures + (2*i);
        gameTextures[i][1] = gameTextures[i][0] = missingTexture;
    }

    gameTextures[SNAKE_TEXTURE][0]         = '(' | COLOR_PAIR(SNAKE_TEXTURE);
    gameTextures[SNAKE_TEXTURE][1]         = ')' | COLOR_PAIR(SNAKE_TEXTURE);
    gameTextures[SNAKE_HEAD_TEXTURE][0]    = '<' | COLOR_PAIR(SNAKE_HEAD_TEXTURE);
    gameTextures[SNAKE_HEAD_TEXTURE][1]    = '>' | COLOR_PAIR(SNAKE_HEAD_TEXTURE);
    gameTextures[SS_SNAKE_TEXTURE][0]      = '(' | COLOR_PAIR(SS_SNAKE_TEXTURE);
    gameTextures[SS_SNAKE_TEXTURE][1]      = ')' | COLOR_PAIR(SS_SNAKE_TEXTURE);
    gameTextures[SS_SNAKE_HEAD_TEXTURE][0] = '<' | COLOR_PAIR(SS_SNAKE_HEAD_TEXTURE);
    gameTextures[SS_SNAKE_HEAD_TEXTURE][1] = '>' | COLOR_PAIR(SS_SNAKE_HEAD_TEXTURE);
    gameTextures[FOOD_TEXTURE][0]          = '{' | COLOR_PAIR(FOOD_TEXTURE);
    gameTextures[FOOD_TEXTURE][1]          = '}' | COLOR_PAIR(FOOD_TEXTURE);
    gameTextures[COLLISION_TEXTURE][0]     = '*' | COLOR_PAIR(COLLISION_TEXTURE);
    gameTextures[COLLISION_TEXTURE][1]     = '*' | COLOR_PAIR(COLLISION_TEXTURE);
    gameTextures[BACKGROUND_TEXTURE][0]    = ' ' | COLOR_PAIR(BACKGROUND_TEXTURE);
    gameTextures[BACKGROUND_TEXTURE][1]    = ' ' | COLOR_PAIR(BACKGROUND_TEXTURE);
}



void Display::updateLengthCounter(std::size_t length)
{
    mvwaddstr(gameWin, 0, windowPadding, lengthLabel);

    std::size_t lengthLabelSize = std::strlen(lengthLabel);
    mvwaddstr(gameWin, 0, lengthLabelSize + windowPadding, "   ");
    mvwprintw(gameWin, 0, lengthLabelSize + windowPadding, "%d", length);
}



void Display::updateMaxLengthCounter(std::size_t maxLength)
{
    std::size_t lengthLabelSize = std::strlen(maxLengthLabel);
    mvwaddstr(gameWin, 0, getmaxx(gameWin) - (lengthLabelSize + windowPadding + 2), maxLengthLabel);

    mvwaddstr(gameWin, 0, getmaxx(gameWin) - (windowPadding + 2), "   ");
    mvwprintw(gameWin, 0, getmaxx(gameWin) - (windowPadding + 2), "%d", maxLength);
}



void Display::update()
{
    pnoutrefresh(snakeWin, 0, 0,
                 windowPadding, windowPadding,
                 ScreenSize.y - (gameTextLines + windowPadding), ScreenSize.x - windowPadding);

    pnoutrefresh(messageWin, 0, 0,
                 windowPadding, windowPadding,
                 ScreenSize.y - (gameTextLines + windowPadding), ScreenSize.x - windowPadding);

    pnoutrefresh(gameWin, 0, 0,
                 ScreenSize.y - (gameTextLines + windowPadding), windowPadding,
                 ScreenSize.y - windowPadding, ScreenSize.x - windowPadding);

    doupdate();
}

}