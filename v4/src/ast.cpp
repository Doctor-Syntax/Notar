#include "../headers/ast.hpp"

//expressions
std::string EmptyExpr::Print(int indent)const{
    return std::string(indent, ' ') + "empty / nullptr\n";
}

std::string LiteralExpr::Print(int indent)const{
    return std::string(indent, ' ') + "Literal expr: [" + value.v + "]\n";
}

std::string ArrayExpr::Print(int indent)const{
    std::string str;

    str += std::string(indent, ' ') + "ArrayExpr:\n";
    str += std::string(indent+2, ' ') + "Elements:\n";
    for(size_t i = 0; i < elements.size(); i++){
        str += std::string(indent, ' ') + elements[i]->Print(indent+2);
    }

    return str;
}

std::string BodyExpr::Print(int indent)const{
    std::string str;

    str += std::string(indent, ' ') + "BodyExpr:\n";
    str += std::string(indent+2, ' ') + "Elements:\n";
    for(const auto& pair : elements){
        str += std::string(indent+4,' ') + "key: [" + pair.first + "] ";
        str += "value: \n" + pair.second->Print(indent+6);
    }

    return str;
}

std::string VariableExpr::Print(int indent)const{
    return std::string(indent, ' ') + "Variable expr: [" + name.v + "]\n";
}

std::string BinaryExpr::Print(int indent)const{

    std::string str;
    str += std::string(indent, ' ') + "binary expr:\n";
    str += std::string(indent + 2, ' ') + "left: \n" + left->Print(indent+4);
    str += std::string(indent + 2, ' ') + "op: ["+op.v+"]\n";
    str += std::string(indent + 2, ' ') + "right: \n" + right->Print(indent+4);
    return str;
}

std::string UnaryExpr::Print(int indent)const{
    std::string str;
    str += std::string(indent, ' ') + "Unary expr: \n";
    str += std::string(indent+2, ' ') + "op: ["+op.v+"]\n";
    str += std::string(indent+2, ' ') + "right: \n" + right->Print(indent+4) + "\n";
    return str;
}

std::string AssignmentExpr::Print(int indent)const{
    std::string str;
    str += std::string(indent, ' ') + "Assignment expr: \n";
    str += std::string(indent+2, ' ') + "target_variable: \n" + target_variable->Print(indent+4);
    str += std::string(indent+2, ' ') + "value: \n" + value->Print(indent+4) + "\n";
    return str;
}

std::string CallExpr::Print(int indent)const{
    std::string str;
    str += std::string(indent, ' ') + "Call expr: \n";
    str += std::string(indent+2, ' ') + "callee: \n" + callee->Print(indent+4) + "\n";
    str += std::string(indent+2,' ') + "agrs: \n";;

    for(size_t i = 0; i < args.size(); i++){
        str += args[i]->Print(indent+4);
    }

    return str;

}

std::string GroupingExpr::Print(int indent)const{
    std::string str;
    str += std::string(indent, ' ') + "Grouping expr: \n";
    str += std::string(indent+2, ' ') + "expr: \n" + expression->Print(indent + 4);
    return str;
}

std::string PropertyAccessExpr::Print(int indent)const{
    std::string str;
    str += std::string(indent, ' ') + "Property access: \n";
    str += std::string(indent+2, ' ') + "object: \n" + object->Print(indent+4);
    str += std::string(indent+2, ' ') + "accessifier: [" + accessifier.v + "]\n";
    return str;
}

std::string IndexAccessExpr::Print(int indent)const{
    std::string str;
    str += std::string(indent, ' ') + "Index access: \n";
    str += std::string(indent+2, ' ') + "object: \n" + object->Print(indent+4);
    str += std::string(indent+2, ' ') + "index: \n" + index->Print(indent+4);
    return str;
}

// expression copy functions

std::unique_ptr<Expr> EmptyExpr::Copy()const{
    return std::make_unique<EmptyExpr>(*this);
}
std::unique_ptr<Expr> LiteralExpr::Copy()const{
    return std::make_unique<LiteralExpr>(*this);
}
std::unique_ptr<Expr> ArrayExpr::Copy()const{
    std::vector<std::unique_ptr<Expr>> ELEMENTS;

    for(size_t i = 0; i < elements.size(); i++){
        ELEMENTS.emplace_back(elements[i]->Copy());
    }
    return std::make_unique<ArrayExpr>(
        std::move(ELEMENTS)
    );
}
std::unique_ptr<Expr> BodyExpr::Copy()const{
    std::unordered_map<std::string, std::unique_ptr<Expr>> ELEMENTS;

    for(const auto& pair : elements){
        ELEMENTS.emplace(pair.first, pair.second->Copy());
    }

    return std::make_unique<BodyExpr>(
        std::move(ELEMENTS)
    );
}
std::unique_ptr<Expr> VariableExpr::Copy()const{
    return std::make_unique<VariableExpr>(*this);
}
std::unique_ptr<Expr> BinaryExpr::Copy()const{
    return std::make_unique<BinaryExpr>(
        left->Copy(),
        op,
        right->Copy()
    );
}
std::unique_ptr<Expr> UnaryExpr::Copy()const{
    return std::make_unique<UnaryExpr>(
        op,
        right->Copy()
    );
}
std::unique_ptr<Expr> AssignmentExpr::Copy()const{
    return std::make_unique<AssignmentExpr>(
        std::move(target_variable->Copy()),
        value->Copy()
    );
}
std::unique_ptr<Expr> CallExpr::Copy()const{
    std::vector<std::unique_ptr<Expr>> ARGS;
    for(size_t i = 0; i < args.size(); i++){
        ARGS.emplace_back(args[i]->Copy());
    }
    return std::make_unique<CallExpr>(
        callee->Copy(),
        std::move(ARGS)
    );
}
std::unique_ptr<Expr> GroupingExpr::Copy()const{
    return std::make_unique<GroupingExpr>(
        expression->Copy()
    );
}
std::unique_ptr<Expr> PropertyAccessExpr::Copy()const{
    return std::make_unique<PropertyAccessExpr>(
        object->Copy(),
        accessifier
    );
}
std::unique_ptr<Expr> IndexAccessExpr::Copy()const{
    return std::make_unique<IndexAccessExpr>(
        object->Copy(),
        index->Copy()
    );
}


