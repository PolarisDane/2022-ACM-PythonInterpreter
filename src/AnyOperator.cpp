#include "AnyOperator.h"
#include "Exception.h"
#include <cmath>
#include <cstdio>

std::ostream& operator<<(std::ostream& os, const antlrcpp::Any& a) {
  if (a.is<std::pair<std::string, antlrcpp::Any>>())
    os << a.as<std::pair<std::string, antlrcpp::Any>>().second;
  else if (a.is<int2048>()) os << a.as<int2048>();
  else if (a.is<double>()) printf("%.6lf", a.as<double>());//小数保留六位
  else if (a.is<std::string>()) os << a.as<std::string>();
  else if (a.is<bool>()) os << (a.as<bool>() ? "True" : "False");
  else if (a.isNull()) os << "None";
  return os;
}

void getVarVal(antlrcpp::Any& a) {
  if (a.is<std::pair<std::string, antlrcpp::Any>>())
    a = (a.as<std::pair<std::string, antlrcpp::Any>>()).second;
}

string getString(antlrcpp::Any const& a) {
  string str;
  if (a.is<int2048>()) {
    int2048 num = a.as<int2048>();
    if (num.Array.size() == 1 && num.Array[0] == 0) return "0";
    if (num.op_minus == -1) str = str + "-";
    for (int i = num.Array.size() - 1; i >= 0; i--) {
      if (i != num.Array.size() - 1) {
        for (int j = base_ / 10; j >= 10; j /= 10) {
          if (num.Array[i] >= j) break;
          str = str + "0";
        }
      }
      int tmp = num.Array[i], j = 0;
      while (tmp) {
        buffer_[++j] = tmp % 10 + '0';
        tmp /= 10;
      }
      for (; j; j--) str = str + buffer_[j];
    }
  }
  else {
    std::sprintf(buffer_, "%lf", a.as<double>());
    str = buffer_;
  }
  return std::move(str);
}

int2048 toInt(antlrcpp::Any const& a) {
  if (a.is<int2048>()) return a.as<int2048>();
  else if (a.is<double>()) return int2048((long long)(a.as<double>()));
  else if (a.is<string>()) return int2048(a.as<string>());
  else if (a.is<bool>()) return a.as<bool>() ? int2048(1) : int2048(0);
  else if (a.isNull()) return int2048(0);
}

double toDouble(antlrcpp::Any const& a) {
  if (a.is<int2048>()) {
    double tmp = 0;
    int2048 num = a.as<int2048>();
    for (int i = num.Array.size() - 1; i >= 0; i--) {
      tmp = tmp * base_ + num.Array[i];
    }
    tmp *= num.op_minus;
    return std::move(tmp);
  }
  else if (a.is<double>()) return a.as<double>();
  else if (a.is<string>()) return stod(a.as<string>());
  else if (a.is<bool>()) return a.as<bool>() ? 1.0 : 0.0;
  else if (a.isNull()) return 0.0;
}

std::string toString(antlrcpp::Any const& a) {
  if (a.is<int2048>()) return getString(a);
  else if (a.is<double>()) return getString(a);
  else if (a.is<string>()) return a.as<string>();
  else if (a.is<bool>()) return a.as<bool>() ? "True" : "False";
  else if (a.isNull()) return "None";
}

bool toBool(antlrcpp::Any const& a) {
  if (a.is<int2048>()) return a.as<int2048>() != int2048(0);
  else if (a.is<double>()) return a.as<double>();
  else if (a.is<string>()) return a.as<string>().length();
  else if (a.is<bool>()) return a.as<bool>();
  else if (a.isNull()) return false;
}

antlrcpp::Any operator-(antlrcpp::Any a) {
  getVarVal(a);
  if (a.is<int2048>()) return int2048(0) - a.as<int2048>();
  else return -a.as<double>();
}//取负数可以作为右值返回

antlrcpp::Any operator+(antlrcpp::Any const& a, antlrcpp::Any const& b) {
  if (a.is<string>() && b.is<string>()) return std::move(toString(a) + toString(b));
  if (a.is<double>() || b.is<double>()) return std::move(toDouble(a) + toDouble(b));
  return std::move(toInt(a) + toInt(b));
}

antlrcpp::Any operator-(antlrcpp::Any const& a, antlrcpp::Any const& b) {
  if (a.is<string>() || b.is<string>()) throw Exception("", UNDEFINED_BEHAVIOUR);
  if (a.is<double>() || b.is<double>()) return std::move(toDouble(a) - toDouble(b));
  return std::move(toInt(a) - toInt(b));
}

std::string operator*(std::string const& a, const int2048& b) {
  std::string str = "";
  int2048 tmp = b;
  while (tmp != int2048(0)) {
    str = str + a;
    tmp = tmp - int2048(1);
  }
  return std::move(str);
}

antlrcpp::Any operator*(antlrcpp::Any const& a, antlrcpp::Any const& b) {
  if (a.is<string>()) return std::move(a.as<string>() * b.as<int2048>());
  if (b.is<string>()) return std::move(b.as<string>() * a.as<int2048>());
  if (a.is<double>() || b.is<double>()) return std::move(toDouble(a) * toDouble(b));
  return std::move(toInt(a) * toInt(b));
}

antlrcpp::Any DivToInt(antlrcpp::Any const& a, antlrcpp::Any const& b) {
  //if (a.is<int2048>() && b.is<int2048>()) {
  //  return a.as<int2048>() / b.as<int2048>();
  //}
  //return int2048((long long)(floor(toDouble(a) / toDouble(b))));
  return std::move(toInt(a) / toInt(b));
}

antlrcpp::Any DivToDouble(antlrcpp::Any const& a, antlrcpp::Any const& b) {
  return std::move(toDouble(a) / toDouble(b));
}

antlrcpp::Any operator%(antlrcpp::Any const& a, antlrcpp::Any const& b) {
  return std::move(a - DivToInt(a, b) * b);
}

antlrcpp::Any operator<(antlrcpp::Any const& a, antlrcpp::Any const& b) {
  if (a.is<string>() && b.is<string>()) return a.as<string>() < b.as<string>();
  if (a.is<double>() || b.is<double>()) return toDouble(a) < toDouble(b);
  return toInt(a) < toInt(b);
}

antlrcpp::Any operator>(antlrcpp::Any const& a, antlrcpp::Any const& b) {
  return b < a;
}

antlrcpp::Any operator==(antlrcpp::Any const& a, antlrcpp::Any const& b) {
  if (a.isNull() || b.isNull()) {
    if (a.isNull() && b.isNull()) return true;
    else return false;
  }
  if (a.is<string>() && b.is<string>()) return a.as<string>() == b.as<string>();
  else if (a.is<string>() || b.is<string>()) return false;
  if (a.is<bool>() && b.is<bool>()) return toBool(a) == toBool(b);
  if (a.is<double>() || b.is<double>()) return toDouble(a) == toDouble(b);
  return toInt(a) == toInt(b);
}

antlrcpp::Any operator>=(antlrcpp::Any const& a, antlrcpp::Any const& b) {
  return a > b || a == b;
}

antlrcpp::Any operator<=(antlrcpp::Any const& a, antlrcpp::Any const& b) {
  return a < b || a == b;
}

antlrcpp::Any operator!=(antlrcpp::Any const& a, antlrcpp::Any const& b) {
  if (a.isNull() || b.isNull()) {
    if (a.isNull() && b.isNull()) return false;
    else return true;
  }
  else return !(a == b);
}

