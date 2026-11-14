#include "../headers/parser.hpp"



Parser::Parser(std::string Code, std::string File_name, std::string File_extension, Debug_mode debug_mode){
    code = Code;
    file_name = File_name;
    file_extension = File_extension;
    d_mode = debug_mode;
}

Token Parser::Advance(){
    if(i < tokens.size()){
        i++;
        return tokens[i-1];
    }
    return Token(TokenType::EndOfFile, "EOF", 0,0,0,0,file_name);
}

Token Parser::Peek(){
    //std::cout << "Peek i=" << i << " size=" << tokens.size() << "\n";

    if(i >= tokens.size()){
        msg.Warning("tried to peek to next token, but index was out of bounds.");
        return Token(TokenType::EndOfFile, "EOF",0,0,0,0,file_name);
    }

    return tokens[i];
    

}

bool Parser::Match(std::string op){
    if(i < tokens.size() && tokens[i].v == op){
        i++;
        return true;
    }

    return false;
}

//expressions

ExprPtr Parser::Primary(){
    if(had_error) return nullptr;

    if(Peek().t == TokenType::EndOfFile) {
        return nullptr;
    }
    if(Peek().t == TokenType::Number || Peek().t == TokenType::String || Peek().t == TokenType::Boolean){
        return std::make_unique<LiteralExpr>(std::move(Advance())); 
    }
    if(Peek().t == TokenType::Identifier){
        return std::make_unique<VariableExpr>(std::move(Advance()));
    }
    if(Peek().t == TokenType::Lparen){
        Advance();
        auto expr = Expression();
        if(Peek().t == TokenType::Rparen){
            Advance();
            auto group_expr = std::make_unique<GroupingExpr>(std::move(expr));
            return group_expr;
        }
        else{
            msg.Error(Peek(),"expected a ')'.",had_error,"PARSER");
            return nullptr;
        }
    }
    if(Peek().t == TokenType::Lbrace){
        Advance();

        std::unordered_map<std::string, std::unique_ptr<Expr>> elements = {};

        while(Peek().t != TokenType::Rbrace && Peek().t != TokenType::EndOfFile){
            std::string key = "";
            if(Peek().t == TokenType::Identifier) key = Advance().v;
            else{
                msg.Error(Peek(),"expected a key for identification in body creation.",had_error,"PARSER");
                return nullptr;
            }
            
            if(Peek().t == TokenType::Colon) Advance();
            else{
                msg.Error(Peek(),"expected a colon (aka ':') for linking an object to a key in body creation.",had_error,"PARSER");
                return nullptr;
            }

            auto key_value = Expression();

            if(!key_value){
                msg.Error(Peek(),"expected an expression to link to a key in body creation.",had_error,"PARSER");
                return nullptr;
            }

            elements.emplace(key, std::move(key_value));

            if(Peek().t == TokenType::Comma_sperator) Advance();
        }

        if(Peek().t == TokenType::Rbrace) Advance();
        else{ 
            msg.Error(Peek(),"expected a '}'.",had_error,"PARSER");
            return nullptr;
        }

        return std::make_unique<BodyExpr>(std::move(elements));

    }
    if(Peek().t == TokenType::LBracket){
        Advance();

        std::vector<std::unique_ptr<Expr>> elements = {};

        while(Peek().t != TokenType::RBracket && Peek().t != TokenType::EndOfFile){
            auto expr = Expression();

            if(!expr){
                msg.Error(Peek(),"expected an expresion in array.",had_error,"PARSER");
                return nullptr;
            }


            elements.emplace_back(std::move(expr));
            if(Peek().t == TokenType::Comma_sperator) Advance();
            else if(Peek().t == TokenType::RBracket) break;
            else{
                msg.Error(Peek(),"no expression or comma seperator in array creation.",had_error,"PARSER");
                return nullptr;
            }
            
        }

        if(Peek().t == TokenType::RBracket) Advance();
        else{ 
            msg.Error(Peek(),"expected a ']'.",had_error,"PARSER");
            return nullptr;
        }

        return std::make_unique<ArrayExpr>(std::move(elements));

    } 
    return nullptr;
}

