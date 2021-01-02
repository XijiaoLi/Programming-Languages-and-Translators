#include <string>
#include <iostream>

#include "HW3Util.h"

/*
 * This helper function prints out the previous declaration of a variable.
 */
void printVariableRedeclarationInformation(const std::string &varName,
                                           int currentDefLineNo,
                                           int initialDefLineNo) {
  std::cout << "Redefining variable \"" << varName << "\" at line "
            << currentDefLineNo << ", which is initially defined at line "
            << initialDefLineNo << std::endl;
}