// statements

std::string EmptyStmt::Print(int indent)const{
    return std::string(indent, ' ') + "nullptr\n";
}

std::string ExpressionStmt::Print(int indent)const{
    std::string str;
    str += std::string(indent, ' ') + "Expression stmt: \n";
    str += std::string(indent+2, ' ') + "expr: \n" + expression->Print(indent+4);
    return str;
}

std::string VarDeclarationStmt::Print(int indent)const{
    std::string str;

    if(initializer){
        str += std::string(indent, ' ') + "VarDeclaration stmt: \n";
        str += std::string(indent+2, ' ') + "name: [" + name.v + "]\n";
        str += std::string(indent+2, ' ') + "initalizer: \n" + initializer->Print(indent+4);
        str += std::string(indent+2, ' ') + "modifier: [" + std::to_string(static_cast<int>(moderator)) + "]\n";
    }
    else{
        str += std::string(indent, ' ') + "VarDeclaration stmt: \n";
        str += std::string(indent+2, ' ') + "name: [" + name.v + "]\n";
        str += std::string(indent+2, ' ') + "initalizer: nullptr";
        str += std::string(indent+2, ' ') + "modifier: [" + std::to_string(static_cast<int>(moderator)) + "]\n";
    }

    return str;

}

std::string BlockStmt::Print(int indent)const{
    std::string str;
    str += std::string(indent,' ') + "Block stmt: \n";
    str += std::string(indent+2, ' ') + "body: \n";

    for(size_t i = 0; i < statements.size(); i++){
        str += std::string(indent, ' ') + statements[i]->Print(indent+4);
    }
    return str;
}

std::string IfStmt::Print(int indent)const{
    std::string str;

    str += std::string(indent, ' ') + "If stmt: \n";
    str += std::string(indent+2, ' ') + "Then branch: \n";
    str += std::string(indent+2, ' ') + then_branch->Print(indent+4);
    str += std::string(indent+2, ' ') + "Else branch: \n";
    str += std::string(indent+2, ' ') + else_branch->Print(indent+4);

    return str;
}

std::string WhileStmt::Print(int indent)const{
    std::string str;
    str += std::string(indent, ' ') + "While stmt: \n";
    str += std::string(indent+2, ' ') + "condition: \n" + condition->Print(indent+4);
    str += std::string(indent+2, ' ') + "body: \n" + body->Print(indent+4);
    return str;
}

std::string Continuestmt::Print(int indent)const{
    return std::string(indent, ' ') + "Continue stmt\n";
}

std::string BreakStmt::Print(int indent)const{
    return std::string(indent, ' ') + "Break stmt\n";
}

std::string ForStmt::Print(int indent)const{
    std::string str;

    str += std::string(indent, ' ') + "For stmt:\n";
    str += std::string(indent+2, ' ') + "initializer:\n" + initializer->Print(indent+4);
    str += std::string(indent+2, ' ') + "condition:\n" + condition->Print(indent+4);
    str += std::string(indent+2, ' ') + "iteration_update:\n" + iteration_update->Print(indent+4);
    str += std::string(indent+2, ' ') + "body:\n" + body->Print(indent+4);
    return str;
}

std::string FuncStmt::Print(int indent)const{
    std::string str;
    str += std::string(indent, ' ') + "Func stmt: \n";
    str += std::string(indent+2, ' ') + "name: [" += name.v + "]\n";
    str += std::string(indent+2, ' ') + "arguments: \n";
    for(size_t i = 0; i < params.size(); i++){
        str += std::string(indent+4, ' ') + "[" + params[i].v + "]\n";
    }
    str += std::string(indent+2, ' ') + "body: \n" + body->Print(indent+4);
    return str;
}

std::string ReturnStmt::Print(int indent)const{
    std::string str; 
    str += std::string(indent, ' ') + "return stmt:\n";
    str += std::string(indent+2, ' ') + value->Print(indent+4);
    return str;
}

//Program

void Prgrm::Print(){
    msg.Message("\n[AST]: syntax tree start ---\n",green);
    for(size_t i = 0; i < body.size(); i++){
        msg.Message(body[i]->Print(4));
    }
    msg.Message("\n[AST]: syntax tree end ---\n", green);
}