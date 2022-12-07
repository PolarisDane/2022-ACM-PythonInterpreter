#ifndef PYTHON_INTERPRETER_SCOPE_H
#define PYTHON_INTERPRETER_SCOPE_H

#include <unordered_map>
#include <string>
#include <vector>
#include "support/Any.h"

class Scope {

private:
  std::vector<std::unordered_map<std::string, antlrcpp::Any>> varTable;

public:
  Scope();

  void NewScope();

  void DeleteScope();

  void varRegister(const std::string& varName, antlrcpp::Any varData);

  void forcevarRegister(const std::string& varName, antlrcpp::Any varData);

  std::pair<bool, antlrcpp::Any> varQuery(const std::string& varName) const;

};

#endif //PYTHON_INTERPRETER_SCOPE_H