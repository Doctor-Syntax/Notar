#include "../headers/calc_err.hpp"

void Msg::Error(const Token &token, const std::string &msg, bool &terminate, std::string error_type){
    if(terminate) return;
    terminate = true;

    core->Print_text(Text("\n[" + error_type + " ERROR]: " + msg + "\n", red));
    core->Print_text(Text("  --> ", red));
    core->Print_text(Text(token.f_n + ":" +std::to_string(token.s_l+1) + ":" +std::to_string(token.s_c) + "\n", yellow));

    std::ifstream file(token.f_n);

    if(!file) return;

    std::string line = "";
    for(int i = 0; i <= token.s_l; i++)
        std::getline(file, line);

    core->Print_text(Text(line+"\n", yellow));

    for(int i = 0; i < token.s_c; i++) core->Print_text(Text(" "));

    int width = std::max(1, token.e_c - token.s_c);

    for(int i = 0; i < width; i++) core->Print_text(Text("^", red));

    core->Print_text(Text("\n"));
}

void Msg::Warning(const Token &token, const std::string &msg){

    core->Print_text(Text("\n[WARNING]: " + msg + "\n", purple));
    core->Print_text(Text("  --> ", purple));
    core->Print_text(Text(token.f_n + ":" +std::to_string(token.s_l+1) + ":" +std::to_string(token.s_c) + "\n", yellow));


    std::ifstream file(token.f_n);

    if(!file) return;

    std::string line = "";
    for(int i = 0; i <= token.s_l; i++)
        std::getline(file, line);

    core->Print_text(Text(line+"\n", yellow));

    for(int i = 0; i < token.s_c; i++) core->Print_text(Text(" "));

    int width = std::max(1, token.e_c - token.s_c);

    for(int i = 0; i < width; i++) core->Print_text(Text("^", purple));
    core->Print_text(Text("\n"));
}

void Msg::Warning(const std::string &msg){

    core->Print_text(Text("\n[WARNING] " + msg + "\n", purple));
}

void Msg::Error(const std::string &msg, bool &terminate, std::string error_type){

    if(terminate) return;
    terminate = true;

    core->Print_text(Text("\n[" + error_type + " ERROR]: "+ msg+"\n", red));
}

void Msg::Message(const std::string &msg, Color color){
    core->Print_text(Text(msg, color));
}

void Msg::Set_terminal_context(Terminal_G &terminal){
    core = &terminal;
}

Terminal_G *Msg::Get_terminal_context(){
    return core;
}



