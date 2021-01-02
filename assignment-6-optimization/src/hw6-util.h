//
// Created by Saikat Chakraborty on 11/14/20.
//

#pragma once

#include "tee.h"
#include "osutils.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

#include <fstream>
#include <iostream>
#include <set>
#include <type_traits>
#include <vector>
#include <sstream>

using namespace std;
using namespace llvm;

// trim from start (in place)
static inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
    return !std::isspace(ch);
  }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

class OptimizationResultWriter {
private:
  std::fstream file;
  Tee<std::fstream &, std::ostream &> tee;
  string moduleName;

  std::string instructionToString(Instruction *inst) {
    std::string s = "";
    raw_string_ostream *strm = new raw_string_ostream(s);
    inst->print(*strm);
    std::string instStr = strm->str();
    trim(instStr);
    return instStr;
  }

public:
  OptimizationResultWriter(Module &module)
      : file(std::to_string(get_pid()) + ".tmp", std::ios::out | std::ios::app),
        tee(file, std::cout) {
    moduleName = module.getName().str();
    int idx = 0;
    int sz = moduleName.size();
    for (; idx < sz; idx++) {
      if (moduleName[idx] == '.') {
        break;
      }
    }
    if (idx < sz - 1) {
      moduleName = moduleName.substr(0, idx);
    }
  }

  void printDeadFunctions(vector<Function *> &deadFunctions) {
    for (auto func : deadFunctions) {
      tee << "Dead Function: " << func->getName().str() << "\n";
    }
  }

  void printCallGraph(map<Function *, vector<Function *>> &callGraph) {
    for (auto graphList : callGraph) {
      for (auto callee : graphList.second) {
        tee << "Edge: " << graphList.first->getName().str() << " -> "
            << callee->getName().str() << "\n";
      }
    }
  }

  void writeModifiedModule(Module &m) {
    std::string s = "";
    raw_string_ostream *strm = new raw_string_ostream(s);
    m.print(*strm, nullptr);
    std::istringstream iss(s);
    std::string line;
    while (getline(iss, line)) {
      tee << "Modified IL: " << line << "\n";
    }
  }

  void printDeadInstruction(Function *function, Instruction* deadInstruction) {
    tee << "Dead Instruction: " << function->getName().str() << ", "
         << instructionToString(deadInstruction) << "\n";
  }

  void printDeadInstructions(Function *function, vector<Instruction*> deadInstructions) {
    for(auto deadInstruction : deadInstructions) {
      printDeadInstruction(function, deadInstruction);
    }
  }
};
