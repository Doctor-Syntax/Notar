#ifndef CALC_ERROR
#define CALC_ERROR

#include "program.hpp"

#include "terminal_functions.hpp"

class Msg{
private:
    Terminal_G *core = nullptr;

public:
    void Error(const Token &token, const std::string &msg, bool &terminate, std::string error_type = "RUNTIME");
    void Warning(const Token &token, const std::string &msg);

    void Error(const std::string &msg, bool &terminate, std::string error_type = "RUNTIME");
    void Warning(const std::string &msg);


    void Message(const std::string &msg, Color color = yellow);

    void Set_terminal_context(Terminal_G &terminal);
    Terminal_G *Get_terminal_context();
};





#endif // CALC_ERROR