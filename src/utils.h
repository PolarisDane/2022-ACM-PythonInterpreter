#ifndef PYTHON_INTERPRETER_UTILS_H
#define PYTHON_INTERPRETER_UTILS_H

#include <string>

const std::string KeyName[]{
  "None","True","False","def","return","break","continue",
  "if","elif","else","while","or","and","not","printf"
};

bool validateVarName(const std::string& str);

#endif //PYTHON_INTERPRETER_UTILS_H