/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include <string>
#include "program.h"
#include "statement.h"
#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"
using namespace std;

Program::Program()= default;
   // Replace this stub with your own code


Program::~Program() {
   // Replace this stub with your own code
   this->clear();
}

void Program::clear() {
   // Replace this stub with your own code
    for(auto & pair : MYmap){
        delete pair.second.statement;
        pair.second.statement = nullptr;
    }
    MYmap.clear();
}

void Program::addSourceLine(int lineNumber, string line) {
   // Replace this stub with your own code
    if(!this->my_find(lineNumber)){
        MYmap.insert(make_pair(lineNumber,sourceline(line)));
    }else{
        MYmap[lineNumber].line = line;
        if (MYmap[lineNumber].statement != nullptr) MYmap[lineNumber].statement = nullptr;
    }
}

void Program::removeSourceLine(int lineNumber) {
   // Replace this stub with your own code
    //如果找不到直接返回？
    if(!this->my_find(lineNumber))return;
    else{
        delete MYmap[lineNumber].statement;
        MYmap.erase(lineNumber);
    }
}

string Program::getSourceLine(int lineNumber) {
    // Replace this stub with your own code
    if(!this->my_find(lineNumber))return "";//返回空字符串
    else return MYmap[lineNumber].line;
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
   // Replace this stub with your own code
    if(!this->my_find(lineNumber))error("SYNTAX ERROR");
    else{
        delete MYmap[lineNumber].statement;
        MYmap[lineNumber].statement = stmt;
    }
}

Statement *Program::getParsedStatement(int lineNumber) {
    // Replace this stub with your own code
    if(!this->my_find(lineNumber))error("SYNTAX ERROR IN getparsedstatement");
    if(MYmap[lineNumber].statement == nullptr)return nullptr;
    else return MYmap[lineNumber].statement;
}

int Program::getFirstLineNumber() {
        // Replace this stub with your own code
    if (MYmap.empty())return -1;
    else return MYmap.begin()->first;
}

int Program::getNextLineNumber(int lineNumber) {
    // Replace this stub with your own code
    auto it = this->MYmap.upper_bound(lineNumber);
    if(it == this->MYmap.end()) return -1;
    else return it->first;
}

bool Program::my_find(int lineNumber) {
    for(auto & pair : MYmap){
        if(pair.first == lineNumber)return true;
    }
    return false;
}

void Program::showlist() {
    if (this->MYmap.empty()){
        return;
    } else{
        auto pair = this->MYmap.begin();
        while (pair != MYmap.end()){
            cout << pair->second.line << endl;
            ++pair;
        }
        return;
    }
}

void Program::runprogram(EvalState &state) {
    if(this->MYmap.empty()){
        return;
    }
    auto pair = this->MYmap.begin();
    while (pair != this->MYmap.end()){
        try {
            pair->second.statement->execute(state);
            ++pair;
        } catch (ErrorException & Error) {
            TokenScanner scanner;
            if(scanner.getTokenType(Error.getMessage()) == NUMBER){
                int tolinenumber = stringToInteger(Error.getMessage());
                if(this->my_find(tolinenumber)){
                    pair = this->MYmap.find(tolinenumber);
                    continue;
                }else{
                    error("goto");
                }
            } else {
                error(Error.getMessage());
            }
        }
    }
}