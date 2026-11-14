#include "../headers/lexer.hpp"

const LexerConfig Notar_language{{"var", "if", "else", "elif", "func", "while", "for", "fix", "pure", "continue", "break", "return"},
                           {"=","!", "==", "!=", "-=", "+=", "/=", "*=", "*", "/", "+", "-", "**", ">", "<", "<=",">=", "&&", "||", "&"},
                           {{"n", '\n'}, {"b", '\b'}, {"\\", '\\'}, {"\"", '"'}, {"t", '\t'}}, 
                            
                           {{"{", TokenType::Lbrace},
                             {"}", TokenType::Rbrace},
                             {"(", TokenType::Lparen},
                             {")", TokenType::Rparen},
                             {";", TokenType::Semicolon},
                             {",", TokenType::Comma_sperator},
                             {".", TokenType::DotAccessifier},
                             {"[", TokenType::LBracket},
                             {"]", TokenType::RBracket},
                             {":", TokenType::Colon}}};


std::string Token::Print_type(TokenType type){
    switch(type){
        case TokenType::Boolean:
            return "Boolean";
        case TokenType::Colon:
            return "Colon";
        case TokenType::Comma_sperator:
            return "Comma_sperator";
        case TokenType::DotAccessifier:
            return "DotAccessifier";
        case TokenType::EndOfFile:
            return "EndOfFile";
        case TokenType::Identifier:
            return "Identifier";
        case TokenType::Keyword:
            return "Keyword";
        case TokenType::Lbrace:
            return "Lbrace";
        case TokenType::LBracket:
            return "LBracket";
        case TokenType::Lparen:
            return "Lparen";
        case TokenType::Number:
            return "Number";
        case TokenType::Operator:
            return "Operator";
        case TokenType::Rbrace:
            return "Rbrace";
        case TokenType::RBracket:
            return "RBracket";
        case TokenType::Rparen:
            return "Rparen";
        case TokenType::Semicolon:
            return "Semicolon";
        case TokenType::String:
            return "String";
    }

    return "UNKNOWN";
}

std::string Token::Print(){
    std::ostringstream out;

    out
    << std::left
    <<std::setw(20) << ("v: ["+ v + "]")
    <<std::setw(20) << ("t: [" + Print_type(t) + "]")
    <<"loc (" << s_l << ", " << s_c << ", " << e_l << ", " << e_c << ")"
    << "\n";

    return out.str();
}

void Lexer::Advance_char(){
    
    if(code[i] == '\n'){
        line++;
        collumn = 0;
    }
    else{
        collumn++;
    }
    i++;
}

char Lexer::Peek_next(std::string str, int current_index, int amount){
    size_t i = current_index+amount;
    if(i < str.size()) return str[i];

    msg.Warning("Err during lexing, could not peek next char in string.\n");
    return '\0';
}

std::vector<Token> Lexer::Gen_tokens(){
    if(Check_debug_enabled(d_mode, "lexer")) msg.Message("[LEXER]: generating tokens...\n", green);


    tokens.clear();
    i=0;
    std::string str;

    while(i < code.length()){
        str.clear();
        if(i<code.length() && code[i] == '\n'){
            Advance_char();
            continue;
        }
        if(std::isspace(code[i])){
            Advance_char();
            continue;
        }
        if(i < code.length() && code[i] == '"'){
            s_line = line;
            s_collumn = collumn;
            Advance_char();
            while(i < code.length() && code[i] != '"'){
                if(code[i] == '\\'){
                    Advance_char();
                    if(config.escape_characters.find(std::string(1, code[i])) != config.escape_characters.end()){
                        str += config.escape_characters[std::string(1, code[i])];
                    }
                }
                else{
                    str += code[i];
                }
                Advance_char();
            }
            if(i<code.length()) {
                Advance_char();
            }
            e_collumn = collumn;
            e_line = line;
            tokens.emplace_back(Token(TokenType::String, str,s_line,s_collumn,e_line, e_collumn,file_name));
            continue;
        }
        else if(i < code.length() && (std::isalpha(code[i]) || code[i] == '_')){
            s_line = line;
            s_collumn = collumn;
            while(i < code.length() && (std::isalnum(code[i]) || code[i] == '_')){
                str += code[i];
                Advance_char();
            }
            e_line = line;
            e_collumn = collumn;
            if(str == "true" || str == "false") tokens.emplace_back(Token(TokenType::Boolean, str,s_line,s_collumn,e_line, e_collumn,file_name));
            else if(config.keywords.find(str) != config.keywords.end()) tokens.emplace_back(Token(TokenType::Keyword, str,s_line,s_collumn,e_line, e_collumn, file_name));
            else tokens.emplace_back(Token(TokenType::Identifier, str,s_line,s_collumn,e_line, e_collumn, file_name));
            str = "";        
            continue;
        }
        else if(i < code.length() && std::isdigit(code[i])){
            s_line = line;
            s_collumn = collumn;
            bool Float = false;
            while((i < code.length() && std::isdigit(code[i])) || (!Float && code[i] == '.' )){
                if(!Float && code[i] == '.') Float = true;
                str += code[i];
                Advance_char();
            }
            e_line = line;
            e_collumn = collumn;
            tokens.emplace_back(Token(TokenType::Number, str,s_line,s_collumn,e_line, e_collumn,file_name));
            str = "";
            continue;
        }
        s_line = line;
        s_collumn = collumn;
        str = std::string(1,code[i]);
        std::string dubble_str = str;
        if(i + 1 < code.size()){
            dubble_str += Peek_next(code,i);
        }
        
        if(config.operators.find(dubble_str) != config.operators.end()){
            Advance_char();
            Advance_char();
            e_line = line;
            e_collumn = collumn;
            tokens.emplace_back(Token(TokenType::Operator, dubble_str, s_line, s_collumn, e_line, e_collumn,file_name));
        }
        else if(config.operators.find(str) != config.operators.end()){
            Advance_char();
            e_line = line;
            e_collumn = collumn;
            tokens.emplace_back(Token(TokenType::Operator, str, s_line, s_collumn, e_line, e_collumn, file_name));
            
        }
        else if(config.symbols.find(str) != config.symbols.end()){
            Advance_char();
            e_line = line;
            e_collumn = collumn;
            tokens.emplace_back(Token(config.symbols.at(str), str, s_line, s_collumn, e_line, e_collumn, file_name));
            
        }
        else {
            Advance_char();
            e_line = line;
            e_collumn = collumn;
            tokens.emplace_back(Token(TokenType::Identifier, str, s_line, s_collumn, e_line, e_collumn, file_name));    
        }
    }
    tokens.emplace_back(Token(TokenType::EndOfFile, "EOF", s_line, s_collumn, e_line, e_collumn, file_name));

    if(Check_debug_enabled(d_mode, "lexer")){
        for(size_t j = 0; j < tokens.size(); j++){
            msg.Message("    " + tokens[j].Print());
        }
    }
    return tokens;
}
