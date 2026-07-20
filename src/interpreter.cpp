#include "../headers/interpreter.hpp"

void Runtime_env::Declare_builtins(){
    builltin_functions["__out"] = [this](std::vector<Function_argument> args) -> Value{
        if(args.empty()){
            return Value::None();
        }
        for(auto& val : args){
            if(val.Is_reference()) PrintValue(*val.val_location);
            else PrintValue(val.value);
        }
        return Value::None();
    };
    builltin_functions["__in"] = [this](std::vector<Function_argument> args) -> Value{
        std::string input = "\0";

        if(args.empty()){
            std::cin>>input;
            return Value(input, ValueType::String);
        }

        for(auto& arg : args){
            std::cin>>input;
            
            arg.Get().data = input;
            arg.Get().type = ValueType::String;
            
            input = "\0";
        }

        return Value::None();
    };
    builltin_functions["__cast"] = [this](std::vector<Function_argument> args) -> Value{
        if(args.size() <= 1) return Value::None();

        std::unordered_map<ValueType, std::function<Value(const Value&)>> type_converter = {
            {ValueType::Bool, [](const Value& v){return v.To_bool();}},
            {ValueType::Number, [](const Value& v){return v.To_number();}},
            {ValueType::String, [](const Value& v){return v.To_string();}},
            {ValueType::Array, [](const Value& v){return v.To_array();}},
        };

        auto cast_type = args[0].Get().type;

        if(args.size() == 2) {
            auto function = type_converter[cast_type];
            return function(args[1].Get());
        }

        std::vector<Value> output_values;

        for(size_t i = 1; i < args.size(); i++){
            auto function = type_converter[cast_type];
            output_values.emplace_back(function(args[i].Get()));
        }
        
        return Value(std::make_shared<ArrayValue>(std::move(output_values)), ValueType::Array);
    };
}

bool Function_argument::Is_reference(){
    return val_location != nullptr;
}

Value& Function_argument::Get(){
    if(val_location) return *val_location;
    return value;
}

Runtime_env::Runtime_env(Prgrm Ast) : ast(std::move(Ast)), current_scope(std::make_shared<Scope>()){
    current_scope->parent = nullptr;
    Declare_builtins();
};

double Value::AsNumber()const{
    return std::get<double>(data);
}
bool Value::AsBool()const{
    return std::get<bool>(data);
}
std::string Value::AsString()const{
    return std::get<std::string>(data);
}
const ArrayValue& Value::AsArray()const{
    return *std::get<std::shared_ptr<ArrayValue>>(data);
}
ArrayValue& Value::AsArray(){
    return *std::get<std::shared_ptr<ArrayValue>>(data);
}
const BodyValue& Value::AsObject()const{
    return *std::get<std::shared_ptr<BodyValue>>(data);
}
BodyValue& Value::AsObject(){
    return *std::get<std::shared_ptr<BodyValue>>(data);
}

void Runtime_env::PrintValue(Value v){
    if(v.type != ValueType::Err) std::cout<<v.To_string().AsString();
    else std::cout<<"[Err]";
}

Value Value::None(){
    return Value(0.0, ValueType::none);
}
Value Value::Error(){
    return Value(0.0, ValueType::Err);
}

Value Operation_calculator::Err(std::string msg){
    std::cout<< msg <<"\n";
    return Value(0.0,ValueType::Err);
}
Value Value::Err(std::string msg)const{
    std::cout<< msg <<"\n";
    return Value(0.0,ValueType::Err);
}

Value Value::To_bool()const{
    auto value_t = type;
    
    if(value_t == ValueType::none || value_t == ValueType::Err) return Err("error, can not convert a 'none' or 'error' value.");

    if(value_t == ValueType::Number) {
        if(AsNumber() == 0) return Value(false, ValueType::Bool);
        return Value(true, ValueType::Bool);
    }
    else if(value_t == ValueType::Bool) {
        return Value(AsBool(), ValueType::Bool);
    }
    else if(value_t == ValueType::String) {
        if(AsString().empty()) return Value(false, ValueType::Bool);
        return Value(true, ValueType::Bool);
    }
    else return Err("error, can not convert an unsupported type or type value of 'none' or 'Err'.");
}
Value Value::To_string()const{
    auto value_t = type;
    
    if(value_t == ValueType::Err) return Err("error, can not convert a 'none' or 'error' value.");

    if(value_t == ValueType::Number) {
        std::ostringstream oss;
        oss << AsNumber();
        return Value(oss.str(), ValueType::String);
    }
    else if(value_t == ValueType::Bool) {
        return Value(((AsBool())?  "true":"false"), ValueType::String);
    }
    else if(value_t == ValueType::String) {
        return Value(AsString(), ValueType::String);
    }
    else if(value_t == ValueType::Array){
        std::string str = "[";
        for(size_t i = 0; i < AsArray().elements.size(); i++) {
            if(i != AsArray().elements.size()-1) {
                str += AsArray().elements[i].To_string().AsString() + ",";
                continue;
            }
            str += AsArray().elements[i].To_string().AsString();
        }
        str += "]";

        return Value(str,ValueType::String);
    }
    else if(value_t == ValueType::Object){
        std::string str = "{\n";
        for(auto &element : AsObject().elements) str += element.first + ":" + element.second.To_string().AsString() + ",\n";
        str += "\n}";

        return Value(str,ValueType::String);
    }
    else if(value_t == ValueType::none) return Value("none",ValueType::String);
    else return Err("error, can not convert an unsupported type or type value of 'none' or 'Err'.");
}
Value Value::To_number()const{
    auto value_t = type;
    
    if(value_t == ValueType::none || value_t == ValueType::Err) return Err("error, can not convert a 'none' or 'error' value.");

    if(value_t == ValueType::Number) {
        return Value(AsNumber(), ValueType::Number);
    }
    else if(value_t == ValueType::Bool) {
        return Value(((AsBool())? double(1):double(0)), ValueType::Number);
    }
    else if(value_t == ValueType::String) {
        double result;
        try{
            result = std::stod(AsString());
        }
        catch(const std::invalid_argument&){
            return Err("error, couldn't convert [" + AsString() + "] to a number.");
        }
        catch(const std::out_of_range&){
            return Err("error, number [" + AsString() + "] out of range.");
        }

        return Value(result, ValueType::Number);
    }
    else return Err("error, can not convert an unsupported type or type value of 'none' or 'Err'.");
}
Value Value::To_array()const{
    auto value_t = type;
    
    if(value_t == ValueType::none || value_t == ValueType::Err) return Err("error, can not convert a 'none' or 'error' value.");

    if(value_t == ValueType::Number) return Err("can not convert a number to an array.");
    else if(value_t == ValueType::Bool) return Err("can not convert a boolean to an array.");
    else if(value_t == ValueType::String) return Err("can not convert a string to an array fro this version (v2).");
    else return Err("error, can not convert an unsupported type or type value of 'none' or 'Err'.");
}


