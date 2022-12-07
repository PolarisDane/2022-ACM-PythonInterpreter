#include "Evalvisitor.h"

//返回一个{}作为Any类的时候会构造一个空的Any类
antlrcpp::Any EvalVisitor::visitFile_input(Python3Parser::File_inputContext* ctx) {
  return visitChildren(ctx);
}//checked

antlrcpp::Any EvalVisitor::visitFuncdef(Python3Parser::FuncdefContext* ctx) {
  std::string functionName = ctx->NAME()->getText();
  auto parameters = visitParameters(ctx->parameters()).as<std::vector<std::pair<std::string, antlrcpp::Any>>>();
  //注意取出来要转换,否则的parameters还是Any类
  auto suite = ctx->suite();
  if (!validateVarName(functionName)) {
    throw Exception("", INVALID_FUNCNAME);
  }//函数名字不合法报错
  scope.varRegister(functionName, std::make_pair(parameters, suite));//将函数的参数表和具体操作(对应节点)一同存入命名空间中
  //注意取出来的时候的操作(as的类型)
  return {};
}//checked

antlrcpp::Any EvalVisitor::visitParameters(Python3Parser::ParametersContext* ctx) {
  std::vector<std::pair<std::string, antlrcpp::Any>> ret;
  auto typedargslist = ctx->typedargslist();
  if (!typedargslist) return ret;//无参数,返回一个空的vector
  auto tfpdefArray = typedargslist->tfpdef();
  auto testArray = typedargslist->test();
  int det = tfpdefArray.size() - testArray.size();//无初始值的参数个数
  for (int i = 0; i < tfpdefArray.size(); i++) {
    std::string tfpdefName = tfpdefArray[i]->NAME()->getText();
    antlrcpp::Any nullAny = {};
    if (i >= det) {
      ret.push_back(std::make_pair(tfpdefName, visitTest(testArray[i - det])));
      //有初值的参数只能出现在最后面
    }
    else ret.push_back(std::make_pair(tfpdefName, nullAny));
  }
  return std::move(ret);
}//checked

antlrcpp::Any EvalVisitor::visitStmt(Python3Parser::StmtContext* ctx) {
  auto simple_stmt = ctx->simple_stmt();
  if (simple_stmt) return visitSimple_stmt(simple_stmt);
  return visitCompound_stmt(ctx->compound_stmt());
}//checked

antlrcpp::Any EvalVisitor::visitSimple_stmt(Python3Parser::Simple_stmtContext* ctx) {
  return visitSmall_stmt(ctx->small_stmt());
}//checked

antlrcpp::Any EvalVisitor::visitSmall_stmt(Python3Parser::Small_stmtContext* ctx) {
  auto expr_stmt = ctx->expr_stmt();
  if (expr_stmt) return visitExpr_stmt(expr_stmt);
  else return visitFlow_stmt(ctx->flow_stmt());
  return {};
}//checked

