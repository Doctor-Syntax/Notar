#include "../headers/parser.hpp"



Parser::Parser(std::string Code, std::string File_name, std::string File_extension, bool debug_mode){
    code = Code;
    file_name = File_name;
    file_extension = File_extension;
    debug_enabled = debug_mode;
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
        std::cout << "OUT OF RANGE\n";
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

void Parser::Error(const std::string& msg, const Token t){
    if(had_error) return;

    had_error = true;
    std::cout<<"parsing err: "<< msg << "\n";
    std::cout<<"  line: "<<t.s_l<<'\n';
    std::cout<<"  collumn: "<<t.s_c<<'\n';
}

//expressions

ExprPtr Parser::Primary(){
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
            Error("expected a ')'", Peek());
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
                Error("expected a key for identification in body creation.", Peek());
                return nullptr;
            }
            
            if(Peek().t == TokenType::Colon) Advance();
            else{
                Error("expected a colon (aka ':') for linking an object to a key in body creation.", Peek());
                return nullptr;
            }

            auto key_value = Expression();

            if(!key_value){
                Error("expected an expression to link to a key in body creation.", Peek());
                return nullptr;
            }

            elements.emplace(key, std::move(key_value));

            if(Peek().t == TokenType::Comma_sperator) Advance();
        }

        if(Peek().t == TokenType::Rbrace) Advance();
        else{ 
            Error("expected a '}'", Peek());
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
                Error("error in parsing, expected an expresion in array.", Peek());
                return nullptr;
            }


            elements.emplace_back(std::move(expr));
            if(Peek().t == TokenType::Comma_sperator) Advance();
            else if(Peek().t == TokenType::RBracket) break;
            else{
                Error("parsing err, no expression or comma seperator in array creation.", Peek());
                return nullptr;
            }
            
        }

        if(Peek().t == TokenType::RBracket) Advance();
        else{ 
            Error("expected a ']'", Peek());
            return nullptr;
        }

        return std::make_unique<ArrayExpr>(std::move(elements));

    } 
    return nullptr;
}

