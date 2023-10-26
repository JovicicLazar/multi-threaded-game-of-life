#include <raylib.h>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include <cstring>
#include <jsoncpp/json/json.h>

#include "./src/board.cpp"

using namespace std;


/**
 * i did this part of the code the "old fashioned" way
 * because i wanted to play with memory instead of using vectors
*/
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

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
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
                DrawRectangleRec(cells[i], SKYBLUE);
            }
        EndDrawing();
        cleanup_array(cells, cell_count);
}

void handle_argv(int argc, char* argv[], bool multi_thread, float &cell_size, int &col_number, int &row_number, bool &set_preset, string &preset){
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--multi-thread") == 0) {
            multi_thread = true;
        } else if (strcmp(argv[i], "--single-thread") == 0) {
            multi_thread = false;
        } else if (strcmp(argv[i], "--cell-size") == 0) {
            try {
                i++;
                float argv_cell_size = std::stof(argv[i]);
                cell_size = argv_cell_size;
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid argument for cell size: " << argv[i] << std::endl;
                exit(0);
            } catch (const std::out_of_range& e) {
                std::cerr << "Out of range cell size: " << argv[i] << std::endl;
                exit(0);
            }
        } else if (strcmp(argv[i], "--cols") == 0) {
            try {
                i++;
                float argv_col_number = std::stoi(argv[i]);
                col_number = argv_col_number;
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid argument for col number: " << argv[i] << std::endl;
                exit(0);
            } catch (const std::out_of_range& e) {
                std::cerr << "Out of range for col number: " << argv[i] << std::endl;
                exit(0);
            }
        } else if (strcmp(argv[i], "--rows") == 0) {
            try {
                i++;
                float argv_row_number = std::stoi(argv[i]);
                row_number = argv_row_number;
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid argument for row number: " << argv[i] << std::endl;
                exit(0);
            } catch (const std::out_of_range& e) {
                std::cerr << "Out of range for row number: " << argv[i] << std::endl;
                exit(0);
            }
        } else if (strcmp(argv[i], "--preset") == 0) {
                i++;

                if(i >= argc) {
                    std::cerr << "Missing preset name \n" << argv[i] << std::endl;
                    exit(0);
                } else {
                    set_preset = true;
                    preset = "./presets/" + std::string(argv[i])+".in";
                }

        } else if (strcmp(argv[i], "--help") == 0) {

            std::cout << "--help           =>   lists all of the commands \n " << endl;
            std::cout << "--multi-thread   =>   runs game of life in multi threaded mode " << endl;
            std::cout << "--single-thread  =>   runs game of life in single thread mode " << endl;
            std::cout << "--cell-size      =>   sets the display size of a alive cell " << endl;
            std::cout << "--col-number     =>   sets the number of columns " << endl;
            std::cout << "--row-number     =>   sets number of rows " << endl;
            std::cout << "--row-number     =>   sets the rpeset that will be run " << endl;

            exit(0);
        }
        
        else {
            std::cout << "Wrong argument: " << argv[i] << std::endl;
            exit(0);
        }
    }
}

int main(int argc, char* argv[]) {

    bool multi_thread = false;
    float cell_size   = 15.0f;
    int col_number    = 10;
    int row_number    = 10;
    bool set_preset       = false;
    string preset     = "./presets/glider.in";

    handle_argv(argc, argv, multi_thread, cell_size, col_number, row_number, set_preset, preset);

    Board board(set_preset, preset , cell_size, row_number, col_number);
    int cols = board.get_board().cols;
    int rows = board.get_board().rows;
    const int screen_width  = cols * cell_size;
    const int screen_height = rows * cell_size;
    int fps                 = 30;
    const bool threaded     = true;

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

            if(multi_thread) {
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