Value Operation_calculator::CTo_bool(const Value& value){
    return value.To_bool();
}
Value Operation_calculator::CTo_string(const Value& value){
    return value.To_string();
}
Value Operation_calculator::CTo_number(const Value& value){
    return value.To_number();
}
Value Operation_calculator::CTo_array(const Value& value){
    return value.To_array();
}


Value Operation_calculator::Add(const Value& left ,const Value& right){
    auto left_t = left.type;
    auto right_t = right.type;

    if(right_t == ValueType::none || right_t == ValueType::Err) return Err("error, can not do operation with a 'none' or 'error' value.");

    if(left_t == ValueType::Number){
        if(right_t == ValueType::Number) return Value(left.AsNumber() + right.AsNumber(), ValueType::Number);
        else if(right_t == ValueType::Bool){
            if(right.AsBool()) return Value(left.AsNumber() + 1, ValueType::Number);
            return Value(left.AsNumber(), ValueType::Number);
        }
        else if(right_t == ValueType::String){
            std::ostringstream oss;
            oss << left.AsNumber();
            return Value(oss.str() + right.AsString(), ValueType::String);
        }
    }
    else if(left_t == ValueType::Bool){
        if(right_t == ValueType::Number) return Err("cant add a number to a boolean.");
        else if(right_t == ValueType::Bool) return Err("cant add two booleans with each other.");
        else if(right_t == ValueType::String) return Value(((left.AsBool())? "true":"false") + right.AsString(), ValueType::String);
    }
    else if(left_t == ValueType::String){
        if(right_t == ValueType::Number) {
            std::ostringstream oss;
            oss << right.AsNumber();
            return Value(left.AsString() + oss.str(), ValueType::String);
        }
        else if(right_t == ValueType::Bool) return Value(left.AsString() + ((right.AsBool())? "true":"false"), ValueType::String);
        else if(right_t == ValueType::String) return Value(left.AsString() + right.AsString(), ValueType::String);
    }
    else return Err("error, can not do operation with a 'none' or 'error' value.");

    return Err("error, not handled operation.");
}
Value Operation_calculator::Subtract(const Value& left ,const Value& right){
    auto left_t = left.type;
    auto right_t = right.type;

    if(right_t == ValueType::none || right_t == ValueType::Err) return Err("error, can not do operation with a 'none' or 'error' value.");

    if(left_t == ValueType::Number){
        if(right_t == ValueType::Number) return Value(left.AsNumber() - right.AsNumber(), ValueType::Number);
        else if(right_t == ValueType::Bool){
            if(right.AsBool()) return Value(left.AsNumber() - 1, ValueType::Number);
            return Value(left.AsNumber(), ValueType::Number);
        }
        else if(right_t == ValueType::String) return Err("error, can not subtract a string from a number.");
    }
    else if(left_t == ValueType::Bool){
        if(right_t == ValueType::Number) return Err("cant subtract a number from a boolean.");
        else if(right_t == ValueType::Bool) return Err("cant subtract a boolean from a boolean.");
        else if(right_t == ValueType::String) return Err("cant subtract a string from a boolean.");
    }
    else if(left_t == ValueType::String){
        if(right_t == ValueType::Number) {
            Value right_string = Value(std::to_string(right.AsNumber()), ValueType::String);
            return Subtract(left, right_string);
        }
        else if(right_t == ValueType::Bool) return Err("error, can not subtract a boolean from a string.");
        else if(right_t == ValueType::String) {
            size_t pos = left.AsString().find(right.AsString());
            
            if(pos != std::string::npos){
               if(pos == left.AsString().length()-right.AsString().length()){
                    return Value(left.AsString().erase(pos, right.AsString().length()),ValueType::String);
                }
            }

            std::cout<<"couldn't find substring in string to subtract or it didn't exist.\n";
            return Value(left.AsString(), ValueType::String);
        }
    }
    else return Err("error, can not do operation with a 'none' or 'error' value.");
    return Err("error, not handled operation.");

}
Value Operation_calculator::Devide(const Value& left ,const Value& right){
    auto left_t = left.type;
    auto right_t = right.type;

    if(right_t == ValueType::none || right_t == ValueType::Err) return Err("error, can not do operation with a 'none' or 'error' value.");

    if(left_t == ValueType::Number){
        if(right_t == ValueType::Number) {
            if(right.AsNumber() == 0) return Err("error, can not devide by zero.");
            return Value(left.AsNumber() / right.AsNumber(), ValueType::Number);
        }
        else if(right_t == ValueType::Bool) return Err("error, can not devide a number by a boolean.");
        else if(right_t == ValueType::String) return Err("error, can not devide a number by a string.");
    }
    else if(left_t == ValueType::Bool){
        if(right_t == ValueType::Number) return Err("error, can not devide a boolean by a number.");
        else if(right_t == ValueType::Bool) return Err("error, can not devide a boolean by a boolean.");
        else if(right_t == ValueType::String) return Err("error, can not devide a boolean by a string.");
    }
    else if(left_t == ValueType::String){
        if(right_t == ValueType::Number) {
            Value result = Devide(Value(left.To_number().AsNumber(), ValueType::Number), right);
            std::ostringstream oss;
            oss << result.AsNumber();
            return Value(oss.str(), ValueType::String);
        }
        else if(right_t == ValueType::Bool) return Err("error, can not devide a string by a boolean.");
        else if(right_t == ValueType::String) return Err("error, can not devide a string by a string.");
    }
    else return Err("error, can not do operation with a 'none' or 'error' value.");
    return Err("error, not handled operation.");
    
}
Value Operation_calculator::Multiply(const Value& left ,const Value& right){
    auto left_t = left.type;
    auto right_t = right.type;

    if(right_t == ValueType::none || right_t == ValueType::Err) return Err("error, can not do operation with a 'none' or 'error' value.");
    

    if(left_t == ValueType::Number){
        if(right_t == ValueType::Number) return Value(left.AsNumber() * right.AsNumber(), ValueType::Number);
        else if(right_t == ValueType::Bool) return Value(left.AsNumber() * ((right.AsBool())? 1:0), ValueType::Number);
        else if(right_t == ValueType::String) return Err("error, can not multiply a number with a string.");
    }
    else if(left_t == ValueType::Bool){
        if(right_t == ValueType::Number) return Value(((left.AsBool())? 1:0) * right.AsNumber(), ValueType::Number);
        else if(right_t == ValueType::Bool) return Err("error, can not multiply a boolean with a boolean.");
        else if(right_t == ValueType::String) return Err("error, can not multiply a boolean with a string.");
    }
    else if(left_t == ValueType::String){
        if(right_t == ValueType::Number) return Err("error, can not multiply a string with a number.");
        else if(right_t == ValueType::Bool) return Err("error, can not multiply a string with a boolean.");
        else if(right_t == ValueType::String) return Err("error, can not multiply a string with a string.");
    }
    else return Err("error, can not do operation with a 'none' or 'error' value.");
    return Err("error, not handled operation.");

}
Value Operation_calculator::Power(const Value& value ,const Value& power){
    auto value_t = value.type;
    auto power_t = power.type;

    if(power_t == ValueType::none || power_t == ValueType::Err) return Err("error, can not do operation with a 'none' or 'error' value.");
    

    if(value_t == ValueType::Number){
        if(power_t == ValueType::Number) return Value(std::pow(value.AsNumber(), power.AsNumber()), ValueType::Number);
        else if(power_t == ValueType::Bool) return Err("error, can not do a number to the power of a boolean.");
        else if(power_t == ValueType::String) return Err("error, can not do a number to the power of a string.");
    }
    else if(value_t == ValueType::Bool || value_t == ValueType::String) return Err("error, can not do a boolean or string to the power of a value.");
    else return Err("error, can not do operation with a 'none' or 'error' value.");
    return Err("error, not handled operation.");
}
Value Operation_calculator::UnaryOperation(const Value& value, const Token& op){
    auto value_t = value.type;
    auto opv = op.v;
    
    if(value_t == ValueType::none || value_t == ValueType::Err) return Err("error, can not apply unary to a 'none' or 'error' value.");

    if(value_t == ValueType::Number) {
        if(opv == "-") return Value(-value.AsNumber(), ValueType::Number);
        else if(opv == "+") return Value(value.AsNumber(), ValueType::Number);
        else return Err("can not apply the [" + opv + "] operator to a number.");
    }
    else if(value_t == ValueType::Bool) {
        if(opv == "!") return Value(!value.AsBool(), ValueType::Bool);
        else return Err("can not apply the [" + opv + "] operator to a boolean.");
    }
    else if(value_t == ValueType::String) return Err("error, can not do a unary aka [" + opv + "] operation to a string value.");
    return Err("error, not handled operation.");
}
Value Operation_calculator::BooleanOperation(const Value& left, const Value& right, const Token& op){
    auto left_t = left.type;
    auto right_t = right.type;
    auto opv = op.v;

    auto left_b = CTo_bool(left).AsBool();
    auto right_b = CTo_bool(right).AsBool();

    if(CTo_bool(left).type == ValueType::Err || CTo_bool(right).type == ValueType::Err) return Err("error, couldn't convert value to boolean for comparison.");
    if(left_t == ValueType::Err) return Err("error, can not do a boolean operation on a 'Err' value.");


    if(opv == "=="){
        if(left_t != right_t) return Value(false, ValueType::Bool);
        return Value(((left.data == right.data)? true:false), ValueType::Bool);
    }
    else if(opv == "!="){
        if(left_t != right_t) return Value(true, ValueType::Bool);
        return Value(((left.data != right.data)? true:false), ValueType::Bool);
    }
    else if(opv == "&&") return Value(((left_b && right_b)? true:false), ValueType::Bool);
    else if(opv == "||") return Value(((left_b || right_b)? true:false), ValueType::Bool);

    if(left_t != right_t) return Err("error, can not compare two different values.");

    if(opv == ">"){
        if(left_t == ValueType::String) return Value(((left.AsString().length() > right.AsString().length())? true:false), ValueType::Bool);
        else if(left_t == ValueType::Bool) return Value(((left.To_number().AsNumber() > right.To_number().AsNumber())? true:false), ValueType::Bool);
        else if(left_t == ValueType::Number) return Value(((left.AsNumber() > right.AsNumber())? true:false), ValueType::Bool);
    }
    else if(opv == "<"){
        if(left_t == ValueType::String) return Value(((left.AsString().length() < right.AsString().length())? true:false), ValueType::Bool);
        else if(left_t == ValueType::Bool) return Value(((left.To_number().AsNumber() < right.To_number().AsNumber())? true:false), ValueType::Bool);
        else if(left_t == ValueType::Number) return Value(((left.AsNumber() < right.AsNumber())? true:false), ValueType::Bool);
    }
    else if(opv == "<="){
        if(left_t == ValueType::String) return Value(((left.AsString().length() <= right.AsString().length())? true:false), ValueType::Bool);
        else if(left_t == ValueType::Bool) return Value(((left.To_number().AsNumber() <= right.To_number().AsNumber())? true:false), ValueType::Bool);
        else if(left_t == ValueType::Number) return Value(((left.AsNumber() <= right.AsNumber())? true:false), ValueType::Bool);
    }
    else if(opv == ">="){
        if(left_t == ValueType::String) return Value(((left.AsString().length() >= right.AsString().length())? true:false), ValueType::Bool);
        else if(left_t == ValueType::Bool) return Value(((left.To_number().AsNumber() >= right.To_number().AsNumber())? true:false), ValueType::Bool);
        else if(left_t == ValueType::Number) return Value(((left.AsNumber() >= right.AsNumber())? true:false), ValueType::Bool);
    }
    
    return Err("error, not handled boolean operation.");
}

