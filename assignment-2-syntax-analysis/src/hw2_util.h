//
// Created by Saikat Chakraborty on 9/24/20.
//
#include <string>

#ifndef LLVM_PRINTING_H
#define LLVM_PRINTING_H

void printCallExprReformatOutput(int lineNo, const std::string& originalSource,
                                 const std::string& formattedSource);

void printRecursiveFunction(const std::string& functionName, bool recursive);

void printForStmt(int ln);
#endif // LLVM_PRINTING_H
