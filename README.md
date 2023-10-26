# multi-threaded-game-of-life

Conway's Game of Life is a cellular automaton devised by mathematician John Conway. It is a zero-player game, meaning its evolution is determined by its initial state, with no further input. The game is played on a grid of cells, where each cell can be either alive or dead. The state of each cell evolves over discrete time steps, following specific rules:

1.Any live cell with fewer than two live neighbors dies, as if by underpopulation.
2.Any live cell with two or three live neighbors lives on to the next generation.
3.Any live cell with more than three live neighbors dies, as if by overpopulation.
4.Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.

In this C++ implementation, you have the flexibility to customize the width and height of the grid. This allows you to create game boards of various sizes. You can also define your own patterns by specifying which cells are initially alive or dead.

Here's how it works:

1.You can set the width and height of the grid, which determines the size of the game board.

2.After setting the grid dimensions, you can create your own patterns by specifying which cells should be alive at the start. These initial patterns can be provided as input, and the game board will be initialized accordingly.

3.The program will then proceed to simulate the game's evolution based on the rules mentioned earlier. Each generation is a discrete step where the state of each cell is updated.

4.You can also visualize the evolution of the game by drawing the grid and watching how patterns change and evolve over time. This visualization can be a text-based representation on the console or a graphical interface, depending on the implementation.

This implementation provides a versatile platform for experimenting with Conway's Game of Life, allowing you to explore various grid sizes and create your own initial patterns to observe the fascinating and sometimes unpredictable behavior that emerges from this simple set of rules.

# Dependencies: 

C++  
raylib  

# Installation
To install raylib:
```bash
$ sudo apt install libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev
$ git clone https://github.com/raysan5/raylib
$ cd raylib
$ mkdir build && cd build
$ cmake -DBUILD_SHARED_LIBS=ON ..
$ make up
$ sudo make install
$ sudo cp /usr/local/lib/libraylib.so.450 /usr/lib/
```
NOTE! if you get and error that /usr/local/lib/libraylib.so.450 doesnt exist just go to /usr/local/lib and there should be file libraylib.so.4xx so unsetad of 450 just write whatever number is there. This error occurs because of version diff.

# How to run
If you want to build and run the game:
```bash
$ ./build.sh
$ ./gol --rows 50 --cols 100 --cell-size 20
$ ./gol --rows 50 --cols 100 --cell-size 20 --preset glider
```
If you want to see other options:
```bash
$ ./build.sh
$ ./gol --help
```
