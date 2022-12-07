#ifndef PYTHON_INTERPRETER_ANY_OPERATOR
#define PYTHON_INTERPRETER_ANY_OPERATOR

#include "support/Any.h"
#include <iostream>
#include <string>
#include "int2048.h"
using std::string;
using sjtu::int2048;

static char buffer_[1010];

std::ostream& operator<<(std::ostream& os, const antlrcpp::Any& a);

void getVarVal(antlrcpp::Any& a);

string getString(antlrcpp::Any const& a);
int2048 toInt(antlrcpp::Any const& a);
double toDouble(antlrcpp::Any const& a);
std::string toString (antlrcpp::Any const& a);
bool toBool(antlrcpp::Any const& a);

antlrcpp::Any operator-(antlrcpp::Any a);

antlrcpp::Any operator+(antlrcpp::Any const& a, antlrcpp::Any const& b);
antlrcpp::Any operator-(antlrcpp::Any const& a, antlrcpp::Any const& b);
antlrcpp::Any operator*(antlrcpp::Any const& a, antlrcpp::Any const& b);
antlrcpp::Any DivToInt(antlrcpp::Any const& a, antlrcpp::Any const& b);
antlrcpp::Any DivToDouble(antlrcpp::Any const& a, antlrcpp::Any const& b);
antlrcpp::Any operator%(antlrcpp::Any const& a, antlrcpp::Any const& b);

antlrcpp::Any operator<(antlrcpp::Any const& a, antlrcpp::Any const& b);
antlrcpp::Any operator>(antlrcpp::Any const& a, antlrcpp::Any const& b);
antlrcpp::Any operator==(antlrcpp::Any const& a, antlrcpp::Any const& b);
antlrcpp::Any operator>=(antlrcpp::Any const& a, antlrcpp::Any const& b);
antlrcpp::Any operator<=(antlrcpp::Any const& a, antlrcpp::Any const& b);
antlrcpp::Any operator!=(antlrcpp::Any const& a, antlrcpp::Any const& b);

#endif //PYTHON_INTERPRETER_ANY_OPERATOR