#include "rpo.h"

namespace compiler
{

void RPO::VisitBasicBlock(std::shared_ptr<BasicBlock> bb)
{
    assert(bb != nullptr && "nullptr bb in RPO pass");

    if (bb->isVisited())
        return;

    auto true_succ = bb->getTrueSucc();
    auto false_succ = bb->getFalseSucc();

    bb->setVisited();

    if (true_succ != nullptr)
        VisitBasicBlock(true_succ);
    if (false_succ != nullptr)
        VisitBasicBlock(false_succ);

    rpo_bbs.push_back(bb);
}

void RPO::RunPassImpl(std::shared_ptr<Graph> graph)
{
    assert(graph != nullptr && "nullptr graph in RPO pass");

    auto bbs = graph->getBBs();
    auto first_BB = bbs[0];

    VisitBasicBlock(first_BB);
    std::reverse(rpo_bbs.begin(), rpo_bbs.end());
    graph->setRPOBBs(rpo_bbs);
}

} // namespace compiler