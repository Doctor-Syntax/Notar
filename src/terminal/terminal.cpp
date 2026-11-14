#include "../headers/terminal.hpp"



Terminal::Terminal(int W, int H){ 
    std::cout<<"-----------  starting terminal  -----------\n";
    Define_command_functions();
    core = Terminal_G(W,H);
}

const LexerConfig Terminal_script{{"run", "debug", "exit", "help", "ast", "lexer", "parser", "full", "get", "dim", "file", "version", "cwd", "user", "content"},
                                  {"--", "?"},
                                  {}, 
                                  {{",", TokenType::Comma_sperator},
                                   {".", TokenType::DotAccessifier},
                                   {":", TokenType::Colon}}};

void Terminal::Error(std::string msg){
    core.Print_text(Text("Error: " + msg + "\n", {206, 40, 17,255}));
}
Token Terminal::Advance(int& iterator, std::vector<Token>& list){
    iterator++;
    return list[iterator-1];
}
Token Terminal::Peek(std::vector<Token> &list, int current_index, int amount){
    size_t index = static_cast<size_t>(current_index+amount);
    if(index < list.size()) return list[current_index+amount];

    return Token(TokenType::EndOfFile,"EOF",0,0,0,0,file_name);
}
Debug_mode Terminal::Modify_debug_flag(Debug_mode current_mode, Token flag){
    
    
    if(flag.t != TokenType::Keyword) {
        Error("expected a keyword to set debug mode but got a non keyword token. [" + flag.v + "]");
        return Debug_mode::off;
    }

    if(flag.v != "full" && flag.v != "ast" && flag.v != "lexer" && flag.v != "parser"){
        Error("flag modifier value is not valid for debug mode modification modifier: [" + flag.v + "].");
        return Debug_mode::off;
    }
    
    if(flag.v == "full") return Debug_mode::full;
    else if(flag.v == "off") return Debug_mode::off;
    
    if(current_mode == Debug_mode::ast_only){
        if(flag.v == "ast") return Debug_mode::ast_only;
        else if(flag.v == "lexer") return Debug_mode::l_ast;
        else if(flag.v == "parser") return Debug_mode::a_parser;
    }
    else if(current_mode == Debug_mode::lexer_only){
        if(flag.v == "ast") return Debug_mode::l_ast;
        else if(flag.v == "lexer") return Debug_mode::lexer_only;
        else if(flag.v == "parser") return Debug_mode::l_parser;
    }
    else if(current_mode == Debug_mode::parser_only){
        if(flag.v == "ast") return Debug_mode::a_parser;
        else if(flag.v == "lexer") return Debug_mode::l_parser;
        else if(flag.v == "parser") return Debug_mode::parser_only;
    }
    else if(current_mode == Debug_mode::l_ast){
        if(flag.v == "ast" || flag.v == "lexer") return Debug_mode::l_ast;
        else if(flag.v == "parser") return Debug_mode::full;
    }
    else if(current_mode == Debug_mode::l_parser){
        if(flag.v == "ast") return Debug_mode::full;
        else if(flag.v == "lexer" || flag.v == "parser") return Debug_mode::l_parser;
    }
    else if(current_mode == Debug_mode::a_parser){
        if(flag.v == "ast" || flag.v == "parser") return Debug_mode::a_parser;
        else if(flag.v == "lexer") return Debug_mode::full;
    }
    else if(current_mode == Debug_mode::off) {
        if(flag.v == "ast") return Debug_mode::ast_only;
        else if(flag.v == "lexer") return Debug_mode::lexer_only;
        else if(flag.v == "parser") return Debug_mode::parser_only;
    }
    
    Error("unexpected Debug_mode to change.");
    return Debug_mode::off;
}


bool Terminal::Get_username(){

    if(!Get_input(cursor_pos,buffer,display_text,history_index)){
        username.text = display_text.text;
        username.Get_lines();
        core.Prompt(Text("Enter name> ",{95, 27, 255,255}), username,cursor_pos);
        return false;
    }
    else{
        core.Print_text(Text("Enter name> ", {95, 27, 255,255}));
        core.Print_text(Text(username.text + "\n", {95, 27, 255,255}));
        username.color = {95, 27, 255,255};
        buffer = "";
        cursor_pos = 0;
        display_text = buffer;
        display_text.color = {32, 61, 160,255};
        return true;
    }
}

bool Terminal::Terminate_program(){
    return terminate_program;
}

