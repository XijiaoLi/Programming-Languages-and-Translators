# COMS W4115: Programming Assignment 4 (Control Flow Analysis)

## Course Summary

Course: COMS 4115 Programming Languages and Translators (Fall 2020)  
Website: https://www.rayb.info/fall2020  
University: Columbia University  
Instructor: Prof. Baishakhi Ray


## Logistics
* **Announcement Date:** Saturday, October 31, 2020
* **Due Date:** Saturday, November 14, 2020 by 11:59 PM. **No extensions!**
* **Total Points:** 100

## Grading Breakdown
* **Task 1 (CFG Generation)**: 50
* **Task 2 (CFG Analysis)**: 50

## Assignment Objectives

From this assignment:

1. You **will learn** how to write an LLVM pass.
2. You **will learn** how to generate a control flow graph by analyzing basic blocks.
3. You **will learn** how to analyze a control flow graph using the LLVM API.

## Assignment

In the previous programming assignments for syntax analysis and sematic analysis, you worked with `clang`, the LLVM front-end for C, C++, and Objective-C.

The core of LLVM is the [intermediate representation](https://en.wikipedia.org/wiki/Intermediate_representation) (IR), a low-level programming language similar to assembly, which abstracts away most details of the high-level programming language and also the target-machine-specific nuances. When compiling a programming language under LLVM, it will first convert the specific programming language into IR and then perform analysis/optimization techniques against the IR. Finally, it will generate the target binary code (*e.g.*, x86, ARM, etc.).

Optimizations are implemented as *passes* that traverse some portion of a program to either collect information or transform the program. For more details about *LLVM Pass*, check out [Writing an LLVM Pass](https://llvm.org/docs/WritingAnLLVMPass.html).

We will work with the IR and the LLVM Pass Framework for this assignment. More specifically, we will learn how to create a [control flow graph](https://en.wikipedia.org/wiki/Control-flow_graph) (CFG) from an LLVM IR, as well as how to perform lightweight analysis on the CFG. The analysis of control flow graphs is an essential part of the compiler for [program optimization](https://en.wikipedia.org/wiki/Program_optimization). We will learn more about CFGs and their other applications in the class.

### Getting Started

1. Convert the `bubble.c` C program (from the `examples` directory) to an IR by running the following commands:
```
export LLVM_HOME="<the absolute path to llvm-project>";
export PATH="$LLVM_HOME/build/bin:$PATH";

clang -O0 -emit-llvm -c bubble.c
llvm-dis bubble.bc
```
You will now receive a `bubble.bc` file, which contains the IR in binary format. You will also see a `bubble.ll` file, which contains the IR in human-readable format. Go ahead and take a look at the contents of those files, and especially try to understand the structure of the `bubble.ll` file.

2. Generate the CFG from the `bubble.bc` file by running the following commands (again, `examples` is the directory we provided for you):
```
cd ./examples
opt -dot-cfg < bubble.bc
dot -Tpdf .bubbleSort.dot -o bubbleSortDetailed.pdf

opt -dot-cfg-only < bubble.bc
dot -Tpdf .bubbleSort.dot -o bubbleSort.pdf

cd ..
```

Now, you should be able to view the CFG of the function `bubbleSort()` in `bubble.c` by looking at the generated PDFs. Do you notice the difference between the two PDFs (*hint*: look at the names of the PDFs)? Similarly, you can view the CFGs of other functions via the `dot` command (*e.g.*, `dot -Tpdf .printArray.dot -o printArray.pdf` for the `printArray` function).

3. Create a directory named `clang-hw4` in `llvm-project/llvm/lib/Transforms` for this assignment, and copy the files from the `src` directory to this new directory, as follows:
```
cp -r ./src "$LLVM_HOME/llvm/lib/Transforms/clang-hw4"
```

4. Append `add_subdirectory(clang-hw4)` to the `$LLVM_HOME/llvm/lib/Transforms/CMakeLists.txt` file.

5. Build `clang-hw4` by running the following commands (you should do this every time you make changes):
```
cd "$LLVM_HOME/build"
make
```

6. The nodes or vertices of the CFG are called [basic blocks](https://en.wikipedia.org/wiki/Basic_block). In the next two subsections, we will describe your tasks for this assignment; you may assume that for each function, exactly one basic block will have `ret` as its [terminator instruction](https://llvm.org/docs/LangRef.html#terminator-instructions), whereas other basic blocks will have `br` as their [terminator instruction](https://llvm.org/docs/LangRef.html#terminator-instructions). You may also assume that `br` will have *at most 2 successors*.

### Task 1: CFG Generation (50 points)

In this task, you will need to create a [function pass](https://llvm.org/doxygen/classllvm_1_1FunctionPass.html) and construct the CFG of a function by analyzing its basic blocks. We have provided the skeleton code for this task in `src/hw4-cfg.cpp` to help you get started.

Instead of generating a `dot` file yourself, please use the `OFile` class to output all of the edges you found. The results will automatically be saved to `<function_name>.txt`. You can run the pass you created as follows:
```
opt -load $LLVM_HOME/build/lib/LLVMcfg.so -hw4-cfg < bubble.bc
```

As a sanity check, you can compare the edges you found with those in the `bubbleSort.pdf` file you generated earlier.

Note that this task description is intentionally short because one primary objective of this task is to allow you to explore, research, and experiment with LLVM passes. You can use the lecture slides posted by the TAs [here](https://piazza.com/class/kekhb0ii3uh23z?cid=101) as a starting point, and any questions that you have about LLVM passes and CFGs can be asked in Piazza.

### Task 2: CFG Analysis (50 points)

It should not be a surprise that there are various types of analysis/transformation techniques that can be performed on a CFG (*e.g.*, dead block elimination, loop detection/simplification, etc.). Given that you generated a CFG in task 1, task 2 requires that you identify all the *key blocks* in a function. We define a *key block* as a basic block of a function such that every path from the entry of the function to the exit of the function **MUST** go through this basic block. Remember that you may assume that exactly **one** basic block will have `ret` as its [terminator instruction](https://llvm.org/docs/LangRef.html#terminator-instructions) in each function, and `ret` is considered to be the exit of a function.

Please use the `OFile` class to output the *key blocks* you found. As before, the results will automatically be saved to `<function_name>.txt`. You can run the pass you created as follows:
```
opt -load $LLVM_HOME/build/lib/LLVMcfg.so -hw4-cfg < bubble.bc
```

A few important notes:
1. Note that the entry basic block and the basic block containing the `ret` instruction are also *key blocks*, by definition.
2. *Hint*: a basic block is a *key block* if and only if "removing that block makes the exit of the function unreachable."
3. You need to figure out which LLVM APIs you should use for this task. It is also **OK** if you prefer to construct a graph structure yourself and analyze it manually.

## Submission
Only one file should be submitted: `src/hw4-cfg.cpp`. Other files will be ignored when grading. Please make sure that your code is properly committed and pushed to the master branch.

## Piazza
If you have any questions about this programming assignment, please post them in the Piazza forum for the course, and an instructor will reply to them as soon as possible. Any updates to the assignment itself will be available in Piazza.

## Disclaimer
This assignment belongs to Columbia University. It may be freely used for educational purposes.
