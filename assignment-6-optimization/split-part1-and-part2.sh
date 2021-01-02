#!/bin/sh

USAGE="Usage:
    $(basename -- "$0") <file1> [<file2> ...]

The part1 and part2 are in the same file, which makes it difficult to grade
separately.

This tool generates two versions of each <fileN>: '<fileN>-part1.cpp' and
'<fileN>-part2.cpp' by substituding the functions correspondingly.

The solution of both parts are hardcoded inside this script.
"

[ ! -f "$1" ] && echo "$USAGE" && exit 1

for f in "$@"; do
    sed -e 's/^[[:space:]]*void removeDeadFunctions(/  void removeDeadFunctions0(/' \
        -e '/  void removeDeadFunctions0(/i \
  void removeDeadFunctions(Module &M, vector<Function *> &deadFunctions) { \
    for (auto function : deadFunctions) { \
      function->replaceAllUsesWith(UndefValue::get(function->getType())); \
      function->eraseFromParent(); \
    } \
  }' \
        "$f" > "${f%.cpp}-part1.cpp"

    sed -e 's/^[[:space:]]*vector<Function \*> getDeadFunctions(/  vector<Function *> getDeadFunctions0(/' \
        -e '/  vector<Function \*> getDeadFunctions0(/i \
  vector<Function *> getDeadFunctions(vector<Function *> &allFunctions, \
                    map<Function *, vector<Function *>> &callGraph, \
                    Function *entryFunction) { \
    vector<Function *> dead; \
    vector<Function *> visitedFunction({entryFunction}); \
    stack<Function *> stack({entryFunction}); \
    while (!stack.empty()) { \
      Function *curr = stack.top(); \
      stack.pop(); \
      for (auto child : callGraph[curr]) { \
        if (std::find(visitedFunction.begin(), visitedFunction.end(), child) == \
            visitedFunction.end()) { \
          visitedFunction.push_back(child); \
          stack.push(child); \
        } \
      } \
    } \
    for (auto function : allFunctions) { \
      if (std::find(visitedFunction.begin(), visitedFunction.end(), function) == \
          visitedFunction.end()) { \
        dead.push_back(function); \
      } \
    } \
 \
    return dead; \
  }' \
        "$f" > "${f%.cpp}-part2.cpp"
done
