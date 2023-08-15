#include <raylib.h>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>

#include "./src/board.cpp"

std::mutex mmtx;  // Mutex for matrix synchronization
std::condition_variable mcv;

using namespace std;

void cleanup_array(Rectangle* &array, int &size) {
    delete[] array;
    array = nullptr;
    size = 0;
}

void expand_array(Rectangle* &array, int &old_array_size) {
    int new_array_size = old_array_size + 1;
    Rectangle* temporary_array = new Rectangle[new_array_size];
    for(int i = 0; i < old_array_size; i++) {
        temporary_array[i] = array[i];
    }
    delete[] array;
    array = temporary_array;
    old_array_size++;
}

void draw_matrix(Board::cell** matrix, int rows ,int cols, float cell_size) {
        int posx         = 0;
        int posy         = 0;
        Rectangle* cells = nullptr;
        int cell_count   = 0;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if(matrix[i][j].state == Board::CellState::ALIVE){
                    expand_array(cells, cell_count);
                    cells[cell_count - 1].height = cell_size;
                    cells[cell_count - 1].width = cell_size;
                    cells[cell_count - 1].x = posx;
                    cells[cell_count - 1].y = posy;
                }
                posx += cell_size;
                if(posx >= cols * cell_size) posx = 0;
                if(j == cols - 1) posy +=cell_size;            
            }
        }
        BeginDrawing();
            ClearBackground(BLACK);
            for (int i = 0; i <= cell_count - 1; i++)
            {
                DrawRectangleRec(cells[i], RED);
            }
        EndDrawing();
        cleanup_array(cells, cell_count);
}

int main() {

    Board board(20, 20, 10.0f);

    int rows = board.get_board().rows;
    int cols = board.get_board().cols;

    const int factor        = board.get_board().cell_size;
    const int screen_width  = cols * factor;
    const int screen_height = rows * factor;
    const float cell_size   = board.get_board().cell_size;

    InitWindow(screen_width, screen_height, "game of l i f e");

    SetTargetFPS(60);
    
    while (!WindowShouldClose())
    {
        board.threadsDone = 1000;
        draw_matrix(board.get_board().cells, rows, cols, cell_size);
        std::cout << board.threadsDone << endl;

        board.generate_board();

    }
    CloseWindow();

    return 1;
}