void Runtime_env::Enter_scope(){
    auto new_scope = std::make_shared<Scope>();
    new_scope->parent = current_scope;
    current_scope = new_scope;
}
void Runtime_env::Exit_scope(){
    current_scope = current_scope->parent;
}
Var* Runtime_env::LookupVar(Token name, Scope* scope){
    auto it = scope->variables.find(name.v);

    if(it != scope->variables.end()) return &it->second;
    if(!scope->parent){
        std::cout << "Runtime error: variable not found\n";
        return nullptr;
    }

    return LookupVar(name, scope->parent.get());
}
FunctionValue* Runtime_env::LookupFunc(Token name, Scope* scope){
    auto it = scope->functions.find(name.v);
    // std::cout<< "in lookup -- " << current_scope << "\n";
    // std::cout<<"in lookup -- ident: " << name.v << "\n";
    // std::cout << "lookup for: " << name.v << '\n';

    // for(auto &param : scope->functions) std::cout<<"list in lookup -- " << param.first<<"\n";

    if(it != scope->functions.end()) return &it->second;
    if(!scope->parent){
        std::cout << "Runtime error: variable not found\n";
        return nullptr;
    }

    return LookupFunc(name, scope->parent.get());
}
AssignmentTarget Runtime_env::ResolveAssignmentTarget(Expr* expr){
    AssignmentTarget target;
    if(auto var = dynamic_cast<VariableExpr*>(expr)) {
        target.variable = LookupVar(var->name, current_scope.get());

        if(target.variable) target.kind = AssignmentTarget::Kind::Variable;
        else target.kind = AssignmentTarget::Kind::Error;
        return target;
    }
    else if(auto var = dynamic_cast<IndexAccessExpr*>(expr)) {
        if(!var->object){
            std::cout<<"error, the object to acces as an array doesn't exist.\n";
            return target;
        }

        auto obj_val = GetValueLocationPtr(var->object.get());
        if(obj_val->type != ValueType::Array){
            std::cout<<"error, object is not accessible as an array.\n";
            return target;
        }

        auto index = Evaluate(var->index.get());
        if(index.type != ValueType::Number){
            std::cout<<"error, can not acces an array with a non number type index.\n";
            return target;
        }


        double index_under_zero = ((index.AsNumber() < 0)? -1:index.AsNumber());
        if(index_under_zero == -1){
            std::cout<<"error, can not acces an array with an index value les then zero.\n";
            return target;
        }
        else if(index_under_zero >= obj_val->AsArray().elements.size()){
            std::cout<<"error, can not acces an array with an index value that is bigger than the array.\n";
            return target;
        }
        else if(std::floor(index_under_zero) != index_under_zero){
            std::cout<<"error, can not acces an array with an index value that is not an integer.\n";
            return target;
        }

        size_t verified_index = index_under_zero;

        target.kind = AssignmentTarget::Kind::ArrayElement;
        target.array_element = &obj_val->AsArray().elements[verified_index];
        return target;
    }
    else if(auto var = dynamic_cast<PropertyAccessExpr*>(expr)){
        if(!var->object){
            std::cout<<"error, the body to access as an object does not exist.\n";
            return target;
        }

        auto object_val = GetValueLocationPtr(var->object.get());
        if(object_val->type != ValueType::Object){
            std::cout<<"error, body is not accessible as an object.\n";
            return target;
        }


        if(object_val->AsObject().elements.find(var->accessifier.v) != object_val->AsObject().elements.end()){
            target.kind = AssignmentTarget::Kind::ObjectElement;
            auto pair = object_val->AsObject().elements.find(var->accessifier.v);
            target.object_element = &pair->second;
            return target;
        }
        else{
            std::cout<<"error, the accesssifier [" + var->accessifier.v + "] to an object does not exist.\n";
            return target;
        }

    }
    return target;
}