antlrcpp::Any EvalVisitor::visitExpr_stmt(Python3Parser::Expr_stmtContext* ctx) {
  auto testlistArray = ctx->testlist();
  if (ctx->augassign()) {
    std::string varName = testlistArray[0]->getText();
    if (!validateVarName(varName)) {
      throw Exception("", INVALID_VARNAME);
    }
    auto varData = scope.varQuery(varName);
    if (!varData.first) {
      throw Exception("", UNDEFINED_VARIABLE);
    }
    auto tmpVal = visitTestlist(testlistArray[1]);
    getVarVal(tmpVal);//从变量中把值取出
    std::string augassignArray = ctx->augassign()->getText();
    if (augassignArray == "+=") scope.varRegister(varName, varData.second + tmpVal);
    else if (augassignArray == "-=") scope.varRegister(varName, varData.second - tmpVal);
    else if (augassignArray == "*=") scope.varRegister(varName, varData.second * tmpVal);
    else if (augassignArray == "/=") scope.varRegister(varName, DivToDouble(varData.second, tmpVal));
    else if (augassignArray == "//=") scope.varRegister(varName, DivToInt(varData.second, tmpVal));
    else if (augassignArray == "%=") scope.varRegister(varName, varData.second % tmpVal);
    return {};
  }//加等等操作
  if (testlistArray.size() == 1) {
    return visitTestlist(testlistArray[0]);
  }//可能为单一的一个testlist(tuple之类的)
  std::vector<antlrcpp::Any> testArray_;
  auto tmp = visitTestlist(testlistArray.back());//右值来源
  if (!tmp.is<std::vector<antlrcpp::Any>>()) testArray_.push_back(tmp);
  //如果为单一一个值，那么就构造一个vector方便统一
  else testArray_ = tmp.as<std::vector<antlrcpp::Any>>();
  std::vector<antlrcpp::Any> varData;//右值来源
  for (int i = 0; i < testArray_.size(); i++) {
    getVarVal(testArray_[i]);//先取出值
    if (testArray_[i].is<std::vector<antlrcpp::Any>>()) {
      auto tmp = testArray_[i].as<std::vector<antlrcpp::Any>>();
      for (int j = 0; j < tmp.size(); j++) {
        getVarVal(tmp[j]);
        varData.push_back(tmp[j]);
      }
    }//如果内部还是tuple则再次分开成为单个值(函数返回多元组赋值)
    else varData.push_back(testArray_[i]);
  }
  for (int i = 0; i < testlistArray.size() - 1; i++) {
    auto testArray = testlistArray[i]->test();
    for (int j = 0; j < varData.size(); j++) {
      std::string varName = testArray[j]->getText();
      if (!validateVarName(varName)) {
        throw Exception("", INVALID_VARNAME);
      }
      scope.varRegister(varName, varData[j]);//对应赋值
    }
  }//带逗号的赋值和连等赋值合并
  return {};
}//checked

antlrcpp::Any EvalVisitor::visitFlow_stmt(Python3Parser::Flow_stmtContext* ctx) {
  if (ctx->break_stmt()) return 1;
  else if (ctx->continue_stmt()) return 2;
  else {
    auto testlist = ctx->return_stmt()->testlist();
    if (!testlist) {
      antlrcpp::Any noneAny = {};
      return std::make_pair(3, noneAny);//此处不能返回none也不能只返回表示return的3，否则会炸
    }
    antlrcpp::Any tmp = visitTestlist(testlist);
    getVarVal(tmp);
    return std::make_pair(3, tmp);
  }//return_stmt要把返回值取出来
  //返回值可能是多个元素
  return {};
}//checked

antlrcpp::Any EvalVisitor::visitCompound_stmt(Python3Parser::Compound_stmtContext* ctx) {
  auto if_stmt = ctx->if_stmt();
  auto while_stmt = ctx->while_stmt();
  if (if_stmt) return visitIf_stmt(if_stmt);
  else if (while_stmt) return visitWhile_stmt(while_stmt);
  else return visitFuncdef(ctx->funcdef());
  return {};
}//checked

antlrcpp::Any EvalVisitor::visitIf_stmt(Python3Parser::If_stmtContext* ctx) {
  auto testArray = ctx->test();
  auto suiteArray = ctx->suite();
  for (int i = 0; i < testArray.size(); i++) {
    auto tmp = visitTest(testArray[i]);
    getVarVal(tmp);
    if (toBool(tmp))
      return visitSuite(suiteArray[i]);
    //注意先访问取值后再转bool(因为存在变量直接作为条件)
  }//if and else if
  if (testArray.size() < suiteArray.size())
    return visitSuite(suiteArray[testArray.size()]);//else
  return {};
}//checked

antlrcpp::Any EvalVisitor::visitWhile_stmt(Python3Parser::While_stmtContext* ctx) {
  auto test = ctx->test();
  auto suite = ctx->suite();
  auto ret = visitTest(test);
  getVarVal(ret);//注意先访问取值后再转bool(因为存在变量直接作为条件)
  while (toBool(ret)) {
    auto tmp = visitSuite(suite);
    if (tmp.is<int>()) {
      int val = tmp.as<int>();
      if (val == 1) break;
      //if(val == 2);
    }//continue直接访问下一次条件即可，只有break才需要特殊处理
    if (tmp.is<std::pair<int, antlrcpp::Any>>())
      return tmp;//返回值
    ret = visitTest(test);
    getVarVal(ret);
  }
  return {};
}//checked

