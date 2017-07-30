# SlicerSnake

A quick and slightly different snake game to play around with curses. 

## Game Instructions:
Play Classic Snake, or Slicer Snake modes. In Slicer Snake mode, you try to get as long as possible while another computer controlled snake should be avoided. If it hits you, a part of you is sliced off, unless he hits your head which is game over. Hitting yourself does not give a game over like in classic snake, but also slices a part of you off. The longer you get, the faster the game gets. The same is true for the computer controlled snake, so being offensive can prevent the game from getting too fast. Sticking against the walls is risky, but at the same time will prevent the other snake from eating you so you can keep your length. Risk has reward in this version of snake!

## Game Controls:
Use the arrow keys or WASD to move your snake. Pressing enter/return will pause and unpause the game. You can also press q to quickly kill yourself to quit the game if you want.

## Build Instructions:
At least Linux (using ncurses) and Windows (using pdcurses) are supported, but this repository is currently set up for easy Cygwin builds.
The easiest way to build is to have g++ and make installed. Edit the makefile if desired, and then simply run make in the directory with the makefile. A compiler with c++11 support is required.