#ifndef PYTHON_INTERPRETER_EXCEPTION_H
#define PYTHON_INTERPRETER_EXCEPTION_H

#include <string>

enum ExceptionType { UNDEFINED_VARIABLE, INVALID_VARNAME, INVALID_FUNC_CALL, INVALID_FUNCNAME, UNDEFINED_FUNCTION, UNDEFINED_BEHAVIOUR };

class Exception {

private:
  std::string message;

public:
  Exception(const std::string& arg, ExceptionType type) {
    if (type == UNDEFINED_BEHAVIOUR) message = "Undefined behaviour detected";
    else if (type == UNDEFINED_VARIABLE) message = "Undefined variable: " + arg;
    else if (type == UNDEFINED_FUNCTION) message = "Undefined function: " + arg;
    else if (type == INVALID_FUNC_CALL) message = "Invalid function call: " + arg;
    else if (type == INVALID_VARNAME) message = "Invalid variable name" + arg;
    else if (type == INVALID_FUNCNAME) message = "Invalid function name" + arg;
  }

  std::string what() { return message; }

};

#endif //PYTHON_INTERPRETER_EXCEPTION_H