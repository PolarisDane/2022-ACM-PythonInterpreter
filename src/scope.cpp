#include <unordered_map>
#include <string>
#include <vector>
#include "scope.h"
#include "support/Any.h"

Scope::Scope() : varTable() {
  std::unordered_map<std::string, antlrcpp::Any> tmp;
  varTable.push_back(tmp);//����ȫ�ֱ��������ռ�
}

void Scope::forcevarRegister(const std::string& varName, antlrcpp::Any varData) {
  (varTable.back())[varName] = varData;
}//ǿ��ע�ᣡ����(�����)

void Scope::varRegister(const std::string& varName, antlrcpp::Any varData) {
  if (varTable.back().find(varName) != varTable.back().end())
    (varTable.back())[varName] = varData;//�����޸ľֲ�����(�����ڱ�)
  else if (varTable.front().find(varName) != varTable.front().end())
    (varTable.front())[varName] = varData;//�޸�ȫ�ֱ���
  else (varTable.back())[varName] = varData;//ע���±���
}//����ע��

void Scope::NewScope() {
  std::unordered_map<std::string, antlrcpp::Any> tmp;
  varTable.push_back(tmp);//���µ������ռ�
}

void Scope::DeleteScope() {
  varTable.pop_back();//ɾ�������ռ�
}

std::pair<bool, antlrcpp::Any> Scope::varQuery(const std::string& varName) const {
  auto it = varTable.back().find(varName);
  if (it == varTable.back().end()) {
    it = varTable.front().find(varName);//�ȷ��ʾֲ������ٷ���ȫ�ֱ���
    //�ں����ڿ���ֱ�ӷ���global
    if (it == varTable.front().end()) return std::make_pair(false, 0);
  }
  return std::make_pair(true, it->second);
}


