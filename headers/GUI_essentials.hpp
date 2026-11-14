#ifndef GUI_WINDOW_TERMINAL
#define GUI_WINDOW_TERMINAL


#include "program.hpp"
#include "window_functions.hpp"
#include "terminal.hpp"

class Window_context{
private:
    int total_w;
    int total_h;
    std::string title;
    bool window_enabled = false;
    bool terminal_enabled = true;

    bool running = true;
    bool startup = true;

    int terminal_w = 1200;
    int terminal_h = 800;
    int std_t_w = terminal_w;
    int std_t_h = terminal_h;

    Dimensions dim;

public:
    Terminal terminal = Terminal(terminal_w, terminal_h);
    GC gc = GC(0, 0);


    Window_context(int W, int H, std::string Title, int T_w = 1200, int T_h = 800);

    void Start();
    void Add_window(int width, int height);
    void Close_window();
    void Close_terminal();
    void Open_terminal();
    bool Is_terminal_enabled();
    bool Is_window_enabled();
    Dimensions Get_dimensions();
    std::string Get_input(Text prompt, Text input);
    
    void Update();
    void Draw();
    void Terminate_program();
};



#endif // GUI_WINDOW_TERMINAL