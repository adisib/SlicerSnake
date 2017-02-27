
// snake.h
// Snakes

#ifndef SLICERSNAKE_SNAKE_H
#define SLICERSNAKE_SNAKE_H


#include <list>
#include <cstdlib> // rand
#include <cassert>

#include "display.h"


namespace ssnake
{

enum Direction_t
{
    RIGHT, LEFT, UP, DOWN
};



class Snake
{

public:

    // PreConditions:
    //   displayHandle points to an active display
    //   startingPos contains a clear position within the boundaries of the display
    //   startingLength is small enough to fit within the display given the startingPos
    //   snakeColor holds a valid texture name
    //   snakeHeadColor holds a valid texture name
    // PostConditions:
    //   A new snake is created at the specified position with the given length and textures
    Snake(Display* displayHandle,
          const Vec2& startingPos,
          const size_t startingLength = 3,
          const Texture_t snakeColor = SNAKE_TEXTURE,
          const Texture_t snakeHeadColor = SNAKE_HEAD_TEXTURE);

    // PreConditions:
    //   The snake has a handle to an active display
    //   foodList contains the positions of all food that could be used in the algorithm
    // PostConditions:
    //   The snake's direction will be set so that it will avoid collision with a wall and grab nearby food
    void ai_getDirection(const std::list<Vec2>& foodList);

    // PreConditions:
    //   The snake has a handle to an active display
    // PostConditions:
    //   Returns true if the snake had a lethal collision, false if not
    //   If the collision was with a wall, the display is instructed to draw a collision texture
    // Does not support maps, only collision with edge of display area for now (I am leaving AI extremely simple for now)
    bool checkCollision();

    // Check to see if snake head has hit the food's coordinates. If so, the snake gets longer.
    // PreConditions:
    // PostConditions:
    //   True is returned and the snake gets longer if it's head overlaps with food's position
    bool checkFood(const Vec2& food);

    // PreConditions:
    // PostConditions:
    //   Returns true if any part of the snake overlaps with checkedPos, false otherwise
    bool checkTouch(const Vec2& checkedPos) const;

    // PreConditions:
    //   snakeList is populated with all snakes to be checked, including this one
    // PostConditions:
    //   If the snake's head has collided with any snake, the hit snake loses all pieces between the tail and collision inclusive
    //   The total amount of lost snake pieces is returned
    size_t checkSlice(std::list<Snake>& snakeList);

    // PreConditions:
    // PostConditions:
    //   The current direction of the snake is returned
    Direction_t getDirection() const;

    // PreConditions:
    // PostConditions:
    //   If newDirection is not opposite of the current direction, or the snake is length 1, its direction becomes newDirection
    void setDirection(Direction_t newDirection);

    // PreConditions:
    // PostConditions:
    //   The current length of the snake is returned
    size_t getLength() const;

    // PreConditions:
    //   The snake has a handle to an active display
    // PostConditions:
    //   The snake's position is updated to move in the direction of its direction property
    //   The display is instructed to reflect the position change
    void move();


protected:

    Display* display;

    Direction_t direction;

    size_t length;
    std::list<Vec2> pos;

    Texture_t texture;
    Texture_t headTexture;
};

}

#endif
