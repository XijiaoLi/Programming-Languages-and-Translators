# COMS W4115: Programming Assignment 3 (Semantic Analysis)


## Course Summary

Course: COMS 4115 Programming Languages and Translators (Fall 2020)  
Website: https://www.rayb.info/fall2020  
University: Columbia University.  
Instructor: Prof. Baishakhi Ray


## Logistics
* **Announcement Date:** Wednesday, October 14, 2020
* **Due Date:** Wednesday, October 28, 2020 by 11:59 PM. **No extensions!**
* **Total Points:** 70

## Grading Breakdown
* **Implementation of Variable Redefinition Warning Policy:** 70

## Assignment Objectives

From this assignment:

1. You **will learn** how to reason about code semantics from an AST.
2. You **will learn** how to identify redefined variables using scope-based semantic analysis.

## Assignment

In Programming Assignment 2, you began to analyze an AST based on the Clang compiler and had the opportunity to perform syntactic analysis (identifying recursive functions and reformatting functions). As you learned in class, the next and last front-end phase of compilation is semantic analysis, which deals with the scoping and typing of code. In this assignment, you will be able to dig a level deeper into compilation by getting your feet wet with the scoping aspect of semantic analysis.

According to `C` language specifications, it is perfectly legal to redefine a variable with the same name in an inner scope. For example:
```c
1. void foo() {
2.     int i = 4, j = 5;
3.     for (i = 0; i < 6; i++){
4.         int j = 9;
5.         // Do something
6.     }
7. }
```
Variable `j` is redefined in line 4, which was already defined in an outer scope (line 2). While such a redefinition is legal, there is a high chance that it is a developer mistake, and we need to build a tool to warn them.

In the above example, we want to warn the developer as follows: 
```c
Redefining variable "j" at line 4, which is initially defined at line 2
```
In order to that, we need to implement a policy (let us call it **Variable Redefinition Warning Policy**) that checks the developer-written code and warns her if there is a policy violation.

As another example, note that the following case, however, **does not** have any variable redefinition and should not generate any warning:
```c
 1. int gcd_recursive(int m2, int n2) {
 2.     int r = m2 % n2;
 3.     if (r != 0) {
 4.         int k4 = 0;
 5.  	      return gcd_recursive(n2,r);
 6.     }
 7.     else {
 8.  	      int k4 = n2;
 9.         return k4;
10.  	  }
11.  }
```
Even though the `k4` variable is defined in line 4 and again in line 8, the earlier definition (at line 4) is not visible at line 8, *i.e.*, these variables are in different scopes.

Here are a couple of things to keep in mind:

1. You may assume there will be no global variables.
2. Changing the value of a variable is **NOT** considered a redefinition. For instance:
```c
1. void foo() {
2.     int i = 4, j = 5;
3.     for (i = 0; i < 6; i++){
4.         int j = 9;
5.         // Do something
6.     }
7. }
```
In this example, at line 3, the value of variable `i` is changed, but because the variable is still the same, this is not a redefinition. A variable will be considered redefined when another variable with the same name (same or different data types) is re-declared in an inner scope.

* [`VisitFunctionDecl`](src/ClangHW3.cpp#L60) calls the function [`analyzeRedefinition`](src/ClangHW3.cpp#L69). Inside [`analyzeRedefinition`](src/ClangHW3.cpp#L49), if you encounter a variable that is redefined, you should call the helper function [`printVariableRedeclarationInformation`](src/HW3Util.cpp#L9) with the variable name, the line number where it was initially defined, and the line number where it is being redefined.

Your task is to implement the [`analyzeRedefinition`](src/ClangHW3.cpp#L49) function, using the hints in the TODO comment as a guide.

### Steps
1. Create a folder named `clang-hw3` under `$LLVM_HOME/clang/tools`.
2. Copy the [ClangHW3.cpp](src/ClangHW3.cpp), [CMakeLists.txt](src/CMakeLists.txt), [HW3Util.h](src/HW3Util.h), and [HW3Util.cpp](src/HW3Util.cpp) files into `$LLVM_HOME/clang/tools/clang-hw3`.
3. Edit the `$LLVM_HOME/clang/tools/CMakeLists.txt` file, and add this line: `add_clang_subdirectory(clang-hw3)`. 
4. Now, go to `$LLVM_HOME/build`, and run `make`. When the build has successfully finished, it will generate a binary file named `clang-hw3` in `$LLVM_HOME/build/bin`.

## Submission

Please follow these steps for submission:

1. Copy the completed `ClangHW3.cpp` file (with the `analyzeRedefinition` function implemented) from your `$LLVM_HOME/clang/tools/clang-hw3/` directory to this projects `src` folder.
2. Commit your code.
3. Push the code to the master branch.


## Piazza
If you have any questions about this programming assignment, please post them in the Piazza forum for the course, and an instructor will reply to them as soon as possible. Any updates to the assignment itself will be available in Piazza.


## Disclaimer
This assignment belongs to Columbia University. It may be freely used for educational purposes.