ExprPtr Parser::Call(){
    if(had_error) return nullptr;

    auto expr = Primary();
    while(Peek().t == TokenType::Lparen || Peek().t == TokenType::LBracket || Peek().t == TokenType::DotAccessifier){
        if(Peek().t == TokenType::Lparen){
            Advance();
            std::vector<ExprPtr> args = {};

            while(Peek().t != TokenType::Rparen && Peek().t != TokenType::EndOfFile){
                auto expr = Expression();
                if(!expr) return nullptr;
                args.emplace_back(std::move(expr));

                if(Peek().t != TokenType::Comma_sperator && Peek().t != TokenType::Rparen){
                    msg.Error(Peek(), "expected a ',' for argument", had_error, "PARSER");
                    return nullptr;
                }
                if(Peek().t == TokenType::Comma_sperator) Advance();
            }


            if(Peek().t == TokenType::EndOfFile){
                msg.Error(Peek(), "expected a ')'", had_error, "PARSER");
                return nullptr;
            }
            Advance();

            expr = std::make_unique<CallExpr>(
                std::move(expr),
                std::move(args)
            );
        }
        else if(Peek().t == TokenType::DotAccessifier){
            Advance();
            std::vector<ExprPtr> args = {};
            if(Peek().t == TokenType::Identifier){
                expr = std::make_unique<PropertyAccessExpr>(
                    std::move(expr),
                    std::move(Advance())
                );
            }
            else{
                msg.Error(Peek(), "expected an identifier", had_error, "PARSER");
                return nullptr;
            }
        }
        else if(Peek().t == TokenType::LBracket){
            Advance();
            std::vector<ExprPtr> args = {};
            ExprPtr index = Expression();
            if(!index) return nullptr;

            if(Peek().t != TokenType::RBracket && Peek().t != TokenType::EndOfFile){
                msg.Error(Peek(), "expected a ']'", had_error, "PARSER");
                return nullptr;
            }
            Advance();

            expr = std::make_unique<IndexAccessExpr>(
                std::move(expr),
                std::move(index)
            );
        }
    }
    return expr;
}

ExprPtr Parser::Unary(){
    if(had_error) return nullptr;

    if(Peek().t == TokenType::Operator && (Peek().v == "!" || Peek().v == "-" || Peek().v == "+" || Peek().v == "&")){
        auto op = Advance();
        auto right = Unary();

        if(!right){
            msg.Error(Peek(), "missing right expr in Unary", had_error, "PARSER");
            return nullptr;
        }
        
        auto expr = std::make_unique<UnaryExpr>(std::move(op), std::move(right));
        if(!expr){
            msg.Error(Peek(), "missing expr in Unary", had_error, "PARSER");
            return nullptr;
        }
        return expr;
    }
    return Call();
}

ExprPtr Parser::Factor(){
    if(had_error) return nullptr;

    auto expr = Unary();

    if(!expr){
        msg.Error(Peek(), "missing expr in Factor", had_error, "PARSER");
        return nullptr;
    }


    while(Peek().t == TokenType::Operator && (Peek().v == "*" || Peek().v == "/")){
        auto op = Advance();
        auto right = Unary();

        if(!right){
            msg.Error(Peek(), "missing right expr in Factor", had_error, "PARSER");
            return nullptr;
        }
        
        auto bin_expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op,
            std::move(right)
        );
        expr = std::move(bin_expr);
    }

    return expr;
}

