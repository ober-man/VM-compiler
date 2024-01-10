#include "domtree.h"
#include "rpo.h"

namespace compiler
{

void DomTree::invalidateAnalysis()
{
    for (auto* bb : bbs)
        bb->getDominators().clear();
}

auto DomTree::getUnreachedBBs(std::vector<BasicBlock*>& reached)
{
    std::vector<BasicBlock*> unreached;
    for (auto* bb : bbs)
        if (std::find_if(reached.begin(), reached.end(), [bb](auto* elem) { return bb == elem; }) ==
            reached.end())
        {
            // if bb was not found in reached array -> it is unreached
            unreached.push_back(bb);
        }

    return unreached;
}

bool DomTree::runPassImpl()
{
    ASSERT(graph != nullptr, "nullptr graph in domtree pass");

    marker_t visited = graph->getNewMarker();
    graph->runPass<Rpo>();
    bbs = graph->getRpoBBs();
    ASSERT(bbs.size() > 0, "empty graph in DomTree");

    for (auto* bb : bbs)
    {
        bb->setMarker(visited);

        graph->runPass<Rpo>(visited);
        auto reached_bbs = graph->getRpoBBs();
        auto unreached_bbs = getUnreachedBBs(reached_bbs);

        for (auto unreached : unreached_bbs)
            unreached->addDominator(bb);

        bb->resetMarker(visited);
    }

    for (auto* bb : bbs)
        bb->countIdom();

    graph->runPass<Rpo>();
    graph->deleteMarker(visited);
    return true;
}

} // namespace compiler