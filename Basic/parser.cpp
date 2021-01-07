/*
 * File: parser.cpp
 * ----------------
 * Implements the parser.h interface.
 */

#include <iostream>
#include <string>

#include "exp.h"
#include "parser.h"

#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/strlib.h"
#include "../StanfordCPPLib/tokenscanner.h"
using namespace std;

/*
 * Implementation notes: parseExp
 * ------------------------------
 * This code just reads an expression and then checks for extra tokens.
 */

Expression *parseExp(TokenScanner & scanner) {
   Expression *exp = readE(scanner);
   if (scanner.hasMoreTokens()) {
      error("parseExp: Found extra token: " + scanner.nextToken());
   }
   return exp;
}

/*
 * Implementation notes: readE
 * Usage: exp = readE(scanner, prec);
 * ----------------------------------
 * This version of readE uses precedence to resolve the ambiguity in
 * the grammar.  At each recursive level, the parser reads operators and
 * subexpressions until it finds an operator whose precedence is greater
 * than the prevailing one.  When a higher-precedence operator is found,
 * readE calls itself recursively to read in that subexpression as a unit.
 */

Expression *readE(TokenScanner & scanner, int prec) {
   Expression *exp = readT(scanner);
   string token;
   while (true) {
      token = scanner.nextToken();
      int newPrec = precedence(token);
      if (newPrec <= prec) break;
      Expression *rhs = readE(scanner, newPrec);
      exp = new CompoundExp(token, exp, rhs);
   }
   scanner.saveToken(token);
   return exp;
}

/*
 * Implementation notes: readT
 * ---------------------------
 * This function scans a term, which is either an integer, an identifier,
 * or a parenthesized subexpression.
 */

Expression *readT(TokenScanner & scanner) {
   string token = scanner.nextToken();
   TokenType type = scanner.getTokenType(token);
   if (type == WORD) return new IdentifierExp(token);
   if (type == NUMBER) return new ConstantExp(stringToInteger(token));
   if (token != "(") error("Illegal term in expression");
   Expression *exp = readE(scanner);
   if (scanner.nextToken() != ")") {
      error("Unbalanced parentheses in expression");
   }
   return exp;
}

/*
 * Implementation notes: precedence
 * --------------------------------
 * This function checks the token against each of the defined operators
 * and returns the appropriate precedence value.
 */

int precedence(string token) {
   if (token == "=") return 1;
   if (token == "+" || token == "-") return 2;
   if (token == "*" || token == "/") return 3;
   return 0;
}

bool check(const string & token){
    if(token == "LET" || token == "REM" || token == "PRINT" || token == "END" || token == "IF" ||
       token == "THEN" || token == "GOTO" || token == "RUN" || token == "LIST" || token == "CLEAR" ||
       token == "QUIT" || token == "HELP" || token == "INPUT")return  true;
    else return false;
}

