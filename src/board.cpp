#include "../headers/board.hpp"

Board::cell** init_board(int rows, int cols) {
    Board::cell** cells = new Board::cell*[rows];

    srand(time(0));

    for(int i = 0; i < rows; ++i) {
        cells[i] = new Board::cell[cols];
        for (int j = 0; j < cols; ++j) {
            cells[i][j].state = ((rand() % 2) == 1) ? Board::CellState::ALIVE:Board::CellState::DEAD;
        }
    }

    return cells;
}

bool is_valid_cell(int row, int col, int rows, int cols) {
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

void Board::cleanup_board() {
    for (int i = 0; i < main_board.rows; ++i) {
        delete[] this->main_board.cells[i];
    }
    delete[] this->main_board.cells;
}

Board::Board(int rows, int cols, float cell_size) {
    this->main_board.rows      = rows;
    this->main_board.cols      = cols;
    this->main_board.cell_size = cell_size;
    this->main_board.cells     = init_board(rows, cols);

}

Board::~Board(){
    cleanup_board();
}

Board::board Board::get_board() {
    return Board::main_board;
}

void clamp(int *n, int high_val) {
    if(*n > high_val - 1) *n = 0;
    if(*n < 0) *n = high_val - 1;
}

void Board::calculate_cell_state(int row, int col) {
    int alive_neighbors = 0;
    int threads = Board::main_board.rows * Board::main_board.cols;
    Board::CellState state = this->main_board.cells[row][col].state;

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;
            int neighbor_row = row + i;
            int neighbor_col = col + j;

            clamp(&neighbor_row, this->main_board.rows);
            clamp(&neighbor_col, this->main_board.cols);

            if(this->main_board.cells[neighbor_row][neighbor_col].state == Board::CellState::ALIVE) {
                ++alive_neighbors;
            }
        }
    }

    if(this->main_board.cells[row][col].state == ALIVE) {
        if(alive_neighbors < 2) state = DEAD;
        if(alive_neighbors > 3) state = DEAD;
    } else {
        if(alive_neighbors == 3) state = ALIVE;
    }
    
    {
        std::unique_lock<std::mutex> lock(mtx);
        Board::threadsReady++;

        if(Board::threadsReady < threads){
            cv.wait(lock, [&] { return Board::threadsReady == threads; });
        } else {
            cv.notify_all();
        }
    }

    this->main_board.cells[row][col].state = state;

    Board::threadsDone --;

}

void Board::generate_board() {
    std::vector<std::thread> threads;
    int rows = this->main_board.rows;
    int cols = this->main_board.cols;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            
            threads.emplace_back(&Board::calculate_cell_state, this,i, j);
        }
    }

    for (std::thread& t : threads) {
        t.join();
    }

    Board::threadsReady = 0;
}