Value Runtime_env::CalculateOperation(Value left, Value right, Token op){
    if(op.v == "+") return calc.Add(left,right);
    else if(op.v == "-") return calc.Subtract(left,right);
    else if(op.v == "*") return calc.Multiply(left,right);
    else if(op.v == "/") return calc.Devide(left,right);
    else if(op.v == "**") return calc.Power(left,right);
    else if(op.v == "==" || op.v == "!=" || op.v == ">" || op.v == "<" || op.v == "&&" || op.v == "||" ||
            op.v == ">=" || op.v == "<=") return calc.BooleanOperation(left,right,op);
    std::cout<<"err, unhandled operation in operation calculation.\n";
    return Value(0.0, ValueType::Err);
}

Value Runtime_env::Evaluate(Expr* expr){
    if(auto literal_expr = dynamic_cast<LiteralExpr*>(expr)){
        if(literal_expr->value.t == TokenType::Number) return Value(std::stod(literal_expr->value.v), ValueType::Number);
        else if(literal_expr->value.t == TokenType::Boolean) return Value((literal_expr->value.v == "false")? false:true, ValueType::Bool);
        else if(literal_expr->value.t == TokenType::String) return Value(literal_expr->value.v, ValueType::String);
    }
    else if(auto var_expr = dynamic_cast<VariableExpr*>(expr)){
        if(LookupVar(var_expr->name, current_scope.get())){
            return Value(LookupVar(var_expr->name, current_scope.get())->value.data, LookupVar(var_expr->name, current_scope.get())->value.type);
        }
        std::cout << "Undefined variable: " << var_expr->name.v << "\n";
        return Value(0.0,ValueType::Err); 
    }
    else if(auto bin_expr = dynamic_cast<BinaryExpr*>(expr)){
        auto left = Evaluate(bin_expr->left.get());
        auto right = Evaluate(bin_expr->right.get());
        Value result = CalculateOperation(left, right, bin_expr->op);
        return result;
    }
    else if(auto group_expr = dynamic_cast<GroupingExpr*>(expr)){
        Value value = Evaluate(group_expr->expression.get());
        return value;
    }
    else if(auto unary_expr = dynamic_cast<UnaryExpr*>(expr)){
        auto right_val = Evaluate(unary_expr->right.get());
        Value result = calc.UnaryOperation(right_val, unary_expr->op);
        return result;
    }
    else if(auto assignment_expr = dynamic_cast<AssignmentExpr*>(expr)){
        auto variable = ResolveAssignmentTarget(assignment_expr->target_variable.get());

        if(!assignment_expr->value){
            std::cout<<"err, right side for assignment doesn't exist.\n";
            return Value(0.0, ValueType::Err);
        }

        auto assignment_value = Evaluate(assignment_expr->value.get());

        if(assignment_value.type == ValueType::Err){
            std::cout<<"err, the assignment expr evaluated to Err.\n";
            return Value(0.0, ValueType::Err);
        }
        if(variable.kind == AssignmentTarget::Kind::Error){
            std::cout<<"err, variables doesn't exist in this scope or parent scopes.\n";
            return Value(0.0, ValueType::Err);
        }

        if(variable.kind == AssignmentTarget::Kind::Variable){
            Var* var = variable.variable;
            if(var->modifier == VarModifier::fix){
                std::cout<<"err, can not change a fixed variable.\n";
                return Value(0.0, ValueType::Err);
            }
            else if(var->modifier == VarModifier::pure){
                if(var->value.type != assignment_value.type){
                    std::cout<<"err, can not change the type of a variable with modifier pure to another type.\n";
                    return Value(0.0, ValueType::Err);
                }
                var->value = assignment_value;
                return Value(var->value.data, var->value.type);
            }
            else{
                var->value = assignment_value;
                return Value(var->value.data, var->value.type);
            }
        }
        else if(variable.kind == AssignmentTarget::Kind::ArrayElement){
            *variable.array_element = assignment_value;
            return Value(variable.array_element->data,variable.array_element->type);
        }
        else if(variable.kind == AssignmentTarget::Kind::ObjectElement){
            *variable.object_element = assignment_value;
            return Value(variable.object_element->data,variable.object_element->type);
        }
    }
    else if(auto array_expr = dynamic_cast<ArrayExpr*>(expr)){
        std::vector<Value> elements = {};
        for(size_t i = 0; i < array_expr->elements.size(); i++){
            elements.emplace_back(Evaluate(array_expr->elements[i].get()));
        }

        return Value(std::make_shared<ArrayValue>(std::move(elements)),ValueType::Array);
    }
    else if(auto index_expr = dynamic_cast<IndexAccessExpr*>(expr)){
        auto object = Evaluate(index_expr->object.get());
        auto index = Evaluate(index_expr->index.get());

        if(index.type != ValueType::Number){
            std::cout<<"error, can not acces an array or string with a non number value.\n";
            return Value(0.0, ValueType::Err);
        }

        if(object.type != ValueType::Array){
            std::cout<<"error, the object type of [";
            PrintValue(object);
            std::cout<<"] value is not accessible like an array.\n";
            return Value(0.0, ValueType::Err);
        }

        double index_under_zero = ((index.AsNumber() < 0)? -1:index.AsNumber());
        if(index_under_zero == -1){
            std::cout<<"error, can not acces an array with an index value les then zero.\n";
            return Value(0.0, ValueType::Err);
        }
        else if(index_under_zero >= object.AsArray().elements.size()){
            std::cout<<"error, can not acces an array with an index value that is bigger than the array.\n";
            return Value(0.0, ValueType::Err);
        }
        else if(std::floor(index_under_zero) != index_under_zero){
            std::cout<<"error, can not acces an array with an index value that is not an integer.\n";
            return Value(0.0, ValueType::Err);
        }

        size_t verified_index = index_under_zero;
        Value result = object.AsArray().elements[verified_index];
        return result;
    }
    else if(auto property_expr = dynamic_cast<PropertyAccessExpr*>(expr)){
        auto object = Evaluate(property_expr->object.get());
        
        if(object.type != ValueType::Object){
            std::cout<<"error, can not acces a non object-type value.\n";
            return Value(0.0, ValueType::Err);
        }

        if(object.AsObject().elements.find(property_expr->accessifier.v) != object.AsObject().elements.end()){
            auto pair = object.AsObject().elements.find(property_expr->accessifier.v);
            return pair->second;
        }
        else{
            std::cout<<"error, the accesssifier [" + property_expr->accessifier.v + "] for the object [";
            PrintValue(object);
            std::cout<<"] does not exist.\n";
            return Value(0.0, ValueType::Err);
        }
    }
    else if(auto object_expr = dynamic_cast<BodyExpr*>(expr)){
        std::unordered_map<std::string, Value> ELEMENTS = {};

        for(const auto& pair : object_expr->elements){
            auto evaluated_value = Evaluate(pair.second.get());
            ELEMENTS.emplace(pair.first, evaluated_value);
        }
        return Value(std::make_shared<BodyValue>(ELEMENTS), ValueType::Object);
    }
    else if(auto call_expr = dynamic_cast<CallExpr*>(expr)){
        std::vector<Function_argument> args;        
        auto identifier = dynamic_cast<VariableExpr*>(call_expr->callee.get());

        if(!identifier){
            std::cout<<"can not call a function something elses than an identifier.\n";
            return Value::Error();
        }

        // std::cout<< current_scope << "\n";
        // std::cout<<"ident: " << identifier->name.v << "\n";

        // for(auto &param : current_scope->functions){
        //     std::cout<< "list -- " <<param.first<<"\n";
        // } func = LookupFunc(Token(TokenType::Identifier,identifier->name.v,0,0,0,0,ast.file_name), current_scope.get());

        auto builtin = builltin_functions.find(identifier->name.v);
        if(builtin != builltin_functions.end()) {
            for(size_t i = 0; i < call_expr->args.size(); i++){
                Function_argument ARG;

                if(auto unary = dynamic_cast<UnaryExpr*>(call_expr->args[i].get())){
                    if(unary->op.v == "&"){

                        auto var_location = ResolveAssignmentTarget(unary->right.get());
                        if(var_location.variable) ARG.val_location = &var_location.variable->value;
                        else if(var_location.array_element) ARG.val_location = var_location.array_element;
                        else if(var_location.object_element) ARG.val_location = var_location.object_element;
                        else {
                            std::cout<<"variable location does not exist, cannot pass by reference to function.\n";
                            return Value::Error(); 
                        }


                        args.emplace_back(ARG);
                        continue;
                    }
                }
                ARG.value = Evaluate(call_expr->args[i].get());
                args.emplace_back(ARG);
            }

            return builtin->second(args);
        }
        else if(auto func = LookupFunc(Token(TokenType::Identifier,identifier->name.v,0,0,0,0,ast.file_name), current_scope.get())){
            if(call_expr->args.size() != func->params.size()){
                std::cout<<"err, not enough or to many arguments given for function call.\n";
                return Value::Error();
            }

            for(size_t i = 0; i < call_expr->args.size(); i++){
                Function_argument ARG;
                ARG.identifier = func->params[i].v;

                if(auto unary = dynamic_cast<UnaryExpr*>(call_expr->args[i].get())){
                    if(unary->op.v == "&"){

                        auto var_location = ResolveAssignmentTarget(unary->right.get());
                        if(var_location.variable) ARG.val_location = &var_location.variable->value;
                        else if(var_location.array_element) ARG.val_location = var_location.array_element;
                        else if(var_location.object_element) ARG.val_location = var_location.object_element;
                        else {
                            std::cout<<"variable location does not exist, cannot pass by reference to function.\n";
                            return Value::Error(); 
                        }


                        args.emplace_back(ARG);
                        continue;
                    }
                }
                ARG.value = Evaluate(call_expr->args[i].get());
                args.emplace_back(ARG);
            }


            Enter_scope();
            for( size_t i = 0; i < args.size(); i++){
                current_scope->variables.try_emplace(func->params[i].v, Var(args[i].Get(), VarModifier::none));
            }
            FlowStream f = Execute(func->body.get());

            Exit_scope();
            if(f.f == Flow::Return) return f.value;
            return Value::None();

        }
        else{
            std::cout<<"err, function doesn't exist.\n";
            return Value::Error();
        }
        
    }
    std::cout<<"err, unhandled expr in evaluation.\n";
    return Value::Error();
}