void Terminal::Frame(Window_context &window){ 
    bool enter = Get_input(cursor_pos,buffer,display_text,history_index);
    if(enter){
        core.Print_text(Text(username.text + "> ", username.color));
        core.Print_text(Text(display_text.text+"\n",display_text.color));
        Lexer L(buffer, file_name, Debug_mode::off, Terminal_script);
        Terminal_Parser(L.Gen_tokens(), window);
        cursor_pos = 0;
        history.emplace_back(display_text.text);
        buffer = "";
        display_text.text = "";
        history_index = history.size();
    }
    core.Prompt(Text(username.text + "> ", username.color),display_text,cursor_pos);
}

void Terminal::Define_command_functions(){
    command_functions["run"] = [this](std::vector<Token> args, Window_context &window) -> void{
        std::string file_path = "";
        Debug_mode d_mode = Debug_mode::off;
        int i = 0;

        

        if(args[i].t == TokenType::String){
            file_path = Advance(i,args).v;

            if(args[i].t == TokenType::Operator && args[i].v == "--") {
                Advance(i,args);
                if(args[i].t == TokenType::Keyword && args[i].v == "debug"){
                    Advance(i,args);
                    while(args[i].t != TokenType::Operator && args[i].t != TokenType::EndOfFile){
                        if(args[i].t == TokenType::DotAccessifier) Advance(i,args);
                        else {
                            Error("expected a '.' to modify the debug mode.");
                            return;
                        }

                        d_mode = Modify_debug_flag(d_mode, Advance(i,args));
                    }

                    if(args[i].t == TokenType::Operator && args[i].v == "--") {
                        Error("can not do more than 1 flag for this version of the terminal (v1).");
                        return;
                    }
                
                }
            }
            else if(args[i].t == TokenType::EndOfFile);
            else {
                Error("expected flagindicator but got [" + args[i].v + "] instead.");
                return;
            }
        
            
            
        }
        else{
            Error("expected a file path string, or questionmark after run keyword");
            return;
        }


        Program P(file_path);

        P.Run_program(core, window, d_mode);
    };
    command_functions["exit"] = [this](std::vector<Token> args, Window_context &window) -> void{
        terminate_program = true;
    };
    command_functions["help"] = [this](std::vector<Token> args, Window_context &window) -> void{
        core.Print_text(Text("Hello " + username.text + "!\n", {174, 83, 0, 255}));
        core.Print_text(Text("Here are some function available for usage:\n", {174, 83, 0, 255}));
        core.Print_text(Text("  -> exit \\\\ for exiting the terminal\n", {174, 83, 0, 255}));
        core.Print_text(Text("  -> run \\\\ for runnning programs.\n", {174, 83, 0, 255}));
        core.Print_text(Text("  -> help \\\\ what brought you here.\n", {174, 83, 0, 255}));
        core.Print_text(Text("  -> ? \\\\ type a ? before a command keyword to get an explaination about it.\n", {174, 83, 0, 255}));
    };
    command_functions["?"] = [this](std::vector<Token> args, Window_context &window) -> void{
        if(args.size() == 0) command_functions["help"](args, window);

        if(args[0].t == TokenType::Keyword){
            if(args[0].v == "run"){
                core.Print_text(Text("The \"run\" command is used to run .ntr files. ", {174, 83, 0, 255}));
                core.Print_text(Text("To run a program simple type; run followed by the path to the program surrounded by dubble quotes.\n\n", {174, 83, 0, 255}));
                core.Print_text(Text("Example:    \"run \"test.ntr\"\"             \\\\ this wil run the program test.ntr\n", {174, 83, 0, 255}));
                core.Print_text(Text("But you can also run with debug info like seeing te tokens, ast, and some default information about te file.\n", {174, 83, 0, 255}));
                core.Print_text(Text("You can achieve this with the --debug flag and by adding nodes after it with the '.'.\n\n", {174, 83, 0, 255}));
                core.Print_text(Text("Example:    \"run \"test.ntr\" --debug .ast\" \\\\ this wil display default info with the ast tree.\n", {174, 83, 0, 255}));
                core.Print_text(Text("Other specifiers: .ast  .lexer  .parser  .full\n", {174, 83, 0, 255}));
                core.Print_text(Text("Hope this helpes a bit, and good luck!\n", {174, 83, 0, 255}));
            }
            else if(args[0].v == "exit"){
                core.Print_text(Text("Use the 'exit' command to close the terminal and terminating the program as a whole.\n"));
            }
            else if(args[0].v == "help"){
                core.Print_text(Text("Use the 'help' command to see what other commands there are.\n"));
            }
            else if(args[0].v == "get"){
                core.Print_text(Text("The 'get' command is used to obtain information of different sources.\n\n"));
                core.Print_text(Text("As example use \"get .file \"filename\"\" to get information about a certain file.\n"));
                core.Print_text(Text("Other specifiers: .file .version .dim .cwd .content .user\n"));
            }
        }
        
    };
    command_functions["get"] = [this](std::vector<Token> args, Window_context &window) -> void{
        if(!(args.size() >= 2)){
            Error("not enough args for get command");
            return;
        }
        
        
        if(args[0].t == TokenType::DotAccessifier && args[0].v == "."){
            if(args[1].t == TokenType::Keyword){
                if(args[1].v == "file"){
                    if(args[2].t == TokenType::String){
                        std::filesystem::path file(args[2].v);

                        if(!std::filesystem::exists(args[2].v)){
                            Error("the given file [" + args[2].v + "] for get command does not exist.");
                            return;
                        }

                        core.Print_text(Text("file name:   [",{14, 123, 34,255}));
                        core.Print_text(Text(std::string(file.filename()), yellow));
                        core.Print_text(Text("]\nfile extension:   [",{14, 123, 34,255}));
                        core.Print_text(Text(std::string(file.extension()), yellow));
                        core.Print_text(Text("]\nfile size:   [",{14, 123, 34,255}));
                        core.Print_text(Text(std::to_string(std::filesystem::file_size(file)), yellow));
                        core.Print_text(Text("] bytes\nabsolute file path:   [",{14, 123, 34,255}));
                        core.Print_text(Text(std::string(std::filesystem::absolute(file)), yellow));
                        core.Print_text(Text("]\ndirectory path:   [",{14, 123, 34,255}));
                        core.Print_text(Text(std::string(std::filesystem::absolute(file).parent_path()), yellow));
                        core.Print_text(Text("]\n",{14, 123, 34,255}));


                    }
                    else {
                        Error("missing file path or file name for get command.");
                        return;
                    }
                }
                else if(args[1].v == "dim"){
                    core.Print_text(Text("terminal x:   [",{14, 123, 34,255}));
                    core.Print_text(Text(std::to_string(dim.terminal_x), yellow));
                    core.Print_text(Text("]\nterminal y:   [",{14, 123, 34,255}));
                    core.Print_text(Text(std::to_string(dim.terminal_y), yellow));
                    core.Print_text(Text("]\nterminal w:   [",{14, 123, 34,255}));
                    core.Print_text(Text(std::to_string(dim.terminal_width), yellow));
                    core.Print_text(Text("]\nterminal h:   [",{14, 123, 34,255}));
                    core.Print_text(Text(std::to_string(dim.terminal_height), yellow));
                    core.Print_text(Text("]\nterminal active:   [",{14, 123, 34,255}));
                    core.Print_text(Text(std::to_string(dim.terminal_active), yellow));
                    core.Print_text(Text("]\n",{14, 123, 34,255}));

                    core.Print_text(Text("window x:   [",{14, 123, 34,255}));
                    core.Print_text(Text(std::to_string(dim.window_x), yellow));
                    core.Print_text(Text("]\nwindow y:   [",{14, 123, 34,255}));
                    core.Print_text(Text(std::to_string(dim.window_y), yellow));
                    core.Print_text(Text("]\nwindow w:   [",{14, 123, 34,255}));
                    core.Print_text(Text(std::to_string(dim.window_width), yellow));
                    core.Print_text(Text("]\nwindow h:   [",{14, 123, 34,255}));
                    core.Print_text(Text(std::to_string(dim.window_height), yellow));
                    core.Print_text(Text("]\nwindow active:   [",{14, 123, 34,255}));
                    core.Print_text(Text(std::to_string(dim.window_active), yellow));

                    core.Print_text(Text("]\ntotal w:   [",{14, 123, 34,255}));
                    core.Print_text(Text(std::to_string(dim.total_screen_width), yellow));
                    core.Print_text(Text("]\ntotal h:   [",{14, 123, 34,255}));
                    core.Print_text(Text(std::to_string(dim.total_screen_height), yellow));
                    core.Print_text(Text("]\n",{14, 123, 34,255}));
                }
                else if(args[1].v == "version"){
                    core.Print_text(Text("version:   [",{14, 123, 34,255}));
                    core.Print_text(Text("v5", yellow));
                    core.Print_text(Text("]\n",{14, 123, 34,255}));
                }
                else if(args[1].v == "cwd"){
                    core.Print_text(Text("current path:   [",{14, 123, 34,255}));
                    core.Print_text(Text(std::string(std::filesystem::current_path()), yellow));
                    core.Print_text(Text("]\n",{14, 123, 34,255}));
                }
                else if(args[1].v == "user"){
                    core.Print_text(Text("username:   [",{14, 123, 34,255}));
                    core.Print_text(Text(username.text, yellow));
                    core.Print_text(Text("]\namount of commands: [",{14, 123, 34,255}));
                    core.Print_text(Text(std::to_string(amount_of_commands), yellow));
                    core.Print_text(Text("]\n",{14, 123, 34,255}));
                }
                else if(args[1].v == "content"){
                    if(args[2].t == TokenType::String){
                        std::filesystem::path file(args[2].v);

                        if(!std::filesystem::exists(args[2].v)){
                            Error("the given file [" + args[2].v + "] for get command does not exist.");
                            return;
                        }

                        core.Print_text(Text("file size:   [",{14, 123, 34,255}));
                        core.Print_text(Text(std::to_string(std::filesystem::file_size(file)), yellow));
                        core.Print_text(Text("] bytes\nabsolute file path:   [",{14, 123, 34,255}));
                        core.Print_text(Text(std::string(std::filesystem::absolute(file)), yellow));
                        core.Print_text(Text("]\ndirectory path:   [",{14, 123, 34,255}));
                        core.Print_text(Text(std::string(std::filesystem::absolute(file).parent_path()), yellow));
                        core.Print_text(Text("]\n",{14, 123, 34,255}));


                        std::ifstream file_c(args[2].v);
                        std::string content((std::istreambuf_iterator<char>(file_c)),std::istreambuf_iterator<char>());

                        core.Print_text(Text("file content:   [\n",{14, 123, 34,255}));
                        core.Print_text(Text(content, yellow));
                        core.Print_text(Text("\n]\n",{14, 123, 34,255}));



                    }
                    else {
                        Error("missing file path or file name for get command.");
                        return;
                    }
                }
            
            }
            else {
                Error("can not use [" + args[1].v + "] as parameter to get command.");
                return;
            }
            
        }
        else {
            Error("missing dot accessifier after get command.");
            return;
        }
        
    };

}