antlrcpp::Any EvalVisitor::visitSuite(Python3Parser::SuiteContext* ctx) {
  auto simple_stmt = ctx->simple_stmt();
  if (simple_stmt) return visitSimple_stmt(simple_stmt);
  else {
    auto stmtArray = ctx->stmt();
    for (int i = 0; i < stmtArray.size(); i++) {
      auto tmp = visitStmt(stmtArray[i]);
      if (tmp.is<int>()) return tmp;//返回到上一层进行操作
      if (tmp.is<std::pair<int, antlrcpp::Any>>()) 
        return tmp;
      //如果有返回值说明出现了控制流语句
      //返回值一定包含一个std::vector<antlrcpp::Any>(直接作为Any类返回)
    }
  }
  return{};
}//checked

antlrcpp::Any EvalVisitor::visitTest(Python3Parser::TestContext* ctx) {
  return visitOr_test(ctx->or_test());
}//checked

antlrcpp::Any EvalVisitor::visitOr_test(Python3Parser::Or_testContext* ctx) {
  auto and_testArray = ctx->and_test();
  if (and_testArray.size() == 1)
    return visitAnd_test(and_testArray[0]);//maybe value or tuple
  for (int i = 0; i < and_testArray.size(); i++) {
    auto tmp = visitAnd_test(and_testArray[i]);
    getVarVal(tmp);
    if (toBool(tmp)) return true;//短路运算
  }
  return false;
}//checked

antlrcpp::Any EvalVisitor::visitAnd_test(Python3Parser::And_testContext* ctx) {
  auto not_testArray = ctx->not_test();
  if (not_testArray.size() == 1)
    return visitNot_test(not_testArray[0]);//maybe value or tuple
  for (int i = 0; i < not_testArray.size(); i++) {
    auto tmp = visitNot_test(not_testArray[i]);
    getVarVal(tmp);
    if (!toBool(tmp)) return false;//短路运算
  }
  return true;
}//checked

antlrcpp::Any EvalVisitor::visitNot_test(Python3Parser::Not_testContext* ctx) {
  auto comparison = ctx->comparison();
  if (comparison) return visitComparison(comparison);
  else {
    auto tmp = visitNot_test(ctx->not_test());
    getVarVal(tmp);
    return !toBool(tmp);
  }
  return {};
}//checked

antlrcpp::Any EvalVisitor::visitComparison(Python3Parser::ComparisonContext* ctx) {
  auto arith_exprArray = ctx->arith_expr();
  if (arith_exprArray.size() == 1)
    return visitArith_expr(arith_exprArray[0]);//maybe value or tuple
  auto comp_opArray = ctx->comp_op();
  auto arith1 = visitArith_expr(arith_exprArray[0]);
  for (int i = 1; i < arith_exprArray.size(); i++) {
    auto ch = comp_opArray[i - 1]->getText();
    auto arith2 = visitArith_expr(arith_exprArray[i]);
    getVarVal(arith1);//从变量中把值取出
    getVarVal(arith2);//从变量中把值取出
    if (ch == "<") {
      if (!(arith1 < arith2)) return false;
    }
    else if (ch == ">") {
      if (!(arith1 > arith2)) return false;
    }
    else if (ch == "==") {
      if (!(arith1 == arith2)) return false;
    }
    else if (ch == ">=") {
      if (!(arith1 >= arith2)) return false;
    }
    else if (ch == "<=") {
      if (!(arith1 <= arith2)) return false;
    }
    else if (ch == "!=") {
      if (!(arith1 != arith2)) return false;
    }
    else throw Exception("", UNDEFINED_BEHAVIOUR);
    arith1 = arith2;//每个值只访问一次
  }
  return true;
}//短路运算
//checked

antlrcpp::Any EvalVisitor::visitArith_expr(Python3Parser::Arith_exprContext* ctx) {
  auto termArray = ctx->term();
  if (termArray.size() == 1)
    return visitTerm(termArray[0]);//maybe value or tuple
  auto opArray = ctx->addorsub_op();
  auto ret = visitTerm(termArray[0]);
  getVarVal(ret);//从变量中把值取出来
  for (int i = 1; i < termArray.size(); ++i) {
    std::string tmpOp = opArray[i - 1]->getText();
    auto tmp = visitTerm(termArray[i]);
    getVarVal(tmp);//从变量中把值取出来
    if (tmpOp == "+") ret = ret + tmp;
    else if (tmpOp == "-") ret = ret - tmp;
    else throw Exception("", UNDEFINED_BEHAVIOUR);
  }
  return std::move(ret);
}//checked

