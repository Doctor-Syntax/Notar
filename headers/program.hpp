#ifndef PROGRAM
#define PROGRAM
#include "common_types.hpp"

class Terminal_G;
class Window_context;

extern Color red;
extern Color green;
extern Color yellow;
extern Color purple;


bool Check_debug_enabled(Debug_mode d_mode, std::string current_fase);
std::string Print_debug_mode(Debug_mode d_mode);

class Program{
private:
    std::string src;
    std::string f_name, f_extension, f_path;
    bool cancel = false;

    bool Verify_file(Terminal_G &core);
    std::string Read_file(std::string path, Terminal_G &core);

public:
    Program(std::string file_path);
    void Print_data(Terminal_G core);

    std::string Get_full_f_name();
    std::string Get_code();

    void Run_program(Terminal_G &core, Window_context &window, Debug_mode debug_mode = Debug_mode::off);
};

#endif //PROGRAM