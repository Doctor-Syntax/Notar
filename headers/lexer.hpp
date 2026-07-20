#ifndef LEXER
#define LEXER

#include "program.hpp"

enum class TokenType{
    Number,
    String,
    Boolean,
    Operator,
    Identifier,
    Keyword,
    Lparen,
    Rparen,
    Lbrace,
    Rbrace,
    LBracket,
    RBracket,
    Semicolon,
    Comma_sperator,
    DotAccessifier,
    Colon,

    EndOfFile
};

extern std::unordered_set<std::string> keywords;
extern std::unordered_set<std::string> operators;
extern std::unordered_map<std::string, char> escape_characters;
extern std::unordered_map<std::string, TokenType> symbols;

struct Token{
    TokenType t;
    std::string v;
    int s_l;
    int s_c;
    int e_l;
    int e_c;
    std::string f_n;
    Token(TokenType type, std::string value, int s_Line, int s_Collumn,int e_Line, int e_Collumn,  std::string file_name)
    : t(type), v(value), s_l(s_Line), s_c(s_Collumn),e_l(e_Line), e_c(e_Collumn), f_n(file_name){};
    void Print();
};


class Lexer{
private:
    std::string code;
    std::vector<Token> tokens;
    std::string file_name;

    size_t i = 0;
    int line = 0;
    int collumn = 0;
    int s_line = 0;
    int e_line = 0;
    int s_collumn = 0;
    int e_collumn = 0;

    
    char Peek_next(std::string str, int current_index, int amount=1);
    void Advance_char();
public:
    Lexer(std::string code,std::string file_name):code(code), file_name(file_name){};
    std::vector<Token> Gen_tokens();
};




#endif //LEXER