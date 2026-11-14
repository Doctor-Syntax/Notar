#ifndef COMMON_TYPES
#define COMMON_TYPES
#include "../dependencies/raylib/src/raylib.h"
#include <string>
#include <iostream>

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
#include <iomanip>

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
    std::string Print();
    std::string Print_type(TokenType type);
};


struct Text{
    std::string text;
    Color color;
    int amount_of_lines = 0;
    int start_x = 0;
    bool is_notar_input_field = false;


    Text(std::string Text, Color COLOR = {255,255,255,255}, int Start_x = 0);
    int Get_lines();
    bool Has_last_newline();
};

enum class Debug_mode{
    lexer_only,
    ast_only,
    parser_only,
    l_ast,
    l_parser,
    a_parser,
    full,
    off
};


#endif // COMMON_TYPES