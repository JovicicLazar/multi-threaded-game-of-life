#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <mutex>
#include <condition_variable>

std::mutex mtx;  // Mutex for matrix synchronization
std::condition_variable cv;  // Condition variable for synchronization


class Board {
    
    public:

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

        Board(int rows, int cols, float cell_size);
        ~Board();

        void cleanup_board();
        void calculate_cell_state(int row, int col);
        void generate_board();

        board get_board();

        int threadsReady = 0;
        int threadsDone = 1000;

    private:
        board main_board;
};