ExprPtr Parser::Term(){
    if(had_error) return nullptr;

    auto expr = Factor();

    if(!expr){
        msg.Error(Peek(), "missing expr in Term", had_error, "PARSER");
        return nullptr;
    }

    while(Peek().t == TokenType::Operator && (Peek().v == "+" || Peek().v == "-")){
        auto op = Advance();
        auto right = Factor();

        if(!right){
            msg.Error(Peek(), "missing right expr in Term", had_error, "PARSER");
            return nullptr;
        }

        auto bin_expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            std::move(op),
            std::move(right)
        );
        expr = std::move(bin_expr);
    }
    return expr;
}

ExprPtr Parser::Comparision(){
    if(had_error) return nullptr;

    auto expr = Term();

    if(!expr){
        msg.Error(Peek(), "missing expr in Comparison", had_error, "PARSER");
        return nullptr;
    }

    while(Peek().t == TokenType::Operator && 
        (Peek().v == ">" || Peek().v == "<" || Peek().v == "<=" || Peek().v == ">=")){
            auto op = Advance();
            auto right = Term();

            if(!right){
            msg.Error(Peek(), "missing right expr in Comparison", had_error, "PARSER");
                return nullptr;
            }

            auto bin_expr = std::make_unique<BinaryExpr>(
                std::move(expr),
                std::move(op),
                std::move(right)
            );
            expr = std::move(bin_expr);
    }
    return expr;
}

ExprPtr Parser::Equality(){
    if(had_error) return nullptr;

    auto expr = Comparision();

    if(!expr){
        msg.Error(Peek(), "missing expr in Equality", had_error, "PARSER");
        return nullptr;
    }

    while(Peek().t == TokenType::Operator && 
        (Peek().v == "==" || Peek().v == "!=")){
        auto op = Advance();
        auto right = Comparision();

        if(!right){
            msg.Error(Peek(), "missing right expr in Equality", had_error, "PARSER");
            return nullptr;
        }

        auto bin_expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            std::move(op),
            std::move(right)
        );
        expr = std::move(bin_expr);
    }
    return expr;
}

ExprPtr Parser::LOG_and(){
    if(had_error) return nullptr;

    auto expr = Equality();

    if(!expr){
        msg.Error(Peek(), "missing expr in LOG_and", had_error, "PARSER");
        return nullptr;
    }

    while(Peek().t == TokenType::Operator && Peek().v == "&&"){
        auto op = Advance();
        auto right = Equality();

        if(!right){
            msg.Error(Peek(), "missing right expr in LOG_and", had_error, "PARSER");
            return nullptr;
        }

        auto bin_expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            std::move(op),
            std::move(right)
        );
        expr = std::move(bin_expr);
    }
    return expr;
}

ExprPtr Parser::LOG_or(){
    if(had_error) return nullptr;

    auto expr = LOG_and();

    if(!expr){
        msg.Error(Peek(), "missing expr in LOG_or", had_error, "PARSER");
        return nullptr;
    }

    while(Peek().t == TokenType::Operator && Peek().v == "||"){
        auto op = Advance();
        auto right = LOG_and();

        if(!right){
            msg.Error(Peek(), "missing right expr in LOG_or", had_error, "PARSER");
            return nullptr;
        }

        auto bin_expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            std::move(op),
            std::move(right)
        );
        expr = std::move(bin_expr);
    }
    return expr;
}