Value* Runtime_env::GetValueLocationPtr(Expr* expr){
    if(auto literal_expr = dynamic_cast<LiteralExpr*>(expr)){
        return nullptr;
    }
    else if(auto var_expr = dynamic_cast<VariableExpr*>(expr)){
        Var* variable = LookupVar(var_expr->name, current_scope.get());

        if(!variable){
            std::cout<<"error, variable for location ptr doen't exist.\n";
            return nullptr;
        }

        return &variable->value;
    }
    else if(auto bin_expr = dynamic_cast<BinaryExpr*>(expr)){
        return nullptr;
    }
    else if(auto group_expr = dynamic_cast<GroupingExpr*>(expr)){
        return nullptr;
    }
    else if(auto unary_expr = dynamic_cast<UnaryExpr*>(expr)){
        return nullptr;
    }
    else if(auto assignment_expr = dynamic_cast<AssignmentExpr*>(expr)){
        return nullptr;
    }
    else if(auto array_expr = dynamic_cast<ArrayExpr*>(expr)){
        std::vector<Value> elements;
        for(size_t i = 0; i < array_expr->elements.size(); i++){
            elements.emplace_back(Evaluate(array_expr->elements[i].get()));
        }

        return nullptr;
    }
    else if(auto index_expr = dynamic_cast<IndexAccessExpr*>(expr)){
        auto object = GetValueLocationPtr(index_expr->object.get());
        auto index = Evaluate(index_expr->index.get());

        if(index.type != ValueType::Number){
            std::cout<<"error, can not acces an array or string with a non number value.\n";
            return nullptr;
        }

        if(object->type != ValueType::Array){
            std::cout<<"error, object is not accessibe like an array.\n";
            return nullptr;
        }

        double index_under_zero = ((index.AsNumber() < 0)? -1:index.AsNumber());
        if(index_under_zero == -1){
            std::cout<<"error, can not acces an array with an index value les then zero.\n";
            return nullptr;
        }
        else if(index_under_zero >= object->AsArray().elements.size()){
            std::cout<<"error, can not acces an array with an index value that is bigger than the array.\n";
            return nullptr;
        }
        else if(std::floor(index_under_zero) != index_under_zero){
            std::cout<<"error, can not acces an array with an index value that is not an integer.\n";
            return nullptr;
        }
        

        size_t verified_index = index_under_zero;
        Value* element_val = &object->AsArray().elements[verified_index];
        return element_val;
    }
    else if(auto property_expr = dynamic_cast<PropertyAccessExpr*>(expr)){
        auto object = GetValueLocationPtr(property_expr->object.get());
        
        if(object->type != ValueType::Object){
            std::cout<<"error, can not acces a non object-type value.\n";
            return nullptr;
        }

        if(object->AsObject().elements.find(property_expr->accessifier.v) != object->AsObject().elements.end()){
            auto pair = object->AsObject().elements.find(property_expr->accessifier.v);
            return &pair->second;
        }
        else{
            std::cout<<"error, the accesssifier [" + property_expr->accessifier.v + "] for an object does not exist.\n";
            return nullptr;
        }
    }
    
    std::cout<<"err, unhandled expr in location access handling.\n";
    return nullptr;
}   

