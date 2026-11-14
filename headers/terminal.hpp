#ifndef TERMINAL
#define TERMINAL

#include "terminal_functions.hpp"
#include "lexer.hpp"

class Program;
class Window_context;



extern const LexerConfig Terminal_script;

struct Dimensions{
    int window_x = 0;
    int window_y = 0;
    int window_width = 0;
    int window_height = 0;
    int terminal_x = 0;;
    int terminal_y = 0;
    int terminal_width = 0;
    int terminal_height = 0;
    int total_screen_width = 0;
    int total_screen_height = 0;
    bool window_active = false;
    bool terminal_active = true;
};

struct Input_value{
    std::vector<Token> tokens;
    std::string input;
    Input_value(std::vector<Token> Tokens = {}, std::string Input = "") : tokens(Tokens), input(Input){};
};

class Terminal{
private:
    std::unordered_map<std::string, std::function<void(std::vector<Token> args, Window_context &window)>> command_functions;
    Text username = Text("");
    std::string file_name;
    std::string file_extension;
    long file_size;
    std::vector<std::string> history = {};

    std::string buffer = "";
    int cursor_pos = 0;
    int history_index = 0;
    Text display_text = Text("");
    int amount_of_commands = 0;

    bool terminate_program = false;

    void Define_command_functions();
    bool Get_input(int &cursor_pos, std::string &buffer, Text &display_text, int &history_index);
    //void Run_program(std::string full_file_name, Debug_mode debug_mode = Debug_mode::off);
    
    Debug_mode Modify_debug_flag(Debug_mode current_mode, Token flag);
    void Terminal_Parser(std::vector<Token> tokens, Window_context &window);
    Token Advance(int& iterator, std::vector<Token>& list);
    Token Peek(std::vector<Token> &list, int current_index,  int amount = 1);

    void Error(std::string msg);



public:
    Dimensions dim;
    Terminal(int W = 1200, int H = 800);
    Terminal_G core = Terminal_G(0,0);

    void Frame(Window_context &window);
    bool Get_username();
    bool Terminate_program();

    


};


#endif //TERMINAL
