#ifndef PROGRAM
#define PROGRAM

#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <variant>
#include <cmath>
#include <functional>


class Program{
private:
    std::string src;
    std::string f_name, f_extension;
    bool debug_enabled = false;

    bool Verify_file();
    std::string Read_file(std::string path);




public:
    Program(std::string file_path);
    void Print_data();

    std::string Get_full_f_name();
    std::string Get_code();

    void Run_program(bool debug_mode = false);


};

#endif //PROGRAM