#include "rpo.h"

namespace compiler
{

Rpo::Rpo(Graph *g, marker_t marker) : Analysis(g), visited(marker)
{
    size_t size = g->size();
    rpo_bbs.reserve(size);
    // rpo_bbs.resize(size);
    // cur_num = size-1;
}

void Rpo::visitBasicBlock(BasicBlock *bb)
{
    assert(bb != nullptr && "nullptr bb in RPO pass");
    if (bb->isMarked(visited))
        return;

    auto *true_succ = bb->getTrueSucc();
    auto *false_succ = bb->getFalseSucc();

    bb->setMarker(visited);

    if (true_succ != nullptr)
        visitBasicBlock(true_succ);
    if (false_succ != nullptr)
        visitBasicBlock(false_succ);

    // rpo_bbs[cur_num--] = bb;
    rpo_bbs.push_back(bb);
}

bool Rpo::runPassImpl()
{
    assert(graph != nullptr && "nullptr graph in RPO pass");

    if (visited == NO_MARKER)
        visited = graph->getNewMarker();

    auto bbs = graph->getBBs();
    assert(bbs.size() > 0 && "empty graph in RPO");

    auto first_BB = bbs[0];

    visitBasicBlock(first_BB);
    std::reverse(rpo_bbs.begin(), rpo_bbs.end());
    graph->setRPOBBs(rpo_bbs);

    for (auto *bb : rpo_bbs)
        bb->resetMarker(visited);

    graph->deleteMarker(visited);
    return true;
}

} // namespace compiler