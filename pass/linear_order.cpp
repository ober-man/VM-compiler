#include "linear_order.h"
#include "loop_analysis.h"

namespace compiler
{

bool LinearOrder::runPassImpl()
{
    assert(graph != nullptr && "nullptr graph in LoopAnalysis pass");

    bool loops = graph->runPassLoopAnalysis();
    if (!loops)
        return false;

    mrk = graph->getNewMarker();

    processBBs();
    graph->setLinearOrderBBs(linear_bbs);

    graph->deleteMarker(mrk);
    return true;
}

void LinearOrder::processBBs()
{
    auto &rpo_bbs = graph->getRpoBBs();

    for (auto *bb : rpo_bbs)
    {
        if (bb->isMarked(mrk))
            continue;

        // loops have to lie contiguously
        if (bb->isHeader() && !bb->getLoop()->isIrreducible())
            processLoop(bb->getLoop());
        else
        {
            bb->setMarker(mrk);
            linear_bbs.push_back(bb);
        }
    }
}

void LinearOrder::processLoop(Loop *loop)
{
    auto &body = loop->getBody();
    for (auto *bb : body)
    {
        if (bb->isMarked(mrk))
            continue;

        // process inner loops
        if (bb->isHeader())
        {
            Loop *inner_loop = bb->getLoop();
            if (inner_loop != loop && !inner_loop->isIrreducible())
            {
                processLoop(inner_loop);
                continue;
            }
        }
        bb->setMarker(mrk);
        linear_bbs.push_back(bb);
    }
}

} // namespace compiler