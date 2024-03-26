#include "graph.h"
#include "pass/liveness.h"
#include "pass/loop_analysis.h"

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

marker_t Graph::getNewMarker()
{
    return mm->getNewMarker();
}

void Graph::deleteMarker(marker_t marker)
{
    mm->deleteMarker(marker);
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

void Graph::addBB(BasicBlock* bb)
{
    BBs.push_back(bb);
    bb->setId(graph_size);
    ++graph_size;
}

void Graph::pushBackConstInst(ConstInst* inst)
{
    if (first_const == nullptr)
        first_const = inst;
    last_const = inst;
}

void Graph::removeBB(BasicBlock* bb)
{
    auto it = BBs.erase(std::find(BBs.begin(), BBs.end(), bb));
    ASSERT(it != BBs.end(), "remove not existing bb");
}

void Graph::removeBB(size_t num)
{
    auto it = BBs.erase(
        std::find_if(BBs.begin(), BBs.end(), [num](auto bb) { return bb->getId() == num; }));
    ASSERT(it != BBs.end(), "remove not existing bb");
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
        {
            true_succ->addPred(bb);
            bb->addSucc(true_succ);
        }
    }
    else
    {
        auto* false_succ = prev_bb->getFalseSucc();
        prev_bb->setFalseSucc(bb);
        bb->addPred(prev_bb);

        if (false_succ != nullptr)
        {
            false_succ->addPred(bb);
            bb->addSucc(false_succ);
        }
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
    ASSERT(it != BBs.end(), "replace not existing bb");
    BBs[(*it)->getId()] = new_bb;
}

void Graph::replaceBB(size_t num, BasicBlock* new_bb)
{
    auto it = std::find_if(BBs.begin(), BBs.end(), [num](auto bb) { return bb->getId() == num; });
    ASSERT(it != BBs.end(), "replace not existing bb");
    BBs[num] = new_bb;
}

} // namespace compiler