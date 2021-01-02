#pragma once

#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"

#include "tee.h"

#include <fstream>
#include <iostream>
#include <type_traits>

// Please use OFile to output the results
class OFile {
public:
  enum class EdgeType {Default, T, F};

private:
  std::fstream file;
  Tee<std::fstream&, std::ostream&> tee;

public:
  OFile(const llvm::Function &F):
    file(F.getName().str() + ".txt", std::ios::out | std::ios::trunc),
    tee(file, std::cout) {
    tee << "Function: " << F.getName().str() << "\n";
  }

  // Support BasicBlock, Function, ...
  template<typename T>
  void printEdge(const T &from, const T &to, EdgeType type = EdgeType::Default) {
    static_assert(std::is_pointer<T>::value == false, "Don't pass a pointer to printEdge()");
    tee << "edge: " << from.getName().str();
    if (type == EdgeType::T) tee << "(T)";
    else if (type == EdgeType::F) tee << "(F)";
    tee << " ---> " << to.getName().str() << "\n";
  }

  void printKeyBlock(const llvm::BasicBlock &BB) {
    tee << "Key Block: " << BB.getName().str() << "\n";
  }
};
