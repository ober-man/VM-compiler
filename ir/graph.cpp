#include "graph.h"
#include "pass/domtree.h"
#include "pass/rpo.h"

namespace compiler
{

// template <typename PassName>
void Graph::runPassRpo()
{
    pm->runPassRpo();
}

void Graph::runPassDomTree()
{
    pm->runPassDomTree();
}

void Graph::addEdge(BasicBlock *prev_bb, BasicBlock *bb)
{
    bb->addPred(prev_bb);
    prev_bb->addSucc(bb);
}

void Graph::replaceBB(BasicBlock *bb, BasicBlock *new_bb)
{
    auto it = std::find(BBs.begin(), BBs.end(), bb);
    assert(it != BBs.end() && "replace not existing bb");
    BBs[(*it)->getId()] = new_bb;
}

void Graph::replaceBB(size_t num, BasicBlock *new_bb)
{
    auto it = std::find_if(BBs.begin(), BBs.end(), [num](auto bb) { return bb->getId() == num; });
    assert(it != BBs.end() && "replace not existing bb");
    BBs[num] = new_bb;
}

} // namespace compiler