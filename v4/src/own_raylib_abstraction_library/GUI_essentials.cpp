#include "../headers/GUI_essentials.hpp"

Window_context::Window_context(int W, int H, std::string Title, int T_w, int T_h) : total_w(W), total_h(H), title(Title), terminal_w(T_w), terminal_h(T_h) {
    std_t_w = terminal_w;
    std_t_h = terminal_h;
};

void Window_context::Start(){
    if(!window_enabled){
        total_w = terminal_w;
        total_h = terminal_h;
    }
    dim.terminal_width = terminal_w;
    dim.terminal_height = terminal_h;
    dim.total_screen_height = total_h;
    dim.total_screen_width = total_w;
    terminal.dim = dim;

    
    InitWindow(total_w,total_h,title.c_str());
    terminal.core.Set_font_size(20);

    while(running && !WindowShouldClose()){
        if(terminal.Terminate_program()) running = false;
        if(terminal_enabled){
            
            if(startup) startup = !(terminal.Get_username());
            else terminal.Frame(*this);
        }

        Update();
        Draw();
    }

    CloseWindow();
}

void Window_context::Terminate_program(){
    running = false;
}

void Window_context::Update(){
    if(terminal_enabled) {
        terminal.core.Update();
    }
}

void Window_context::Draw(){
    BeginDrawing();
    if(window_enabled) gc.Draw();
    if(terminal_enabled) terminal.core.Draw();

    EndDrawing();
}

void Window_context::Add_window(int width, int height){
    window_enabled = true;
    int window_w = width;
    int window_h = height;
    int window_x = 0;
    int window_y = 0;


    if(width + terminal_w <= GetMonitorWidth(0) && width + terminal_w >= 100) {
        total_w = width + terminal_w;
        terminal.core.Set_cords(width,0);
        dim.terminal_x = width;
    }
    else{
        total_w = GetMonitorWidth(0);
        if(GetMonitorWidth(0) - width < terminal_w && GetMonitorWidth(0) - width >= 900) {
            terminal_w = GetMonitorWidth(0) - width;
            terminal.core.Set_cords(GetMonitorWidth(0) - terminal_w, 0);
            dim.terminal_x = GetMonitorWidth(0) - terminal_w;
        }
        else if(GetMonitorWidth(0) - width < 900 && GetMonitorWidth(0) - width > 0){
            terminal_w = 900;
            terminal.core.Set_cords(GetMonitorWidth(0) - terminal_w, 0);
            dim.terminal_x = GetMonitorWidth(0) - terminal_w;
        }
        else{
            window_w = GetMonitorWidth(0);
        }
    }

    if(height > terminal_h && height <= GetMonitorHeight(0)) {
        total_h = height;
    }
    else if(height < terminal_h && height > 0){
        window_y = static_cast<int>(terminal_h/2 - window_h/2);
        total_h = terminal_h;
    }

    gc = GC(window_w, window_h);
    gc.Set_cords(window_x, window_y);
    terminal.core.Set_dimensions(terminal_w, terminal_h);

    dim.terminal_height = terminal_h;
    dim.terminal_width = terminal_w;
    dim.window_height = window_h;
    dim.terminal_width = window_w;
    dim.window_y = window_y;
    dim.window_x = window_x;
    dim.total_screen_height = total_h;
    dim.total_screen_width = total_w;
    dim.terminal_active = terminal_enabled;
    dim.window_active = window_enabled;

    terminal.dim = dim;
    SetWindowSize(total_w, total_h);
}

void Window_context::Close_window(){
    gc = GC(0,0);
    window_enabled = false;
    terminal_w = std_t_w;
    terminal_h = std_t_h;

    terminal.core.Set_dimensions(terminal_w, terminal_h);
}

void Window_context::Close_terminal(){
    terminal_enabled = false;
}

void Window_context::Open_terminal(){
    terminal_enabled = true;
}

bool Window_context::Is_terminal_enabled(){
    return terminal_enabled;
}

bool Window_context::Is_window_enabled(){
    return window_enabled;
}

Dimensions Window_context::Get_dimensions(){
    return dim;
}

std::string Window_context::Get_input(Text prompt, Text input){

    std::string output;
    size_t cursor_x = 0;

    

    while(true){
        Update();
        Draw();

        if(IsKeyPressed(KEY_ENTER)) break;
        
        else if(IsKeyPressed(KEY_BACKSPACE) && !output.empty() && cursor_x > 0) {
            output.erase(output.begin()+cursor_x-1);
            input.text = output;
            cursor_x--;
        }
        else if(IsKeyPressed(KEY_LEFT) && cursor_x > 0) cursor_x--;
        else if(IsKeyPressed(KEY_RIGHT) && cursor_x < output.length()) cursor_x++;

        int key = GetCharPressed();

        while(key > 0){
            output.insert(cursor_x,1,static_cast<char>(key));
            input.text = output;
            cursor_x++;
            key = GetCharPressed();
        };

        terminal.core.Prompt(prompt, input, cursor_x, false);

    }

    input.text += "\n";
    terminal.core.Print_text(prompt);
    terminal.core.Print_text(input);

    return output;
}