ExprPtr Parser::Assignment(){
    if(had_error) return nullptr;

    auto expr = LOG_or();

    if(!expr){
        msg.Error(Peek(), "no expression for assignment.", had_error, "PARSER");

        return nullptr;
    }

    auto varExpr = dynamic_cast<VariableExpr*>(expr.get());
    auto accesExpr = dynamic_cast<PropertyAccessExpr*>(expr.get());
    auto indexExpr = dynamic_cast<IndexAccessExpr*>(expr.get());

    if(!varExpr && !accesExpr && !indexExpr){
        return expr;
    }

    if(Peek().t == TokenType::Operator && Peek().v == "="){
        Advance();
        ExprPtr target = std::move(expr);
        auto value = Expression();
        if(!value) {
            msg.Error(Peek(), "expected an expression voor the assignment.", had_error, "PARSER");
            return nullptr;
        }

        auto assignment = std::make_unique<AssignmentExpr>(
            std::move(target->Copy()),
            std::move(value)
        );
        return assignment;
    }

    if(Peek().t == TokenType::Operator && (
        Peek().v == "+=" ||
        Peek().v == "-=" ||
        Peek().v == "*=" ||
        Peek().v == "/=")){
            
        ExprPtr target = std::move(expr);
        Token bin_token = Advance();
        bin_token.v.pop_back();
        auto value = Expression();

        auto bin_expr = std::make_unique<BinaryExpr>(
            std::move(target->Copy()),
            std::move(bin_token),
            std::move(value)
        );

        auto assign_expr = std::make_unique<AssignmentExpr>(
            std::move(target),
            std::move(bin_expr)
        );
        return assign_expr;
    }
    
    else return expr;
}

ExprPtr Parser::Expression(){
    if(had_error) return nullptr;

    auto expr = Assignment();
    return expr;
}


//statements

StmtPtr Parser::Stmt_expr(){
    if(had_error) return nullptr;

    auto expr = Expression();

    if(!expr){
        msg.Error(Peek(), "expected an expression", had_error, "PARSER");
        return nullptr;
    }

    if(Peek().t != TokenType::Semicolon){
        msg.Error(Peek(), "expected a ';'", had_error, "PARSER");
        return nullptr;
    }

    Advance();
    auto expr_stmt = std::make_unique<ExpressionStmt>(std::move(expr));
    
    return expr_stmt;
}

StmtPtr Parser::Block_stmt(){
    if(had_error) return nullptr;

    std::vector<StmtPtr> statements;
    Advance();

    while(Peek().t != TokenType::Rbrace && Peek().t != TokenType::EndOfFile){
        auto stmt = Statement();
        if(!stmt) return nullptr;

        statements.emplace_back(std::move(stmt));
    }
    if(Peek().t == TokenType::Rbrace) Advance();
    else{
        msg.Error(Peek(), "expected a '}'", had_error, "PARSER");
        return nullptr;
    }
    auto block = std::make_unique<BlockStmt>(std::move(statements));
    return block;
    
}

StmtPtr Parser::If_stmt(){
    if(had_error) return nullptr;

    StmtPtr then_branch;
    StmtPtr else_branch;
    ExprPtr condition;
    if(Peek().t == TokenType::Keyword && (Peek().v == "if" || Peek().v == "elif")) Advance();

    if(Peek().t == TokenType::Lparen){
        Advance();
        condition = Expression();
        if(Peek().t == TokenType::Rparen) Advance();
        else{
            msg.Error(Peek(), "expected a ')'", had_error, "PARSER");
            return nullptr;
        }
    }
    
    then_branch = Statement();
    if(!then_branch){
        msg.Error(Peek(), "no then branch in if stmt", had_error, "PARSER");
        then_branch = nullptr;
    }
    if(Peek().t == TokenType::Keyword && Peek().v == "elif"){
        Advance();
        else_branch = If_stmt();
    }
    else if(Peek().t == TokenType::Keyword && Peek().v == "else"){
        Advance();
        else_branch = Statement();
    }

    if(!else_branch){
        else_branch = std::make_unique<EmptyStmt>();
    }
    
    return std::make_unique<IfStmt>(std::move(condition), std::move(then_branch),std::move(else_branch));
}