Runtime_env::FlowStream Runtime_env::Exe_Expr_stmt(ExpressionStmt* expr_stmt){
    //PrintValue(Evaluate((expr_stmt->expression.get())));
    Evaluate((expr_stmt->expression.get()));
    return {Flow::Normal, Value::None()};
};
Runtime_env::FlowStream Runtime_env::Exe_Vardeclaration_stmt(VarDeclarationStmt* vardeclaration_stmt){
    Value right_expr = Value(0.0, ValueType::none);
    //std::cout<<"enter vardec stmt\n";


    if(vardeclaration_stmt->initializer.get()){
        right_expr = Evaluate(vardeclaration_stmt->initializer.get());
    }
    if(right_expr.type == ValueType::Err){
        std::cout<<"err, the right value for assignment was of error type.\n";
        return {Flow::Normal, Value::None()};
    }
    if(current_scope->variables.find(vardeclaration_stmt->name.v) != current_scope->variables.end()){
        std::cout<<"err, variable already exists in scope, shadowing is not supported.\n";
        return {Flow::Normal, Value::None()};
    }

    Var variable = Var(right_expr, vardeclaration_stmt->moderator);
    current_scope->variables.emplace(vardeclaration_stmt->name.v, variable);
    return {Flow::Normal, Value::None()};
};
Runtime_env::FlowStream Runtime_env::Exe_Block_stmt(BlockStmt* block_stmt){
    //std::cout<<"enter block stmt\n";

    Enter_scope();

    for(size_t i = 0; i < block_stmt->statements.size(); i++){
        FlowStream f = Execute(block_stmt->statements[i].get());

        if(f.f != Flow::Normal){
            Exit_scope();
            return f;
        }

    }
    Exit_scope();
    return {Flow::Normal, Value::None()};
};
Runtime_env::FlowStream Runtime_env::Exe_If_stmt(IfStmt* if_stmt){
    //std::cout<<"enter if stmt\n";

    auto condition = Evaluate(if_stmt->condition.get());

    if(calc.CTo_bool(condition).AsBool()){
        return Execute(if_stmt->then_branch.get());
    }
    else{
        if(!dynamic_cast<EmptyStmt*>(if_stmt->else_branch.get())){
            return Execute(if_stmt->else_branch.get());
        }
    }

    return {Flow::Normal, Value::None()};
};
Runtime_env::FlowStream Runtime_env::Exe_For_stmt(ForStmt* for_stmt){
    //std::cout<<"start for loop\n";

    Enter_scope();

    Execute(for_stmt->initializer.get());

    while(calc.CTo_bool(Evaluate(for_stmt->condition.get())).AsBool()){
        FlowStream f = Execute(for_stmt->body.get());
        if(f.f == Flow::Return){
            Exit_scope();
            return f;
        }
        Evaluate(for_stmt->iteration_update.get());
    }
    Exit_scope();

    return {Flow::Normal, Value::None()};
    //std::cout<<"end for loop\n";
}

