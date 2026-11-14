#ifndef LEXER
#define LEXER

#include "program.hpp"
#include "calc_err.hpp"

struct LexerConfig{
    std::unordered_set<std::string> keywords;
    std::unordered_set<std::string> operators;
    std::unordered_map<std::string, char> escape_characters;
    std::unordered_map<std::string, TokenType> symbols;

    LexerConfig(std::unordered_set<std::string> K, std::unordered_set<std::string> O, std::unordered_map<std::string, char> E, std::unordered_map<std::string, TokenType> S) : keywords(K), operators(O), escape_characters(E), symbols(S){};
};

extern const LexerConfig Notar_language;

                            

class Lexer{
private:
    std::string code;
    std::vector<Token> tokens;
    std::string file_name;
    LexerConfig config;
    Debug_mode d_mode = Debug_mode::off;

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
    Msg msg;
    Lexer(std::string code,std::string file_name, Debug_mode D_mode = Debug_mode::off, LexerConfig Config = Notar_language):code(code), file_name(file_name), config(Config), d_mode(D_mode){};
    std::vector<Token> Gen_tokens();
};




#endif //LEXER