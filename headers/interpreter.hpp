#ifndef INTERPRETER
#define INTERPRETER

#include "program.hpp"
#include "ast.hpp"

enum class ValueType{
    Number,
    Bool,
    String,
    Array,
    Object,
    Function,
    Builtin_function,
    none,

    Err
};

struct ArrayValue;
struct BodyValue;
struct FunctionValue;
// struct Builtin_functionValue;
struct Function_argument;

struct Value{
    std::variant<double, bool, std::string, std::shared_ptr<ArrayValue>, std::shared_ptr<BodyValue>  /*std::shared_ptr<FunctionValue>, std::shared_ptr<Builtin_functionValue>*/> data;
    ValueType type;
    Value(std::variant<double, bool, std::string, std::shared_ptr<ArrayValue>,std::shared_ptr<BodyValue> /*std::shared_ptr<FunctionValue>, std::shared_ptr<Builtin_functionValue>*/> DATA, ValueType Type) : data(std::move(DATA)), type(Type){}
    
    static Value None();
    static Value Error();

    Value Err(std::string msg)const;

    double AsNumber()const;
    bool AsBool()const;
    std::string AsString()const;
    const ArrayValue& AsArray()const;
    ArrayValue& AsArray();
    const BodyValue& AsObject()const;
    BodyValue& AsObject();
    Value To_bool()const;
    Value To_number()const;
    Value To_string()const;
    Value To_array()const;
};

struct Function_argument{
    Value value;
    Value* val_location = nullptr;
    std::string identifier = "";

    bool Is_reference();
    Value& Get();
    Function_argument(Value VALUE = Value::None(), Value* reference = nullptr, std::string Identifier = "") : value(VALUE), val_location(reference), identifier(Identifier){};

};

struct ArrayValue{
    std::vector<Value> elements;
    ArrayValue(std::vector<Value> Elements) : elements(std::move(Elements)){};
};

struct BodyValue{
    std::unordered_map<std::string, Value> elements;
    BodyValue(std::unordered_map<std::string, Value> Elements) : elements(std::move(Elements)){};
};

struct Var{
    Value value;
    VarModifier modifier;
    Var(Value val, VarModifier mod) : value(std::move(val)), modifier(std::move(mod)){};
};

struct AssignmentTarget{
    enum class Kind{
        Variable,
        ArrayElement,
        ObjectElement,
        Error
    };

    Kind kind = Kind::Error;
    Var* variable = nullptr;
    Value* array_element = nullptr;
    Value* object_element = nullptr;
};

struct FunctionValue{
    Token name;
    std::vector<Token> params;
    StmtPtr body;
    FunctionValue(Token Name): name(std::move(Name)){};
};

struct Scope{
    std::shared_ptr<Scope> parent;
    std::unordered_map<std::string, Var> variables;
    std::unordered_map<std::string, FunctionValue> functions;
};

class Operation_calculator{
private:
    Value Err(std::string msg);

public:
    Value Add(const Value& left ,const Value& right);
    Value Subtract(const Value& left ,const Value& right);
    Value Devide(const Value& left ,const Value& right);
    Value Multiply(const Value& left ,const Value& right);
    Value Power(const Value& value ,const Value& power);
    Value BooleanOperation(const Value& left ,const Value& right, const Token& op);

    Value UnaryOperation(const Value& value, const Token& op);
    Value CTo_bool(const Value& value);
    Value CTo_number(const Value& value);
    Value CTo_string(const Value& value);
    Value CTo_array(const Value& value);
};


class Runtime_env{
private:
    Prgrm ast;
    std::shared_ptr<Scope> current_scope;
    Operation_calculator calc;
    enum class Flow{
        Normal,
        Continue,
        Break,
        Return  
    };

    struct FlowStream{
        Flow f = Flow::Normal;
        Value value = Value::None();
    };

    FlowStream flow = {Flow::Normal, Value::None()};

    std::unordered_map<std::string, std::function<Value(std::vector<Function_argument>)>> builltin_functions;
    
    void Declare_builtins();
    void Enter_scope();
    void Exit_scope();
    Var* LookupVar(Token name, Scope* scope);
    FunctionValue* LookupFunc(Token name, Scope* scope);
    Value CalculateOperation(Value right, Value left, Token op);
    void PrintValue(Value v);
    AssignmentTarget ResolveAssignmentTarget(Expr* expression);

    Value Evaluate(Expr* expr);
    Value* GetValueLocationPtr(Expr* expr);
    FlowStream Execute(Stmt* statement);

    FlowStream Exe_Expr_stmt(ExpressionStmt* expr_stmt);
    FlowStream Exe_Vardeclaration_stmt(VarDeclarationStmt* vardeclaration_stmt);
    FlowStream Exe_Block_stmt(BlockStmt* block_stmt);
    FlowStream Exe_If_stmt(IfStmt* if_stmt);
    FlowStream Exe_While_stmt(WhileStmt* while_stmt);
    FlowStream Exe_For_stmt(ForStmt* for_stmt);
    FlowStream Exe_Continue_stmt(Continuestmt* continue_stmt);
    FlowStream Exe_Break_stmt(BreakStmt* break_stmt);
    FlowStream Exe_Func_stmt(FuncStmt* func_stmt);
    FlowStream Exe_Return_stmt(ReturnStmt* return_stmt);
    

public:
    void Run();
    Runtime_env(Prgrm Ast);

};




#endif // INTERPRETER