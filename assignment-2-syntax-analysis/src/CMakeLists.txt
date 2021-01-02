add_clang_executable(clang-hw2 hw2_util.h hw2_util.cpp ClangHw2.cpp)

target_link_libraries(clang-hw2 PRIVATE clangTooling)