Runtime_env::FlowStream Runtime_env::Exe_While_stmt(WhileStmt* while_stmt){
    //std::cout<<"start while\n";
    Enter_scope();
    while(calc.CTo_bool(Evaluate(while_stmt->condition.get())).AsBool()){
        FlowStream f = Execute(while_stmt->body.get());
        if(f.f == Flow::Break) break;
        else if(f.f == Flow::Continue) continue;
        else if(f.f == Flow::Return) {
            Exit_scope();
            return f;
        }
    }
    Exit_scope();
    //std::cout<<"done loop\n";
    return {Flow::Normal, Value::None()};
};
Runtime_env::FlowStream Runtime_env::Exe_Continue_stmt(Continuestmt* continue_stmt){
    //std::cout<<"enter continue stmt\n";
    return {Flow::Continue, Value::None()};
}
Runtime_env::FlowStream Runtime_env::Exe_Break_stmt(BreakStmt* break_stmt){
    //std::cout<<"enter break stmt\n";

    return {Flow::Break, Value::None()};
}
Runtime_env::FlowStream Runtime_env::Exe_Func_stmt(FuncStmt* func_stmt){
    FunctionValue function(func_stmt->name);
    for(auto &arg : func_stmt->params){
        function.params.emplace_back(arg);
    }
    function.body = std::move(func_stmt->body);

    current_scope->functions.try_emplace(func_stmt->name.v, std::move(function));

    // std::cout<<"scope after insert-- "<<current_scope<<"\n";

    // for(auto &param : current_scope->functions) std::cout<<"list after insert-- "<<param.first<<"\n";
    // std::cout<<"ident after insert-- "<<func_stmt->name.v<<"\n";

    
    return {Flow::Normal, Value::None()};
};
Runtime_env::FlowStream Runtime_env::Exe_Return_stmt(ReturnStmt* return_stmt){
    if(!dynamic_cast<EmptyExpr*>(return_stmt->value.get())) return {Flow::Return, Evaluate(return_stmt->value.get())};
    return {Flow::Return, Value::None()};
};



