#ifndef PARSER
#define PARSER

#include "ast.hpp"
#include "program.hpp"
#include "lexer.hpp"

class Parser{
private:
    std::string code;
    std::vector<Token> tokens;
    std::string file_name;
    std::string file_extension;
    size_t i = 0;
    bool had_error = false;
    bool debug_enabled = false;

    Token Advance();
    Token Peek();
    bool Match(std::string op);
    void Error(const std::string& msg, const Token t);

    //expressions

    ExprPtr Primary();
    ExprPtr Call();
    ExprPtr Unary();
    ExprPtr Expression();
    ExprPtr Factor();
    ExprPtr Term();
    ExprPtr Comparision();
    ExprPtr Equality();
    ExprPtr LOG_and();
    ExprPtr LOG_or();
    ExprPtr Assignment();

    // expression helper functions
    // ExprPtr AssignmentHandlePropertyAcces_expr(ExprPtr object, Token accessifier);
    // ExprPtr AssignmentHandleIndex_expr(ExprPtr object, ExprPtr index);

    // statements
    StmtPtr Stmt_expr();
    StmtPtr Statement();
    StmtPtr Block_stmt();
    StmtPtr If_stmt();
    StmtPtr Var_stmt();
    StmtPtr While_stmt();
    StmtPtr For_stmt();
    StmtPtr Return_stmt();
    StmtPtr Continue_stmt();
    StmtPtr Break_stmt();
    StmtPtr Func_stmt();
    StmtPtr Empty_stmt();
    



public:
    Parser(std::string code, std::string file_name, std::string file_extension, bool debug_mode = false);

    Prgrm Parse();
};




#endif //PARSER