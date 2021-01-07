/*
 * File: Basic.cpp
 * ---------------
 * Name: [TODO: enter name here]lht
 * Section: [TODO: enter section leader here]zyl
 * This file is the starter project for the BASIC interpreter from
 * Assignment #6.
 * [TODO: extend and correct the documentation]
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.h"
#include "parser.h"
#include "program.h"
#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"

#include "../StanfordCPPLib/simpio.h"
#include "../StanfordCPPLib/strlib.h"
using namespace std;

/* Function prototypes */

void processLine(string line, Program & program, EvalState & state);

/* Main program */

int main() {
   EvalState state;
   Program program;
    //cout << "Stub implementation of BASIC" << endl;
   while (true) {
      try {
         processLine(getLine(), program, state);
      } catch (ErrorException & ex) {
         cerr << "Error: " << ex.getMessage() << endl;
      }
   }
   return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version,
 * the implementation does exactly what the interpreter program
 * does in Chapter 19: read a line, parse it as an expression,
 * and then print the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(string line, Program & program, EvalState & state) {
   TokenScanner scanner;
   string token;
   TokenType token_type;
   scanner.ignoreWhitespace();
   scanner.scanNumbers();
   scanner.setInput(line);
   if(scanner.hasMoreTokens()) token = scanner.nextToken();
   else return;
   token_type = scanner.getTokenType(token);
   if (token_type == NUMBER){
       int line_number;
       try{
           line_number = stringToInteger(token);
       } catch (...) {
           cout << "SYNTAX ERROR" <<endl;
           return;
       }
       if(!scanner.hasMoreTokens()){
           program.removeSourceLine(line_number);
           return;
       }
       try {
           Statement *statement = parsestatement(scanner,line);
           //scanner = scanner.nextToken();1bug
           scanner.nextToken();
           program.addSourceLine(line_number,line);
           program.setParsedStatement(line_number,statement);
           return;//wangjileQWQ
       } catch (...) {
           cout << "SYNTAX ERROR" << endl;
           return;
       }
   }
   else if (token_type == WORD){
       if(check(token)){
           if( token == "PRINT" || token == "INPUT" || token == "LET"){
                scanner.setInput(line);
                Statement *statement = nullptr;
                if(!scanner.hasMoreTokens()){
                    cout << "SYNTAX ERROR" << endl;
                    return;
                }else {
                    try {
                        statement = parsestatement(scanner, line);
                    } catch (...) {
                        cout << "SYNTAX ERROR" << endl;
                        return;
                    }
                    try {
                        statement->execute(state);
                    } catch (ErrorException &Error) {
                        if(Error.getMessage() == "zero"){
                            cout << "DIVIDE BY ZERO" << endl;
                            delete statement;
                            return;
                        }else{
                            cout << "VARIABLE NOT DEFINED" << endl;
                            delete statement;
                            return;
                        }
                    }
                    delete statement;
                }
           }
           else if (token == "RUN") {
               if (scanner.hasMoreTokens()) {
                   error("SYNTAX ERROR");
               } else {
                   try {
                       program.runprogram(state);
                   } catch (ErrorException &a) {
                       if (a.getMessage() == "end")return;
                       if (a.getMessage() == "zero") {
                           cout << "DIVIDE BY ZERO" << endl;
                           return;
                       }
                       if (a.getMessage() == "goto") {
                           cout << "LINE NUMBER ERROR" << endl;
                       } else {
                           cout << "VARIABLE NOT DEFINED" << endl;
                           return;
                       }
                   }
               }
           }
           else if(token == "QUIT"){
               exit(0);
           }
           else if(token == "HELP"){
               cout << "WE DONT KNOW EITHER" << endl;
           }
           else if(token == "LIST"){
               if(scanner.hasMoreTokens()){
                   cout << "SYNTAX ERROR" << endl;
                   return;
               }
               program.showlist();
           }
           else if(token == "CLEAR"){
                if (scanner.hasMoreTokens()){
                    cout << "SYNTAX ERROR" << endl;
                    return;
                }
                program.clear();
                state.clear();
           }
           else if (token == "RUN"){
                if (scanner.hasMoreTokens()){
                    cout << "SYNTAX ERROR" << endl;
                    return;
                }
               try {
                   program.runprogram(state);
               } catch (ErrorException &Error) {
                    if (Error.getMessage() == "end"){
                        return;
                    }
                    if (Error.getMessage() == "zero"){
                        cout << "DIVIDE BY ZERO" << endl;
                        return;
                    }
                    if (Error.getMessage() == "goto"){
                        cout << "LINE NUMBER ERROR" << endl;
                        return;
                    }
                    cout << "VARIABLE NOT DEFINED" << endl;
                   return;
               }
           }
       }else{
           cout << "SYNTAX ERROR" << endl;
           return;
       }
   }
}
