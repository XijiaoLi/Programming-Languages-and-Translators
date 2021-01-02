#include <string>

#ifndef LLVM_PRINTING_H
#define LLVM_PRINTING_H

/*
 * This helper function prints out the previous declaration of a variable.
 */
void printVariableRedeclarationInformation(const std::string &varName,
                                           int currentDefLineNo,
                                           int initialDefLineNo);

#endif
