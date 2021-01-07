/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include <string>
#include "statement.h"
#include "../StanfordCPPLib/tokenscanner.h"
#include "../StanfordCPPLib/simpio.h"
using namespace std;

/* Implementation of the Statement class */

Statement::Statement() {
   /* Empty */
}

Statement::~Statement() {
   /* Empty */
}

REM::REM() = default;
REM::~REM() = default;
void REM::execute(EvalState &state){
}

LET::LET(Expression *exp):exp(exp){}
LET::~LET(){
    delete exp;
}
void LET::execute(EvalState &state) {
    exp->eval(state);
}

PRINT::PRINT(Expression *exp) :exp(exp){}
PRINT::~PRINT() {
    delete exp;
}
void PRINT::execute(EvalState &state) {
    cout << exp->eval(state) << endl;
}

INPUT::INPUT(string &x) {
    var_name = x;
}
INPUT::~INPUT() = default;
void INPUT::execute(EvalState &state) {
    cout << " ? ";
    string token;
    int var_value;
    TokenScanner scanner;
    TokenType token_type;
    scanner.ignoreWhitespace();
    scanner.ignoreComments();
    scanner.scanNumbers();
    while (true){
        scanner.setInput(getLine());
        if(!scanner.hasMoreTokens())continue;
        token = scanner.nextToken();
        token_type = scanner.getTokenType(token);
        if(token == "-"){
            if(!scanner.hasMoreTokens()){
                cout << "INVALID NUMBER" << endl << " ? ";continue;
            } else{
                token = scanner.nextToken();
                token_type = scanner.getTokenType(token);
                if(token_type != NUMBER || scanner.hasMoreTokens()){
                    cout << "INVALID NUMBER" << endl << " ? ";
                    continue;
                }
                try{
                    var_value = stringToInteger(token);
                } catch (...) {
                    cout << "INVALID NUMBER" << endl << " ? ";
                    continue;
                }
                state.setValue(var_name , -var_value);
                break;
            }
        }
        else{
            if(token_type != NUMBER || scanner.hasMoreTokens()){
                cout << "INVALID NUMBER" << endl << " ? ";
                continue;
            }
            try{
                var_value = stringToInteger(token);
            } catch (...) {
                cout << "INVALID NUMBER" << endl << " ? ";
                continue;
            }
            state.setValue(var_name,var_value);
            break;
        }
    }
}

END::END() = default;
END::~END() = default;
void END::execute(EvalState &state) {
    error("end");
}

GOTO::GOTO(int x) {
    tolinenumber = x;
}
GOTO::~GOTO() = default;
void GOTO::execute(EvalState &state) {
    error(integerToString(tolinenumber));
}

IF::IF(string &a, Expression *b, Expression *c, GOTO *d) {
    str = a;
    exp1 = b;
    exp2 = c;
    go_to = d;
}
IF::~IF(){
    delete exp1;
    delete exp2;
    delete go_to;
}
void IF::execute(EvalState &state) {
    bool flag;
    int first_var = exp1->eval(state);
    int second_var = exp2->eval(state);
    if(str == "=") flag = (first_var == second_var);
    if(str == "<") flag = (first_var < second_var);
    if(str == ">") flag = (first_var > second_var);
    if(!flag) return;
    else go_to->execute(state);
}