ExprPtr Parser::Call(){
    auto expr = Primary();
    while(Peek().t == TokenType::Lparen || Peek().t == TokenType::LBracket || Peek().t == TokenType::DotAccessifier){
        if(Peek().t == TokenType::Lparen){
            Advance();
            std::vector<ExprPtr> args = {};
            while(Peek().t != TokenType::Rparen && Peek().t != TokenType::EndOfFile){
                args.emplace_back(Expression());
                if(Peek().t != TokenType::Comma_sperator && Peek().t != TokenType::Rparen){
                    Error("expected a ',' for argument", Peek());
                    return nullptr;
                }
                if(Peek().t == TokenType::Comma_sperator) Advance();
            }
            if(Peek().t == TokenType::EndOfFile){
                Error("expected a ')'", Peek());
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
                Error("expected an identifier", Peek());
                return nullptr;
            }
        }
        else if(Peek().t == TokenType::LBracket){
            Advance();
            std::vector<ExprPtr> args = {};
            ExprPtr index = Expression();
            if(Peek().t != TokenType::RBracket && Peek().t != TokenType::EndOfFile){
                Error("expected a ']'", Peek());
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
    if(Peek().t == TokenType::Operator && (Peek().v == "!" || Peek().v == "-" || Peek().v == "+" || Peek().v == "&")){
        auto op = Advance();
        auto right = Unary();

        if(!right){
            Error("missing right expr in Unary", Peek());
            return nullptr;
        }
        
        auto expr = std::make_unique<UnaryExpr>(std::move(op), std::move(right));
        if(!expr){
            Error("missing expr in Unary",Peek());
            return nullptr;
        }
        return expr;
    }
    return Call();
}

ExprPtr Parser::Factor(){
    auto expr = Unary();

    if(!expr){
        Error("missing expr in Factor", Peek());
        return nullptr;
    }


    while(Peek().t == TokenType::Operator && (Peek().v == "*" || Peek().v == "/")){
        auto op = Advance();
        auto right = Unary();

        if(!right){
            Error("missing right expr in Factor",Peek());
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
    auto expr = Factor();

    if(!expr){
        Error("missing expr in Term",Peek());
        return nullptr;
    }

    while(Peek().t == TokenType::Operator && (Peek().v == "+" || Peek().v == "-")){
        auto op = Advance();
        auto right = Factor();

        if(!right){
            Error("missing right expr in Term",Peek());
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
    auto expr = Term();

    if(!expr){
        Error("missing expr in Comparison",Peek());
        return nullptr;
    }

    while(Peek().t == TokenType::Operator && 
        (Peek().v == ">" || Peek().v == "<" || Peek().v == "<=" || Peek().v == ">=")){
            auto op = Advance();
            auto right = Term();

            if(!right){
            Error("missing right expr in Comparison",Peek());
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
    auto expr = Comparision();

    if(!expr){
        Error("missing expr in Equality",Peek());
        return nullptr;
    }

    while(Peek().t == TokenType::Operator && 
        (Peek().v == "==" || Peek().v == "!=")){
        auto op = Advance();
        auto right = Comparision();

        if(!right){
            Error("missing right expr in Equality",Peek());
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
    auto expr = Equality();

    if(!expr){
        Error("missing expr in LOG_and",Peek());
        return nullptr;
    }

    while(Peek().t == TokenType::Operator && Peek().v == "&&"){
        auto op = Advance();
        auto right = Equality();

        if(!right){
            Error("missing right expr in LOG_and",Peek());
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
    auto expr = LOG_and();

    if(!expr){
        Error("missing expr in LOG_or",Peek());
        return nullptr;
    }

    while(Peek().t == TokenType::Operator && Peek().v == "||"){
        auto op = Advance();
        auto right = LOG_and();

        if(!right){
            Error("missing right expr in LOG_or",Peek());
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
    auto expr = LOG_or();

    if(!expr){
        Error("err, no expression for assignment.", Peek());
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
            Error("err, expected an expression voor the assignment.", Peek());
            return nullptr;
        }

        auto assignment = std::make_unique<AssignmentExpr>(
            std::move(target->Copy()),
            std::move(value)
        );
        return assignment;
    }

    if(Peek().t == TokenType::Operator && (Peek().v == "+=" ||
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
    auto expr = Assignment();
    return expr;
}


//statements

StmtPtr Parser::Stmt_expr(){
    auto expr = Expression();

    if(!expr){
        Error("expected an expression", Peek());
        return nullptr;
    }

    if(Peek().t != TokenType::Semicolon){
        Error("expected a ';'", Peek());
        return nullptr;
    }

    Advance();
    auto expr_stmt = std::make_unique<ExpressionStmt>(std::move(expr));
    
    return expr_stmt;
}

StmtPtr Parser::Block_stmt(){
    std::vector<StmtPtr> statements;
    Advance();

    while(Peek().t != TokenType::Rbrace && Peek().t != TokenType::EndOfFile){
        statements.emplace_back(Statement());
    }
    if(Peek().t == TokenType::Rbrace) Advance();
    else{
        Error("expected a '}'", Peek());
        return nullptr;
    }
    auto block = std::make_unique<BlockStmt>(std::move(statements));
    return block;
    
}

StmtPtr Parser::If_stmt(){
    StmtPtr then_branch;
    StmtPtr else_branch;
    ExprPtr condition;
    if(Peek().t == TokenType::Keyword && (Peek().v == "if" || Peek().v == "elif")) Advance();

    if(Peek().t == TokenType::Lparen){
        Advance();
        condition = Expression();
        if(Peek().t == TokenType::Rparen) Advance();
        else{
            Error("expected a ')'", Peek());
            return nullptr;
        }
    }
    
    then_branch = Statement();
    if(!then_branch){
        Error("no then branch in if stmt", Peek());
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
    Advance();
    Token name(TokenType::Identifier,"",0,0,0,0,file_name);
    ExprPtr initializer;
    VarModifier mod = VarModifier::none;
    
    if(Peek().t == TokenType::Identifier ){
        name = Advance();
    }
    else{
        Error("expected an identifier",Peek());
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
        Error("expected an assignment operator", Peek());
        return nullptr;
    }

    if(Peek().t != TokenType::Semicolon){
        Error("expected a ';'", Peek());
        return nullptr;
    }

    Advance();

    auto var_stmt = std::make_unique<VarDeclarationStmt>(std::move(name), std::move(initializer), std::move(mod));

    return var_stmt;
}

StmtPtr Parser::While_stmt(){
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
            Error("expected a ')'", Peek());
        }
    }
    else{
        Error("expected a '('",Peek());
        return nullptr;
    }

    body = Statement();
    if(!body){
        Error("no body in while loop", Peek());
        return nullptr;
    }

    auto while_stmt = std::make_unique<WhileStmt>(std::move(condition), std::move(body));
    return while_stmt;
}

StmtPtr Parser::For_stmt(){
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
            Error("iterator is not valid for a for loop", Peek());
            return nullptr;
        }
        if(!initializer){
            Error("missing iterator declaration in for loop", Peek());
            return nullptr;
        }


        condition = Expression();
        if(!condition){
            Error("missing a condition in for loop", Peek());
            return nullptr;
        }
        if(Peek().t == TokenType::Semicolon) Advance();
        else{
            Error("missing ';' in for loop declaration",Peek());
            return nullptr;
        }

        
        iteration_update = Expression();
        if(!iteration_update){
            Error("missing update condition in for loop", Peek());
            return nullptr;
        }
        
        if(Peek().t != TokenType::Rparen){
            Error("expected a ')'", Peek());
            return nullptr;
        }
        else{
            Advance();
        }
    }
    else{
        Error("expected a '('",Peek());
        return nullptr;
    }

    body = Statement();
    if(!body){
        Error("no body in for loop", Peek());
        return nullptr;
    }

    auto for_stmt = std::make_unique<ForStmt>(std::move(initializer), std::move(condition), std::move(iteration_update), std::move(body));
    return for_stmt;
}

StmtPtr Parser::Continue_stmt(){
    Advance();
    return std::make_unique<Continuestmt>();
}

StmtPtr Parser::Break_stmt(){
    Advance();
    return std::make_unique<BreakStmt>(); 
}

StmtPtr Parser::Func_stmt(){
    Advance();
    Token name(TokenType::Identifier, "",0,0,0,0,file_name);
    std::vector<Token> args;
    if(Peek().t == TokenType::Identifier){
        name = Advance();
    }
    else{
        Error("expected an identifier for function.",Peek());
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
            Error("expected a ')'", Peek());
            return nullptr;
        }

    }
    else{
        Error("expected a '('", Peek());
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
    Advance();
    return std::make_unique<EmptyStmt>();
}


StmtPtr Parser::Statement(){
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
    //std::cout<<"enter parsing\n";

    i=0;
    Lexer l(code, file_name);
    tokens = l.Gen_tokens();
    // for(size_t i = 0; i < tokens.size(); i++){
    //    tokens[i].Print();
    // }

    auto ast = Prgrm();

    while(Peek().t != TokenType::EndOfFile){
        ast.body.emplace_back(Statement());
        //std::cout<<"parsing stmt...\n";
        if(had_error) {
            //std::cout<<"an error occurt while parsing...\n";
            break;
        }

    }

    if(ast.body.empty()){
        std::cout<<"nullptr in parsing...\n";
        ast.body.clear();
    }

    ast.code = code;
    ast.file_name = file_name;
    ast.file_extension = file_extension;
    ast.debug_enabled = debug_enabled;

    //ast.Print();
    //std::cout<<"done parsing\n";

    return ast;
}


