#include "gtest/gtest.h"
#include "osutils.h"

#include <vector>
#include <string>
#include <fstream>
#include <unordered_set>
#include <regex>
#include <iostream>
#include <tuple>

using namespace std;

//#define LLVM_BUILD_DIR "/home/liuqx/codes/llvm-project/build"
#ifndef LLVM_BUILD_DIR
#error *** Please define LLVM_BUILD_DIR by yourself (absolute path)
#endif

#define CLANG_PATH LLVM_BUILD_DIR "/bin/clang"
#define OPT_PATH LLVM_BUILD_DIR "/bin/opt"

/*
 * @input: code snippet as the input of clang-hw6
 * @return: lines of the output of clang-hw6 (by hw6-util.cpp)
 *   If clang-hw6 failed to execute, the return value will be an error message
 */
static vector<string> run_clang_hw6(const string &input) {
    vector<string> ret;

    string ofn, ifn, bfn, bfn2, line;
    int pid = 0, rc = 0;

    // Generate .c file
    ofn = to_string(get_pid()) + ".c";
    {
        ofstream ofs(ofn, ios::out | ios::trunc);
        ofs << input;
        if (!ofs) {
            ret = {string("test-clang-hw6: Failed to write ") + ofn};
            goto end;
        }
    }

    // Run clang and generate .bc file
    tie(pid, rc) = run_and_wait(CLANG_PATH, "-O0", "-Xclang", "-disable-O0-optnone", "-emit-llvm", "-c", ofn.c_str());
    if (!pid || rc != 0) {
        ret = {"test-clang-hw6: Failed to execute clang"};
        goto end;
    }

    // Convert .bc to ssa format
    bfn = to_string(get_pid()) + ".bc";
    bfn2 = to_string(get_pid()) + "-ssa.bc";
    tie(pid, rc) = run_and_wait(OPT_PATH, "-mem2reg", "-o", bfn2.c_str(), bfn.c_str());
    if (!pid || rc != 0) {
        ret = {"test-clang-hw6: Failed to execute opt -mem2reg"};
        goto end;
    }

    // Run opt
    tie(pid, rc) = run_and_wait(OPT_PATH, "-load", "./LLVMOptimizer.so", "-optimize", "-o", "/dev/null", bfn2.c_str());
    if (!pid || rc != 0) {
        ret = {"test-clang-hw6: Failed to execute opt"};
        goto end;
    }

    // Read the results
    ifn = to_string(pid) + ".tmp";
    {
        ifstream ifs(ifn);
        if (!ifs) {
            ret = {string("test-clang-hw6: Failed to open ") + ifn};
            goto end;
        }
        while (getline(ifs, line)) {
            ret.emplace_back(move(line));
        }
    }

end:
    remove(ifn.c_str());
    remove(bfn.c_str());
    remove(bfn2.c_str());
    remove(ofn.c_str());
    return ret;
}

/*
 * 1. Filter out the lines by a regex @pattern and convert to a hash set
 * 2. Compare the filtered lines with the solution
 * @return: the number of correct lines, incorrect lines, solution lines
 */
static tuple<int, int, int> judge(const vector<string> &lines,
                                  const string &pattern,
                                  const unordered_set<string> &solution) {
    regex re(pattern);
    unordered_set<string> uniq_lines;
    for (auto &line: lines) {
        if (!regex_search(line, re)) continue;
        uniq_lines.insert(line);
    }

    int correct = 0, incorrect = 0, total = 0;
    for (auto &line: uniq_lines) {
        if (solution.count(line)) {
            cout << "*** Correct: " << line << "\n";
            correct++;
        } else {
            cout << "*** Incorrect: " << line << "\n";
            incorrect++;
        }
    }
    for (auto &line: solution) {
        if (!regex_search(line, re)) continue;
        total++;
    }

    return make_tuple(correct, incorrect, total);
}

static bool judge_dead_functions(const string &src, 
                                 const unordered_set<string> &solution) {
    string pattern = "^Dead Function:";

    auto res = run_clang_hw6(src);
    int correct = 0, incorrect = 0, total = 0;
    tie(correct, incorrect, total) = judge(res, pattern, solution);

    cout << "*** Dead Functions: " << correct << " correct, " << incorrect << " incorrect\n";
    return correct == total && incorrect == 0;
}

