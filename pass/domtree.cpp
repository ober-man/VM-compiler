#include "domtree.h"

namespace compiler
{

auto &DomTree::getUnreachedBBs(
    std::vector<std::shared_ptr<BasicBlock>> &reached)
{
    std::vector<std::shared_ptr<BasicBlock>> unreached;
    for (auto bb : reached)
        if (std::find(bbs.begin(), bbs.end(), bb) == bbs.end())
            unreached.push_back(bb);
    return unreached;
}

void DomTree::RunPassImpl(std::shared_ptr<Graph> graph) override
{
    assert(graph != nullptr && "nullptr graph in domtree pass");

    graph->RunPass<RPO>();
    bbs = graph->getRPOBBs();

    auto first_BB = bbs[0];
    first_BB->addDominator(first_BB);

    for (auto bb : bbs)
    {
        bb->setVisited(true);

        graph->RunPass<RPO>();
        auto reached_bbs = graph->getRPOBBs();
        auto unreached_bbs = getUnreachedBBs(reached_bbs);

        for (auto unreached : unreached_bbs)
            unreached->addDominator(bb);

        bb->setVisited(false);
    }

    graph->RunPass<RPO>();
}

} // namespace compiler