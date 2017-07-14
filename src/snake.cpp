
#include "snake.h"

#include <list>
#include <cstdlib> // rand
#include <cassert>

#include "display.h"


namespace ssnake
{

Snake::Snake(Display* displayHandle,
             const SnakeTextureList& textureList,
             const Vec2& startingPos,
             const size_t startingLength)
{
    display = displayHandle;

    Vec2 win = {display->getSize_x(), display->getSize_y()};

    snakeTextures = textureList;

    // starting directions depend on starting postions
    // so don't immediately face a wall
    if (startingPos.x < (win.x / 2))
    {
        direction = RIGHT;
    }
    else
    {
        direction = LEFT;
    }

    // For now, truncate requested length if length doesn't fit
    if (static_cast<size_t>(startingPos.x) < startingLength)
    {
        length = startingPos.x;
    }
    else
    {
        length = startingLength;
    }

    // initialize positions
    int directionModifier = 1;
    if (direction == RIGHT)
    {
        directionModifier = -1;
    }
    for (int i = length - 1; i >= 0; --i)
    {
        Vec2 coord;
        coord.x = startingPos.x + directionModifier*i;
        coord.y = startingPos.y;
        if (coord.x > 0 && coord.x < win.x && coord.y > 0 && coord.y < win.y)
        {
            pos.push_back(coord);
        }
    }
}



void Snake::ai_getDirection(const std::list<Vec2>& foodList)
{
    if (pos.empty())
    {
        return;
    }

    Vec2 coord = pos.back();
    Vec2 win = {display->getSize_x(), display->getSize_y()};

    Direction_t ai_dir = direction;
    Direction_t newDir = ai_dir;

    int xOffset = win.x - coord.x;
    int yOffset = win.y - coord.y;

    // prevent hitting wall
    if (coord.x < 4 || coord.y < 4 || xOffset < 4 || yOffset < 4)
    {
        if (ai_dir != LEFT && coord.x < 3)
        {
            newDir = RIGHT;
        }
        else if (ai_dir != RIGHT && xOffset < 3)
        {
            newDir = LEFT;
        }
        if (ai_dir != UP && coord.y < 3)
        {
            newDir = DOWN;
        }
        else if (ai_dir != DOWN && yOffset < 3)
        {
            newDir = UP;
        }
        // if can't go backwards have to do multi-step turn around
        if ((ai_dir == LEFT && coord.x < 4) || (ai_dir == RIGHT && xOffset < 4))
        {
            if ((rand() % 2 + 1) == 2 && coord.y > 4)
            {
                newDir = UP;
            }
            else if (yOffset > 4)
            {
                newDir = DOWN;
            }
            else
            {
                newDir = UP;
            }
        }
        if ((ai_dir == UP && coord.y < 4) || (ai_dir == DOWN && yOffset < 4))
        {
            if ((rand() % 2 + 1) == 2 && coord.x > 4)
            {
                newDir = RIGHT;
            }
            else if (xOffset > 4)
            {
                newDir = LEFT;
            }
            else
            {
                newDir = RIGHT;
            }
        }
    } // end if (coord.x < 4 || coord.y < 4 || xOffset < 4 || yOffset < 4)
    else // not near wall, so evasive maneuvers not required, do something else
    {
        // random movement every so often
        if (coord.x > 3 && coord.y > 3 && xOffset > 3 && yOffset > 3)
        {
            if (ai_dir == DOWN || ai_dir == UP)
            {
                if ((rand() % 16 + 1) == 2)
                {
                    newDir = LEFT;
                }
                else if ((rand() % 18 + 1) == 3)
                {
                    newDir = RIGHT;
                }
            }
            else
            {
                if ((rand() % 16 + 1) == 2)
                {
                    newDir = UP;
                }
                else if ((rand() % 18 + 1) == 3)
                {
                    newDir = DOWN;
                }
            }
        }

        // Only go for food if close to it and not near a wall
        for (std::list<Vec2>::const_iterator food = foodList.cbegin(); food != foodList.cend(); ++food)
        {
            if (ai_dir != LEFT && (food->x - coord.x) <= 3 && food->x > coord.x && food->x <= (win.x-4))
            {
                newDir = RIGHT;
                break;
            }
            else if (ai_dir != RIGHT && (coord.x - food->x) <= 3 && coord.x > food->x && food->x >= 4)
            {
                newDir = LEFT;
                break;
            }
            else if (ai_dir != DOWN && (coord.y - food->y) <= 3 && coord.y > food->y && food->y >= 4)
            {
                newDir = UP;
                break;
            }
            else if (ai_dir != UP && (food->y - coord.y) <= 3 && food->y > coord.y && food->y <= (win.y-4))
            {
                newDir = DOWN;
                break;
            }
        }
    }

    direction = newDir;

    return;
}



bool Snake::checkCollision()
{
    // head collision with other snake will leave snake empty from checkslice
    if (pos.empty())
    {
        return true;
    }

    Vec2 win = {display->getSize_x(), display->getSize_y()};

    Vec2 head = pos.back();

    // wall collision
    if (head.x >= (win.x - 1) || head.x <= 0 || head.y >= (win.y - 1) || head.y <= 0)
    {
        display->drawTexture(TEXTURE_COLLISION, head);

        return true;
    }


    // self collision
    // If checkSlice happens first, then it can't collide with itself.
    std::list<Vec2>::const_iterator stop = --pos.cend();
    for (std::list<Vec2>::const_iterator it = pos.cbegin(); it != stop; ++it)
    {
        if (head.x == it->x && head.y == it->y)
        {
            return true;
        }
    }

    return false;
}



bool Snake::checkFood(const Vec2& food)
{
    if (pos.empty())
    {
        return false;
    }

    Vec2 snakeHead = pos.back();

    if (snakeHead.y != food.y || snakeHead.x != food.x)
    {
        return false;
    }

    ++length;

    return true;
}



bool Snake::checkTouch(const Vec2& checkedPos) const
{
    if (pos.empty())
    {
        return false;
    }

    for (std::list<Vec2>::const_iterator it = pos.cbegin(); it != pos.cend(); ++it)
    {
        if (checkedPos.x == it->x && checkedPos.y == it->y)
        {
            return true;
        }
    }

    return false;
}



size_t Snake::checkSlice(std::list<Snake>& snakeList)
{
    if (pos.empty())
    {
        return 0;
    }

    size_t totalCount = 0;

    Vec2 ss_coord = pos.back();

    for (std::list<Snake>::iterator slicedIter = snakeList.begin(); slicedIter != snakeList.end(); ++slicedIter)
    {
        if (slicedIter->pos.empty())
        {
            continue;
        }

        Vec2 sd_coord;

        size_t checkSize = slicedIter->pos.size();

        // work-around to make snake not collide with it's own head
        if (this == &(*slicedIter))
        {
            --checkSize;
        }

        std::list<Vec2>::iterator it = slicedIter->pos.begin();
        std::list<Vec2>::iterator cut;
        for (size_t i = 1; i <= checkSize; ++i)
        {
            sd_coord = *it++;

            if (ss_coord.x == sd_coord.x && ss_coord.y == sd_coord.y)
            {
                cut = it;
                totalCount += i;

                assert(slicedIter->length >= i);
                slicedIter->length -= i;

                if (slicedIter->length != 0)
                {
                    for (it = slicedIter->pos.begin(); it != cut; ++it)
                    {
                        display->drawTexture(TEXTURE_BACKGROUND, *it);
                    }
                }
                slicedIter->pos.erase(slicedIter->pos.begin(), cut);

                break;
            }
        }
    }

    return totalCount;
}



Direction_t Snake::getDirection() const
{
    return direction;
}



size_t Snake::getLength() const
{
    return length;
}



void Snake::move()
{
    if (pos.empty())
    {
        return;
    }

    Vec2 coord = pos.back();

    switch (direction)
    {
        case (LEFT) :
            --coord.x;
            break;
        case (RIGHT) :
            ++coord.x;
            break;
        case (UP) :
            --coord.y;
            break;
        case (DOWN) :
            ++coord.y;
            break;
    }

    display->moveSnakeHead(pos.back(), coord, snakeTextures);
    if (pos.size() > 1)
    {
        std::list<Vec2>::iterator start = ++pos.begin();
        std::list<Vec2>::iterator stop = --pos.end();
        for (std::list<Vec2>::iterator it = start; it != stop;)
        {
            display->moveSnakeBody(*it, *(++it), snakeTextures);
        }
        if (length <= pos.size())
        {
            display->moveSnakeTail(pos.front(), *(start), snakeTextures);
        }
    }
    else
    {
        display->drawTexture(TEXTURE_BACKGROUND, pos.back());
    }

    // remove at tail but not if length increased
    if (length <= pos.size())
    {
        pos.pop_front();
    }

    pos.push_back(coord);

    return;
}



void Snake::setDirection(Direction_t newDirection)
{
    if ( ( (newDirection == LEFT && direction != RIGHT) ||
           (newDirection == RIGHT && direction != LEFT) ||
           (newDirection == UP && direction != DOWN) ||
           (newDirection == DOWN && direction != UP)
         ) ||
         (length == 1)
       )
    {
        direction = newDirection;
    }
}

}
