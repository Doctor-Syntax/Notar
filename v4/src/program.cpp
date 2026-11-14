#include "../headers/program.hpp"
#include "../headers/lexer.hpp"
#include "../headers/parser.hpp"
#include "../headers/interpreter.hpp"
#include "../headers/GUI_essentials.hpp"

Color red({206, 40, 17,255});
Color green({14, 123, 34,255});
Color yellow({174, 83, 0, 255});
Color purple({84, 10, 124,255});

// program functionss

Program::Program(std::string file_path){
    std::filesystem::path p(file_path);
    f_name = p.stem().string();
    f_extension = p.extension().string();
    f_path = file_path;
}

bool Check_debug_enabled(Debug_mode d_mode, std::string current_fase){
    if (d_mode == Debug_mode::off) return false;
    
    
    if(current_fase == "ast"){
        if(d_mode == Debug_mode::a_parser || d_mode == Debug_mode::ast_only || d_mode == Debug_mode::l_ast || d_mode == Debug_mode::full) return true;
        return false;
    }
    else if(current_fase == "lexer"){
        if(d_mode == Debug_mode::l_ast || d_mode == Debug_mode::l_parser || d_mode == Debug_mode::lexer_only || d_mode == Debug_mode::full) return true;
        return false;
    }
    else if(current_fase == "parser"){
        if(d_mode == Debug_mode::parser_only || d_mode == Debug_mode::a_parser || d_mode == Debug_mode::l_parser || d_mode == Debug_mode::full) return true;
        return false;
    }
    else if(current_fase == "runtime"){
        if(d_mode == Debug_mode::full) return true;
        return false;
    }

    std::cout<<"error, the given runtime fase does not exist [" + current_fase + "]\n";
    return false;
}
std::string Print_debug_mode(Debug_mode d_mode){
    switch(d_mode){
        case Debug_mode::a_parser:
            return "a_parser";
            break;
        case Debug_mode::l_ast:
            return "l_ast";
            break;
        case Debug_mode::l_parser:
            return "l_parser";
            break;
        case Debug_mode::lexer_only:
            return "lexer_only";
            break;
        case Debug_mode::ast_only:
            return "ast_only";
            break;
        case Debug_mode::parser_only:
            return "parser_only";
            break;    
        case Debug_mode::full:
            return "full";
            break;
        case Debug_mode::off:
            return "off";
            break;
    }

    return "UNKNOWN";
}

std::string Program::Read_file(std::string path, Terminal_G &core){
    std::ifstream file(path);

    if(!file) {
        core.Print_text(Text("[PROGRAM ERROR]: file does not exist.\n", red));
        cancel = true;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Program::Print_data(Terminal_G core){
    core.Print_text(Text("file name: [", {14, 123, 34,255}));
    core.Print_text(Text(f_name));
    core.Print_text(Text("\nfile extension: [", {14, 123, 34,255}));
    core.Print_text(Text(f_extension));
    core.Print_text(Text("\nsrc **********\n", {14, 123, 34,255}));
    core.Print_text(Text(src));
    core.Print_text(Text("\n**********\n", {14, 123, 34,255}));
}

std::string Program::Get_full_f_name(){
    return f_name += f_extension;
}

bool Program::Verify_file(Terminal_G &core){
    if(f_extension != ".ntr"){
        core.Print_text(Text("[PROGRAM ERROR]: file format not supported.\n",red));
        return false;
    }
    return true;
}

std::string Program::Get_code(){
    return src;
}

void Program::Run_program(Terminal_G &core, Window_context &window, Debug_mode d_mode){
    src = Read_file(f_path, core);
    if(Verify_file(core) && !cancel){
        core.Print_text(Text("[PROGRAM]: debug mode {", green));
        core.Print_text(Text(Print_debug_mode(d_mode),yellow));
        core.Print_text(Text("}\n", green));
        if(d_mode != Debug_mode::off) Print_data(core);



        Parser P(Get_code(),Get_full_f_name(), f_extension, d_mode);
        P.msg.Set_terminal_context(core);
        auto ast = P.Parse();

        

        Runtime_env R(std::move(ast),window);
        R.msg.Set_terminal_context(core);
        R.Run();
    }
}