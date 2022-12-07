#include "utils.h"
#include <string>

bool validateVarName(const std::string& str) {
  for (const auto& ch : KeyName) {
    if (ch == str)
      return false;
  }
  if (isdigit(str[0]) || str[0] == '_' || str.length() > 64) return false;
  for (const auto& ch : str) {
    if (!(ch >= 'a' && ch <= 'z') &&
        !(ch >= 'A' && ch <= 'Z') &&
        !isdigit(ch) && ch != '_') {
      return false;
    }
  }
  return true;
}

