#include "hw5-util.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/Pass.h"

#include <algorithm>
#include <map>
#include <set>
#include <vector>

using namespace llvm;
using namespace std;

struct Liveness : public FunctionPass {
  static char ID; // Pass identification, replacement for typeid

  Liveness() : FunctionPass(ID) {}

  set<Value *> setUnion(set<Value *> &a, set<Value *> &b) {
    set<Value *> result;
    std::set_union(a.begin(), a.end(), b.begin(), b.end(),
                   std::inserter(result, result.begin()));
    return result;
  }

  set<Value *> setIntersection(set<Value *> &a, set<Value *> &b) {
    set<Value *> result;
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(),
                          std::inserter(result, result.begin()));
    return result;
  }

  set<Value *> setDifference(set<Value *> &a, set<Value *> &b) {
    set<Value *> result;
    std::set_difference(a.begin(), a.end(), b.begin(), b.end(),
                        std::inserter(result, result.begin()));
    return result;
  }

  map<Instruction *, vector<Instruction *>>
  extractControlFlowGraph(vector<Instruction *> &allInstructions) {
    map<Instruction *, vector<Instruction *>> controlFlowGraph;
    int nIsnt = allInstructions.size();
    for (int i = 0; i < nIsnt; i++) {
      controlFlowGraph[allInstructions[i]] = vector<Instruction *>();
      if (allInstructions[i]->isTerminator()) {
        int nSuccessors = allInstructions[i]->getNumSuccessors();
        for (int j = 0; j < nSuccessors; j++) {
          auto successor = allInstructions[i]->getSuccessor(j);
          controlFlowGraph[allInstructions[i]].push_back(&successor->front());
        }
      } else {
        controlFlowGraph[allInstructions[i]].push_back(allInstructions[i + 1]);
      }
    }
    return controlFlowGraph;
  }

  /**
   * This function populates the initial values of different data flow
   * ingredients. For every instruction, if it defines anything, `def` will
   * contain that value. `use` will contain all the values that are used in an
   * instruction. Initially, both `in` and `out` are populated with the empty set.
   * `phiUse` and `phiIn` are two special cases.
   * Not all the values are used (and thus `in`) for a `PHINode`.
   * `PHINode` only uses the values from the predecessor basic block it is coming from.
   * Thus, for every instruction, `phiUse` and `phiIN` contain a map from the incoming
   * `BasicBlock*` to the incoming `Value*` set.
   * Note that all the function arguments are pointers in this function.
   * Thus, any changes made in this function will be reflected in the original maps.
   */
  void populateInitialValues(
      vector<Instruction *> &allInstructions,
      map<Instruction *, set<Value *>> &def,
      map<Instruction *, set<Value *>> &use,
      map<Instruction *, set<Value *>> &in,
      map<Instruction *, set<Value *>> &out,
      map<PHINode *, map<BasicBlock *, set<Value *>>> &phiUse,
      map<PHINode *, map<BasicBlock *, set<Value *>>> &phiIn) {
    for (Instruction *pI : allInstructions) {
      Value *p = dyn_cast<Value>(pI);
      set<Value *> s;
      if (!isa<BranchInst>(pI) && !isa<ReturnInst>(pI)) {
        s.insert(p);
      }
      def[pI] = s;
      in[pI] = set<Value *>();
      out[pI] = set<Value *>();

      set<Value *> s2;
      for (auto opnd = pI->op_begin(); opnd != pI->op_end(); ++opnd) {
        Value *val = *opnd;
        if (isa<Instruction>(val) || isa<Argument>(val)) {
          s2.insert(val);
        }
      }
      use[pI] = s2;
      // handle PHI NODE
      if (isa<PHINode>(pI)) {
        PHINode *phiInstruction = dyn_cast<PHINode>(pI);
        map<BasicBlock *, set<Value *>> tempUseMap;
        map<BasicBlock *, set<Value *>> tempInMap;
        for (unsigned int ind = 0; ind < phiInstruction->getNumIncomingValues();
             ind++) {
          set<Value *> tempSet;
          Value *val = phiInstruction->getIncomingValue(ind);
          if (isa<Instruction>(val) || isa<Argument>(val)) {
            BasicBlock *valBlock = phiInstruction->getIncomingBlock(ind);
            if (tempInMap.find(valBlock) == tempInMap.end()) {
              tempInMap[valBlock] = set<Value *>();
            }
            if (tempUseMap.find(valBlock) == tempUseMap.end()) {
              tempSet.insert(val);
              tempUseMap[valBlock] = tempSet;
            } else {
              tempUseMap[valBlock].insert(val);
            }
          }
        }
        phiUse[phiInstruction] = tempUseMap;
        phiIn[phiInstruction] = tempInMap;
      }
    }
  }

  /*
   * The following function is for generating the data flow graph.
   * When you find a data flow edge from an instruction *FROM to an
   * instruction *TO that flows the value *VALUE, call
   * "writer.printDataFlowEdge(FROM, TO, VALUE);" to print out the edge.
   */
  void
  generateDataFlowGraph(vector<Instruction *> allInstructions,
                        map<Instruction *, set<Value *>> DEF,
                        map<Instruction *, set<Value *>> USE,
                        map<PHINode *, map<BasicBlock *, set<Value *>>> PHI_USE,
                        DataFlowWriter &writer) {

    map<Value *, set<Instruction *>> V2I;

    map<Instruction *, set<Value *>>::iterator u_it;
    map<PHINode *, map<BasicBlock *, set<Value *>>>::iterator pu_it;
    map<BasicBlock *, set<Value *>>::iterator bb_it;
    set<Value *>::iterator val_it;

    for ( u_it = USE.begin(); u_it != USE.end(); u_it++ ){
      for ( val_it = u_it->second.begin(); val_it != u_it->second.end(); val_it++) {
        if (V2I.count(*val_it) == 0) {
          V2I[*val_it] = set<Instruction *>();
        }
        V2I[*val_it].insert(u_it->first);
      }
    }

    for ( pu_it = PHI_USE.begin(); pu_it != PHI_USE.end(); pu_it++ ){
      Instruction * phi_inst = dyn_cast<Instruction>(pu_it->first);
      for ( bb_it = pu_it->second.begin(); bb_it != pu_it->second.end(); bb_it++) {
        for ( val_it = bb_it->second.begin(); val_it != bb_it->second.end(); val_it++) {
          if (V2I.count(*val_it) == 0) {
            V2I[*val_it] = set<Instruction *>();
          }
          V2I[*val_it].insert(phi_inst);
        }
      }
    }

    for (Instruction *inst : allInstructions) {
      for ( val_it = DEF[inst].begin(); val_it != DEF[inst].end(); val_it++) {
        if (V2I.count(*val_it) > 0) {
          for (Instruction *to : V2I[*val_it]) {
            writer.printDataFlowEdge(inst, to, *val_it);
          }
        }
      }
    }
  }

  bool runOnFunction(Function &F) override {
    DataFlowWriter writer(F);
    vector<Instruction *> allInstructions = writer.getAllInstructions();
    /* The flow graph is represented as adjacency list.
     * For every instruction in the graph, controlFlowGraph contains a vector
     * of its neighboring instruction (only outgoing edges). */
    map<Instruction *, vector<Instruction *>> controlFlowGraph =
        extractControlFlowGraph(allInstructions);

    map<Instruction *, set<Value *>> USE;
    map<Instruction *, set<Value *>> DEF;
    map<PHINode *, map<BasicBlock *, set<Value *>>> PHI_USE;
    map<Instruction *, set<Value *>> LIVE_IN;
    map<Instruction *, set<Value *>> LIVE_OUT;
    map<PHINode *, map<BasicBlock *, set<Value *>>> PHI_IN;
    populateInitialValues(
        allInstructions, DEF, USE, LIVE_IN, LIVE_OUT, PHI_USE, PHI_IN);

    /* These two printers show the DEFs and USEs of every instruction.
     * You can use these two functions to see the values of DEF and USE.
     * If you do so, make sure to comment out these two lines before submission.*/
    // writer.printDefs(DEF);
    // writer.printUses(USE);

    /* Task 1 - Generating the data flow graph */
    generateDataFlowGraph(allInstructions, DEF, USE, PHI_USE, writer);

    /* Task 2 - Identifying the live-in and live-out sets */
    int iterationCount = 0;
    bool possibleToUpdate = true;
    /* If you change anything in the existing code and the output format
     * does not match, your assignment might be penalized. */

    /*
     * Initially, we assume that the data flow update is possible.
     * We control the update loop using the variable `possibleToUpdate`.
     * Inside the data flow analysis, when you want to terminate updating
     * the LIVE_IN and LIVE_OUT sets, set `possibleToUpdate = false`.
     */
    while (possibleToUpdate) {
      iterationCount++;
      writer.updateIteration();
      /* TODO: Add the code for solving the data flow equations.
       * Please do not delete any line of code from this function.
       * You can add code as necessary.
       * The data flow equations are as follows (in no particular order):
       *    in[n] = use[n] ∪ (out[n] – def[n])
       *    out[n] = ∪(s in succ) {in[s]}
       * TODO: Consider `PHINode` as a special case.
       * Carefully consider the structure of `PHI_USE`, and `PHI_IN` maps.
       * We will provide NO further hint about how to process the PHINode.
       * Please read through the documentation. */
      possibleToUpdate = false;

      for (auto pI = allInstructions.rbegin(); pI != allInstructions.rend(); ++pI) {
      // for (Instruction *pI : allInstructions) {
        set<Value *> temp_out = LIVE_OUT[*pI];
        vector<Instruction *> successor_inst = controlFlowGraph[*pI];
        for (Instruction *pI_succesor : successor_inst) {
          if (isa<PHINode>(pI_succesor)) {
            PHINode *phi_node = dyn_cast<PHINode>(pI_succesor);
            temp_out = setUnion(temp_out, PHI_IN[phi_node][(*pI)->getParent()]);
          } else {
            temp_out = setUnion(temp_out, LIVE_IN[pI_succesor]);
          }
        }

        if (temp_out.size() != LIVE_OUT[*pI].size()) {
          LIVE_OUT[*pI] = temp_out;
          possibleToUpdate = true;
        }

        if (isa<PHINode>(*pI)) {
          PHINode *phi_node = dyn_cast<PHINode>(*pI);

          set<Value *> temp_in = LIVE_IN[*pI];
          set<Value *> set_diff = setDifference(LIVE_OUT[*pI], DEF[*pI]);

          for (unsigned int index = 0; index < phi_node->getNumIncomingValues(); index++) {
            BasicBlock *BB = phi_node->getIncomingBlock(index);
            set<Value *> p_temp_in;

            if (PHI_USE[phi_node].count(BB) == 0) {
              p_temp_in = set_diff;
              if (PHI_IN[phi_node].count(BB) == 0) {
                PHI_IN[phi_node][BB] = set<Value *>();
              }
            } else {
              p_temp_in = setUnion(PHI_USE[phi_node][BB], set_diff);
            }

            if (p_temp_in.size() != PHI_IN[phi_node][BB].size()) {
              PHI_IN[phi_node][BB] = p_temp_in;
              temp_in = setUnion(temp_in, p_temp_in);
              possibleToUpdate = true;
            }
          }
          LIVE_IN[*pI] = temp_in;
        } else {
          set<Value *> set_diff = setDifference(LIVE_OUT[*pI], DEF[*pI]);
          set<Value *> temp_in = setUnion(USE[*pI], set_diff);
          if (temp_in.size() != LIVE_IN[*pI].size()) {
            LIVE_IN[*pI] = temp_in;
            possibleToUpdate = true;
          }
        }
      }
    }

    /* Please do not modify the following lines. */
    writer.printLiveIns(LIVE_IN);
    writer.printLiveOuts(LIVE_OUT);
    writer.printIterationCount(iterationCount);
    return true;
  }
};

char Liveness::ID = 0;
static RegisterPass<Liveness> X("liveness", "My Liveness Analysis Pass");
