#include "../headers/lexer.hpp"


std::unordered_set<std::string> keywords = {
    "var", "if", "else", "elif", "func", "while", "for", "fix", "pure", "continue", "break", "return"
};

std::unordered_map<std::string, char> escape_characters = {
    {"n", '\n'}, {"b", '\b'}, {"\\", '\\'}, {"\"", '"'}, {"t", '\t'}
};

std::unordered_set<std::string> operators = {
    "=","!", "==", "!=", "-=", "+=", "/=", "*=", "*", "/", "+", "-", "**", ">", "<", "<=",
    ">=", "&&", "||", "&"
};

std::unordered_map<std::string, TokenType> symbols = {
    {"{", TokenType::Lbrace},
    {"}", TokenType::Rbrace},
    {"(", TokenType::Lparen},
    {")", TokenType::Rparen},
    {";", TokenType::Semicolon},
    {",", TokenType::Comma_sperator},
    {".", TokenType::DotAccessifier},
    {"[", TokenType::LBracket},
    {"]", TokenType::RBracket},
    {":", TokenType::Colon}
};


void Token::Print(){
    std::cout<<"v: ["<<v<<"]  ";
    std::cout<<"t: ["<<int(t)<<"]\n";
    std::cout<<"s_l: ["<<s_l<<"]   ";
    std::cout<<"s_c: ["<<s_c<<"]\n";
    std::cout<<"e_l: ["<<e_l<<"]   ";
    std::cout<<"e_c: ["<<e_c<<"]\n";
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

    std::cout<<"Err during lexing, could not peek next char in string.\n";
    return '\0';
}

std::vector<Token> Lexer::Gen_tokens(){
    //std::cout << "NEW LEXER VERSION\n";
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
                    if(escape_characters.find(std::string(1, code[i])) != escape_characters.end()){
                        str += escape_characters[std::string(1, code[i])];
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
            else if(keywords.find(str) != keywords.end()) tokens.emplace_back(Token(TokenType::Keyword, str,s_line,s_collumn,e_line, e_collumn, file_name));
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
        
        if(operators.find(dubble_str) != operators.end()){
            Advance_char();
            Advance_char();
            e_line = line;
            e_collumn = collumn;
            tokens.emplace_back(Token(TokenType::Operator, dubble_str, s_line, s_collumn, e_line, e_collumn,file_name));
        }
        else if(operators.find(str) != operators.end()){
            Advance_char();
            e_line = line;
            e_collumn = collumn;
            tokens.emplace_back(Token(TokenType::Operator, str, s_line, s_collumn, e_line, e_collumn, file_name));
            
        }
        else if(symbols.find(str) != symbols.end()){
            Advance_char();
            e_line = line;
            e_collumn = collumn;
            tokens.emplace_back(Token(symbols.at(str), str, s_line, s_collumn, e_line, e_collumn, file_name));
            
        }
        else {
            Advance_char();
            e_line = line;
            e_collumn = collumn;
            tokens.emplace_back(Token(TokenType::Identifier, str, s_line, s_collumn, e_line, e_collumn, file_name));    
        }
    }
    tokens.emplace_back(Token(TokenType::EndOfFile, "EOF", s_line, s_collumn, e_line, e_collumn, file_name));
    return tokens;
}
