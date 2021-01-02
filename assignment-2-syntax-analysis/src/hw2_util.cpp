//
// Created by Saikat Chakraborty on 9/24/20.
//
#include <string>
#include <iostream>

#include "hw2_util.h"

void printCallExprReformatOutput(int lineNo, const std::string& originalSource,
                                 const std::string& formattedSource){
  std::cout << "==================================================\n";
  std::cout << "Function call found at line " << lineNo << "\n";
  std::cout << "Original Source\n";
  std::cout << "---------------------------------------------------\n";
  std::cout << originalSource << "\n";
  std::cout << "---------------------------------------------------\n";
  std::cout << "Formatted Source\n";
  std::cout << "---------------------------------------------------\n";
  std::cout << formattedSource << "\n";
  std::cout << "==================================================\n";
}


void printRecursiveFunction(const std::string& functionName, bool recursive){
  std::cout << functionName << " - ";
  if(!recursive) {
    std::cout << "non-";
  }
  std::cout << "recursive\n";
}

void printForStmt(int ln){
  std::cout << "==================================================\n";
  std::cout << "Found For Statement at line : " << ln
            << "\n";
  std::cout << "==================================================\n";
}