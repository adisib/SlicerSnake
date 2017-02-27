SlicerSnake

A quick game to play around with curses. 

Game Instructions:
Play Classic Snake, or Slicer Snake modes. In Slicer Snake mode, you try to get as long as possible while another computer controlled snake should be avoided to prevent part of you being sliced off. Hitting yourself does not give a game over like in classic snake, but shortens yourself. The longer you get, the faster the game gets. The same is true for the computer controlled snake, so being offensive can prevent the game from getting too fast. A head on collision with the other snake will be a game over, however.

Build Instructions:
At least Linux and Windows are supported.
The easiest way to build is to have g++ and make installed. Edit the makefile if desired, and then simply run make in the directory with the makefile. A compiler with c++11 support is required.