Statement *parsestatement(TokenScanner &scanner, string line){
    string token;
    TokenType token_type;
    token = scanner.nextToken();
    token_type = scanner.getTokenType(token);
    Expression *exp = nullptr;
    if(token_type != WORD){
        error("SYNTAX ERROR");
    } else{
     if (token == "REM"){
         if (!scanner.hasMoreTokens())error("SYNTAX ERROR");
         else return new REM();
     }
     else if(token == "END"){
         if(scanner.hasMoreTokens())error("SYNTAX ERROR");
         else return new END();
     }
     else if(token == "PRINT"){
         if(!scanner.hasMoreTokens()) error("SYNTAX ERROR");
         else{
             Expression *exp = parseExp(scanner);
             if (exp->getType() == COMPOUND){
                 if (((CompoundExp *) exp)->getOp() == "="){
                     error("SYNTAX ERROR");
                     delete exp;
                 }
             }else if(scanner.hasMoreTokens()){
                 error("SYNTAX ERROR");
                 delete exp;
             }
             return new PRINT(exp);
         }
     }
     else if(token == "GOTO"){
         if(!scanner.hasMoreTokens())error("SYNTAX ERROR");
         token = scanner.nextToken();
         token_type = scanner.getTokenType(token);
         if (scanner.hasMoreTokens())error("SYNTAX ERROR");
         else {
             if (token_type != NUMBER) error("SYNTAX ERROR");
             int linenumber = stringToInteger(token);
             return new GOTO(linenumber);
         }
     }
     else if (token == "LET"){
         if (!scanner.hasMoreTokens())error("SYNTAX ERROR");
         Expression *exp = parseExp(scanner);
         if (exp->getType() != COMPOUND){
             error("SYNTAX ERROR");
             delete exp;
         }
         if (((Expression *) (((CompoundExp *) exp)->getLHS()))->getType() != IDENTIFIER) {
             error("SYNTAX ERROR");
             delete exp;
         }
         if(check(((IdentifierExp *)(((CompoundExp *)exp)->getLHS()))->getName())){
             error("SYNTAX ERROR");
             delete exp;
         }
         return new LET(exp);
     }
     else if(token == "INPUT"){
         if(!scanner.hasMoreTokens())error("SYNTAX ERROR");
         else{
             token = scanner.nextToken();
             token_type = scanner.getTokenType(token);
             if (check(token))error("SYNTAX ERROR");
             if (token_type != WORD)error("SYNTAX ERROR");
             if (scanner.hasMoreTokens())error("SYNTAX ERROR");
             return new INPUT(token);
         }
     }
     else if(token == "IF"){
         if (!scanner.hasMoreTokens()){error("SYNTAX ERROR");}
         Expression *exp1 , *exp2;
         exp1 = readE(scanner);
         token = scanner.nextToken();
         GOTO *go_to;//此处等号=要用单引号
         if(line.find('=') == string::npos) {//表示没找到等号，等价于end（）
             string tmp = token;//token 永远截出来字符串
             if (token != "<" && token != ">" && token != "=") {
                 error("SYNTAX ERROR");
                 delete exp1;
             }
             exp2 = readE(scanner);
             token = scanner.nextToken();
             if (token != "THEN"){
                 error("SYNTAX ERROR");
                 delete exp1;
                 delete exp2;
             }
             if (!scanner.hasMoreTokens()){
                 error("SYNTAX ERROR");
                 delete exp1;
                 delete exp2;
             }
             token = scanner.nextToken();
             token_type = scanner.getTokenType(token);
             if (token_type != NUMBER){
                 error("SYNTAX ERROR");
                 delete exp1;
                 delete exp2;
             }
             go_to = new GOTO (stringToInteger(token));
             return new IF(tmp,exp1,exp2,go_to);
         }
         else{
             string tmp = "=";
             string newa;
             scanner.setInput(line);
             if (line.find_first_of('=') != line.find_last_of('='))error("SYNTAX ERROR");
             token = scanner.nextToken();
             token = scanner.nextToken();
             while (scanner.hasMoreTokens()) {
                 token = scanner.nextToken();
                 if (token == "=")break;
                 newa += (token + " ");//basic_string被重载了
             }
             if (!scanner.hasMoreTokens())error("SYNTAX ERROR");
             exp2 = readE(scanner);
             if (!scanner.hasMoreTokens()){error("SYNTAX ERROR");delete exp2;}
             token = scanner.nextToken();
             if (token != "THEN") {
                 error("SYNTAX ERROR");
             }
             if (!scanner.hasMoreTokens()){error("SYNTAX ERROR");delete exp2;}
             token = scanner.nextToken();
             token_type = scanner.getTokenType(token);
             if (token_type != NUMBER) {
                 error("SYNTAX ERROR");
             }
             go_to = new GOTO(stringToInteger(token));
             scanner.setInput(newa);
             exp1 = readE(scanner);
             return new IF(tmp , exp1 , exp2 , go_to);
         }
     } else{
         error("SYNTAX ERROR");
     }
    }
    return nullptr;
}