void Terminal::Terminal_Parser(std::vector<Token> tokens, Window_context &window){
    if(tokens[0].t == TokenType::EndOfFile) return;
    auto it = command_functions.find(tokens[0].v);

    amount_of_commands++;

    if(it == command_functions.end()) {
        Error("the ["+tokens[0].v+"] command does not exist. use \"help\" for more information"); 
        return;
    }

    std::vector<Token> args(tokens.begin() +1, tokens.end());

    it->second(args, window);

}

bool Terminal::Get_input(int &cursor_pos, std::string &buffer, Text &display_text, int &history_index){

    if(IsKeyPressed(KEY_ENTER)) return true;
    else if(IsKeyPressed(KEY_BACKSPACE) && !buffer.empty()) {
        if(cursor_pos != 0){
            buffer.erase(cursor_pos-1,1);
            display_text.text = buffer;
            cursor_pos--;
        }
    }
    else if(IsKeyPressed(KEY_UP)) {
        if((history_index > 0)){
            buffer = history[history_index-1];
            display_text.text = buffer;
            history_index--;
        }
    }
    else if(IsKeyPressed(KEY_DOWN)) {
        if((history_index + 1) < history.size()){
            buffer = history[history_index+1];
            display_text.text = buffer;
            history_index++;
        }
        else if((history_index + 1) >= (history.size())){
            buffer = "";
            display_text.text = "";
        }
    }
    else if(IsKeyPressed(KEY_LEFT)) {
        if(cursor_pos > 0) cursor_pos--;
    }
    else if(IsKeyPressed(KEY_RIGHT)) {
        if(cursor_pos < display_text.text.length()) cursor_pos++;
    }
    else{
        int key = GetCharPressed();

        while(key > 0){
            buffer.insert(cursor_pos,1,static_cast<char>(key));

            cursor_pos++;

            key = GetCharPressed();
        };

        display_text.text = buffer;
    }
    return false;

}