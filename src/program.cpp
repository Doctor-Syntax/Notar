#include "../headers/program.hpp"
#include "../headers/lexer.hpp"
#include "../headers/parser.hpp"
#include "../headers/interpreter.hpp"


Program::Program(std::string file_path){
    std::filesystem::path p(file_path);
    f_name = p.stem().string();
    f_extension = p.extension().string();
    if(Verify_file()) src = Read_file(file_path);
}

std::string Program::Read_file(std::string path){
    std::ifstream file(path);

    if(!file) {
        std::cout<<"Err loading file, file does not exist.\n"; 
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Program::Print_data(){
    std::cout<<"debug mode: ["<<((debug_enabled)? "inactive":"active")<<"]\n";

    std::cout<<"file name: ["<<f_name<<"]\n";
    std::cout<<"file extension: ["<<f_extension<<"]\n";

    std::cout<<"src: \n*******\n"<<src<<"\n*******\n";
    // Lexer l(src,Get_full_f_name());
    // std::vector<Token> tokens = l.Gen_tokens();
    // std::cout<<"tokens:[\n";
    // for(size_t i = 0; i < tokens.size(); i++){
    //     tokens[i].Print();
    // }
}

std::string Program::Get_full_f_name(){
    return f_name += f_extension;
}

bool Program::Verify_file(){
    if(f_extension != ".ntr"){
        std::cout<<"Err during validating, file format not supported.\n";
        return false;
    }
    return true;
}

std::string Program::Get_code(){
    return src;
}

void Program::Run_program(bool debug_mode){
    Parser P(Get_code(),Get_full_f_name(), f_extension, debug_mode);

    auto ast = P.Parse();

    if(debug_mode){
        Print_data();
        ast.Print();
    }

    Runtime_env R(std::move(ast));
    R.Run();
}