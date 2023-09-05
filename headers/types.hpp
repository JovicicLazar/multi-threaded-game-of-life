#ifndef BOARDTYPES_H
#define BOARDTYPES_H

    enum CellState { 
        DEAD, 
        ALIVE 
    };

    struct cell { 
        CellState state;
    };

    struct board {
        cell** cells;
        int rows;
        int cols;
        float cell_size;
    };

#endif