Runtime_env::FlowStream Runtime_env::Execute(Stmt* statement){
    if(auto expr_stmt = dynamic_cast<ExpressionStmt*>(statement)) return Exe_Expr_stmt(expr_stmt);
    else if(auto vardec_stmt = dynamic_cast<VarDeclarationStmt*>(statement)) return Exe_Vardeclaration_stmt(vardec_stmt);
    else if(auto block_stmt = dynamic_cast<BlockStmt*>(statement)) return Exe_Block_stmt(block_stmt);
    else if(auto if_stmt = dynamic_cast<IfStmt*>(statement)) return Exe_If_stmt(if_stmt);
    else if(auto while_stmt = dynamic_cast<WhileStmt*>(statement)) return Exe_While_stmt(while_stmt);
    else if(auto for_stmt = dynamic_cast<ForStmt*>(statement)) return Exe_For_stmt(for_stmt);
    else if(auto func_stmt = dynamic_cast<FuncStmt*>(statement)) return Exe_Func_stmt(func_stmt);
    else if(auto return_stmt = dynamic_cast<ReturnStmt*>(statement)) return Exe_Return_stmt(return_stmt);
    else if(auto break_stmt = dynamic_cast<BreakStmt*>(statement)) return Exe_Break_stmt(break_stmt);
    else if(auto continue_stmt = dynamic_cast<Continuestmt*>(statement)) return Exe_Continue_stmt(continue_stmt);

    return {Flow::Normal, Value::None()};
}
void Runtime_env::Run(){
    std::cout<<"\nstart runtime.\n\n";

    for(size_t i = 0; i < ast.body.size(); i++){
        Execute(ast.body[i].get());
    }
    std::cout<<"\nend runtime.\n";
}