StmtPtr Parser::Var_stmt(){
    if(had_error) return nullptr;

    Advance();
    Token name(TokenType::Identifier,"",0,0,0,0,file_name);
    ExprPtr initializer;
    VarModifier mod = VarModifier::none;
    
    if(Peek().t == TokenType::Identifier ){
        name = Advance();
    }
    else{
        msg.Error(Peek(), "expected an identifier", had_error, "PARSER");
        return nullptr;
    }
    if(Peek().t == TokenType::Colon){
        Advance();
        if(Peek().t == TokenType::Keyword && (Peek().v == "fix" || Peek().v == "pure")){
            if(Peek().v == "fix") mod = VarModifier::fix;
            else mod = VarModifier::pure;
            Advance();
        }
    }
    if(Peek().t == TokenType::Operator && Peek().v == "=" ){
        Advance();
        initializer = Expression();
    }
    else{
        msg.Error(Peek(), "expected an assignment operator", had_error, "PARSER");
        return nullptr;
    }

    if(Peek().t != TokenType::Semicolon){
        msg.Error(Peek(), "expected a ';'", had_error, "PARSER");
        return nullptr;
    }

    Advance();

    auto var_stmt = std::make_unique<VarDeclarationStmt>(std::move(name), std::move(initializer), std::move(mod));

    return var_stmt;
}

StmtPtr Parser::While_stmt(){
    if(had_error) return nullptr;

    Advance();
    ExprPtr condition;
    StmtPtr body;

    if(Peek().t == TokenType::Lparen){
        Advance();
        condition = Expression();
        if(Peek().t == TokenType::Rparen){
            Advance();
        }
        else{
            msg.Error(Peek(), "expected a ')'", had_error, "PARSER");
        }
    }
    else{
        msg.Error(Peek(), "expected a '('", had_error, "PARSER");
        return nullptr;
    }

    body = Statement();
    if(!body){
        msg.Error(Peek(), "no body in while loop", had_error, "PARSER");
        return nullptr;
    }

    auto while_stmt = std::make_unique<WhileStmt>(std::move(condition), std::move(body));
    return while_stmt;
}

StmtPtr Parser::For_stmt(){
    if(had_error) return nullptr;

    Advance();
    StmtPtr initializer;
    ExprPtr iteration_update;
    ExprPtr condition;
    StmtPtr body;

    if(Peek().t == TokenType::Lparen){
        Advance();
        auto init = Statement();
        if(dynamic_cast<VarDeclarationStmt*>(init.get())){
            initializer = std::move(init);
        }
        else{
            msg.Error(Peek(), "iterator is not valid for a for loop", had_error, "PARSER");
            return nullptr;
        }
        if(!initializer){
            msg.Error(Peek(), "missing iterator declaration in for loop", had_error, "PARSER");
            return nullptr;
        }


        condition = Expression();
        if(!condition){
            msg.Error(Peek(), "missing a condition in for loop", had_error, "PARSER");
            return nullptr;
        }
        if(Peek().t == TokenType::Semicolon) Advance();
        else{
            msg.Error(Peek(), "missing ';' in for loop declaration", had_error, "PARSER");
            return nullptr;
        }

        
        iteration_update = Expression();
        if(!iteration_update){
            msg.Error(Peek(), "missing update condition in for loop", had_error, "PARSER");
            return nullptr;
        }
        
        if(Peek().t != TokenType::Rparen){
            msg.Error(Peek(), "expected a ')'", had_error, "PARSER");
            return nullptr;
        }
        else{
            Advance();
        }
    }
    else{
        msg.Error(Peek(), "expected a '('", had_error, "PARSER");
        return nullptr;
    }

    body = Statement();
    if(!body){
        msg.Error(Peek(), "no body in for loop", had_error, "PARSER");
        return nullptr;
    }

    auto for_stmt = std::make_unique<ForStmt>(std::move(initializer), std::move(condition), std::move(iteration_update), std::move(body));
    return for_stmt;
}

StmtPtr Parser::Continue_stmt(){
    if(had_error) return nullptr;

    Advance();
    return std::make_unique<Continuestmt>();
}

StmtPtr Parser::Break_stmt(){
    if(had_error) return nullptr;

    Advance();
    return std::make_unique<BreakStmt>(); 
}

