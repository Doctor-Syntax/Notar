#ifndef AST
#define AST

#include "program.hpp"
#include "lexer.hpp"

// root classes

struct Stmt{
    virtual ~Stmt() = default;
    virtual std::string Print(int indent = 0) const = 0;
};


struct Expr{
    virtual ~Expr() = default;
    virtual std::string Print(int indent = 0) const = 0;
    virtual std::unique_ptr<Expr> Copy() const = 0;
};

enum class VarModifier{
        none,
        fix,
        pure
};

using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;

//expressions

struct EmptyExpr : Expr{
    EmptyExpr(){};
    std::string Print(int indent) const override;
    std::unique_ptr<Expr> Copy()const override;

};

struct LiteralExpr : Expr{
    Token value;
    LiteralExpr(Token Value) : value(std::move(Value)){}
    std::string Print(int indent = 0)const override;
    std::unique_ptr<Expr> Copy()const override;

};

struct ArrayExpr : Expr{
    std::vector<ExprPtr> elements;

    ArrayExpr(std::vector<ExprPtr> Elements) : elements(std::move(Elements)){}
    std::string Print(int indent = 0)const override;
    std::unique_ptr<Expr> Copy()const override;

};

struct BodyExpr : Expr{
    std::unordered_map<std::string, ExprPtr> elements;

    BodyExpr(std::unordered_map<std::string, ExprPtr> Elements) : elements(std::move(Elements)){}
    std::string Print(int indent = 0)const override;
    std::unique_ptr<Expr> Copy()const override;

};

struct VariableExpr : Expr{
    Token name;
    VariableExpr(Token Name) : name(std::move(Name)){}
    std::string Print(int indent = 0)const override;
    std::unique_ptr<Expr> Copy()const override;

};

struct BinaryExpr : Expr{
    ExprPtr left;
    Token op;
    ExprPtr right;
    BinaryExpr(ExprPtr Left, Token Op, ExprPtr Right) : left(std::move(Left)), op(std::move(Op)), right(std::move(Right)){}
    std::string Print(int indent = 0)const override;
    std::unique_ptr<Expr> Copy()const override;

};

struct UnaryExpr : Expr{
    Token op;
    ExprPtr right;
    UnaryExpr(Token Op, ExprPtr Right) : op(std::move(Op)), right(std::move(Right)){}
    std::string Print(int indent = 0)const override;
    std::unique_ptr<Expr> Copy()const override;

};

struct AssignmentExpr : Expr{
    ExprPtr target_variable;
    ExprPtr value;
    AssignmentExpr(ExprPtr Target, ExprPtr Value) : target_variable(std::move(Target)), value(std::move(Value)){}
    std::string Print(int indent = 0)const override;
    std::unique_ptr<Expr> Copy()const override;
};

struct CallExpr : Expr{
    ExprPtr callee;
    std::vector<ExprPtr> args;
    CallExpr(ExprPtr Callee, std::vector<ExprPtr> Args): callee(std::move(Callee)), args(std::move(Args)){}
    std::string Print(int indent = 0)const override;
    std::unique_ptr<Expr> Copy()const override;
    
};

struct PropertyAccessExpr : Expr{
    ExprPtr object;
    Token accessifier; 
    PropertyAccessExpr(ExprPtr Object, Token Accessifier) : object(std::move(Object)), accessifier(std::move(Accessifier)){}
    std::string Print(int indent = 0)const override;
    std::unique_ptr<Expr> Copy()const override;

};

struct IndexAccessExpr : Expr{
    ExprPtr object;
    ExprPtr index;
    IndexAccessExpr(ExprPtr Object, ExprPtr Index) : object(std::move(Object)), index(std::move(Index)){};
    std::string Print(int indent = 0)const override;
    std::unique_ptr<Expr> Copy()const override;

};




struct GroupingExpr : Expr{
    ExprPtr expression;
    GroupingExpr(ExprPtr Expression) : expression(std::move(Expression)){}
    std::string Print(int indent = 0)const override;
    std::unique_ptr<Expr> Copy()const override;
};

// statements

struct EmptyStmt : Stmt{
    EmptyStmt(){};
    std::string Print(int indent) const override;
};

struct ExpressionStmt : Stmt{
    ExprPtr expression;
    ExpressionStmt(ExprPtr Expression) : expression(std::move(Expression)) {};
    std::string Print(int indent = 0)const override;
};

struct VarDeclarationStmt : Stmt{
    Token name;
    ExprPtr initializer;
    VarModifier moderator;
    VarDeclarationStmt(Token Name, ExprPtr Initializer, VarModifier Mod = VarModifier::none) 
        : name(std::move(Name)), initializer(std::move(Initializer)), moderator(std::move(Mod)){};
    std::string Print(int indent = 0)const override;
};
 
struct BlockStmt : Stmt{
    std::vector<StmtPtr> statements;
    BlockStmt(std::vector<StmtPtr> Statements):statements(std::move(Statements)){};
    std::string Print(int indent = 0)const override;
};

using BlockStmtPtr = std::unique_ptr<BlockStmt>;

struct IfStmt : Stmt{
    ExprPtr condition;
    StmtPtr then_branch;
    StmtPtr else_branch;
    IfStmt(ExprPtr Condition, StmtPtr Then_br, StmtPtr Else_br) : condition(std::move(Condition)), then_branch(std::move(Then_br)), else_branch(std::move(Else_br)){};
    std::string Print(int indent = 0)const override;
};

struct WhileStmt : Stmt{
    ExprPtr condition;
    StmtPtr body;
    WhileStmt(ExprPtr Condition, StmtPtr Body) : condition(std::move(Condition)), body(std::move(Body)){};
    std::string Print(int indent = 0)const override;
};

struct ForStmt : Stmt{
    StmtPtr initializer;
    ExprPtr condition;
    ExprPtr iteration_update;
    StmtPtr body;
    ForStmt(StmtPtr Init, ExprPtr Condition, ExprPtr I_Update, StmtPtr Body) : initializer(std::move(Init)), condition(std::move(Condition)), iteration_update(std::move(I_Update)), body(std::move(Body)) {};

    std::string Print(int indent)const override;
};

struct Continuestmt : Stmt{
    Continuestmt(){};
    std::string Print(int indent)const override;
};
struct BreakStmt : Stmt{
    BreakStmt(){};
    std::string Print(int indent)const override;
};


struct FuncStmt : Stmt{
    Token name;
    std::vector<Token> params;
    StmtPtr body;
    FuncStmt(Token Name, std::vector<Token> Params, StmtPtr Body)
        : name(std::move(Name)), params(std::move(Params)), body(std::move(Body)){};
    std::string Print(int indent = 0)const override;
};

struct ReturnStmt : Stmt{
    ExprPtr value;
    std::string Print(int indent = 0)const override;
    ReturnStmt(ExprPtr Value) : value(std::move(Value)){};
};


//program

struct Prgrm{
    bool debug_enabled = false;
    std::vector<StmtPtr> body;
    std::string file_name;
    std::string file_extension;
    std::string code;
    void Print();
};




#endif //AST