#include "graph.h"
#include "pass/domtree.h"
#include "pass/linear_order.h"
#include "pass/liveness.h"
#include "pass/loop_analysis.h"
#include "pass/rpo.h"

namespace compiler
{

Graph::~Graph()
{
    for (auto* bb : BBs)
        delete bb;
    if (root_loop != nullptr)
        delete root_loop;
    for (auto [inst, live_int] : live_intervals)
        delete live_int;
}

// template <typename PassName, typename... Args>
bool Graph::runPassRpo(marker_t marker)
{
    return pm->runPassRpo(marker);
}

bool Graph::runPassDomTree()
{
    return pm->runPassDomTree();
}

bool Graph::runPassLoopAnalysis()
{
    return pm->runPassLoopAnalysis();
}

bool Graph::runPassLinearOrder()
{
    return pm->runPassLinearOrder();
}

bool Graph::runPassLivenessAnalysis()
{
    return pm->runPassLivenessAnalysis();
}

marker_t Graph::getNewMarker()
{
    return mm->getNewMarker();
}

void Graph::deleteMarker(marker_t marker)
{
    mm->deleteMarker(marker);
    for (auto bb : BBs)
        bb->resetMarker(marker);
}

void Graph::insertBB(BasicBlock* bb)
{
    if (graph_size == 0)
    {
        BBs.push_back(bb);
        ++graph_size;
        return;
    }

    auto* pred = BBs[graph_size - 1];
    pred->addSucc(bb);
    bb->addPred(pred);
    BBs.push_back(bb);
    ++graph_size;
}

/**
 * Insert bb after prev_bb
 */
void Graph::insertBBAfter(BasicBlock* prev_bb, BasicBlock* bb, bool is_true_succ)
{
    if (is_true_succ)
    {
        auto* true_succ = prev_bb->getTrueSucc();
        prev_bb->setTrueSucc(bb);
        bb->addPred(prev_bb);

        if (true_succ != nullptr)
            true_succ->addPred(bb);
    }
    else
    {
        auto* false_succ = prev_bb->getFalseSucc();
        prev_bb->setFalseSucc(bb);
        bb->addPred(prev_bb);

        if (false_succ != nullptr)
            false_succ->addPred(bb);
    }
    BBs.push_back(bb);
    ++graph_size;
}

void Graph::addEdge(BasicBlock* prev_bb, BasicBlock* bb)
{
    bb->addPred(prev_bb);
    prev_bb->addSucc(bb);
}

void Graph::replaceBB(BasicBlock* bb, BasicBlock* new_bb)
{
    auto it = std::find(BBs.begin(), BBs.end(), bb);
    assert(it != BBs.end() && "replace not existing bb");
    BBs[(*it)->getId()] = new_bb;
}

void Graph::replaceBB(size_t num, BasicBlock* new_bb)
{
    auto it = std::find_if(BBs.begin(), BBs.end(), [num](auto bb) { return bb->getId() == num; });
    assert(it != BBs.end() && "replace not existing bb");
    BBs[num] = new_bb;
}

} // namespace compiler