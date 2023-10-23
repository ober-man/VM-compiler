#include "domtree.h"
#include "rpo.h"

namespace compiler
{

auto DomTree::getUnreachedBBs(std::vector<BasicBlock *> &reached)
{
    std::vector<BasicBlock *> unreached;
    for (auto *bb : bbs)
    {
        if (std::find_if(reached.begin(), reached.end(), [bb](auto *elem) { return bb == elem; }) ==
            reached.end())
        {
            // if bb was not found in reached array -> it is unreached
            unreached.push_back(bb);
        }
    }

    return unreached;
}

bool DomTree::runPassImpl()
{
    assert(graph != nullptr && "nullptr graph in domtree pass");

    graph->runPassRpo();
    bbs = graph->getRpoBBs();
    assert(bbs.size() > 0 && "empty graph in DomTree");

    for (auto *bb : bbs)
    {
        bb->setVisited(true);

        graph->runPassRpo();
        auto reached_bbs = graph->getRpoBBs();
        auto unreached_bbs = getUnreachedBBs(reached_bbs);

        for (auto unreached : unreached_bbs)
            unreached->addDominator(bb);

        bb->setVisited(false);
    }

    graph->runPassRpo();
    return true;
}

} // namespace compiler