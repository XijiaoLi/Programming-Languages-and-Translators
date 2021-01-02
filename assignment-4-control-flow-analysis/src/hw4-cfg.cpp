#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/IR/Dominators.h"

#include "hw4-utils.h"

using namespace llvm;

#define DEBUG_TYPE "hw4_cfg"

struct hw4_cfg : public FunctionPass {
  static char ID; // Pass identification, replacement for typeid
  hw4_cfg() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    OFile ofile(F);

    // TODO: write something here
    DominatorTree DT = DominatorTree(F);
    BasicBlock &ret_block = F.back();
    for (Function::iterator block_i = F.begin(), block_end = F.end(); block_i != block_end; block_i++) {
      BasicBlock* curr_block = dyn_cast<BasicBlock>(&*block_i);
      Instruction* term_inst = curr_block->getTerminator();
      if (DT.dominates(term_inst, &ret_block)) {
        ofile.printKeyBlock(*curr_block);
      }
      unsigned succ_num = term_inst->getNumSuccessors();
      if (succ_num == 1) {
        BasicBlock *succ_block = term_inst->getSuccessor(0);
        ofile.printEdge(*curr_block, *succ_block);
      } else if (succ_num == 2) {
        BasicBlock *succ_block1 = term_inst->getSuccessor(0);
        BasicBlock *succ_block2 = term_inst->getSuccessor(1);
        ofile.printEdge(*curr_block, *succ_block1, OFile::EdgeType::T);
        ofile.printEdge(*curr_block, *succ_block2, OFile::EdgeType::F);
      }
    }
    ofile.printKeyBlock(ret_block);

    return false;
  }
};

char hw4_cfg::ID = 0;
static RegisterPass<hw4_cfg> X("hw4-cfg", "Generate control flow graph", false, false);