StmtPtr Parser::Func_stmt(){
    if(had_error) return nullptr;

    Advance();
    Token name(TokenType::Identifier, "",0,0,0,0,file_name);
    std::vector<Token> args;
    if(Peek().t == TokenType::Identifier){
        name = Advance();
    }
    else{
        msg.Error(Peek(), "expected an identifier for function.", had_error, "PARSER");
        return nullptr;
    }

    if(Peek().t == TokenType::Lparen){
        Advance();
        while(Peek().t != TokenType::Rparen && Peek().t != TokenType::EndOfFile){
            args.emplace_back(Advance());
            if(Peek().t == TokenType::Comma_sperator) Advance();
        }
        if(Peek().t == TokenType::Rparen) Advance();
        else{
            msg.Error(Peek(), "expected a ')'", had_error, "PARSER");
            return nullptr;
        }

    }
    else{
        msg.Error(Peek(), "expected a '('", had_error, "PARSER");
        return nullptr;
    }

    StmtPtr body = Block_stmt();
    auto func_stmt = std::make_unique<FuncStmt>(
        std::move(name),
        std::move(args),
        std::move(body)
    );
        
    return func_stmt;
}

StmtPtr Parser::Return_stmt(){
    if(had_error) return nullptr;

    Advance();
    ExprPtr value;
    if(Peek().t == TokenType::Semicolon){
        value = std::make_unique<EmptyExpr>();
        Advance();
    }
    else value = Expression();
    

    auto return_stmt = std::make_unique<ReturnStmt>(
        std::move(value)
    );

    return return_stmt;

}

StmtPtr Parser::Empty_stmt(){
    if(had_error) return nullptr;

    Advance();
    return std::make_unique<EmptyStmt>();
}


StmtPtr Parser::Statement(){
    if(had_error) return nullptr;


    if(Peek().t == TokenType::Lbrace){
        return Block_stmt();
    }
    else if (Peek().t == TokenType::Keyword && Peek().v == "if"){
        return If_stmt();
    }
    else if(Peek().t == TokenType::Keyword && Peek().v == "var"){
        return Var_stmt();
    }
    else if(Peek().t == TokenType::Keyword && Peek().v == "while"){
        return While_stmt();
    }
    else if(Peek().t == TokenType::Keyword && Peek().v == "for"){
        return For_stmt();
    }
    else if(Peek().t == TokenType::Keyword && Peek().v == "func"){
        return Func_stmt();
    }
    else if(Peek().t == TokenType::Keyword && Peek().v == "return"){
        return Return_stmt();
    }
    else if(Peek().t == TokenType::Keyword && Peek().v == "continue"){
        return Continue_stmt();
    }
    else if(Peek().t == TokenType::Keyword && Peek().v == "break"){
        return Break_stmt();
    }
    else if(Peek().t == TokenType::Semicolon){
        return Empty_stmt();
    }
    else{
        return Stmt_expr();
    }
}

Prgrm Parser::Parse(){
    if(Check_debug_enabled(d_mode, "parser")) msg.Message("[PARSER]: starting ast construction...\n",green);

    i=0;
    Lexer l(code, file_name, d_mode);
    l.msg.Set_terminal_context(*msg.Get_terminal_context());
    tokens = l.Gen_tokens();

    auto ast = Prgrm();
    ast.msg.Set_terminal_context(*msg.Get_terminal_context());

    while(Peek().t != TokenType::EndOfFile){
        auto stmt = Statement();
        if(!stmt) break;


        ast.body.emplace_back(std::move(stmt));
        if(had_error) {
            break;
        }

    }

    if(ast.body.empty()){
        msg.Message("[PARSER]: body of program is is empty after parsing.\n");
        ast.body.clear();
    }

    ast.code = code;
    ast.file_name = file_name;
    ast.file_extension = file_extension;
    
    if(Check_debug_enabled(d_mode, "ast")) ast.Print();
    if(Check_debug_enabled(d_mode, "parser")) msg.Message("[PARSER]: succesfully made ast, starting program...\n",green);

    return ast;
}