static bool judge_removed_functions(const string &src, 
                                    const unordered_set<string> &solution) {
    string pattern = "^Modified IL: define dso_local";

    auto res = run_clang_hw6(src);
    int correct = 0, incorrect = 0, total = 0;
    tie(correct, incorrect, total) = judge(res, pattern, solution);

    cout << "*** Functions In Modified IL: " << correct << " correct, " << incorrect << " incorrect\n"; 
    return correct == total && incorrect == 0;
}

static bool judge_dead_insts(const string &src, 
                             const unordered_set<string> &solution) {
    string pattern = "^Dead Instruction:";

    auto res = run_clang_hw6(src);
    int correct = 0, incorrect = 0, total = 0;
    tie(correct, incorrect, total) = judge(res, pattern, solution);

    cout << "*** Dead Instructions: " << correct << " correct, " << incorrect << " incorrect\n";
    return correct == total && incorrect == 0;
}


static auto src1 = R"_(
int func1() {
    return 0;
}

int main() {
    int a = 1, b = 2;
    int c = 0;

    if (c == 0) {
        c = c + 1;
    }
    a = a * 2;
    b = b + c;
    
    if (c == 0) {
        return c + 1;
    } else {
        return c + 2;
    }

    return a + b;
}
)_";

static unordered_set<string> solution1 {
    "Dead Function: func1", 
    "Dead Instruction: main, %mul = mul nsw i32 1, 2", 
    "Dead Instruction: main, %add1 = add nsw i32 2, %c.0", 
    "Modified IL: define dso_local i32 @main() #0 {"
};

static auto src2 = R"_(
#include <stdio.h>

int func2(int n, int m) {	 
    return n + m;
}
int func3(int m, int n) {
    int res = 0, t = m + n, x = m * n;
    while(res != x) {
        res = res + m;
    }
    return res;
}
int func4(int n) {
    if (n == 0) return 1;
    else {
        return func3(n, func4(func2(n, -1)));
    }
}

int main() {
    int n = 9, m = 8;
    printf("%d\n", func3(m, n));
    return 0;
}
)_";

static unordered_set<string> solution2 {
    "Dead Function: func2", 
    "Dead Function: func4", 
    "Dead Instruction: func3, %add = add nsw i32 %m, %n", 
    "Modified IL: define dso_local i32 @func3(i32 %m, i32 %n) #0 {", 
    "Modified IL: define dso_local i32 @main() #0 {"
};

static auto src3 = R"_(
#include <stdio.h>

int func5(int n);
int func6(int n);
int func7(int n);
int func8(int n);

int func5(int n) {
    return func6(n);
    return func7(n) + func8(n + n);
}

int func6(int n) {
    return func8(n);
}

int func7(int n) {
    return func5(n);
}

int func8(int n) {
    return func6(func8(func5(n)));
}

int main() {
    int n = 9, m = 8;
    printf("%d\n", func5(n));
    return 0;
}
)_";

static unordered_set<string> solution3 {
    "Dead Function: func7", 
    "Modified IL: define dso_local i32 @func5(i32 %n) #0 {", 
    "Modified IL: define dso_local i32 @func6(i32 %n) #0 {", 
    "Modified IL: define dso_local i32 @func8(i32 %n) #0 {", 
    "Modified IL: define dso_local i32 @main() #0 {"
};


#define DF_TEST(n)                                                  \
    TEST(Src##n, DeadFunctions) {                                   \
        EXPECT_TRUE(judge_dead_functions(src##n, solution##n));     \
    }

#define REMOVEDF_TEST(n)                                            \
    TEST(Src##n, RemovedDeadFunctions) {                            \
        EXPECT_TRUE(judge_removed_functions(src##n, solution##n));  \
    }

#define DI_TEST(n)                                                  \
    TEST(Src##n, DeadInstructions) {                                \
        EXPECT_TRUE(judge_dead_insts(src##n, solution##n));         \
    }

#ifdef TEST_PART1
DF_TEST(1)
DF_TEST(2)
DF_TEST(3)
#endif
#ifdef TEST_PART2
REMOVEDF_TEST(1)
REMOVEDF_TEST(2)
REMOVEDF_TEST(3)
#endif
#ifdef TEST_PART3
DI_TEST(1)
DI_TEST(2)
DI_TEST(3)
#endif