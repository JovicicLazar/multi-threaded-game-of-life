#include <raylib.h>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include <jsoncpp/json/json.h>

#include "./src/board.cpp"

using namespace std;

void cleanup_array(Rectangle* &array, int &size) {
    delete[] array;
    array = nullptr;
    size  = 0;
}

void expand_array(Rectangle* &array, int &old_array_size) {
    int new_array_size         = old_array_size + 1;
    Rectangle* temporary_array = new Rectangle[new_array_size];
    for(int i = 0; i < old_array_size; i++) {
        temporary_array[i] = array[i];
    }
    delete[] array;
    array = temporary_array;
    old_array_size++;
}

void clamp(float &n, int high_val) {
    if(n > high_val - 1) n = high_val - 1.0f;
    if(n < 0) n = 0.0f;
}

void draw_matrix(cell** matrix, int rows ,int cols, float cell_size) {
        int posx         = 0;
        int posy         = 0;
        Rectangle* cells = nullptr;
        int cell_count   = 0;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if(matrix[i][j].state == CellState::ALIVE) {
                    expand_array(cells, cell_count);
                    cells[cell_count - 1].height = cell_size;
                    cells[cell_count - 1].width  = cell_size;
                    cells[cell_count - 1].x      = posx;
                    cells[cell_count - 1].y      = posy;
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

    Board board("./presets/glider.in", 15.0f);
    
    const int rows          = board.get_board().rows;
    const int cols          = board.get_board().cols;
    const int factor        = board.get_board().cell_size;
    const int screen_width  = cols * factor;
    const int screen_height = rows * factor;
    const float cell_size   = board.get_board().cell_size;

    int fps                 = 10;

    const bool threaded     = false;

    InitWindow(screen_width, screen_height, "g a m e  of  l i f e");
    SetTargetFPS(fps);
    
    while (!WindowShouldClose())
    {

        if(IsKeyDown(KEY_DOWN) && fps > 2){
            fps--;
            SetTargetFPS(fps);
        } 
        if(IsKeyDown(KEY_UP) && fps < 60){
            fps++;
            SetTargetFPS(fps);
        } 

        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse_pos = GetMousePosition();

            clamp(mouse_pos.x, screen_width);
            clamp(mouse_pos.y, screen_height);

            int mat_x = mouse_pos.x / board.get_board().cell_size;
            int mat_y = mouse_pos.y / board.get_board().cell_size;
            
            board.get_board().cells[mat_y][mat_x].state = CellState::ALIVE;
        }
        
        if(!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

            if(threaded) {
                board.generate_board_mthread();
            } else {
                board.generate_board_sthread();
            }
            
        }

        DrawText(to_string(GetFPS()).c_str(), 0, 0, (int)screen_height / 25, WHITE);
        draw_matrix(board.get_board().cells, rows, cols, cell_size);

    }

    CloseWindow();

    return 1;
}