antlrcpp::Any EvalVisitor::visitTerm(Python3Parser::TermContext* ctx) {
  auto factorArray = ctx->factor();
  if (factorArray.size() == 1)
    return visitFactor(factorArray[0]);//maybe value or tuple
  auto opArray = ctx->muldivmod_op();
  auto ret = visitFactor(factorArray[0]);
  getVarVal(ret);//从变量中把值取出来
  for (int i = 1; i < factorArray.size(); ++i) {
    std::string tmpOp = opArray[i - 1]->getText();
    auto tmp = visitFactor(factorArray[i]);
    getVarVal(tmp);//从变量中把值取出来
    if (tmpOp == "*") ret = ret * tmp;
    else if (tmpOp == "/") ret = DivToDouble(ret, tmp);
    else if (tmpOp == "//") ret = DivToInt(ret, tmp);
    else if (tmpOp == "%") ret = ret % tmp;
    else throw("", UNDEFINED_BEHAVIOUR);
  }
  return std::move(ret);
}//checked

antlrcpp::Any EvalVisitor::visitFactor(Python3Parser::FactorContext* ctx) {
  antlrcpp::Any tmp = visitChildren(ctx);
  if (ctx->MINUS()) tmp = -tmp;//加上负号
  return std::move(tmp);
}//checked
//fuck fuck fuck fuck fuck!!!!!!!!!!!!!
//注意看.g4里面，+-和factor没有括号,所以atom_expr可负

antlrcpp::Any EvalVisitor::visitAtom_expr(Python3Parser::Atom_exprContext* ctx) {
  auto atom = ctx->atom();
  if (!ctx->trailer()) return visitAtom(atom);
  //以下的部分为函数相关,因为atom后面带有括号
  auto functionName = atom->getText();
  auto argsArray = visitTrailer(ctx->trailer()).as<std::vector<antlrcpp::Any>>();//参数表
  //内置函数
  if (functionName == "print") {
    for (int i = 0; i < argsArray.size(); i++) {
      getVarVal(argsArray[i]);
      std::cout << argsArray[i] << " ";
    }
    puts("");
    return {};
  }
  else if (functionName == "int") {
    getVarVal(argsArray[0]);
    return toInt(argsArray[0]);
  }
  else if (functionName == "float") {
    getVarVal(argsArray[0]);
    return toDouble(argsArray[0]);
  }
  else if (functionName == "str") {
    getVarVal(argsArray[0]);
    return toString(argsArray[0]);
  }
  else if (functionName == "bool") {
    getVarVal(argsArray[0]);
    return toBool(argsArray[0]);
  }
  else {
    auto functionResult = scope.varQuery(functionName);
    if (!functionResult.first)
      throw Exception("", UNDEFINED_FUNCTION);
    auto function = functionResult.second;
    auto functionInfo = function.as<std::pair<std::vector<std::pair<std::string, antlrcpp::Any>>, Python3Parser::SuiteContext*>>();
    //注意取出来的时候的类型
    auto varArray = functionInfo.first;
    scope.NewScope();//创造新的命名空间
    for (int i = 0; i < varArray.size(); i++) {
      scope.forcevarRegister(varArray[i].first, varArray[i].second);
    }//参数注册并赋初值
    for (int i = 0; i < argsArray.size(); i++) {
      if (argsArray[i].is<std::pair<std::string, antlrcpp::Any>>()) {
        auto tmp = argsArray[i].as<std::pair<std::string, antlrcpp::Any>>();
        scope.forcevarRegister(tmp.first, tmp.second);
      }
      else scope.forcevarRegister(varArray[i].first, argsArray[i]);
    }//函数参数中可能出现Positional argument,其在Keyword argument之前
    auto tmp = visitSuite(functionInfo.second);
    if (tmp.is<std::pair<int, antlrcpp::Any>>()) {
      tmp = tmp.as<std::pair<int, antlrcpp::Any>>().second;
    }//将返回值取出(可能为tuple)
    scope.DeleteScope();//删除函数命名空间
    return std::move(tmp);//类型为std::vector<antlrcpp::Any>
  }//自定义函数
}//checked

