#include <unordered_map>
#include <string>
#include <vector>
#include "scope.h"
#include "support/Any.h"

Scope::Scope() : varTable() {
  std::unordered_map<std::string, antlrcpp::Any> tmp;
  varTable.push_back(tmp);//创造全局变量命名空间
}

void Scope::forcevarRegister(const std::string& varName, antlrcpp::Any varData) {
  (varTable.back())[varName] = varData;
}//强行注册！！！(最后冲刺)

void Scope::varRegister(const std::string& varName, antlrcpp::Any varData) {
  if (varTable.back().find(varName) != varTable.back().end())
    (varTable.back())[varName] = varData;//优先修改局部变量(变量遮蔽)
  else if (varTable.front().find(varName) != varTable.front().end())
    (varTable.front())[varName] = varData;//修改全局变量
  else (varTable.back())[varName] = varData;//注册新变量
}//正常注册

void Scope::NewScope() {
  std::unordered_map<std::string, antlrcpp::Any> tmp;
  varTable.push_back(tmp);//开新的命名空间
}

void Scope::DeleteScope() {
  varTable.pop_back();//删除命名空间
}

std::pair<bool, antlrcpp::Any> Scope::varQuery(const std::string& varName) const {
  auto it = varTable.back().find(varName);
  if (it == varTable.back().end()) {
    it = varTable.front().find(varName);//先访问局部变量再访问全局变量
    //在函数内可以直接访问global
    if (it == varTable.front().end()) return std::make_pair(false, 0);
  }
  return std::make_pair(true, it->second);
}


