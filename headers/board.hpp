#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <utility>

#include <random>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "types.hpp"

using namespace std;

mutex mtx;
condition_variable cv;

class Board {
    
    public:
        Board(int rows, int cols, float cell_size);
        Board(string preset_path, float cell_size, int rows, int cols);
        ~Board();

        void cleanup_board();
        void calculate_cell_state_mthread(int row, int col);
        void generate_board_mthread();
        CellState calculate_cell_state_sthread(int row, int col);
        void generate_board_sthread();

        board get_board();

    private:
        board main_board;
        int threadsReady = 0;
};