antlrcpp::Any EvalVisitor::visitTrailer(Python3Parser::TrailerContext* ctx) {
  std::vector<antlrcpp::Any> tmp;
  auto arglist = ctx->arglist();
  if (arglist) return visitArglist(arglist);
  return tmp;//不能返回none
  //返回类型std::vector<antlrcpp::Any>
}//checked

antlrcpp::Any EvalVisitor::visitAtom(Python3Parser::AtomContext* ctx) {
  if (ctx->NUMBER()) {
    std::string str = ctx->NUMBER()->getText();
    if (str.find('.') != str.npos) return std::move(toDouble(str));//存在小数点
    else return std::move(toInt(str));//整数
  }
  else if (ctx->NAME()) {
    auto varName = ctx->NAME()->getText();
    auto result = scope.varQuery(varName);
    return std::move(std::make_pair(varName, result.second));
    //if (result.first) {
    //  return std::make_pair(varName, result.second);//方便函数参数表使用，把变量名一起传出去，通过getVarVal访问变量的值就可以了
    //}
    //else throw Exception(varName, UNDEFINED_VARIABLE);
    //此写法不可用，因为函数中参数keyword argument初始化的时候需要注册
    //如果本身外面没有这个就会出问题
  }
  else if (ctx->TRUE()) return true;
  else if (ctx->FALSE()) return false;
  else if (ctx->test()) return visitTest(ctx->test());//括号
  else if (ctx->NONE()) return {};
  else {
    auto stringArray = ctx->STRING();
    std::string str = "";
    for (int i = 0; i < stringArray.size(); i++) {
      std::string tmp = stringArray[i]->getText();
      tmp = tmp.substr(1, tmp.length() - 2);//去掉引号
      str = str + tmp;
    }//此处要允许多个""相连形成一个字符串
    return std::move(str);
  }
}//checked

antlrcpp::Any EvalVisitor::visitTestlist(Python3Parser::TestlistContext* ctx) {
  auto testArray = ctx->test();
  if (testArray.size() == 1) return visitTest(testArray[0]);
  std::vector<antlrcpp::Any> testlist;
  for (int i = 0; i < testArray.size(); i++) {
    testlist.push_back(visitTest(testArray[i]));
  }
  return std::move(testlist);
}//返回类型std::vector<antlrcpp::Any>
//checked

antlrcpp::Any EvalVisitor::visitArglist(Python3Parser::ArglistContext* ctx) {
  auto argumentArray = ctx->argument();
  std::vector<antlrcpp::Any> arglist;
  for (int i = 0; i < argumentArray.size(); i++) {
    antlrcpp::Any tmp = visitArgument(argumentArray[i]);
    if (tmp.is<std::vector<antlrcpp::Any>>()) {
      auto ret = tmp.as<std::vector<antlrcpp::Any>>();
      for (int j = 0; j < ret.size(); j++) {
        arglist.push_back(ret[j]);
      }
    }
    else arglist.push_back(tmp);
    //在此处将参数表拆分开来(可以支持函数返回tuple作为参数)
    //不能从变量中把值取出来
  }
  return std::move(arglist);
  //返回类型std::vector<antlrcpp::Any>
}//checked

antlrcpp::Any EvalVisitor::visitArgument(Python3Parser::ArgumentContext* ctx) {
  auto testArray = ctx->test();
  auto var = visitTest(testArray[0]);
  if (ctx->ASSIGN()) {
    std::string varName;
    varName = var.as<std::pair<std::string, antlrcpp::Any>>().first;//得到变量名
    auto varData = visitTest(testArray[1]);//访问等号后的值
    getVarVal(varData);
    return std::move(std::make_pair(varName, varData));
  }//参数有等号赋值
  else {
    getVarVal(var);//直接将变量转为数值传出
    return std::move(var);
  }
}//checked
