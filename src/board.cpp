#include "../headers/board.hpp"

cell** init_board(int rows, int cols, bool random) {
    cell** cells = new cell*[rows];

    if( random ) {
        srand(time(0));
        for(int i = 0; i < rows; ++i) {
            cells[i] = new cell[cols];
            for (int j = 0; j < cols; ++j) {
                cells[i][j].state = ((rand() % 2) == 1) ? CellState::ALIVE : CellState::DEAD;
            }
        }
    } else {
        for(int i = 0; i < rows; ++i) {
            cells[i] = new cell[cols];
            for (int j = 0; j < cols; ++j) {
                cells[i][j].state = CellState::DEAD;
            }
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
    this->main_board.cells     = init_board(rows, cols, true);
}

Board::Board(string preset_path, float cell_size) {
    int rows = 0;
    int cols = 0;
    char cell;
    vector<pair<int ,int>> cells;
    fstream fin(preset_path, fstream::in);

    this->main_board.cell_size = cell_size;

    if(fin) {
        while (fin >> noskipws >> cell) {
            if(cell == '.' ) {
                cols++;
            } 
            else if(cell == '\n') { 
                rows ++; 
                cols = 0;
            }
            else if(cell == '#') {
                cols++;
                cells.push_back(make_pair(rows, cols));
            }
        }

    } else {
        cout << "file does not exist" << endl;
        exit(0);
    }
    rows += 1;
    this->main_board.rows = rows;
    this->main_board.cols = cols;
    this->main_board.cells = init_board(rows, cols, false);
    
    for(int i = 0; i < cells.size(); i ++) {
        this->main_board.cells[cells[i].first][cells[i].second].state = CellState::ALIVE;
    }
}

Board::~Board(){
    cleanup_board();
}

board Board::get_board() {
    return this->main_board;
}

void clamp(int *n, int high_val) {
    if(*n > high_val - 1) *n = 0;
    if(*n < 0) *n = high_val - 1;
}

void Board::calculate_cell_state_mthread(int row, int col) {
    int alive_neighbors = 0;
    int threads = this->main_board.rows * this->main_board.cols;
    CellState state = this->main_board.cells[row][col].state;

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;
            int neighbor_row = row + i;
            int neighbor_col = col + j;

            clamp(&neighbor_row, this->main_board.rows);
            clamp(&neighbor_col, this->main_board.cols);

            if(this->main_board.cells[neighbor_row][neighbor_col].state == CellState::ALIVE) {
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
        this->threadsReady++;

        if(this->Board::threadsReady < threads){
            cv.wait(lock, [&] { return Board::threadsReady == threads; });
        } else {
            cv.notify_all();
        }
    }

    this->main_board.cells[row][col].state = state;
}

void Board::generate_board_mthread() {
    std::vector<std::thread> threads;
    int rows = this->main_board.rows;
    int cols = this->main_board.cols;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            
            threads.emplace_back(&Board::calculate_cell_state_mthread, this, i, j);
        }
    }

    for (std::thread& t : threads) {
        t.join();
    }

    this->threadsReady = 0;
}

CellState Board::calculate_cell_state_sthread(int row, int col) {
    int alive_neighbors = 0;
    CellState state = this->main_board.cells[row][col].state;

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;
            int neighbor_row = row + i;
            int neighbor_col = col + j;

            clamp(&neighbor_row, this->main_board.rows);
            clamp(&neighbor_col, this->main_board.cols);

            if(this->main_board.cells[neighbor_row][neighbor_col].state == CellState::ALIVE) {
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
    
    return state;
}

void Board::generate_board_sthread() {
    int rows = this->main_board.rows;
    int cols = this->main_board.cols;
    
    cell** tmp_cells = init_board(rows, cols , false);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if(calculate_cell_state_sthread(i, j) == CellState::ALIVE) tmp_cells[i][j].state = CellState::ALIVE;
        }
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            this->main_board.cells[i][j].state = tmp_cells[i][j].state;
        }
    }

    for (int i = 0; i < rows; ++i) {
        delete[] tmp_cells[i];
    }

    delete[] tmp_cells; 
}

