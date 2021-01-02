add_clang_executable(clang-hw3 ClangHW3.cpp HW3Util.h HW3Util.cpp)

target_link_libraries(clang-hw3 PRIVATE clangTooling)
